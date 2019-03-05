#include "couche3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inode_t read_inode(virtual_disk_t *r5Disk, uint pos){

}

inode_table_t read_inodes_table(virtual_disk_t *r5Disk){
  stripe_t *bandesLues;
  inode_table_t tableI;
  int nbBandes = compute_nstripe(r5Disk, INODE_SIZE);
  bandesLues = (stripe_t*)malloc(sizeof(stripe_t)*nbBandes);

  /**
   * Inutiles ?
   *for(int i = 0; i < r5Disk -> ndisk; i++){
   * fseek(r5Disk->storage[i], INODES_START, SEEK_SET);
   *}
   **/
   for (int i = 0; i<nbBandes; i++){
  /*Retour de fonction ?*/read_stripe(r5, &bandesLues[i], INODES_START+(BLOCK_SIZE*i));
  // Position en bande ou en octets ?
  }

  free(bandesLues);
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
