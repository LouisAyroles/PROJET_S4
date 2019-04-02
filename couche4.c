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

   int nbfiles = get_nb_files(r5Disk->inodes)
   /*Il n'y a plus de places dans le systeme*/
   if (nbfiles==10) {
     return 0;
   }
  /*Est ce que le fichier est present dans le systeme?*/
   while ( (nbfiles > 0) && (inodes[i]->filename != nomFichier) ) {
     nbfiles--;
   }
   /*Le fichier n'est pas present dans le systeme*/
   if (nbfiles == 0) {
     uint start = r5Disk->super_block.first_free_byte;
     int posInode = get_unused_inodes(r5Disk->inodes);
     r5Disk->inodes[posInode]=init_inode(nomFichier, fichier->size, start);
     write_inodes_table(r5Disk,)
     r5Disk->number_of_files+=1;

   /*Le fichier est present dans le systeme*/
   }else{
     /*Le fichier est plus petit que le fichier present*/
     if (fichier.size <= r5Disk->inodes[nbfiles].size) {
       r5Disk->inodes[nbfiles].size = fichier.size;
       //Mettre a jour les données

     /*Le fichier est plus grand que le fichier present*/
     }else{
       delete_inode(r5Disk,nbfiles);
       //identique au 1er if
     }

   }
}



 /** \brief
   * Lit un fichier sur le systeme RAID et renvoie le contenu dans file_t
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @param : file_t
   * @return int (1 si fichier present, 0 si fichier non present)
 **/
 int read_file(virtual_disk_t *r5Disk, char *NomFichier, file_t fichier){

 }



 /** \brief
   * Supprime un fichier sur le systeme RAID
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @return int (1 si fichier supprimé, 0 si fichier non present)
 **/
 int delete_file(virtual_disk_t *r5Disk,char *NomFichier){

 }

 /** \brief
   * Ecrit un fichier present (dans l'ordinateur (host)) sur le systeme RAID
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @return void
 **/
 void load_file_from_host(virtual_disk_t *r5Disk, char *NomFichier){

 }

 /** \brief
   * Ecrit un fichier present (dans le systeme RAID) sur l'ordinateur (host)
   * @param : virtual_disk_t
   * @param : chaine de char (nom du Fichier)
   * @return void
 **/
 void store_file_to_host(virtual_disk_t *r5Disk, char *NomFichier);
