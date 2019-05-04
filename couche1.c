#include <stdio.h>
#include <stdlib.h>
#include "raid_defines.h"
#include "couche1.h"
#include <string.h>
#include <fts.h>
#include <errno.h>
#include <stdbool.h>

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

/** \brief
  * Copie la chaine "repertoire" dans nomDisque en y ajoutant "/d0\0"
  * @param : chaine de char (repertoire)
  * @param : chaine de char (disk)
  * @param : size_t
  * @return void
**/
void add_finChemin(const char * repertoire, char * nomDisque, size_t lengthRep){
    strcpy(nomDisque, repertoire);    /*strcpy: fonction copiant la chaine repertoire dans nomDisque*/
    nomDisque[lengthRep] = '/';
    nomDisque[lengthRep+1] = 'd';
    nomDisque[lengthRep+2] = '0';
    nomDisque[lengthRep+3] = '\0';
}


/** \brief
  * Initialise la variable globale r5Disk
  * @param : chaine de char (repertoire cible)
  * @param : virtual_disk_t
  * @return void
**/
virtual_disk_t* init_disk_raid5(const char * repertoire){
    size_t lengthRep = strlen(repertoire);
    char *nomDisque = malloc(sizeof(char)*(lengthRep+10));  /*Creation d'une chaine pouvant contenir [repertoire]+10 caracteres*/
    add_finChemin(repertoire, nomDisque, lengthRep);
    virtual_disk_t* r5Disk=malloc(sizeof(virtual_disk_t));
    r5Disk->ndisk=4;
    r5Disk->raidmode=CINQ;
    r5Disk->storage=malloc(r5Disk->ndisk*sizeof(FILE *));
    for (int i = 0; i < r5Disk->ndisk; i++){
        nomDisque[lengthRep+2] = i + '0';         /*Transforme le i en caractere et le met dans le "/di"*/
        r5Disk->storage[i]=fopen(nomDisque,"r+b");  /*Ouvre le fichier "disque" EN READ/WRITE*/
    }
    free(nomDisque);
    return r5Disk;
}



/** \brief
  * Ferme les fichiers ouverts et sauvegarde le super block?
  * @param : chaine de char (repertoire cible)
  * @param : virtual_disk_t
  * @return void
**/
void turn_off_disk_raid5(virtual_disk_t *r5Disk){
    for (int i = 0; i < r5Disk->ndisk; i++){
        fclose(r5Disk->storage[i]);
    }
    free(r5Disk->storage);
    free(r5Disk);
}

/** \brief
  * Affiche des infos sur les disques ouverts
  * @param : virtual_disk_t
  * @return : void
**/
void info_disque(virtual_disk_t *r5Disk){
	printf("---- Informations sur le disque raid ----\nNombre de disques physiques : %d\nMode de raid : %s\n",r5Disk->ndisk ,nomRaid[r5Disk->raidmode]);
	for(int i=0;i<r5Disk->ndisk;i++){
		if(r5Disk->storage[i]==NULL){
			printf("Null pointer\n");
		}else{printf("Valid pointer disque d%d\n", i);}
	}
}

/** \brief
  * calcule le nombre de blocs pour coder "n" octets
  * @param integer
  * @return integer
**/
int compute_nblock(int n)
{
    int nbBlocks;
    nbBlocks = n/4;
    if (n%4!=0){
        nbBlocks += 1;
    }
    return nbBlocks;
}

/** \brief
  * Ecrit un bloc à la position pos sur le disque
  * @param virtual_disk_t
  * @param block_t (à ecrire)
  * @param uint (position à laquelle on ecrit)
  * @param integer (n° disk)
  * @return void
**/
void write_block(virtual_disk_t *RAID5, block_t *entrant, uint pos, int idDisk){
  fseek(RAID5->storage[idDisk], (long)pos*BLOCK_SIZE, SEEK_SET);
  int retour = fwrite(entrant->data, 1, BLOCK_SIZE, RAID5->storage[idDisk]);
}



/** \brief
  * Lit un bloc à la position pos sur le disque
  * @param virtual_disk_t
  * @param block_t (à lire)
  * @param uint (position à laquelle on lit)
  * @param integer (n° disk)
  * @return integer
**/
int read_block(virtual_disk_t *RAID5, block_t *recup, uint pos, int idDisk){
  fseek(RAID5->storage[idDisk], (long) pos*BLOCK_SIZE, SEEK_SET);
  size_t lu = fread(recup->data, 1, BLOCK_SIZE, RAID5->storage[idDisk]);
  if (lu != sizeof(block_t)) {
    return 1;
  }
  return 0;
}

