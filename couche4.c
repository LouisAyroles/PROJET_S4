#include "couche1.h"
#include "couche2.h"
#include "couche3.h"
#include <stdlib.h>
#include <math.h>

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
 int write_file(virtual_disk_t *r5Disk, char *nomFichier, file_t fichier){
   inode_table_t inodes;
   int nbfiles = get_nb_files(r5Disk->inodes);
   /*Il n'y a plus de places dans le systeme*/
   if (nbfiles==10) {
     return 0;
   }
  /*Est ce que le fichier est present dans le systeme?*/
   while ( (nbfiles >= 0) && (r5Disk->inodes[nbfiles].filename != nomFichier) ) {
     nbfiles--;
   }
   /*Le fichier n'est pas present dans le systeme*/
   if (nbfiles < 0) {
     uint start = r5Disk->super_block.first_free_byte;
     inode_t inode = init_inode(nomFichier, fichier.size, start);
     read_inodes_table(r5Disk,&inodes);
     r5Disk->inodes[get_unused_inodes(inodes)]=inode;
     write_inodes_table(r5Disk, inodes);
     write_chunk(r5Disk,fichier.data,fichier.size,r5Disk->super_block.first_free_byte);
   /*Le fichier est present dans le systeme*/
   }else{
     /*Le fichier est plus petit que le fichier present*/
     if (fichier.size <= r5Disk->inodes[nbfiles].size) {
       r5Disk->inodes[nbfiles].size = fichier.size;
       r5Disk->inodes[nbfiles].nblock = compute_nblock(fichier.size);
       write_chunk(r5Disk,fichier.data,fichier.size,r5Disk->super_block.first_free_byte);


     /*Le fichier est plus grand que le fichier present*/
     }else{
       delete_inode(r5Disk,nbfiles);
       uint start = r5Disk->super_block.first_free_byte;
       inode_t inode = init_inode(nomFichier, fichier.size, start);
       read_inodes_table(r5Disk,&inodes);
       r5Disk->inodes[get_unused_inodes(inodes)]=inode;
       write_inodes_table(r5Disk, inodes);
       write_chunk(r5Disk,fichier.data,fichier.size,r5Disk->super_block.first_free_byte);
     }

   }
   first_free_byte(r5Disk);
   r5Disk->number_of_files+=1;
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
   int nbfiles = get_nb_files(r5Disk->inodes);
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
     fichier->data = read_chunk(r5Disk, r5Disk->inodes[nbfiles].first_byte, fichier->size);
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
   int nbfiles = get_nb_files(r5Disk->inodes);
   /*Est ce que le fichier est present dans le systeme?*/
   while ( (nbfiles >= 0) && (r5Disk->inodes[nbfiles].filename != nomFichier) ) {
     nbfiles--;
   }
   /*Le fichier n'est pas present sur le systeme*/
   if (nbfiles < 0) {
     return 0;
   /*Le fichier est present sur le systeme*/
   }else{
     r5Disk->inodes[nbfiles].first_byte = 0;
     delete_inode(r5Disk,nbfiles);
   }
   first_free_byte(r5Disk);
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
   fd = fopen(nomFichier, "r+");
   if (fd == NULL){
     printf("Impossible d'ouvrir le fichier %s",nomFichier);
  }else{
    fichier.size = fseek(fd, 0, SEEK_END);
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
