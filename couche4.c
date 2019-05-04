#include "couche1.h"
#include "couche2.h"
#include "couche3.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

/**
 * \file couche4.c
 * \brief Programme couche4 du raid5.
 * \author Groupe14
 * \version 0.1
 * \date 01 avril 2019
 *
 * Programme de la couche 4 du raid5.
 *
 */

 /** \brief
   * Ecrit un fichier (contenu dans file_t) sur le systeme RAID
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @param : file_t (contenu du fichier à ecrire)
   * @return int (1 si on a ecrit le fichier, 0 si plus de place ou echec)
 **/
/*int write_file(virtual_disk_t *r5Disk, char *nomFichier, file_t fichier){
   printf("\nwrite_file\n");
   inode_table_t inodes;
   super_block_t sup;
   read_super_block(r5Disk,&sup);
   first_free_byte(r5Disk);
   read_inodes_table(r5Disk,(inode_t *) inodes);
   int nbfiles = get_nb_files(r5Disk);
   //Il n'y a plus de places dans le systeme
   if (nbfiles==10) {
     return 0;
   }
  //Est ce que le fichier est present dans le systeme?
   while ( (nbfiles >= 0) && (r5Disk->inodes[nbfiles].filename != nomFichier) ) {
     nbfiles--;
   }
   //Le fichier n'est pas present dans le systeme
   if (nbfiles < 0) {
     printf("Fichier absent\n");
     uint start = sup.first_free_byte;
     inode_t inode = init_inode(nomFichier, fichier.size, start);
     inode_dump(inode);
     read_inodes_table(r5Disk,(inode_t *) inodes);
     r5Disk->inodes[get_unused_inodes(r5Disk)]=inode;
     write_inodes_table(r5Disk, r5Disk->inodes);
     //printf("taille : %d Debut : %d\n", fichier.size,sup.first_free_byte );
     write_chunk(r5Disk,fichier.data,fichier.size,start);
     sup.first_free_byte+=fichier.size;
     sup.nb_blocks_used+=compute_nblock(fichier.size);
   //Le fichier est present dans le systeme
   }else{
     //Le fichier est plus petit que le fichier present
     if (fichier.size <= r5Disk->inodes[nbfiles].size) {
      printf("Fichier présent , plus petit\n");
      uint start = r5Disk->inodes[nbfiles].first_byte;
      inode_t inode = init_inode(nomFichier, fichier.size, start);
      read_inodes_table(r5Disk,(inode_t *) inodes);
      r5Disk->inodes[get_unused_inodes(r5Disk)]=inode;
      write_inodes_table(r5Disk, r5Disk->inodes);
      write_chunk(r5Disk,fichier.data,fichier.size,start);
      //Mise a jour des valeurs dans le super_block
      //
      //
     //Le fichier est plus grand que le fichier present
     }else{
       int old_size=r5Disk->inodes[nbfiles].size;
       delete_inode(r5Disk,nbfiles);
       uint start = r5Disk->super_block.first_free_byte;
       inode_t inode = init_inode(nomFichier, fichier.size, start);
       read_inodes_table(r5Disk,(inode_t *) inodes);
       r5Disk->inodes[get_unused_inodes(r5Disk)]=inode;
       write_inodes_table(r5Disk, inodes);
       write_chunk(r5Disk,fichier.data,fichier.size,r5Disk->super_block.first_free_byte);
       sup.first_free_byte+=fichier.size;
       sup.nb_blocks_used=sup.nb_blocks_used-compute_nblock(old_size);
       sup.nb_blocks_used+=compute_nblock(fichier.size);
     }

   }
   write_super_block(r5Disk,sup);
   r5Disk->number_of_files+=1;
   printf("Out of write file.\n");
   return 1;
}*/
int write_file(virtual_disk_t *r5Disk, char *nomFichier, file_t fichier){
   printf("\nwrite_file\n");
   inode_table_t inodes;
   super_block_t sup;
   read_super_block(r5Disk,&sup);
   first_free_byte(r5Disk);
   read_inodes_table(r5Disk,(inode_t *) inodes);
   int nbfiles = get_nb_files(r5Disk);
   //Il n'y a plus de places dans le systeme
   if (nbfiles==10) {
     return 0;
   }
  //Est ce que le fichier est present dans le systeme?
   while ( (nbfiles >= 0) && (r5Disk->inodes[nbfiles].filename != nomFichier) ) {
     nbfiles--;
   }
   //Le fichier n'est pas present dans le systeme
   if (nbfiles < 0) {
     printf("Fichier absent\n");
     uint start = sup.first_free_byte;
     inode_t inode = init_inode(nomFichier, fichier.size, start);
     inode_dump(inode);
     read_inodes_table(r5Disk,(inode_t *) inodes);
     r5Disk->inodes[get_unused_inodes(r5Disk)]=inode;
     write_inodes_table(r5Disk, r5Disk->inodes);
     //printf("taille : %d Debut : %d\n", fichier.size,sup.first_free_byte );
     write_chunk(r5Disk,fichier.data,fichier.size,start);
     sup.first_free_byte+=fichier.size;
     sup.nb_blocks_used+=compute_nblock(fichier.size);
 }else{
    printf("Fichier present\n");
    return 0;
 }
   write_super_block(r5Disk,sup);
   r5Disk->number_of_files+=1;
   printf("Out of write file.\n");
   return 1;
}



 /** \brief
   * Lit un fichier sur le systeme RAID et renvoie le contenu dans fichier
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @param : file_t
   * @return int (1 si fichier present, 0 si fichier non present)
 **/
 int read_file(virtual_disk_t *r5Disk, char *nomFichier, file_t *fichier){
   printf("read_file\n");
   char *buffer;
   int nbfiles = get_nb_files(r5Disk);
   /*Est ce que le fichier est present dans le systeme?*/
   while ( (nbfiles >= 0) && (r5Disk->inodes[nbfiles].filename != nomFichier) ) {
     nbfiles--;
   }
   /*Le fichier n'est pas present sur le systeme*/
   if (nbfiles < 0) {
     return 0;

   /*Le fichier est present sur le systeme*/
   }else{
     fichier->size = r5Disk->inodes[nbfiles].size;
     read_chunk(r5Disk, buffer ,r5Disk->inodes[nbfiles].first_byte, fichier->size);
     for (int i = 0; i < fichier->size; i++) {
       fichier->data[i] = *buffer;
     }first_free_byte(r5Disk);
   }
   return 1;
 }



 /** \brief
   * Supprime un fichier sur le systeme RAID
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @return int (1 si fichier supprimé, 0 si fichier non present)
 **/
 int delete_file(virtual_disk_t *r5Disk,char *nomFichier){
   printf("delete_file\n");
   super_block_t sup;
   read_super_block(r5Disk,&sup);
   int nbfiles = get_nb_files(r5Disk);
   /*Est ce que le fichier est present dans le systeme?*/
   while ( (nbfiles >= 0) && strcmp(r5Disk->inodes[nbfiles].filename,nomFichier) ) {
     nbfiles--;
   }
   /*Le fichier n'est pas present sur le systeme*/
   if (nbfiles < 0) {
     return 0;
   /*Le fichier est present sur le systeme*/
   }else{
     printf("trouvé a l'indice %d\n",nbfiles);
     r5Disk->inodes[nbfiles].first_byte = 0;
     sup.nb_blocks_used-=compute_nblock(r5Disk->inodes[nbfiles].size);
     delete_inode(r5Disk,nbfiles);
   }
   write_super_block(r5Disk,sup);
   r5Disk->number_of_files-=1;
   return 1;
 }

 /** \brief
   * Ecrit un fichier present (dans l'ordinateur (host)) sur le systeme RAID
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @return void
 **/
