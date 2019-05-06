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
  if(fichier.size < MAX_FILE_SIZE){
     inode_table_t inodes;
     super_block_t sup;
     read_inodes_table(r5Disk,(inode_t *) inodes);
     int nbfiles = get_nb_files(r5Disk);
     //Il n'y a plus de places dans le systeme
     if (nbfiles==10) {
       printf("\033[31;49mLe systeme est plein.\033[39;49m\n");
       return;
     }
    //Est ce que le fichier est present dans le systeme?
     while ( (nbfiles >= 0) && strcmp(inodes[nbfiles].filename,nomFichier) ) {
       nbfiles--;
     }
     //Le fichier n'est pas present dans le systeme
     if (nbfiles < 0) {
       inode_t inode = init_inode(nomFichier, fichier.size, first_free_byte(r5Disk));
       inodes[get_unused_inodes(r5Disk)]=inode;
       write_chunk(r5Disk,fichier.data,fichier.size,inode.first_byte);
       r5Disk->number_of_files+=1;
       write_inodes_table(r5Disk, inodes);
      //Le fichier est present dans le systeme
     }else{
      //Le fichier passé en parametre est plus petit que le fichier présent
      int start = inodes[nbfiles].first_byte;
      if (inodes[nbfiles].size >= fichier.size){
        write_chunk(r5Disk, fichier.data, fichier.size, inodes[nbfiles].first_byte);
        inodes[nbfiles].size = fichier.size;
      }else{
        delete_file(r5Disk,nomFichier);
        int ffb = first_free_byte(r5Disk);
        add_inode(r5Disk, nomFichier, fichier.size);
        write_chunk(r5Disk,fichier.data,fichier.size,ffb);
        read_inodes_table(r5Disk,(inode_t *) inodes);
      }
    }
    update_super_block(r5Disk);
    write_inodes_table(r5Disk, inodes);
  } else{
      printf("\033[31;49mFichier trop volumineux. \033[39;49m Taille maximale : %d.\n", MAX_FILE_SIZE);
  }
}



 /** \brief
   * Lit un fichier sur le systeme RAID et renvoie le contenu dans fichier
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @param : file_t
   * @return int (1 si fichier present, 0 si fichier non present)
 **/
 int read_file(virtual_disk_t *r5Disk, char *nomFichier, file_t *fichier){
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
     free(buffer);
     printf("\033[31;49mLe fichier %s n'est pas present dans le Systeme.\033[39;49m\n",nomFichier);
     return 0;

   /*Le fichier est present sur le systeme*/
   }else{
     fichier->size = inodes[nbfiles].size;
     read_chunk(r5Disk, buffer, fichier->size, inodes[nbfiles].first_byte);
     for (int i = 0; i < fichier->size; i++) {
       fichier->data[i] = buffer[i];
     }
     fichier->data[fichier->size] = '\0';
     update_super_block(r5Disk);
   }
   free(buffer);
   return 1;
 }



 /** \brief
   * Supprime un fichier sur le systeme RAID
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @return int (1 si fichier supprimé, 0 si fichier non present)
 **/
 int delete_file(virtual_disk_t *r5Disk,char *nomFichier){
   super_block_t sup;
   read_super_block(r5Disk,&sup);
   int nbfiles = get_nb_files(r5Disk);
   /*Est ce que le fichier est present dans le systeme?*/
   while ( (nbfiles >= 0) && strcmp(r5Disk->inodes[nbfiles].filename,nomFichier) ) {
     nbfiles--;
   }
   /*Le fichier n'est pas present sur le systeme*/
   if (nbfiles < 0) {
     printf("\033[31;49mLe fichier %s n'est pas present dans le Systeme.\033[39;49m\n",nomFichier);
     return 0;
   /*Le fichier est present sur le systeme*/
   }else{
     delete_inode(r5Disk,nbfiles);
     defragmentation(r5Disk);
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
    FILE* fd;
    file_t fichier;
    char buffer[MAX_FILE_SIZE];
    fd = fopen(nomFichier, "r");
    if(strlen(nomFichier)>FILENAME_MAX_SIZE){
      fclose(fd);
      printf("\033[31;49mNom de fichier trop long. \033[39;49m Taille maximale : %d.\n", FILENAME_MAX_SIZE);
    }
    if (fd == NULL){
      printf("\033[31;49mImpossible d'ouvrir le fichier %s\033[39;49m\n",nomFichier);
   }else{
     fseek(fd, 0, SEEK_END);
     fichier.size = ftell(fd);
     fseek(fd,0,SEEK_SET);
     if(fichier.size < MAX_FILE_SIZE){
       fread(fichier.data, fichier.size, 1, fd);
       fclose(fd);
       write_file(r5Disk,nomFichier,fichier);
       update_super_block(r5Disk);
     }else{
       fclose(fd);
       printf("\033[31;49mFichier trop volumineux. \033[39;49m Taille maximale : %d.\n", MAX_FILE_SIZE);
     }
   }
 }

 /** \brief
   * Ecrit un fichier present (dans le systeme RAID) sur l'ordinateur (host)
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @return void
 **/
 void store_file_to_host(virtual_disk_t *r5Disk, char *nomFichier){
   file_t fichier;
   FILE* fd;
   if ( read_file(r5Disk, nomFichier, &fichier) ) {
     fd = fopen(nomFichier,"w");
     if (fd == NULL) {
       printf("\033[31;49mImpossible de creer le fichier %s.\033[39;49m",nomFichier);
     }else{
       fwrite(fichier.data,1, fichier.size, fd);
       fclose(fd);
     }
   }
}
/** \brief
  * Defragmente le systeme
  * @param : virtual_disk_t*
  * @return void
**/
void defragmentation(virtual_disk_t* r5Disk){
  inode_t maTable[10];
  file_t fichier;
  int departTable = startTable(r5Disk);
  int departFichier = departTable +(INODE_TABLE_SIZE*compute_nstripe(r5Disk, INODE_SIZE));
  read_inodes_table(r5Disk,maTable);
  for (int i = 0; i < INODE_TABLE_SIZE; i++) {
    if (maTable[i].first_byte !=0) {
      if (maTable[i].first_byte != departFichier ) {
        read_file(r5Disk, maTable[i].filename, &fichier);
        write_chunk(r5Disk, fichier.data, fichier.size, departFichier);
        maTable[i].first_byte = r5Disk->inodes[i].first_byte = departFichier;
      }
      departFichier += compute_nstripe(r5Disk,maTable[i].nblock);
    }
  }
  write_inodes_table(r5Disk, maTable);
}
