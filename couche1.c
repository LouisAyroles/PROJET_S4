#include <stdio.h>
#include <stdlib.h>
#include "raid_defines.h"
#include <string.h>
#include <fts.h>
#include <errno.h>


/**
 * \file couche1.c
 * \brief Programme couche1 du raid5.
 * \author Groupe14
 * \version 0.1
 * \date 6 fevrier 2019
 *
 * Programme de la couche 1 du raid5.
 *
 */

//Global raid disk variable
virtual_disk_t *r5Disk;

/**
  * Copie la chaine "repertoire" dans nomDisque en y ajoutant "/d0\0"
  * @return void
**/
void add_finChemin(const char * repertoire, char * nomDisque, size_t lengthRep){
    strcpy(nomDisque, repertoire);    // strcpy: fonction copiant la chaine repertoire dans nomDisque
    nomDisque[lengthRep] = '/';
    nomDisque[lengthRep+1] = 'd';
    nomDisque[lengthRep+2] = '0';
    nomDisque[lengthRep+3] = '\0';
}

/**
  * Initialise la variable globale r5Disk
  * @param chaine de char (repertoire cible)
  * @return void
**/
void init_disk_raid5(const char * repertoire){
    size_t lengthRep = strlen(repertoire);
    char *nomDisque = malloc(sizeof(char)*lengthRep+10);  //Creation d'une chaine pouvant contenir [repertoire]+10 caracteres
    add_finChemin(repertoire, nomDisque, lengthRep);
    r5Disk=malloc(sizeof(virtual_disk_t));
    r5Disk->ndisk=4;
    r5Disk->raidmode=CINQ;
    r5Disk->storage=malloc(r5Disk->ndisk*sizeof(FILE *));
    for (int i = 0; i < r5Disk->ndisk; i++){
        nomDisque[lengthRep+2] = i + '0';         //Transforme le i en caractere et le met dans le "/di"
        r5Disk->storage[i]=fopen(nomDisque,"r");  //Ouvre le fichier "disque"
    }
}

/**
  * Ferme les fichiers ouverts et sauvegarde le super block?
  * @param chaine de char (repertoire cible)
  * @return void
**/
void turn_off_disk_raid5(const char * repertoire){
    size_t lengthRep = strlen(repertoire);
    char *nomDisque = malloc(sizeof(char)*lengthRep+10);
    add_finChemin(repertoire, nomDisque, lengthRep);
    for (int i = 0; i < r5Disk->ndisk; i++){
        nomDisque[lengthRep+2] = i + '0';
        r5Disk->storage[i]=fclose(nomDisque);
        free(r5Disk->storage[i]);
    }
    free(r5Disk);
}

/**
  * Affiche des infos sur les disques ouverts
  * @param void
  * @return void
**/
void info_disque(void){
	printf("---- Informations sur le disque raid ----\nNombre de disques physiques : %d\nMode de raid : %d\n",r5Disk->ndisk ,r5Disk->raidmode);
	for(int i=0;i<r5Disk->ndisk;i++){
		if(r5Disk->storage[i]==NULL){
			printf("Null pointer\n");
		}else{printf("Valid pointer\n");}
	}
}

/**
  * \brief calcule le nombre de blocs pour coder "n" octets
  * @param integer
  * @return integer
**/
int compute_nblock(int n)
{
    int nbBlocks;
    nbBlocks = n/4;
    if (n%4!=0)
    {
        nbBlocks += 1;
    }
    return nbBlocks;
}

/** \brief
  * Ecrit un bloc à la position pos sur le disque
  * @param virtual_disk_t
  * @param block_t
  * @param uint
  * @param integer
  * @return void
**/
void write_block(virtual_disk_t *RAID5, block_t *entrant, uint pos, int idDisk){
  fseek(RAID5[idDisk], (long)pos, SEEK_SET);
  fwrite(entrant, sizeof(block_t), 1, RAID5->storage[idDisk]);
}



/** \brief
  * Lit un bloc à la position pos sur le disque
  * @param virtual_disk_t
  * @param block_t
  * @param uint
  * @param integer
  * @return integer
**/
int read_block(virtual_disk_t *RAID5, block_t *recup, uint pos, int idDisk){
  fseek(RAID5[idDisk], (long) pos, SEEK_SET);
  size_t lu = fread(recup, sizeof(block_t), 1, RAID5->storage[idDisk]);
  if (lu != sizeof(block_t)) {
    return 1;
  }
  return 0;
}

///\brief utiliser "fseek", "fwrite" et "fread"
void block_repair(virtual_disk_t RAID5, uint pos, int idDisk);

void affichageBlockHexa(block_t donnees, FILE *output);

int main(void){
	init_disk_raid5("./RAIDFILES");
	info_disque();
  turn_off_disk_raid5("./RAIDFILES");
	exit(0);
}
