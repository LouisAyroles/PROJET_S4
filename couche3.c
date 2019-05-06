#include "couche3.h"
#include "couche2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/**
 * \file couche3.c
 * \brief Programme couche3 du raid5.
 * \author Groupe14
 * \version 0.1
 * \date 01 avril 2019
 *
 * Programme de la couche 2 du raid5.
 *
 */

 /** \brief
   * Affichage de la table d'inodes
   * @param : inode_table_t
   * @return : void
 **/
void inode_table_dump(inode_table_t t){
  for(int i=0;i<INODE_TABLE_SIZE;i++){
    inode_dump(t[i]);
  }
}

/** \brief
  * Affichage d'une inode
  * @param : inode_t
  * @return : void
**/
void inode_dump(inode_t i){
  printf("\tFilename : %s\n",i.filename);
  printf("\t Size : %d |NBlocks : %d |First_Byte : %d \n",i.size,i.nblock,i.first_byte);
}

/** \brief
  * Affichage d'un superblock
  * @param : inode_t
  * @return : void
**/
void super_block_dump(super_block_t s){
  printf("Superbloc\n");
  printf("\tRaid type : %d |nb_blocks_used : %d | first_free_byte : %d\n",s.raid_type,s.nb_blocks_used,s.first_free_byte);
}


 /** \brief
   * Transforme un entier en un tableau de 4 char
   * @param : int ,uchar*
   * @return : void
 **/
 void int_To_uChar(int nb, uchar *buffer){
   for (int i = 0; i < sizeof(int); i++) {
     buffer[i] = ((uchar*)&nb)[i];
   }
 }

 /** \brief
   * Transforme un tableau de sz char en un int
   * @param : int ,uchar*
   * @return : int
 **/
 int uChar_To_Int(uchar *buf){
   int nb = 0;
   for (int i=0;i<sizeof(int);i++){
     ((uchar*)&nb)[i] = buf[i];
   }
   return nb;
 }

 /** \brief
   * Renvoie le numero de bande ou la table d'inode commence
   * @param : virtual_disk_t
   * @return : void
 **/
 int startTable(virtual_disk_t *r5Disk){
   if(r5Disk->ndisk >=4){
     return 1;
   }
   return 2;
 }


 /** \brief
   * Lecture du superbloc
   * @param : virtual_disk_t , super_block_t
   * @return void
 **/
void read_super_block(virtual_disk_t *r5Disk, super_block_t *superblock){
  int posread = 0;
  uchar *buffer = (uchar *)malloc(sizeof(char)*sizeof(super_block_t));
  uchar *bufferConversion = (uchar *)malloc(sizeof(char)*sizeof(int));

  read_chunk(r5Disk, buffer, sizeof(super_block_t),0);

  for(int i = 0; i < sizeof(enum raid); i++){
    bufferConversion[i]=buffer[i];
  }
  superblock->raid_type = uChar_To_Int(bufferConversion);
  r5Disk->super_block.raid_type=uChar_To_Int(bufferConversion);
  posread+=sizeof(enum raid);


  for(int i = 0; i < sizeof(int); i++){
    bufferConversion[i]=buffer[posread+i];
  }
  superblock->nb_blocks_used = uChar_To_Int(bufferConversion);
  r5Disk->super_block.nb_blocks_used=uChar_To_Int(bufferConversion);
  posread+=sizeof(int);

  for(int i = 0; i < (sizeof(int)); i++){
    bufferConversion[i]=buffer[posread+i];
  }
  superblock->first_free_byte = uChar_To_Int(bufferConversion);
  r5Disk->super_block.first_free_byte=uChar_To_Int(bufferConversion);
  free(buffer);
  free(bufferConversion);
}


/** \brief
  * Supression d'inode
  * @param : virtual_disk_t, num de l'inode
  * @return void
**/
void delete_inode(virtual_disk_t *r5Disk, int numInode){
  inode_t maTable[10];
  int i=numInode;
  read_inodes_table(r5Disk,maTable);
  if (get_nb_files(r5Disk) > numInode) {
    while (maTable[i].first_byte != 0 && i < INODE_TABLE_SIZE-1) {
        maTable[i] = maTable[i+1];
        i++;
    }
    if (i == INODE_TABLE_SIZE-1) {
        maTable[i] = init_inode("",0,0);
    }
    write_inodes_table(r5Disk, maTable);
  }
  update_super_block(r5Disk);
}

/** \brief
  * Retourne le nombre de fichiers dans la table d'inodes
  * @param : inode_table_t
  * @return Nombre de fichiers (int)
**/
int get_nb_files(virtual_disk_t *r5d){
  inode_t tab[10];
  read_inodes_table(r5d,tab);
  int nbfiles = 0;
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    if (tab[i].first_byte != 0) {
      nbfiles+=1;
    }
  }
  return nbfiles;
}

