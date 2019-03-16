#include "couche3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void read_inodes_table(virtual_disk_t *r5Disk, inode_table_t *table){
  stripe_t *bandesLues;
  int nbBandes = compute_nstripe(r5Disk, INODE_SIZE);
  bandesLues = (stripe_t *) malloc(sizeof(stripe_t)*nbBandes);
  for(int i = 0; i<nbBandes; i++){
    &bandesLues[i] = init_bande();
  }
  for (int i = 0; i<INODE_TABLE_SIZE; i++){
    //Faire l'inode à chaque tour de i
    for (int j = 0; j<nbBandes; j++){
      //BLOCK_SIZE*i*nbBandes*r5->ndisk = taille d'un inode sur disque * i
      //BLOCK_SIZE*j*r5Disk->ndisk = j * taille d'une bande
      read_stripe(r5Disk, &bandesLues[j], INODES_START+(BLOCK_SIZE*j*r5Disk->ndisk)+(BLOCK_SIZE*i*nbBandes*r5Disk->ndisk));
    }
    for(int k = 0; k<BLOCK_SIZE*r5Disk->ndisk; k++){
      table[i]->filename[k] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[k/BLOCK_SIZE]->data[k%BLOCK_SIZE];
    }
  }
}


void write_inodes_table();
void delete_inodes_table();
void get_unused_inodes();

inode_t init_inode(char nomFichier[FILENAME_MAX_SIZE], uint taille, uint start){
  inode_t result;
  strcpy(result.filename, nomFichier); // ==32octets
  result.size = taille; // == 4octets
  result.nblock = compute_nblock(result.size); // == 4octets
  result.first_byte = start; // == 4 octets
  return result;
}
// 1 block = 4 octets  -->  un Inode = (32+4*3)/4 blocks
// ---> Inode = 11 blocks

void cmd_dump_inode();
void write_super_block();
void read_super_block();
void first_free_byte();