/** \brief
  * Fait un xor entre 2 block et renvoie le resultat dans destination
  * @param : block_t, block_t, block_t
  * @return void
**/
void xorbl(block_t *xa,block_t *xb,block_t *destination){
  for(int i=0;i<BLOCK_SIZE;i++){
    destination->data[i]=(xa->data[i])^(xb->data[i]);
  }
}


/** \brief
  * Fonction de reparation d'un block
  * @param : virtual_disk_t * , uint , int
  * @return void
**/
void block_repair(virtual_disk_t *RAID5,uint pos,int idDisk){
  uint iterateur=0;
  bool initialisation=false;
  block_t repare;
  block_t xorblock;
  while(iterateur<(RAID5->ndisk)){
    if(iterateur!=idDisk){
      if(!initialisation){
        read_block(RAID5,&repare,pos,iterateur);
        initialisation=true;
      }
      else{
        read_block(RAID5,&xorblock,pos,iterateur);
        xorbl(&repare,&xorblock,&repare);
      }
    }
    iterateur++;
  }
}



/** \brief
  * prend un tableau de 4 octets (char) et le transforme en Hexadecimal
  * assert(monBloc[i]<256);
  * @param : block_t (Contient le tableau de bits)
  * @param : char* (Caractere dans lequel on met l'hexa)
  * @return : void
**/
void octetsToHexa(block_t monBloc, char* nbHexa){
  char reste, diviseur;
  for (int i = 0; i < BLOCK_SIZE; i++){
    diviseur = monBloc.data[i] / 16;
    reste = monBloc.data[i] % 16;
    nbHexa[2*i+1]=conversionHexa(reste);
    nbHexa[2*i]=conversionHexa(diviseur);
  }
}

/** \brief
  * transforme un nombre en son chiffre en hexa
  * @param : nb4bits, la valeur de 4 bits en entier
  * @return : le chiffre en hexadecimal
**/
char conversionHexa(char nb4bits){
  switch(nb4bits){
    case 10:
      return 'A';
    case 11:
      return 'B';
    case 12:
      return 'C';
    case 13:
      return 'D';
    case 14:
      return 'E';
    case 15:
      return 'F';
    default:
      return nb4bits+'0';
  }
}

void affichageNoConversion(virtual_disk_t *RAID5, int idDisk, int n){
  int nbBlocs = compute_nblock(n);
  block_t lectoure;
  for(int i = 0; i < nbBlocs; i++){
    if(i!=0 && i%4 == 0){
      printf("\n");
    }
    read_block(RAID5, &lectoure, i, idDisk);
    printf("[");
    for(int j = 0; j < BLOCK_SIZE; j++){
      printf(" %-4d", lectoure.data[j]);
    }
    printf(" ]");
  }
  printf("\n");
}

/** \brief
  * Fonction d'affichage du contenu du disk idDisk sur output
  * @param : virtual_disk_t * , int , FILE *
  * @return void
**/
void affichageDisque(virtual_disk_t *RAID5, int idDisk){
  fseek(RAID5->storage[idDisk], 0, SEEK_END);
  int taille = (int)ftell(RAID5->storage[idDisk]);
  printf("Affichage disque %d:\n", idDisk);
  affichageNoConversion(RAID5, idDisk, taille);
}


void couche1(void) {
  virtual_disk_t *r5Disk =init_disk_raid5("./RAIDFILES");
  char hexa[10];

	info_disque(r5Disk);
  block_t ecrire, lire;


  ecrire.data[0] = 131;
  ecrire.data[1] = 39;
  ecrire.data[2] = 140;
  ecrire.data[3] = 42;
  /*ecrire.data[0] = 42;
  ecrire.data[1] = 140;
  ecrire.data[2] = 39;
  ecrire.data[3] = 131;*/
  write_block(r5Disk, &ecrire, 0, 0);
  write_block(r5Disk, &ecrire, 4, 0);
  write_block(r5Disk, &ecrire, 8, 0);
  ecrire.data[0] = 0;
  ecrire.data[1] = 0;
  ecrire.data[2] = 0;
  ecrire.data[3] = 0;
  write_block(r5Disk, &ecrire, 8, 0);
  read_block(r5Disk, &lire, 4, 0);
  printf("\n");
  for(int i = 0; i<r5Disk->ndisk; i++){
    affichageDisque(r5Disk,i);
    printf("\n");
  }

  turn_off_disk_raid5(r5Disk);
}