/** \brief
  * Retourne le nombre d'inodes inutilisés dans la table d'inodes
  * @param : inode_table_t
  * @return Nombre de fichiers (int)
**/
int get_unused_inodes(virtual_disk_t *r5d){
  inode_t tab[10];
  read_inodes_table(r5d,tab);
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    if (tab[i].first_byte == 0) {
      return i;
    }
  }
  return -1;
}


/** \brief
  * Ajout d'une inode
  * @param : virtual_disk_t* nom_de_fichier (char *),taille (uint),debut (uint)
  * @return void
**/
void add_inode(virtual_disk_t *r5Disk, char nomFICHIER[50], int size){
  inode_t maTable[10];
  read_inodes_table(r5Disk,maTable);
  if(get_nb_files(r5Disk) < INODE_TABLE_SIZE){
    inode_t in = init_inode(nomFICHIER,size, first_free_byte(r5Disk));
    maTable[get_unused_inodes(r5Disk)] = in;
    r5Disk->number_of_files+=1;
    write_inodes_table(r5Disk,maTable);
  }else{
    fprintf(stderr, "\033[31;49m%s non ecrit, nombre de fichier max atteint.\033[39;49m\n", nomFICHIER);
  }
}

/** \brief
  * Initialisation d'une inode
  * @param : nom_de_fichier (char *),taille (uint),debut (uint)
  * @return inode_t
**/
inode_t init_inode(char nomFichier[FILENAME_MAX_SIZE], uint taille, uint start){
  inode_t result;
  int i = 0;
  for (int i = 0; i < FILENAME_MAX_SIZE; i++) {
    result.filename[i] = 0;
  }
  while(i < FILENAME_MAX_SIZE && nomFichier[i] != '\0'){
    result.filename[i] = nomFichier[i];
    i++;
  }
  result.filename[i] = '\0';
  result.size = taille;
  result.nblock = compute_nblock(result.size);
  result.first_byte = start;
  return result;
}

void cmd_dump_inode(char *nomRep, virtual_disk_t *r5Disk){
  printf("Dump_inode\n");
  inode_table_t table;
  read_inodes_table(r5Disk, (inode_t*)table);
  r5Disk->number_of_files = get_nb_files(r5Disk);
  printf("Number of files : %d\n",r5Disk->number_of_files);
  for(int i = 0; i < (r5Disk->number_of_files); i++){
      printf("fichier n°%d :\n%s\n", i ,r5Disk->inodes[i].filename);
      printf("%d octets\n", r5Disk->inodes[i].size);
      printf("%d blocks\n", r5Disk->inodes[i].nblock);
      printf("commence a l'octet %d\n\n", r5Disk->inodes[i].first_byte);
  }
}


/** \briefiles()
  * Ecriture du superbloc
  * @param : virtual_disk_t
  * @return void
**/
void write_super_block(virtual_disk_t *r5Disk, super_block_t sb){
  inode_table_t inodes;
  int poswrite=0;
  uchar *buffer = (uchar *)malloc(sizeof(char)*sizeof(super_block_t));
  uchar *bufferConversion = (uchar *)malloc(sizeof(char)*sizeof(int));


  int_To_uChar(sb.raid_type, bufferConversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = bufferConversion[i];
  }
  poswrite += sizeof(enum raid);


  int_To_uChar(sb.nb_blocks_used, bufferConversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = bufferConversion[i];
  }
  poswrite += sizeof(int);


  int_To_uChar(sb.first_free_byte, bufferConversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = bufferConversion[i];
  }
  write_chunk(r5Disk, buffer, sizeof(super_block_t), 0);
  free(buffer);
  free(bufferConversion);
}

/** \brief
  * Ecriture de la position du premier octet libre dans le r5Disk
  * @param : virtual_disk_t
  * @return int
**/
int first_free_byte(virtual_disk_t *r5Disk){
  inode_t table[INODE_TABLE_SIZE];
  read_inodes_table(r5Disk, table);
  int lastfirstbyte=0, lastindice=0, i=0;
  if (table[0].first_byte == 0) {
    return compute_nstripe(r5Disk, compute_nblock(sizeof(inode_t)))*INODE_TABLE_SIZE+startTable(r5Disk);
  }else{
    int firstfree = get_unused_inodes(r5Disk);
    if (firstfree == -1) {
      return -1;
    }else{
      return table[firstfree-1].first_byte + compute_nstripe(r5Disk,table[firstfree-1].nblock );
    }
  }
}

