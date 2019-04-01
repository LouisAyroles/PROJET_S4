#include "couche1.h"
#include "couche2.h"
#include <stdlib.h>
#include <math.h>

/**
 * \file couche2.c
 * \brief Programme couche2 du raid5.
 * \author Groupe14
 * \version 0.1
 * \date 24 fevrier 2019
 *
 * Programme de la couche 2 du raid5.
 *
 */

 /** \brief
   * Retourne le nombre de bandes necessaires pour écrire n blocs
   * @param : Nombre de blocs (int)
   * @return Nombre de bandes (int)
 **/

 int compute_nstripe(virtual_disk_t *r5Disk,int nblocks){
   //Dans le cas du raid5 , une bande = n-1 blocs avec n le nombre de disques
   //une bande = n blocs, avec n-1 blocs de données et 1 bloc de parité
   //Une bande contient donc n-1 bloc des n blocs de données à stocker
   return ((nblocks/((r5Disk->ndisk)-1))+(nblocks%((r5Disk->ndisk)-1) == 0));
 }

/** \brief
  * Calcule le bloc de parité d'une stripe
  * @param : virtual_disk_t *, stripe_t *,int
  * @return block_t
**/
block_t compute_parity(virtual_disk_t *r5, stripe_t *tocompute, int indice_parite){
  block_t retour=tocompute->stripe[(indice_parite+1)%(tocompute->nblocks)];
  for(int i=0;i<tocompute->nblocks;i++){
    if(i!=indice_parite && i!=((indice_parite+1)%(tocompute->nblocks))){
      xorbl(&retour,&(tocompute->stripe[i]),&retour);
    }
  }
  return retour;
}

/** \brief
  * Calcule la position du block de parité d'une stripe
  * @param : virtual_disk_t , int numero de la bande dans la liste du raid
  * @return int
**/
int compute_parity_index(virtual_disk_t *r5,int numbd){
  if (numbd % r5->ndisk == 0){
    return r5->ndisk-1;
  }
  return ((r5->ndisk-1)-(numbd%r5->ndisk));
  //return abs(((r5->ndisk)-numbd-1)%(r5->ndisk));
}

/** \brief
  * Ecrit une stripe a la position passée en argument sur le raid passé en argument
  * @param : virtual_disk_t ,stripe_t ,int
  * @return void
**/
void write_stripe(virtual_disk_t *r5, stripe_t *ecrire, uint pos){
  for(int i=0;i<r5->ndisk;i++){
    write_block(r5, &(ecrire->stripe[i]), pos, i);
  }
}


stripe_t *init_bande(virtual_disk_t *r5){
  stripe_t *bande = malloc(sizeof(stripe_t));
  bande->nblocks = r5->ndisk;
  bande->stripe = malloc(sizeof(block_t)*r5->ndisk);
  return bande;
}

/** \brief
  * Libere la mémoir réservée par une bande
  * @param : stripe_t *
  * @return void
**/
void delete_bande(stripe_t **bande){
  free((*bande)->stripe);
  free(*bande);
  *bande = NULL;
}

/** \brief
  * Affiche une bande
  * @param : virtual_disk_t ,stripe_t
  * @return void
**/
void print_stripe(virtual_disk_t *r5,stripe_t *stripe){
  block_t current;
  char nbHexa[BLOCK_SIZE*2];
  for(int i=0;i<r5->ndisk;i++){
    current=stripe->stripe[i];
    octetsToHexa(current, nbHexa);
    for(int i=0; i<BLOCK_SIZE*2; i++){
      fprintf(stdout, "[%c]", nbHexa[i]);
    }
    printf(" ");
  }
  printf("\n");
}

/** \brief
  * Ecrit une stripe a la position passée en argument sur le raid passé en argument
  * @param : virtual_disk_t ,stripe_t ,int
  * @return void
**/
void write_chunk(virtual_disk_t *r5, char *buffer, int n, uint startBlock){
  int nbBlocks = compute_nblock(n);
  int nbBandes = compute_nstripe(r5,nbBlocks);
  int pos = 0;
  block_t bloc[nbBlocks];
  for(int i = 0; i<nbBlocks; i++){
    for(int j = 0; j<BLOCK_SIZE; j++){
      if (pos<n){
        bloc[i].data[j] = buffer[pos];
        pos++;
      }
      else{
        bloc[i].data[j] = 0;
      }
    }
  }
  //Pos sert maintenant de numdeParité pour les bandes.
  pos = 0;
  stripe_t *bande=init_bande(r5);
  for(int i = 0; i <= nbBandes; i++){   //On parcourt le nbBandes
    pos = compute_parity_index(r5, i); //On recupere l'indice de parite
    int x=0;
    for(int j = 0; j <= r5->ndisk - 1 ; j++){ //On ajoute à la bande les blocs
        if (j != pos){
          bande->stripe[j]=bloc[(i*(r5->ndisk - 1))+j-x]; // ajout de bloc de donnees
        } // Fin IF
        else{
          x=1;
        }
    } // Fin FOR j
    bande->stripe[pos] = compute_parity(r5, bande, pos);
    write_stripe(r5, bande, startBlock*4); // ecriture de la bande sur disque
    startBlock+=1;  // Decalage du block de depart de un block
  } // Fin FOR i
  delete_bande(&bande);
}
/*pas pris en compte l'eventualité de commencer au milieu d'une bande*/