void load_file_from_host(virtual_disk_t *r5Disk, char *nomFichier){
    printf("load_file_from_host\n");
    FILE* fd;
    file_t fichier;
    fd = fopen(nomFichier, "r+");
    if (fd == NULL){
      printf("Impossible d'ouvrir le fichier %s",nomFichier);
   }else{
     fseek(fd, 0, SEEK_END);
     fichier.size = ftell(fd);
     printf("File size : %d\n",fichier.size);
     fseek(fd,0,SEEK_SET);
     fread(fichier.data, fichier.size, 1, fd);
     fclose(fd);
   }
   write_file(r5Disk,nomFichier,fichier);
 }

 /** \brief
   * Ecrit un fichier present (dans le systeme RAID) sur l'ordinateur (host)
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @return void
 **/
 void store_file_to_host(virtual_disk_t *r5Disk, char *nomFichier){
   printf("store_file_to_host\n");
   file_t fichier;
   FILE* fd;
   if ( read_file(r5Disk, nomFichier, &fichier) ) {
     fd = fopen(nomFichier,"a");
     if (fd == NULL) {
       printf("Impossible de creer le fichier %s",nomFichier);
     }else{
       fwrite(fichier.data, fichier.size, 1, fd);
     }
  }
}


int main(int argc, char const *argv[]) {
  //couche3();
  virtual_disk_t *r5d=init_disk_raid5("./RAIDFILES");
  inode_table_t inodes;
  inode_table_t *table = malloc(sizeof(inode_t)*(INODE_TABLE_SIZE+1));

  for(int i = 0; i<INODE_TABLE_SIZE; i++){
    inodes[i].first_byte = 0;
    inodes[i].size = 2;
    inodes[i].nblock = 4;
    for (int j = 0; j < FILENAME_MAX_SIZE-1; j++) {
      inodes[i].filename[j] = 32;
    }
    inodes[i].filename[FILENAME_MAX_SIZE-1] = '\0';
  }

  write_inodes_table(r5d, inodes);
  read_inodes_table(r5d,(inode_t *) table);

  super_block_t sb, sbd;
  sb.raid_type = r5d->raidmode;
  sb.nb_blocks_used = 0;
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    sb.nb_blocks_used += inodes[i].nblock;
  }
  sb.first_free_byte = 43;
  write_super_block(r5d, sb);
  //affichageSysteme(r5d);
  first_free_byte(r5d);
  read_super_block(r5d,&sbd);
  super_block_dump(sbd);
  load_file_from_host(r5d,"test.txt");
  load_file_from_host(r5d,"test2.txt");
  read_super_block(r5d,&sbd);
  read_inodes_table(r5d,(inode_t *) table);
  super_block_dump(sbd);
  inode_table_dump((inode_t *) table);
  //delete_inode(r5d,1);
  delete_file(r5d,"test2.txt");
  read_inodes_table(r5d,(inode_t *) table);
  inode_table_dump((inode_t *) table);
  //affichageSysteme(r5d);
  turn_off_disk_raid5(r5d);
  exit(0);
}