/** \brief
  * Lecture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
  void read_inodes_table(virtual_disk_t *r5Disk, inode_t maTable[INODE_TABLE_SIZE]){
    inode_t Ino;
    for(int i = 0; i< INODE_TABLE_SIZE; i++){
      read_inode_table_i(r5Disk, &Ino, i);
      for(int j = 0; j < FILENAME_MAX_SIZE; j++){
        maTable[i].filename[j] = Ino.filename[j];
      }
      maTable[i].first_byte = Ino.first_byte;
      maTable[i].size = Ino.size;
      maTable[i].nblock = Ino.nblock;
      r5Disk->inodes[i]=maTable[i];
    }
  }


/** \brief
  * Lecture de l'inode indice
  * @param : virtual_disk_t* ,inode_t*, indice
  * @return : void
**/
void read_inode_table_i(virtual_disk_t *r5Disk, inode_t *Ino, int indice){
  int depart = startTable(r5Disk), posread = 0, k;
  uchar *buffer = (uchar *)malloc(sizeof(char)*sizeof(inode_t));
  uchar *conversion = (uchar *)malloc(sizeof(char)*sizeof(int));
  read_chunk(r5Disk, buffer, INODE_SIZE*BLOCK_SIZE, depart+(indice*compute_nstripe(r5Disk, INODE_SIZE)));
  for(k = 0; k<FILENAME_MAX_SIZE; k++){
    Ino->filename[k] = buffer[k];
  }
  posread+=FILENAME_MAX_SIZE;


  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->size = uChar_To_Int(conversion);
  posread+= sizeof(int);


  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->nblock = uChar_To_Int(conversion);
  posread+= sizeof(int);

  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->first_byte = uChar_To_Int(conversion);
  free(buffer);
  free(conversion);
}

/** \brief
  * Ecriture de l'inode indice
  * @param : virtual_disk_t ,inode_t, indice
  * @return : void
**/
void write_inode_table_i(virtual_disk_t *r5Disk, inode_t Ino, int indice){
  int depart = startTable(r5Disk), poswrite = 0;
  uchar *buffer = (uchar *)malloc(sizeof(char)*sizeof(inode_t));
  uchar *conversion = (uchar *)malloc(sizeof(char)*sizeof(int));
  for (int i = 0; i < FILENAME_MAX_SIZE; i++) {
    buffer[i]= Ino.filename[i];
  }
  poswrite+=FILENAME_MAX_SIZE;


  int_To_uChar(Ino.size,conversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = conversion[i];
  }
  poswrite+=sizeof(int);


  int_To_uChar(Ino.nblock, conversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = conversion[i];
  }
  poswrite+=sizeof(int);


  int_To_uChar(Ino.first_byte, conversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = conversion[i];
  }
  write_chunk(r5Disk,buffer, INODE_SIZE*BLOCK_SIZE, depart+(indice*compute_nstripe(r5Disk, INODE_SIZE)));
  free(buffer);
  free(conversion);
}

/** \brief
  * Ecriture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
void write_inodes_table(virtual_disk_t *r5Disk, inode_t maTable[INODE_TABLE_SIZE]){
  inode_t Ino;
  for(int i = 0; i < INODE_TABLE_SIZE; i++){
    for(int j = 0; j < FILENAME_MAX_SIZE; j++){
      Ino.filename[j] = maTable[i].filename[j];
    }
    Ino.first_byte = maTable[i].first_byte;
    Ino.size = maTable[i].size;
    Ino.nblock = maTable[i].nblock;
    write_inode_table_i(r5Disk, Ino, i);
  }
}
/*briefiles
* Fonction de mise à jour du Superbloc
* @param : virtual_disk_t*
* @return : void
*/
void update_super_block(virtual_disk_t* r5Disk){
  inode_t table[INODE_TABLE_SIZE];
  super_block_t sb;
  int i=0;
  sb.raid_type = r5Disk->super_block.raid_type =  r5Disk->raidmode;
  sb.nb_blocks_used = r5Disk->super_block.nb_blocks_used =  0;
  read_inodes_table(r5Disk,table);
  while (i < INODE_TABLE_SIZE && table[i].first_byte != 0) {
    sb.nb_blocks_used += table[i].nblock;
    i++;
  }
  r5Disk->super_block.nb_blocks_used = sb.nb_blocks_used;
  sb.first_free_byte = r5Disk->super_block.first_free_byte = first_free_byte(r5Disk);
  write_super_block(r5Disk, sb);
}

/*brief
* Fonction de remise à zero du Systeme
* @param
* @return : void
*/
void reinit_systeme(virtual_disk_t* r5Disk){
  inode_t maTable[10];
  read_inodes_table(r5Disk, maTable);
  stripe_t *bande = init_bande(r5Disk);
  for(int i = 0; i < r5Disk->ndisk; i++){
    for(int j = 0; j < BLOCK_SIZE; j++){
      bande->stripe[i].data[j] = 0;
    }
  }
  for(int i = 0; i < compute_nstripe(r5Disk,INODE_SIZE)*INODE_TABLE_SIZE+compute_nstripe(r5Disk, SUPER_BLOCK_SIZE-1); i++){
    write_stripe(r5Disk, bande, i);
  }
  delete_bande(&bande);
}
