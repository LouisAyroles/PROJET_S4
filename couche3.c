#include "couche3.h"
#include "couche2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>


void read_super_block(virtual_disk_t *r5Disk, super_block_t *superblock){
  printf("read_super_block\n");
  int nbBandes = SUPER_BLOCK_SIZE;
  stripe_t *bandesLues;
  char conversion[BLOCK_SIZE];
  bandesLues = (stripe_t *) malloc(sizeof(stripe_t)*nbBandes);
  for(int j = 0; j<nbBandes; j++){
    read_stripe(r5Disk, &bandesLues[j], BLOCK_SIZE*j*r5Disk->ndisk);
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



void read_inodes_table(virtual_disk_t *r5Disk, inode_table_t *table){
  printf("read_inodes_table\n");
  stripe_t **bandesLues;
  int nbBandes = compute_nstripe(r5Disk, INODE_SIZE), k;
  char conversion[4];
  char *buffer = (char*)malloc(sizeof(char) * FILENAME_MAX_SIZE);
  bandesLues = (stripe_t **) malloc(sizeof(stripe_t)*nbBandes);

  for(int i = 0; i<nbBandes; i++){
    bandesLues[i] = init_bande(r5Disk);
  }
 for (int i = 0; i<INODE_TABLE_SIZE; i++){
   printf("OOF\n");
    //Faire l'inode à chaque tour de i
    /* LECTURE DES BANDES CONSTITUANTS L'INODE */
    for (int j = 0; j<nbBandes; j++){ //Ici on lit toutes les bandes
      //BLOCK_SIZE*i*nbBandes*r5->ndisk = taille d'un inode sur disque * i
      //BLOCK_SIZE*j*r5Disk->ndisk = j * taille d'une bande
      printf("ridrid\n");
      read_stripe(r5Disk, bandesLues[j], r5Disk->ndisk+(j*BLOCK_SIZE)+(BLOCK_SIZE*nbBandes*i));
    }
    printf("outrid\n");
    /* RECUPERATION DES INFOS DEPUIS LES BANDES */
    //printf("Bande 1:\n");
    //print_stripe(r5Disk , bandesLues[0]);
    //printf("Bande 2:\n");
    //print_stripe(r5Disk , bandesLues[1]);
    //Recup du filename (pas de conversion)
    for(k = 0; k < FILENAME_MAX_SIZE/4; k++){ //k allant de 0 à (2bandes/octets)-1
                                                      //avec BLOCK_SIZE = 4 et ndisk =4; 0<=k<32
      //printf("BandesLues = %d, stripe = %d, data = %d, ndisk=%d, k=%d, i = %d\n\n", k/(BLOCK_SIZE*r5Disk->ndisk), (k/BLOCK_SIZE)%r5Disk->ndisk, k%BLOCK_SIZE, r5Disk->ndisk, k, i);
      buffer[k] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%(r5Disk->ndisk)].data[k%BLOCK_SIZE];
      table[i]->filename[k] = buffer[k];
      printf("iteration FSIZE\n");
      //table[i]->filename[k] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%(r5Disk->ndisk)].data[k%BLOCK_SIZE];
      //test->stripe = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%(r5Disk->ndisk)];
      //printf("file = bande ? %d\n\n", table[i]->filename[k] == bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%(r5Disk->ndisk)].data[k%BLOCK_SIZE]);
      //k/(BLOCK_SIZE*r5Disk->ndisk) = indice de bande (0 puis 1)
      //(k/BLOCK_SIZE)%r5Disk->ndisk] = indice de block dans la bande (0-4)
      //k%BLOCK_SIZE = indice de l'octet dans le block (0-4)
    }
    printf("contenu du filename n° %d: \n",i);
    for(k = 0; k < FILENAME_MAX_SIZE; k++){
      printf("Iterate\n");
      printf("|%d|",table[i]->filename[k]);
    }
    printf("OUT\n");
    //Recup de la size (conversion int)
    for (k = BLOCK_SIZE*r5Disk->ndisk*2; k < (BLOCK_SIZE*r5Disk->ndisk*2)+4; k++){
      printf("loup\n");
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    printf("memucypy1\n");
    memcpy(&(table[i]->size), conversion, sizeof(table[i]->size));

    //Ou memcpy(&(table[i]->size), bandesLues[BLOCK_SIZE*r5Disk->ndisk*2/(BLOCK_SIZE*r5Disk->ndisk)]->
    //stripe[(BLOCK_SIZE*r5Disk->ndisk*2/BLOCK_SIZE)%r5Disk->ndisk]->data[BLOCK_SIZE*r5Disk->ndisk*2%BLOCK_SIZE],
    //sizeof(table[i]->size));

    //Recup du nblock(conversion int)
    for (k = (BLOCK_SIZE*r5Disk->ndisk*2)+4; k < (BLOCK_SIZE*r5Disk->ndisk*2)+8; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    printf("memucypy2\n");
    memcpy(&(table[i]->nblock), conversion, sizeof(table[i]->nblock));

    //Recup du first_byte(conversion int)
    for (k = (BLOCK_SIZE*r5Disk->ndisk*2)+8; k < (BLOCK_SIZE*r5Disk->ndisk*2)+12; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    printf("memucypy3\n");
    memcpy(&(table[i]->first_byte), conversion, sizeof(table[i]->first_byte));
    printf("memucypyOUT\n");
  }
  for(int i = 0; i<nbBandes; i++){
    printf("Deleting bd\n");
    delete_bande(&bandesLues[i]);
  }
  free(bandesLues);
  free(buffer);
  return;
}


void write_inodes_table(virtual_disk_t *r5Disk, inode_table_t inode){
  printf("write_inodes_table\n");
  int noBande=0;
  char *buffer = (char *)malloc(sizeof(inode_t)*INODE_TABLE_SIZE);
  char temp[4];
  int size, nblock, first_byte, poswrite;
  int pos = BLOCK_SIZE*compute_nblock(sizeof(super_block_t));
  int blockDebutInode = pos / BLOCK_SIZE;     /*= dernier block du SUPERBLOCK*/
  if(blockDebutInode>=r5Disk->ndisk){         /*rajoute le bloc de parité*/
    blockDebutInode++;
  }
  noBande = blockDebutInode/r5Disk->ndisk;
  blockDebutInode = blockDebutInode%r5Disk->ndisk; /*= premier block de l'inode*/
  if (blockDebutInode == compute_parity_index(r5Disk ,noBande)) {
    blockDebutInode++;
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
  write_chunk(r5Disk, buffer, sizeof(inode_t)*INODE_TABLE_SIZE, noBande*r5Disk->ndisk+blockDebutInode);
}


void delete_inode(virtual_disk_t *r5Disk, int numInode){
  printf("delete_inode\n");
  if(numInode <= r5Disk->number_of_files){
    for (int i = numInode+1; i < (r5Disk->number_of_files); i++) {
        r5Disk->inodes[i-1]=r5Disk->inodes[i];
    }
    r5Disk->inodes[r5Disk->number_of_files-1].first_byte=0;
    r5Disk->number_of_files = r5Disk->number_of_files-1;
  }
}

int get_nb_files(inode_table_t tab){
  printf("get_nb_files\n");
  int i = get_unused_inodes(tab);
  if (i == -1){
    return 10;
  }
  return i;
}

int get_unused_inodes(inode_table_t tab){
  printf("get_unused_inodes\n");
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    if (tab[i].first_byte == 0) {
      return i;
   }
  return -1;
}

}
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
  read_inodes_table(r5Disk, &table);
  r5Disk->number_of_files = get_nb_files(table);
  printf("Number of files : %d\n",r5Disk->number_of_files);
  for(int i = 0; i < (r5Disk->number_of_files); i++){
      printf("fichier n°%d :\n%s\n", i ,r5Disk->inodes[i].filename);
      printf("%d octets\n", r5Disk->inodes[i].size);
      printf("%d blocks\n", r5Disk->inodes[i].nblock);
      printf("commence a l'octet %d\n\n", r5Disk->inodes[i].first_byte);
  }
}



void write_super_block(virtual_disk_t *r5Disk){
  printf("write_super_block\n");
  int lastfirstbyte=0;
  int lastindice=0;
  int poswrite=0;
  char *buffer = (char *)malloc(sizeof(super_block_t));

  /*Init du superblock*/
  read_inodes_table(r5Disk, &(r5Disk->inodes));
  r5Disk->super_block.nb_blocks_used=0;
  for (int i = 0; i < get_unused_inodes(r5Disk->inodes); i++) {
    r5Disk->super_block.nb_blocks_used += r5Disk->inodes[i].nblock;
    if (r5Disk->inodes[i].first_byte > lastfirstbyte) {
      lastfirstbyte = r5Disk->inodes[i].first_byte;
      lastindice=i;
    }
  }
  r5Disk->super_block.first_free_byte= lastfirstbyte + r5Disk->inodes[lastindice].size;
  r5Disk->super_block.raid_type = r5Disk->raidmode;

  /*Transformation des int en char pour ecriture sur le disque*/

    /*ecriture de l'enum*/
    buffer[poswrite+3] = (r5Disk->super_block.raid_type>>24) & 0xFF;
    buffer[poswrite+2] = (r5Disk->super_block.raid_type>>16) & 0xFF;
    buffer[poswrite+1] = (r5Disk->super_block.raid_type>>8) & 0xFF;
    buffer[poswrite] = r5Disk->super_block.raid_type & 0xFF;

    /*ecriture de nb_block_used*/
    poswrite+=4;
    buffer[poswrite+3] = (r5Disk->super_block.nb_blocks_used>>24) & 0xFF;
    buffer[poswrite+2] = (r5Disk->super_block.nb_blocks_used>>16) & 0xFF;
    buffer[poswrite+1] = (r5Disk->super_block.nb_blocks_used>>8) & 0xFF;
    buffer[poswrite] = r5Disk->super_block.nb_blocks_used & 0xFF;

    /*ecriture de first_free_byte*/
    poswrite+=4;
    buffer[poswrite+3] = (r5Disk->super_block.first_free_byte>>24) & 0xFF;
    buffer[poswrite+2] = (r5Disk->super_block.first_free_byte>>16) & 0xFF;
    buffer[poswrite+1] = (r5Disk->super_block.first_free_byte>>8) & 0xFF;
    buffer[poswrite] = r5Disk->super_block.first_free_byte & 0xFF;

      /*Ecriture du Superblock*/
    write_chunk(r5Disk, buffer, sizeof(super_block_t), 0);
}


void first_free_byte(virtual_disk_t *r5Disk){
  printf("first_free_byte\n");
  int lastfirstbyte=0, lastindice=0;
  for (int i = 0; i < get_unused_inodes(r5Disk->inodes); i++) {
    if (r5Disk->inodes[i].first_byte > lastfirstbyte) {
        lastfirstbyte = r5Disk->inodes[i].first_byte;
        lastindice=i;
    }
  }
  r5Disk->super_block.first_free_byte = lastfirstbyte + r5Disk->inodes[lastindice].size;
}

void main() {
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
  printf("wololo\n");
}
