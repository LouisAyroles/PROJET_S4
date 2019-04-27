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
   * Lecture du superbloc
   * @param : virtual_disk_t , super_block_t
   * @return void
 **/
void read_super_block(virtual_disk_t *r5Disk, super_block_t *superblock){
  printf("read_super_block\n");
  int nbBandes = SUPER_BLOCK_SIZE;
  stripe_t *bandesLues;
  char conversion[BLOCK_SIZE];
  bandesLues = (stripe_t *) malloc(sizeof(stripe_t)*nbBandes);
  for(int j = 0; j<nbBandes; j++){
    read_stripe(r5Disk, &bandesLues[j], j);
  }

  for(int i = 0; i < sizeof(enum raid); i++){
    conversion[i%BLOCK_SIZE] = bandesLues[i/(BLOCK_SIZE*r5Disk->ndisk)].stripe[(i/BLOCK_SIZE)%r5Disk->ndisk].data[i%BLOCK_SIZE];
  }
  memcpy(&(superblock->raid_type), conversion, sizeof(superblock->raid_type));

  for(int i = sizeof(enum raid); i < sizeof(enum raid)+sizeof(int); i++){
    conversion[i%BLOCK_SIZE] = bandesLues[i/(BLOCK_SIZE*r5Disk->ndisk)].stripe[(i/BLOCK_SIZE)%r5Disk->ndisk].data[i%BLOCK_SIZE];
  }
  memcpy(&(superblock->nb_blocks_used), conversion, sizeof(superblock->nb_blocks_used));

  for(int i = sizeof(enum raid)+sizeof(int); i < sizeof(enum raid)+(sizeof(int)*2); i++){
    conversion[i%BLOCK_SIZE] = bandesLues[i/(BLOCK_SIZE*r5Disk->ndisk)].stripe[(i/BLOCK_SIZE)%r5Disk->ndisk].data[i%BLOCK_SIZE];
  }
  memcpy(&(superblock->first_free_byte), conversion, sizeof(superblock->first_free_byte));
}

int startTable(virtual_disk_t *r5Disk){
  if(r5Disk->ndisk >=4){
    return 1;
  }
  return 2;
}

/** \brief
  * Lecture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
void read_inodes_table(virtual_disk_t *r5Disk, inode_table_t *table){
  printf("read_inodes_table\n");
  int nbBandes = compute_nstripe(r5Disk, INODE_SIZE), k, debutTable = startTable(r5Disk);
  char conversion[4];
  char *bufferInode;

  for (int i = 0; i<INODE_TABLE_SIZE; i++){
    /* LECTURE DES BANDES CONSTITUANTS L'INODE */
    printf("Indice de lecture : %d, nbBandes = %d\n", debutTable+i*nbBandes, nbBandes);
    bufferInode=read_chunk(r5Disk, debutTable+i*nbBandes, INODE_OCT);
    printf("On est passe !!\n");
    for(k = 0; k < FILENAME_MAX_SIZE; k++){ //k allant de 0 à (2bandes/octets)-1
      table[i]->filename[k] = bufferInode[k];
    }
    for (k = FILENAME_MAX_SIZE; k < FILENAME_MAX_SIZE+sizeof(int); k++){
      conversion[k%BLOCK_SIZE] = bufferInode[k];
    }
    memcpy(&(table[i]->size), conversion, sizeof(table[i]->size));


    for (k = FILENAME_MAX_SIZE+sizeof(int); k < FILENAME_MAX_SIZE+sizeof(int)+sizeof(int); k++){
      conversion[k%BLOCK_SIZE] = bufferInode[k];
    }
    memcpy(&(table[i]->nblock), conversion, sizeof(table[i]->nblock));

    //Recup du first_byte(conversion int)
    for (k = FILENAME_MAX_SIZE+sizeof(int)*2; k < FILENAME_MAX_SIZE+sizeof(int)*3; k++){
      conversion[k%BLOCK_SIZE] = bufferInode[k];
    }
    memcpy(&(table[i]->first_byte), conversion, sizeof(table[i]->first_byte));
    printf("memucypyOUT\n");
  }
}

