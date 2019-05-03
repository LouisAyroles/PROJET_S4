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
  printf("read_super_block\n");
  int noBande = compute_nstripe(r5Disk, SUPER_BLOCK_SIZE-1), posread = 0;
  stripe_t *bandesLues;
  char *buffer = (char *)malloc(sizeof(char)*sizeof(super_block_t));
  char *bufferConversion = (char *)malloc(sizeof(char)*sizeof(int));

  for(int j = 0; j<noBande; j++){
    read_chunk(r5Disk, buffer, sizeof(super_block_t), j);
  }

  for(int i = 0; i < sizeof(enum raid); i++){
    bufferConversion[i]=buffer[i];
  }
  superblock->raid_type = uChar_To_Int(bufferConversion);

  posread+=sizeof(enum raid);
  for(int i = 0; i < sizeof(int); i++){
    bufferConversion[i]=buffer[posread+i];
  }
  superblock->nb_blocks_used = uChar_To_Int(bufferConversion);

  posread+=sizeof(int);
  for(int i = 0; i < (sizeof(int)); i++){
    bufferConversion[i]=buffer[posread+i];
  }
  superblock->first_free_byte = uChar_To_Int(bufferConversion);
  free(buffer);
  free(bufferConversion);
}


/** \brief
  * Supression d'inode
  * @param : virtual_disk_t
  * @return void
**/
void delete_inode(virtual_disk_t *r5Disk, int numInode){
  printf("delete_inode\n");
  if(numInode <= r5Disk->number_of_files){
    for (int i = numInode+1; i < (r5Disk->number_of_files); i++) {
        r5Disk->inodes[i-1]=r5Disk->inodes[i];
    }
    r5Disk->inodes[r5Disk->number_of_files-1].first_byte=0;
    r5Disk->number_of_files = r5Disk->number_of_files-1;
    write_inodes_table(r5Disk,r5Disk->inodes);
  }
}

