#include "couche1.h"
#include "couche2.h"
#include "couche3.h"
#include "couche4.h"
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
void write_file(virtual_disk_t *r5Disk, char *nomFichier, file_t fichier){
   printf("\nwrite_file\n");
   inode_table_t inodes;
   super_block_t sup;
   read_inodes_table(r5Disk,(inode_t *) inodes);
   int nbfiles = get_nb_files(r5Disk);
   //Il n'y a plus de places dans le systeme
   if (nbfiles==10) {
     return;
   }
  //Est ce que le fichier est present dans le systeme?
   while ( (nbfiles >= 0) && strcmp(inodes[nbfiles].filename,nomFichier) ) {
     nbfiles--;
   }
   //Le fichier n'est pas present dans le systeme
   if (nbfiles < 0) {
     printf("Fichier absent\n");
     inode_t inode = init_inode(nomFichier, fichier.size, first_free_byte(r5Disk));
     inodes[get_unused_inodes(r5Disk)]=inode;
     write_inodes_table(r5Disk, inodes);
     write_chunk(r5Disk,fichier.data,fichier.size,inode.first_byte);
    //Le fichier est present dans le systeme
   }else{
    printf("Fichier present\n");
    //Le fichier passé en parametre est plus petit que le fichier présent
    int start = inodes[nbfiles].first_byte;
    if (inodes[nbfiles].size >= fichier.size){
      write_chunk(r5Disk, fichier.data, fichier.size, inodes[nbfiles].first_byte);
    }else{
      delete_file(r5Disk,nomFichier);
      add_inode(r5Disk, nomFichier, fichier.size);
    }
  }
   r5Disk->number_of_files+=1;
   update_super_block(r5Disk);
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
   char *buffer = (char*)malloc(sizeof(char)*MAX_FILE_SIZE);
   int nbfiles = get_nb_files(r5Disk);
   inode_t inodes[INODE_TABLE_SIZE];
   read_inodes_table(r5Disk, inodes);
   /*Est ce que le fichier est present dans le systeme?*/
   while ( (nbfiles >= 0) && strcmp(inodes[nbfiles].filename,nomFichier) ) {
     nbfiles--;
   }
   /*Le fichier n'est pas present sur le systeme*/
   if (nbfiles < 0) {
     return 0;

   /*Le fichier est present sur le systeme*/
   }else{
     fichier->size = inodes[nbfiles].size;
     read_chunk(r5Disk, buffer, fichier->size, inodes[nbfiles].first_byte);
     for (int i = 0; i < fichier->size; i++) {
       fichier->data[i] = buffer[i];
     }
     update_super_block(r5Disk);
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
     delete_inode(r5Disk,nbfiles);
   }
   update_super_block(r5Disk);
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
     fd = fopen(nomFichier,"w");
     if (fd == NULL) {
       printf("Impossible de creer le fichier %s",nomFichier);
     }else{
       fwrite(fichier.data, fichier.size-1, 1, fd);
     }
  }
}


int main(int argc, char const *argv[]) {
  //couche3();
  virtual_disk_t *r5d=init_disk_raid5("./RAIDFILES");
  file_t fichier,f2;
  inode_t maTable[INODE_TABLE_SIZE];
  //reinit_systeme(r5d);
  load_file_from_host(r5d,"Test.txt");
  affichageSysteme(r5d);
  read_inodes_table(r5d, maTable);
  printf("INODE AJOUTEE : \n Filename : %s\t Size : %d\t Nblock :%d\t First_Byte : %d\n",maTable[0].filename,maTable[0].size,maTable[0].nblock, maTable[0].first_byte);
  read_file(r5d,"Test.txt",&f2);
  printf("LECTURE DU FICHIER :\nSize : %d\nContenu du fichier : %s\n",f2.size, f2.data);

  turn_off_disk_raid5(r5d);
  exit(0);
}
