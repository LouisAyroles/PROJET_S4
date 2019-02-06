#include <stdio.h>
#include <stdlib.h>
#include "raid_defines.h"
#include <string.h>
#include <fts.h>

//Global raid disk variable 
virtual_disk_t *r5Disk;

void init_disk_raid5(const char * repertoire){
	r5Disk=malloc(sizeof(virtual_disk_t));
	r5Disk->ndisk=4;
	r5Disk->raidmode=CINQ;
	r5Disk->storage=malloc(r5Disk->ndisk*sizeof(FILE *));
	//Ouverture des fichiers de disque
	//A réparer et rendre modulaire
	r5Disk->storage[0]=fopen("./RAIDFILES/d0","r");
	r5Disk->storage[1]=fopen("./RAIDFILES/d1","r");
	r5Disk->storage[2]=fopen("./RAIDFILES/d2","r");
        r5Disk->storage[3]=fopen("./RAIDFILES/d3","r");
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

void write_block(virtual_disk_t RAID5, block_t entrant, uint pos, int idDisk);

///\brief renvoie un code d'erreur si echec de lecture
int read_block(virtual_disk_t RAID5, block_t *recup, uint pos, int idDisk);

///\brief utiliser "fseek", "fwrite" et "fread"
void block_repair(virtual_disk_t RAID5, uint pos, int idDisk);

void affichageBlockHexa(block_t donnees, FILE *output);

void main(void){
	init_disk_raid5("./RAIDFILES");
	info_disque();

	exit(0);
}
