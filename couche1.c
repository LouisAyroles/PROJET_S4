#include <stdio.h>
#include <stdlib.h>
#include "raid_defines.h"
#include <string.h>
#include <fts.h>
#include <errno.h>
#include <time.h>


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
    strcpy(nomDisque, repertoire);    // strcpy: fonction copiant la chaine repertoire dans nomDisque
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
    char *nomDisque = malloc(sizeof(char)*lengthRep+10);  //Creation d'une chaine pouvant contenir [repertoire]+10 caracteres
    add_finChemin(repertoire, nomDisque, lengthRep);
    //r5Disk=malloc(sizeof(virtual_disk_t));
    r5Disk->ndisk=4;
    r5Disk->raidmode=CINQ;
    r5Disk->storage=malloc(r5Disk->ndisk*sizeof(FILE *));
    for (int i = 0; i < r5Disk->ndisk; i++){
        nomDisque[lengthRep+2] = i + '0';         //Transforme le i en caractere et le met dans le "/di"
        r5Disk->storage[i]=fopen(nomDisque,"r+w");  //Ouvre le fichier "disque" EN READ/WRITE
    }
}



/** \brief
  * Ferme les fichiers ouverts et sauvegarde le super block?
  * @param : chaine de char (repertoire cible)
  * @param : virtual_disk_t
  * @return void
**/
void turn_off_disk_raid5(const char * repertoire, virtual_disk_t *r5Disk){
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
	printf("---- Informations sur le disque raid ----\nNombre de disques physiques : %d\nMode de raid : %d\n",r5Disk->ndisk ,r5Disk->raidmode);
	for(int i=0;i<r5Disk->ndisk;i++){
		if(r5Disk->storage[i]==NULL){
			printf("Null pointer\n");
		}else{printf("Valid pointer\n");}
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
  for (int i = 0; i < BLOCK_SIZE; i++){
    fprintf(stdout, "%d:%d\n", i, entrant->data[i]);
  }
  int retour=-1;
  retour=fwrite(entrant->data, 1, 4, RAID5->storage[idDisk]);
  perror("Debugging fwrite:");
  printf("write:%d\n",retour);
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
  perror("Debugging read:");
  if (lu != sizeof(block_t)) {
    return 1;
  }
  return 0;
}

/** \brief
  * Repare un bloc erroné suite à un echec de lecture 
  * @param virtual_disk_t
  * @param uint 
  * @param integer (n° disk)
  * @return void
**/
void block_repair(virtual_disk_t *RAID5, uint pos, int idDisk){
  block_t monBloc;
  for(int i = 0; i < RAID5->nbdisk; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      //xor(a,b,c,d)= 1 ssi une seule des 4 variables est à 1.
    }
    
  }
  
}

/** \brief
  * prend un tableau de 4 bits (char) et le transforme en Hexadecimal
  * @param : block_t (Contient le tableau de bits)
  * @param : char* (Caractere dans lequel on met l'hexa)
  * @return : void
**/
void bitToHexa(block_t monBloc, char* nbHexa){
  int nb = 0;
  if(monBloc.data[BLOCK_SIZE-1]){
      nb+=8;
  }
  if(monBloc.data[BLOCK_SIZE-2]){
      nb+=4;
  }
  if(monBloc.data[BLOCK_SIZE-3]){
      nb+=2;
  }
  if(monBloc.data[BLOCK_SIZE-4]){
      nb+=1;
  }
  switch(nb){
    case 10 : *nbHexa='A';
              break;
    case 11:  *nbHexa='B';
              break;
    case 12 : *nbHexa='C';
              break;
    case 13 : *nbHexa='D';
              break;
    case 14 : *nbHexa='E';
              break;
    case 15 : *nbHexa='F';
              break;
    default:  *nbHexa=nb+'0';
  }
}

/** \brief
  * affiche un bloc de donnees en hexadecimal
  * @param : virtual_disk_t
  * @param : integer (n° disk)
  * @param : integer (posit° de ce qu'on veut afficher)
  * @return : void
**/
void affichageBlockHexa(virtual_disk_t *RAID5, int idDisk, uint pos, FILE *output){
  fprintf(output,"---Block:\n");
  block_t monBloc;
  char nbHexa;
  read_block(RAID5, &monBloc, pos, idDisk);
  bitToHexa(monBloc, &nbHexa);
  fprintf(output, "%c", nbHexa);
  fprintf(output,"\n\n");
}

int main(void){
  virtual_disk_t *r5Disk;
  r5Disk=malloc(sizeof(virtual_disk_t));
  srand(time(NULL));
	init_disk_raid5("./RAIDFILES", r5Disk);
	info_disque(r5Disk);
  block_t ecrire;
  for (int i=0; i< 4; i++){
    ecrire.data[i]=rand()%2;
  }
  write_block(r5Disk, &ecrire, 0, 0);
  for(int i=0;i<4;i++){
    affichageBlockHexa(r5Disk,i,0,stdout);
  }
  //system("hexdump ./RAIDFILES/d0");
  turn_off_disk_raid5("./RAIDFILES", r5Disk);
	exit(0);
}