/** \brief
  * Ecriture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
void write_inodes_table(virtual_disk_t *r5Disk, inode_table_t inode){
  printf("write_inodes_table\n");
  char *buffer = (char *)malloc(sizeof(inode_t)*INODE_TABLE_SIZE);
  char temp[4];
  int noBande,size, nblock, first_byte, poswrite;
  if(r5Disk->ndisk < 3){         /*sur quelle bande commence la table d'inodes*/
    noBande = 2;
  }else{
    noBande = 1;
  }
  /*construction du buffer*/

  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    /*ecriture du filename*/
    poswrite = i*sizeof(inode_t);
    for (int j = 0; j < FILENAME_MAX_SIZE; j++) {
        buffer[poswrite+j]=inode[i].filename[j];
    }

    /*ecriture de la size*/
    poswrite += FILENAME_MAX_SIZE;
    buffer[poswrite+3] = (inode[i].size>>24) & 0xFF;
    buffer[poswrite+2] = (inode[i].size>>16) & 0xFF;
    buffer[poswrite+1] = (inode[i].size>>8) & 0xFF;
    buffer[poswrite] = inode[i].size & 0xFF;

    /*ecriture du nblock*/
    poswrite+=4;
    buffer[poswrite+3] = (inode[i].nblock>>24) & 0xFF;
    buffer[poswrite+2] = (inode[i].nblock>>16) & 0xFF;
    buffer[poswrite+1] = (inode[i].nblock>>8) & 0xFF;
    buffer[poswrite] = inode[i].nblock & 0xFF;

    /*ecriture du first_byte*/
    poswrite+=4;
    buffer[poswrite+3] = (inode[i].first_byte>>24) & 0xFF;
    buffer[poswrite+2] = (inode[i].first_byte>>16) & 0xFF;
    buffer[poswrite+1] = (inode[i].first_byte>>8) & 0xFF;
    buffer[poswrite] = inode[i].first_byte & 0xFF;

  }
  write_chunk(r5Disk, buffer, sizeof(inode_t)*INODE_TABLE_SIZE, noBande);
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
  int i = get_unused_inodes(tab);
  if (i == -1){
    return 10;
  }
  return i;
}

/** \brief
  * Retourne le nombre d'inodes inutilisés dans la table d'inodes
  * @param : inode_table_t
  * @return Nombre de fichiers (int)
**/
int get_unused_inodes(inode_table_t tab){
  printf("get_unused_inodes\n");
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    if (tab[i].first_byte == 0) {
      return i;
    }
  }
  return -1;
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
void write_super_block(virtual_disk_t *r5Disk){
  printf("write_super_block\n");
  inode_table_t inodes;
  int lastfirstbyte=0;
  int lastindice=0;
  int poswrite=0;
  char *buffer = (char *)malloc(sizeof(char)*sizeof(super_block_t));

  /*Init du superblock*/
  printf("ici2, R5DISK = %d\n\n\n\n\n\n______________\n", r5Disk->ndisk);
  read_inodes_table(r5Disk, &inodes);
  r5Disk->super_block.nb_blocks_used=0;
  first_free_byte(r5Disk);
  r5Disk->super_block.raid_type = r5Disk->raidmode;
  printf("Ouallez !! %d %d\n", r5Disk->super_block.first_free_byte, r5Disk->inodes[lastindice].size);
  /*Transformation des int en char pour ecriture sur le disque*/

    /*ecriture de l'enum*/

    buffer[0] = ((char*)&(r5Disk->super_block.raid_type))[0];
    buffer[1] = ((char*)&(r5Disk->super_block.raid_type))[1];
    buffer[2] = ((char*)&(r5Disk->super_block.raid_type))[2];
    buffer[3] = ((char*)&(r5Disk->super_block.raid_type))[3];


    buffer[4] = ((char*)&(r5Disk->super_block.nb_blocks_used))[0];
    buffer[5] = ((char*)&(r5Disk->super_block.nb_blocks_used))[1];
    buffer[6] = ((char*)&(r5Disk->super_block.nb_blocks_used))[2];
    buffer[7] = ((char*)&(r5Disk->super_block.nb_blocks_used))[3];


    buffer[8] = ((char*)&(r5Disk->super_block.first_free_byte))[0];
    buffer[9] = ((char*)&(r5Disk->super_block.first_free_byte))[1];
    buffer[10] = ((char*)&(r5Disk->super_block.first_free_byte))[2];
    buffer[11] = ((char*)&(r5Disk->super_block.first_free_byte))[3];


      /*Ecriture du Superblock*/
    write_chunk(r5Disk, buffer, sizeof(super_block_t), 0);
    printf("ici\n");
}

/** \brief
  * Ecriture de la position du premier octet libre dans le r5Disk
  * @param : virtual_disk_t
  * @return void
**/
void first_free_byte(virtual_disk_t *r5Disk){
  printf("first_free_byte\n");
  int lastfirstbyte=0, lastindice=0;
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    lastfirstbyte += compute_nstripe(r5Disk, INODE_SIZE);
  }
  lastfirstbyte += startTable(r5Disk) +1;
  for (int i = 0; i < get_unused_inodes(r5Disk->inodes); i++) {
    if (r5Disk->inodes[i].first_byte > lastfirstbyte) {
        lastfirstbyte = r5Disk->inodes[i].first_byte;
        lastindice=i;
    }
  }
  r5Disk->super_block.first_free_byte = lastfirstbyte + compute_nstripe(r5Disk, (r5Disk->inodes[lastindice].size/4));
  printf("ici3, R5DISK = %d\n\n\n\n\n\n______________\n", r5Disk->ndisk);
}




void couche3() {
  //couche2();
  virtual_disk_t *r5d=malloc(sizeof(virtual_disk_t));
  inode_table_t inodes;
  for(int i = 0; i<INODE_TABLE_SIZE; i++){
    inodes[i].first_byte = 0;
  }
  init_disk_raid5("./RAIDFILES",r5d);
  write_inodes_table(r5d, inodes);
  printf("\nCMD_DUMP_INODE\n");
  cmd_dump_inode("RAIDFILES", r5d);
}
