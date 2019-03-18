#include "couche3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void read_inodes_table(virtual_disk_t *r5Disk, inode_table_t *table){
  stripe_t *bandesLues, *marqueur;
  int nbBandes = compute_nstripe(r5Disk, INODE_SIZE);
  char conversion[4];
  bandesLues = (stripe_t *) malloc(sizeof(stripe_t)*nbBandes);
  marqueur = bandesLues;
  for(int i = 0; i<nbBandes; i++){
    marqueur = init_bande();
    marqueur+=1;
  }
  for (int i = 0; i<INODE_TABLE_SIZE; i++){
    //Faire l'inode à chaque tour de i

    /* LECTURE DES BANDES CONSTITUANTS L'INODE */
    for (int j = 0; j<nbBandes; j++){ //Ici on lit toutes les bandes
      //BLOCK_SIZE*i*nbBandes*r5->ndisk = taille d'un inode sur disque * i
      //BLOCK_SIZE*j*r5Disk->ndisk = j * taille d'une bande
      read_stripe(r5Disk, &bandesLues[j], INODES_START+(BLOCK_SIZE*j*r5Disk->ndisk)+(BLOCK_SIZE*r5Disk->ndisk*nbBandes*i));
    }

    /* RECUPERATION DES INFOS DEPUIS LES BANDES */

    //Recup du filename (pas de conversion)
    for(int k = 0; k<BLOCK_SIZE*r5Disk->ndisk*2; k++){ //k allant de 0 à (2bandes/octets)-1
                                                      //avec BLOCK_SIZE = 4 et ndisk =4; 0<=k<32
      table[i]->filename[k] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)].stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE];
      //k/(BLOCK_SIZE*r5Disk->ndisk) = indice de bande (0 puis 1)
      //(k/BLOCK_SIZE)%r5Disk->ndisk] = indice de block dans la bande (0-4)
      //k%BLOCK_SIZE = indice de l'octet dans le block (0-4)
    }

    //Recup de la size (conversion int)
    for (int k = BLOCK_SIZE*r5Disk->ndisk*2; k < (BLOCK_SIZE*r5Disk->ndisk*2)+4; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)].stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    memcpy(&(table[i]->size), conversion, sizeof(table[i]->size));
    //Ou memcpy(&(table[i]->size), bandesLues[BLOCK_SIZE*r5Disk->ndisk*2/(BLOCK_SIZE*r5Disk->ndisk)]->
    //stripe[(BLOCK_SIZE*r5Disk->ndisk*2/BLOCK_SIZE)%r5Disk->ndisk]->data[BLOCK_SIZE*r5Disk->ndisk*2%BLOCK_SIZE],
    //sizeof(table[i]->size));

    //Recup du nblock(conversion int)
    for (int k = (BLOCK_SIZE*r5Disk->ndisk*2)+4; k < (BLOCK_SIZE*r5Disk->ndisk*2)+8; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)].stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    memcpy(&(table[i]->nblock), conversion, sizeof(table[i]->nblock));

    //Recup du first_byte(conversion int)
    for (int k = (BLOCK_SIZE*r5Disk->ndisk*2)+8; k < (BLOCK_SIZE*r5Disk->ndisk*2)+12; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)].stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    memcpy(&(table[i]->first_byte), conversion, sizeof(table[i]->first_byte));
  }
  for(int i = 0; i<nbBandes; i++){
    delete_bande(&marqueur);
    marqueur-=1;
  }
  free(bandesLues); //??
}


void write_inodes_table(virtual_disk_t *r5Disk, inode_table_t inode){
  int noBande=0;
  char *buffer = (char *)malloc(sizeof(inode_t)*INODE_TABLE_SIZE);
  char temp[4];
  int size, nblock, first_byte, int poswrite;
  int pos = BLOCK_SIZE*compute_nblock(r5Disk, sizeof(super_block_t));
  int blockDebutInode = pos / BLOCK_SIZE;     /*= dernier block du SUPERBLOCK*/
  if(blockDebutInode>=r5Disk->ndisk){         /*rajoute le bloc de parité*/
    blockDebutInode++;
  }
  noBande = blockDebutInode/r5Disk->ndisk);
  blockDebutInode = blockDebutInode%r5Disk->ndisk; /*= premier block de l'inode*/
  if (blockDebutInode == compute_parity_index(r5Disk->ndisk,noBande)) {
    blockDebutInode++;
  }
  /*construction du buffer*/
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {

    /*ecriture du filename*/
    poswrite = i*sizeof(inode_t);
    for (j = 0; j < FILENAME_MAX_SIZE; j++) {
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

    /*ecriture du first_byte */
    poswrite+=4;
    buffer[poswrite+3] = (inode[i].first_byte>>24) & 0xFF;
    buffer[poswrite+2] = (inode[i].first_byte>>16) & 0xFF;
    buffer[poswrite+1] = (inode[i].first_byte>>8) & 0xFF;
    buffer[poswrite] = inode[i].first_byte & 0xFF;

  }
  write_chunk(r5Disk, buffer, sizeof(inode_t)*INODE_TABLE_SIZE, noBande*r5Disk->ndisk+blockDebutInode);
}


void delete_inode(virtual_disk_t *r5Disk, int numInode){
  for (int i = numInode; i < (r5Disk->number_of_files-numInode); i++) {
      r5Disk->inodes[i]=r5Disk->inodes[i+1];
  }
  r5Disk->number_of_files = r5Disk->number_of_files-1; //peut etre deja fait avant?
}


int get_unused_inodes(virtual_disk_t *r5Disk){
  return (r5Disk->number_of_files)+1; //Qu'est ce qu'on retourne s'il n'y a plus de place?
}

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
void write_super_block(virtual_disk_t *r5Disk, super_block_t superblock);
void read_super_block(virtual_disk_t *r5Disk, super_block_t superblock);
void first_free_byte(virtual_disk_t *r5Disk);
