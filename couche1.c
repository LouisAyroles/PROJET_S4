#include <stdio.h>
#include <stdlib.h>
#include "raid_defines.h"
#include "couche1.h"
#include <string.h>
#include <fts.h>
#include <errno.h>
#include <time.h>
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
void init_disk_raid5(const char * repertoire, virtual_disk_t *r5Disk){
    size_t lengthRep = strlen(repertoire);
    char *nomDisque = malloc(sizeof(char)*lengthRep+10);  /*Creation d'une chaine pouvant contenir [repertoire]+10 caracteres*/
    add_finChemin(repertoire, nomDisque, lengthRep);
    //r5Disk=malloc(sizeof(virtual_disk_t));
    r5Disk->ndisk=4;
    r5Disk->raidmode=CINQ;
    r5Disk->storage=malloc(r5Disk->ndisk*sizeof(FILE *));
    for (int i = 0; i < r5Disk->ndisk; i++){
        nomDisque[lengthRep+2] = i + '0';         /*Transforme le i en caractere et le met dans le "/di"*/
        r5Disk->storage[i]=fopen(nomDisque,"r+w");  /*Ouvre le fichier "disque" EN READ/WRITE*/
    }
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
    if (n%4!=0)
    {
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
  fseek(RAID5->storage[idDisk], (long)pos, SEEK_SET);
  int retour=-1;
  retour=fwrite(entrant->data, 1, BLOCK_SIZE, RAID5->storage[idDisk]);
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
  fseek(RAID5->storage[idDisk], (long) pos, SEEK_SET);
  size_t lu = fread(recup->data, 1, 4, RAID5->storage[idDisk]);
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

/** \brief
  * transforme un nombre en son chiffre en decimal
  * @param : nb4bits
  * @return : le chiffre en decimal
**/
int conversionDec(int nb4bits){
  switch(nb4bits){
    case 48:
      return 0;
    case 49:
      return 1;
    case 50:
      return 2;
    case 51:
      return 3;
    case 52:
      return 4;
    case 53:
      return 5;
    case 54:
      return 6;
    case 55:
      return 7;
    case 56:
      return 8;
    case 57:
      return 9;
    case 65:
      return 10;
    case 66:
      return 11;
    case 67:
      return 12;
    case 68:
      return 13;
    case 69:
      return 14;
    case 70:
      return 15;
    default:
      return nb4bits+'0';
  }
}



/** \brief
  * affiche un bloc de donnees en hexadecimal
  * @param : virtual_disk_t
  * @param : integer (n° disk)
  * @param : integer (posit° de ce qu'on veut afficher)
  * @return : void
**/
int affichageBlockHexa(virtual_disk_t *RAID5, int idDisk, uint pos, FILE *output){
  block_t monBloc;
  char nbHexa[BLOCK_SIZE*2];
  int retour;
  retour=read_block(RAID5, &monBloc, pos, idDisk);
  if(retour!=1){
    octetsToHexa(monBloc, nbHexa);
    for(int i=0; i<BLOCK_SIZE*2; i++){
      fprintf(output, "[%c]", nbHexa[i]);
    }
    return 0;
  }
  else{
    return 1;
  }
}

/** \brief
  * Fonction d'affichage d'un block qui est dans le disk idDisk et commence a pos
  * @param : virtual_disk_t * , int , int, FILE*
  * @return int
**/
int affichageBlockDecimal(virtual_disk_t *RAID5, int idDisk, uint pos, FILE *output){
  block_t monBloc;
  char nbHexa[BLOCK_SIZE*2];
  int retour;
  retour=read_block(RAID5, &monBloc, pos, idDisk);
  unsigned char shuffle;
  if(retour!=1){
    octetsToHexa(monBloc, nbHexa);
    for(int i=0; i<BLOCK_SIZE*2; i=i+2){
      shuffle=conversionDec(nbHexa[i+1]);
      shuffle=shuffle+16*conversionDec(nbHexa[i]);
      fprintf(output, "[%d]", shuffle);
    }
    return 0;
  }
  else{
    return 1;
  }
}

/** \brief
  * Fonction d'affichage du contenu du disk idDisk sur output
  * @param : virtual_disk_t * , int , FILE *
  * @return void
**/
void affichageDisque(virtual_disk_t *RAID5, int idDisk,FILE *output){
  for(int i=0;i<=16;i=i+4){
    affichageBlockHexa(RAID5,idDisk,i,stdout);
    printf("\n");
  }
  printf("\n");
}

int couche1(void){
  virtual_disk_t *r5Disk;
  char hexa[10];
  r5Disk=malloc(sizeof(virtual_disk_t));
  srand(time(NULL));
	init_disk_raid5("./RAIDFILES", r5Disk);
	info_disque(r5Disk);
  block_t ecrire, lire;
  ecrire.data[0] = 42;
  ecrire.data[1] = 140;
  ecrire.data[2] = 39;
  ecrire.data[3] = 131;
  write_block(r5Disk, &ecrire, 4, 0);
  read_block(r5Disk, &lire, 4, 0);
  octetsToHexa(lire, hexa);
  for(int i = 0; i<8; i++){
    printf("%c", hexa[i]); // resultat attendu: 2A 8C 27 83
  }
  printf("\n");
  //affichageBlockHexa(r5Disk,0,4,stdout);
  affichageDisque(r5Disk,0,stdout);
  //affichageDisque(r5Disk,1,stdout);
  //affichageDisque(r5Disk,2,stdout);
  //affichageDisque(r5Disk,3,stdout);
  turn_off_disk_raid5(r5Disk);
	return 0;
}