/** \brief
  * Retourne le nombre de fichiers dans la table d'inodes
  * @param : inode_table_t
  * @return Nombre de fichiers (int)
**/
int get_nb_files(inode_table_t tab){
  printf("get_nb_files\n");
  int nbfiles = 0;
  printf("Affichage dans get_nb_files\n");
  for(int i = 0; i<INODE_TABLE_SIZE; i++){
      printf("first_byte de l'inode n°%d= %d\n",i,tab[i].first_byte);
  }
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    if (tab[i].first_byte != 0) {
      printf("first_byte = %d",tab[i].first_byte);
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
int get_unused_inodes(inode_table_t tab){
  printf("get_unused_inodes\n");
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    printf("i = %d\n", i);
    if (tab[i].first_byte == 0) {
      printf("On sort ici! \n");
      return i;
    }
  }
  printf("On sort !\n");
  return -1;
}

void add_inode(virtual_disk_t *r5Disk, char nomFICHIER[50], int size, int startBande){
  printf("add_inode\n");
  if(get_nb_files(r5Disk->inodes) != 10){
    inode_t in = init_inode(nomFICHIER,size,startBande);
    r5Disk->inodes[get_unused_inodes(r5Disk->inodes)] = in;
    r5Disk->number_of_files+=1;
  }else{
    fprintf(stderr, "%s non ecrit, nb de fichier max atteint.\n", nomFICHIER);
  }
}

/** \brief
  * Initialisation d'une inode
  * @param : nom_de_fichier (char *),taille (uint),debut (uint)
  * @return inode_t
**/
inode_t init_inode(char nomFichier[FILENAME_MAX_SIZE], uint taille, uint start){
  printf("init_inode\n");
  inode_t result;
  strcpy(result.filename, nomFichier); // ==32octets
  result.size = taille; // == 4octets
  result.nblock = compute_nblock(result.size); // == 4octets
  result.first_byte = start; // == 4 octets
  return result;
}
// 1 block = 4 octets  -->  un Inode = (32+4*3)/4 blocks
// ---> Inode = 11 blocks

void cmd_dump_inode(char *nomRep, virtual_disk_t *r5Disk){
  printf("Dump_inode\n");
  inode_table_t table;
  // read_inodes_table(r5Disk, &table);
  r5Disk->number_of_files = get_nb_files(table);
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
  printf("write_super_block\n");
  inode_table_t inodes;
  int poswrite=0;
  //char *buffer = (char *)malloc(sizeof(char)*sizeof(super_block_t));
  //char *bufferConversion = (char *)malloc(sizeof(char)*sizeof(int));
  char buffer[sizeof(char)*sizeof(super_block_t)];
  char bufferConversion[sizeof(char)*sizeof(int)];


  int_To_uChar(sb.raid_type, bufferConversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = bufferConversion[i];
  }

  poswrite += 4;
  int_To_uChar(sb.nb_blocks_used, bufferConversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = bufferConversion[i];
  }

  /*Ecriture du First free Byte*/
  poswrite += 4;
  int_To_uChar(sb.first_free_byte, bufferConversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = bufferConversion[i];
  }

    write_chunk(r5Disk, buffer, sizeof(super_block_t), 0);
    //free(buffer);free(bufferConversion);
}

/** \brief
  * Ecriture de la position du premier octet libre dans le r5Disk
  * @param : virtual_disk_t
  * @return void
**/
void first_free_byte(virtual_disk_t *r5Disk){
  inode_table_t inodes;
  super_block_t sb;
  // read_inodes_table(r5Disk, &inodes);
  read_super_block(r5Disk,&sb);
  int lastfirstbyte=0, lastindice=0, i=0;
  if (inodes[0].first_byte == 0) {
    printf("No files\n");
    sb.first_free_byte = compute_nstripe(r5Disk, compute_nblock(sizeof(inode_t)))*INODE_TABLE_SIZE+startTable(r5Disk)+1;
  }else{
    while(inodes[i].first_byte != 0) {
      if (inodes[i].first_byte > lastfirstbyte) {
          lastfirstbyte = inodes[i].first_byte;
          lastindice=i;
      }
      i++;
    }
    printf("lastfirstbyte : %d\n",lastindice );
    sb.first_free_byte = lastfirstbyte + compute_nstripe(r5Disk, (r5Disk->inodes[lastindice].nblock));
  }
  write_super_block(r5Disk,sb);
}

/** \brief
  * Lecture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
  void read_inodes_table(virtual_disk_t *r5Disk, inode_t maTable[INODE_TABLE_SIZE]){
    inode_t Ino;
    for(int i = 0; i< 10; i++){
      read_inode_table_i(r5Disk, &Ino, i);
      for(int j = 0; j < FILENAME_MAX_SIZE; j++){
        maTable[i].filename[j] = Ino.filename[j];
      }
      maTable[i].first_byte = Ino.first_byte;
      maTable[i].size = Ino.size;
      maTable[i].nblock = Ino.nblock;
    }
    printf("\n\n__%d__\n\n", maTable[3].first_byte);
  }

void read_inode_table_i(virtual_disk_t *r5Disk, inode_t *Ino, int indice){
  int depart = startTable(r5Disk), posread = 0, k;
  uchar buffer[50];
  uchar conversion[4];
  read_chunk(r5Disk, buffer, 44, depart+(indice*4));
  for(k = 0; k<FILENAME_MAX_SIZE; k++){
    Ino->filename[k] = buffer[k];
  }
  printf("\n");
  posread+=FILENAME_MAX_SIZE;
  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->size = uChar_To_Int(conversion);
  printf("--%d--\n", uChar_To_Int(conversion));

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
}

/** \brief
  * Ecriture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
void write_inodes_table(virtual_disk_t *r5Disk, inode_table_t inode){
  printf("write_inodes_table\n");
  char *buffer = (char *)malloc(sizeof(inode_t)*INODE_TABLE_SIZE);
  char *bufferConversion = (char *)malloc(sizeof(char)*4);
  int noBande,size, nblock, first_byte, poswrite;
  noBande = startTable(r5Disk);
  /*construction du buffer*/

  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    /*ecriture du filename*/
    poswrite = i*sizeof(inode_t);
    for (int j = 0; j < FILENAME_MAX_SIZE; j++) {
        buffer[poswrite+j]=inode[i].filename[j];
    }

    /*ecriture de la size*/
    poswrite += FILENAME_MAX_SIZE;
    int_To_uChar(inode[i].size,bufferConversion);
    for (int j = 0; j < 4; j++) {
      buffer[poswrite+j] = bufferConversion[j];
    }

    /*ecriture du nblock */
    poswrite+=4;
    int_To_uChar(inode[i].nblock,bufferConversion);
    for (int j = 0; j < 4; j++) {
      buffer[poswrite+j] = bufferConversion[j];
    }

    /*ecriture du first_byte*/
    poswrite+=4;
    int_To_uChar(inode[i].first_byte,bufferConversion);
    for (int j = 0; j < 4; j++) {
      buffer[poswrite+j] = bufferConversion[j];
    }
    write_chunk(r5Disk, buffer, sizeof(inode_t), noBande);
    noBande+=compute_nstripe(r5Disk ,compute_nblock(sizeof(inode_t)));
  }
  for(int i=0;i<INODE_TABLE_SIZE-1;i++){
    r5Disk->inodes[i]=inode[i];
  }
}

int main(int argc, char const *argv[]) {
  //couche2();
  printf("todo : read_inodes_table\n");
  virtual_disk_t *r5d=init_disk_raid5("./RAIDFILES");
  stripe_t *bande = init_bande(r5d);
  inode_t table[10];
  inode_t Ino;
  for(int i = 0; i < r5d->ndisk; i++){
    for(int j = 0; j < BLOCK_SIZE; j++){
      if(j == 0){
        bande->stripe[i].data[j] = 2;
      }else{
        bande->stripe[i].data[j] = 0;
      }
    }
  }
  for(int i = 0; i < r5d->ndisk; i++){
    printf("[ ");
    for(int j = 0; j < BLOCK_SIZE; j++){
      printf("%d, ", bande->stripe[i].data[j]);
    }
    printf("]\n");
  }
  for(int i = 0; i < 42; i++){
    write_stripe(r5d, bande, i);
  }
  read_inodes_table(r5d, table);
  printf("\ntest : %d--\n", table[3].first_byte);
  // for(int i = 0; i< 10; i++){
  //   read_inode_table_i(r5d, &Ino, 0);
  // }
  delete_bande(&bande);

  turn_off_disk_raid5(r5d);
  return 0;
}
