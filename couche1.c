#include <stdio.h>
#include <stdlib.h>
#include "raid_defines.h"
#include <string.h>
#include <fts.h>
#include <errno.h>

//Global raid disk variable
virtual_disk_t *r5Disk;

void init_disk_raid5(const char * repertoire){
    size_t lengthRep = strlen(repertoire);
    char *nomDisque = malloc(sizeof(char)*lengthRep+10);  //Creation d'une chaine pouvant contenir [repertoire]+10 caracteres
    strcpy(nomDisque, repertoire);
    nomDisque[lengthRep] = '/';
    nomDisque[lengthRep+1] = 'd';
    nomDisque[lengthRep+2] = '0';
    nomDisque[lengthRep+3] = '\0';
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
    strcpy(nomDisque, repertoire);
    nomDisque[lengthRep] = '/';
    nomDisque[lengthRep+1] = 'd';
    nomDisque[lengthRep+2] = '0';
    nomDisque[lengthRep+3] = '\0';
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


///\brief renvoie un code d'erreur si echec de lecture
int read_block(virtual_disk_t RAID5, block_t *recup, uint pos, int idDisk);

///\brief utiliser "fseek", "fwrite" et "fread"
void block_repair(virtual_disk_t RAID5, uint pos, int idDisk);

void affichageBlockHexa(block_t donnees, FILE *output);

int main(void){
	init_disk_raid5("./RAIDFILES");
	info_disque();
	exit(0);
}
