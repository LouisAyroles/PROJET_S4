#include "couche3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct inode_s{
  char filename[FILENAME_MAX_SIZE]; // dont '\0'
  uint size; // du fichier en octets
  uint nblock; // nblock du fichier = (size+BLOCK_SIZE-1)/BLOCK_SIZE ?
  uint first_byte; // start block number on the virtual disk
} inode_t;

void read_inodes_table(virtual_disk_t *r5Disk, inode_table_t *table){
  stripe_t *bandesLues;
  int nbBandes = compute_nstripe(r5Disk, INODE_SIZE);
  char conversion[4];
  bandesLues = (stripe_t *) malloc(sizeof(stripe_t)*nbBandes);
  for(int i = 0; i<nbBandes; i++){
    &bandesLues[i] = init_bande();
  }
  for (int i = 0; i<INODE_TABLE_SIZE; i++){
    //Faire l'inode à chaque tour de i

    /* LECTURE DES BANDES CONSTITUANTS L'INODE */
    for (int j = 0; j<nbBandes; j++){ //Ici on lit toutes les bandes
      //BLOCK_SIZE*i*nbBandes*r5->ndisk = taille d'un inode sur disque * i
      //BLOCK_SIZE*j*r5Disk->ndisk = j * taille d'une bande
      read_stripe(r5Disk, &bandesLues[j], INODES_START+(BLOCK_SIZE*j*r5Disk->ndisk)+(BLOCK_SIZE*i*nbBandes*r5Disk->ndisk));
    }

    /* RECUPERATION DES INFOS DEPUIS LES BANDES */

    //Recup du filename (pas de conversion)
    for(int k = 0; k<BLOCK_SIZE*r5Disk->ndisk*2; k++){ //k allant de 0 à (2bandes/octets)-1
                                                      //avec BLOCK_SIZE = 4 et ndisk =4; 0<=k<32
      table[i]->filename[k] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk]->data[k%BLOCK_SIZE];
      //k/(BLOCK_SIZE*r5Disk->ndisk) = indice de bande (0 puis 1)
      //(k/BLOCK_SIZE)%r5Disk->ndisk] = indice de block dans la bande (0-4)
      //k%BLOCK_SIZE = indice de l'octet dans le block (0-4)
    }

    //Recup de la size (conversion int)
    for (int k = BLOCK_SIZE*r5Disk->ndisk*2; k < (BLOCK_SIZE*r5Disk->ndisk*2)+4; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk]->data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    memcpy(&(table[i]->size), conversion, sizeof(table[i]->size));
    //Ou memcpy(&(table[i]->size), bandesLues[BLOCK_SIZE*r5Disk->ndisk*2/(BLOCK_SIZE*r5Disk->ndisk)]->
    //stripe[(BLOCK_SIZE*r5Disk->ndisk*2/BLOCK_SIZE)%r5Disk->ndisk]->data[BLOCK_SIZE*r5Disk->ndisk*2%BLOCK_SIZE],
    //sizeof(table[i]->size));

    //Recup du nblock(conversion int)
    for (int k = (BLOCK_SIZE*r5Disk->ndisk*2)+4; k < (BLOCK_SIZE*r5Disk->ndisk*2)+8; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk]->data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    memcpy(&(table[i]->nblock), conversion, sizeof(table[i]->nblock));

    //Recup du first_byte(conversion int)
    for (int k = (BLOCK_SIZE*r5Disk->ndisk*2)+8; k < (BLOCK_SIZE*r5Disk->ndisk*2)+12; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk]->data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    memcpy(&(table[i]->first_byte), conversion, sizeof(table[i]->first_byte));
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
