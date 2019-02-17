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
  //perror("Debugging fwrite:");
  //printf("written:%d elements\n",retour);
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
  //perror("Debugging read:");
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

/*
void block_repair(virtual_disk_t *RAID5, uint pos, int idDisk){
  //convertir l'hexa en bits ou recuperer les bits de chaque bloc directement
  //creer un tableau de taille nbdisk qui recevra les iemes bits de chaque disk
  //creer un tableau de 32 cases qui va recevoir les bits reparés
  for(int i = 0; i < 32; i++)
  {
    for(int j = 0; j < RAID5->ndisk; j++)
    {
      // ieme[j]=  bit du disk j
    }
    //repare[i]=xor(RAID5,ieme)
  }
}
*/
void xorbl(block_t *xa,block_t *xb,block_t *destination){
  for(int i=0;i<BLOCK_SIZE;i++){
    destination->data[i]=(xa->data[i])^(xb->data[i]);
  }
}

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
/*
void block_repair(virtual_disk_t *RAID5, uint pos, int idDisk){
  //convertir l'hexa en bits ou recuperer les bits de chaque bloc directement
  //creer un tableau de taille nbdisk qui recevra les iemes bits de chaque disk
  //creer un tableau de 32 cases qui va recevoir les bits reparés
  for(int i = 0; i < 32; i++)
  {
    for(int j = 0; j < RAID5->ndisk; j++)
    {
      // ieme[j]=  bit du disk j
    }
    //repare[i]=xor(RAID5,ieme)
  }
}
*/

/** \brief
  * Prend un tableau d'int (representant des bits) et renvoie le xor de ces valeurs
  * @param virtual_disk_t
  * @param integer (nb disk)
  * @return int
**/
int xor(virtual_disk_t *RAID5, int *tab){
  for(int i = 0; i < (RAID5->ndisk); i++)
  {
    tab[0]=tab[0] ^ tab[i];
  }
  return tab[0];

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
    // QUESTION: Dans quel sens on ecrit dans le tableau d'hexadecimal?
    // Comme je l'ai fait ça ecrit dans la case 0 et 1 l'hexadecimal de l'octet
    // 0, 2 et 3 l'hexadecimal de l'octet 1, etc... en mettant en premier l'hexa
    // des bits de poids fort.
    // Comme ça quand on print de 0 à BLOCK_SIZE*2 (le nombre de chiffre en
    // hexadecimal pour ecrire BLOCK_SIZE octets) ça affiche dans le sens de
    // lecture.
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
  * affiche un bloc de donnees en hexadecimal
  * @param : virtual_disk_t
  * @param : integer (n° disk)
  * @param : integer (posit° de ce qu'on veut afficher)
  * @return : void
**/
void affichageBlockHexa(virtual_disk_t *RAID5, int idDisk, uint pos, FILE *output){
  block_t monBloc;
  char nbHexa[BLOCK_SIZE*2];
  read_block(RAID5, &monBloc, pos, idDisk);
  octetsToHexa(monBloc, nbHexa);
  for(int i=0; i<BLOCK_SIZE*2; i++){
    fprintf(output, "[%c]", nbHexa[i]);
  }
  fprintf(output,"\n");
}

void affichageDisque(virtual_disk_t *RAID5, int idDisk,FILE *output){
  for(int i=0;i<8;i++){
    affichageBlockHexa(RAID5,idDisk,i,stdout);
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
  affichageBlockHexa(r5Disk,0,4,stdout);
  affichageDisque(r5Disk,0,stdout);
  turn_off_disk_raid5("./RAIDFILES", r5Disk);
	return 0;
}