int afficher_raid_hexa(virtual_disk_t *r5){
  int retour=0;
  int z=0;
  while(retour!=1){
    for(int i=0;i<r5->ndisk;i++){
      retour=affichageBlockHexa(r5,i,z,stdout);
      if(retour){
        return(retour);
      }
      printf(" ");
    }
    printf("\n");
    z=z+4;
  }
  return 0;
}

/** \brief
  * affiche les disques du raid en valeurs décimales
  * @param : virtual_disk_t *
  * @return int
**/
int afficher_raid_decimal(virtual_disk_t *r5){
  int retour=0;
  int z=0;
  while(retour!=1){
    for(int i=0;i<r5->ndisk;i++){
      retour=affichageBlockDecimal(r5,i,z,stdout);
      if(retour){
        return(retour);
      }
      printf(" ");
    }
    printf("\n");
    z=z+4;
  }
  return 0;
}


/** \brief
  * fonction de test pour write_chunk
  * @param virtual_disk_t *
**/
void cmd_test1(virtual_disk_t *r5){
  unsigned char buffer[256];
  afficher_raid_decimal(r5);
  for(int i=0;i<256;i++){
    buffer[i]=i;
  }
  write_chunk(r5,buffer,256,0);
  printf("\n\n");
  afficher_raid_hexa(r5);
}


/** \brief
  * Lis une bande a partir de la position passée en parametre
  * @param : virtual_disk_t *,stripe_t *, uint
  * @return int
**/
int read_stripe(virtual_disk_t *r5, stripe_t *lire, uint pos){
  int retour=0;
  for(int i=0;i<r5->ndisk;i++){
    retour=read_block(r5, &(lire->stripe[i]), pos, i);
    if(retour){
      printf("Erreur lecture , arrêt\n");
      return 1;
    }
  }
  return 0;
}

/** \brief
  * Fonction de test pour read_chunk
  * @param : virtual_disk_t *
  * @return void
**/
void cmd_test2(virtual_disk_t *r5){
  unsigned char buffer[256];
  afficher_raid_hexa(r5);
  read_chunk(r5,0,48);
  printf("\n\n");
  turn_off_disk_raid5(r5);
}

/** \brief
  * Fonction retournant le num de la bande actuelle
  * @param : virtual_disk_t * le systeme
  * @param : int le numéro du block actuel
  * @return : int le numero de la bande
**/
int compute_num_bande(virtual_disk_t *r5,int nbloc){
  return nbloc/r5->ndisk;
}

/** \brief
  * Fonction de lecture de tableau de char
  * @param : virtual_disk_t * , uint , int
  * @return char *
**/
char *read_chunk(virtual_disk_t *r5, uint start_block, int n){
  uint nbBlocks = compute_nblock(n);
  unsigned char *buffer=malloc(sizeof(unsigned char)*n);
  int indice_buffer=0;
  int current=start_block;
  int nbBlocksLus = 0;              /*Nombre de Blocks effectivement lus (sans lire les blocs de parité)*/
  block_t *blc=malloc(sizeof(struct block_s));
  printf("Lecture de chunk : [start]:%d ,[end]:%d ,[size]:%d\n",start_block,start_block+n,compute_nblock(n));
  while((uint) nbBlocksLus<nbBlocks){
    printf("\nCurrent block :%d\n",current);
    if(current%(r5->ndisk)!=compute_parity_index(r5, compute_num_bande(r5,current))){
      printf("Reading position %d at disk %d\n",(current/4)*4,current%r5->ndisk);
      read_block(r5,blc,(current/4)*4,current%r5->ndisk);
      for(int i=0; i<BLOCK_SIZE; i++){
        buffer[indice_buffer]=blc->data[i];
        printf("%d ",blc->data[i]);
        indice_buffer=indice_buffer+1;
      }
      nbBlocksLus++;
    }
    current++;
  }
  free(blc);
  return buffer;
}


int couche2(void){
  //couche1();
  virtual_disk_t *r5d=malloc(sizeof(virtual_disk_t));
  init_disk_raid5("./RAIDFILES",r5d);
  cmd_test1(r5d);
  printf("\nCMD_TEST2\n");
  cmd_test2(r5d);
  return 0;
}
