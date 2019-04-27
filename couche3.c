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
  * Lecture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
void read_inodes_table(virtual_disk_t *r5Disk, inode_table_t *table){
  printf("read_inodes_table\n");
  int nbBandes = compute_nstripe(r5Disk, INODE_SIZE), k, debutTable = startTable(r5Disk);
  char conversion[4];
  char *bufferInode = (char*)malloc(sizeof(char)*sizeof(inode_table_t));

  for (int i = 0; i<INODE_TABLE_SIZE; i++){
    /* LECTURE DES BANDES CONSTITUANTS L'INODE */
    printf("Indice de lecture : %d, nbBandes = %d\n", debutTable+i*nbBandes, nbBandes);
    read_chunk(r5Disk, bufferInode, INODE_OCT,  debutTable+i*nbBandes);
    printf("Affichage du Buffer de l'inode %d : \n",i);
    for(int z = 0; z < INODE_OCT; z++){
      printf("%d ", bufferInode[z]);
    }
    printf("\n");
    for(k = 0; k < FILENAME_MAX_SIZE; k++){ //k allant de 0 à (2bandes/octets)-1
      table[i]->filename[k] = bufferInode[k];
    }

    for (k = FILENAME_MAX_SIZE; k < FILENAME_MAX_SIZE+sizeof(int); k++){
      conversion[k%BLOCK_SIZE] = bufferInode[k];
    }
    table[i]->size = uChar_To_Int(conversion, BLOCK_SIZE);


    for (k = FILENAME_MAX_SIZE+sizeof(int); k < FILENAME_MAX_SIZE+sizeof(int)+sizeof(int); k++){
      conversion[k%BLOCK_SIZE] = bufferInode[k];
    }
    table[i]->nblock = uChar_To_Int(conversion, BLOCK_SIZE);

    //Recup du first_byte(conversion int)
    for (k = FILENAME_MAX_SIZE+sizeof(int)*2; k < FILENAME_MAX_SIZE+sizeof(int)*3; k++){
      conversion[k%BLOCK_SIZE] = bufferInode[k];
    }
    table[i]->first_byte = uChar_To_Int(conversion, BLOCK_SIZE);
    printf("memucypyOUT\n");
  }
  printf("On Sort.\n");
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



    /*ecriture du nblock*/
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
  //write_chunk(r5Disk, buffer, sizeof(inode_t)*INODE_TABLE_SIZE, noBande);
}



int main(int argc, char const *argv[]) {
  //couche2();
  virtual_disk_t *r5d=malloc(sizeof(virtual_disk_t));
  init_disk_raid5("./RAIDFILES",r5d);
  inode_table_t inodes;
  for(int i = 0; i<INODE_TABLE_SIZE; i++){
    inodes[i].first_byte = 1;
    inodes[i].size = 2;
    inodes[i].nblock = 3;
    for (int j = 0; j < FILENAME_MAX_SIZE-1; j++) {
      inodes[i].filename[j] = 49;
    }
    inodes[i].filename[FILENAME_MAX_SIZE-1] = '\0';
  }

  write_inodes_table(r5d, inodes);
  affichageSysteme(r5d);
  printf("Avant : %d\n", inodes[0].first_byte);
  //read_inodes_table(r5d, &inodes);
  printf("Apres : %d\n", inodes[0].first_byte);
  /*for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    printf("\ninode n°%d\n",i);
    printf("first_byte : %d  ",inodes[i].first_byte );
    printf("size : %d  ", inodes[i].size);
    printf("nblock : %d  ", inodes[i].nblock);
  }*/
  return 0;
}
