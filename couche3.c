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
 uchar *int_To_uChar(int nb, uchar *buffer){
   for (int i = 0; i < sizeof(int); i++) {
     buffer[i] = ((uchar*)&nb)[i];
   }
   return buffer;
 }

 /** \brief
   * Transforme un tableau de sz char en un int
   * @param : int ,uchar*
   * @return : int
 **/
 int uChar_To_Int(uchar *buf,int sz){
   int nb = 0;
   for (int i=0;i<sz;i++){
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
  superblock->raid_type = uChar_To_Int(bufferConversion,sizeof(enum raid));

  posread+=sizeof(enum raid);
  for(int i = 0; i < sizeof(int); i++){
    bufferConversion[i]=buffer[posread+i];
  }
  superblock->nb_blocks_used = uChar_To_Int(bufferConversion,sizeof(int));

  posread+=sizeof(int);
  for(int i = 0; i < (sizeof(int)); i++){
    bufferConversion[i]=buffer[posread+i];
  }
  superblock->first_free_byte = uChar_To_Int(bufferConversion,sizeof(int));
}




/** \brief
  * Lecture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
  void read_inodes_table(virtual_disk_t *r5Disk, inode_table_t *table){
    printf("read_inodes_table\n");
    int nbBandes = compute_nstripe(r5Disk, INODE_SIZE),k, debutTable = startTable(r5Disk);
    int posread=0;
    //char *bufferConversion = malloc(sizeof(char)*sizeof(int));
    //char *bufferInode = malloc(sizeof(char)*(sizeof(inode_t)*INODE_TABLE_SIZE*BLOCK_SIZE*r5Disk->ndisk));
    char bufferConversion[sizeof(char)*sizeof(int)];
    char bufferInode[INODE_OCT];


    for (int i = 0; i<INODE_TABLE_SIZE; i++){
      read_chunk(r5Disk, bufferInode, sizeof(inode_t), debutTable+(i*compute_nstripe(r5Disk, compute_nblock(sizeof(inode_t)))));
      /* LECTURE DES BANDES CONSTITUANTS L'INODE */
      //posread = i*(INODE_OCT +sizeof(int));
      posread = 0;
      for(k = 0; k < FILENAME_MAX_SIZE; k++){ //k allant de 0 à (2bandes/octets)-1
        table[i]->filename[k] = bufferInode[k];
      }
      posread+=FILENAME_MAX_SIZE;

      for (k = posread; k < posread+sizeof(int); k++){
        bufferConversion[k-posread] = bufferInode[k];
      }

      table[i]->size = uChar_To_Int(bufferConversion, sizeof(int));
      posread+= sizeof(int);
      for (k = posread; k < posread+sizeof(int); k++){
        bufferConversion[k-posread] = bufferInode[k];
      }
      table[i]->nblock = uChar_To_Int(bufferConversion, sizeof(int));
      posread+= sizeof(int);


      for (k = posread; k < posread+sizeof(int); k++){
        bufferConversion[k-posread] = bufferInode[k];
      }
      table[i]->first_byte = uChar_To_Int(bufferConversion, BLOCK_SIZE);
      posread+=sizeof(int);
    }

    for(int z=0;z<INODE_TABLE_SIZE;z++){
      r5Disk->inodes[z]=*(table[z]);
    }
    //free(bufferConversion);free(bufferInode);
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
  inode_table_t *table=malloc(sizeof(inode_table_t));
  read_inodes_table(r5Disk, table);
  r5Disk->number_of_files = get_nb_files(*table);
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
  inode_table_t *inodes=malloc(sizeof(inode_t)*(INODE_TABLE_SIZE+1));
  super_block_t sb;
  read_inodes_table(r5Disk, inodes);
  read_super_block(r5Disk,&sb);
  int lastfirstbyte=0, lastindice=0;
  printf("Affichage dans FFB\n");
  for(int i = 0; i<INODE_TABLE_SIZE; i++){
    printf("first_byte de l'inode n°%d= %d\n",i,inodes[i]->first_byte);
  }
  int nbfichiers = get_nb_files(*inodes);
  printf("nbfichiers : %d\n",nbfichiers );
  if (nbfichiers == 0) {
    printf("No files\n");
    sb.first_free_byte = compute_nstripe(r5Disk, compute_nblock(sizeof(inode_t)))*INODE_TABLE_SIZE+startTable(r5Disk)+1;
  }else{
    for (int i = 0; i < nbfichiers; i++) {
      if (r5Disk->inodes[i].first_byte > lastfirstbyte) {
          lastfirstbyte = r5Disk->inodes[i].first_byte;
          lastindice=i;
      }
    }
    printf("lastfirstbyte : %d\n",lastindice );
    sb.first_free_byte = lastfirstbyte + compute_nstripe(r5Disk, (r5Disk->inodes[lastindice].nblock));
  }
  write_super_block(r5Disk,sb);
}


int main(int argc, char const *argv[]) {
  //couche2();
  printf("todo : read_inodes_table\n");
  virtual_disk_t *r5d=malloc(sizeof(virtual_disk_t));
  init_disk_raid5("./RAIDFILES",r5d);
  inode_table_t inodes;
  inode_table_t *table = (inode_table_t*)malloc(sizeof(inode_table_t));

  for(int i = 0; i<INODE_TABLE_SIZE; i++){
    inodes[i].first_byte = 64;
              printf("%d\t\n",inodes[i].first_byte);
    inodes[i].size = 16;
    inodes[i].nblock = 4;
    for (int j = 0; j < FILENAME_MAX_SIZE-1; j++) {
      inodes[i].filename[j] = 49;
    }
    inodes[i].filename[FILENAME_MAX_SIZE-1] = '\0';
  }
  inodes[4].first_byte = 0;
  inodes[4].size = 32;
  inodes[4].nblock = 8;

  printf("Affichage avant ecriture\n");
  for(int i = 0; i<INODE_TABLE_SIZE; i++){
      printf("first_byte de l'inode n°%d= %d\n",i,inodes[i].first_byte);
  }
  for(int i = 0; i<INODE_TABLE_SIZE; i++){
      table[i]->first_byte = 3;
  }

    write_inodes_table(r5d, inodes);
    read_inodes_table(r5d, table);

printf("Affichage apres lecture\n");
    for(int i = 0; i<INODE_TABLE_SIZE; i++){
          printf("first_byte de l'inode n°%d= %d\n",i,table[i]->first_byte);
    }

  super_block_t sb, sbd;
  sb.raid_type = r5d->raidmode;
  sb.nb_blocks_used = 0;
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    sb.nb_blocks_used += inodes[i].nblock;
  }
  write_super_block(r5d, sb);



  affichageSysteme(r5d);
  first_free_byte(r5d);
  affichageSysteme(r5d);
  read_super_block(r5d,&sbd);
  printf("Affichage du SB qu'on vient de lire :\n");
  printf("Raid type : %d\t NbBlocksUsed : %d\t FirstFreeByte : %d\n",sbd.raid_type, sbd.nb_blocks_used, sbd.first_free_byte);
  turn_off_disk_raid5(r5d);

  printf("\ntaille Inode en octets %d\n", sizeof(inode_t));
  return 0;
}
