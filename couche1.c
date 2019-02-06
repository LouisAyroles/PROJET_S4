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

@param void
@return void
**/
void add_finChemin(const char * repertoire, char * nomDisque, size_t lengthRep){
    strcpy(nomDisque, repertoire);
    nomDisque[lengthRep] = '/';
    nomDisque[lengthRep+1] = 'd';
    nomDisque[lengthRep+2] = '0';
    nomDisque[lengthRep+3] = '\0';
}

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

void info_disque(){
	printf("---- Informations sur le disque raid ----\nNombre de disques physiques : %d\nMode de raid : %d\n",r5Disk->ndisk ,r5Disk->raidmode);
	for(int i=0;i<r5Disk->ndisk;i++){
		if(r5Disk->storage[i]==NULL){
			printf("Null pointer\n");
		}else{printf("Valid pointer\n");}
	}
}

///\brief calcule le nombre de blocs pour coder "n" octets
int compute_nblock(int n)
{
    int nbBlocks;
    nbBlocks = n/4;
    if (n%4!=0)
    {
        nbBlocks += 1;
    }
    return nbBlocks;
    //Magnifique
}

void write_block(virtual_disk_t *RAID5, block_t entrant, uint pos, int idDisk){
    RAID5->storage[idDisk] = entrant;
}

///\brief renvoie un code d'erreur si echec de lecture
int read_block(virtual_disk_t RAID5, block_t *recup, uint pos, int idDisk);

///\brief utiliser "fseek", "fwrite" et "fread"
void block_repair(virtual_disk_t RAID5, uint pos, int idDisk);

void affichageBlockHexa(block_t donnees, FILE *output);

int main(void){
	init_disk_raid5("./RAIDFILES");
	info_disque();
  turn_off_disk_raid5("./RAIDFILES");
	exit(0);
}
