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
   return ((nblocks/((r5Disk->ndisk)-1))+(nblocks%((r5Disk->ndisk)-1) != 0));
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
  * Ecrit n bytes du buffer a partir de StartBandes le raid passé en argument
  * @param : virtual_disk_t ,buffer* ,int, int
  * @return void
**/
void write_chunk(virtual_disk_t *r5, char *buffer, int n, uint startBande){
  int nbBlocks = compute_nblock(n);
  int nbBandes = compute_nstripe(r5,nbBlocks);
  stripe_t *stripe = init_bande(r5);
  int indice_data = 0;
  int parity_index;
  for (int i = 0; i < nbBandes; i++){
    parity_index = compute_parity_index(r5, startBande+i);
    for(int j = 0; j < r5->ndisk; j++){
      for(int k = 0; k < BLOCK_SIZE; k++){
        if(indice_data < n && j != parity_index){
          stripe->stripe[j].data[k] = buffer[indice_data];
          indice_data+=1;
        }else{
          stripe->stripe[j].data[k] = 0;
        }
      }
    }
    stripe->stripe[parity_index] = compute_parity(r5, stripe, parity_index);
    write_stripe(r5, stripe, startBande+i);
  }
  delete_bande(&stripe);
}


/** \brief
  * Affiche une bande
  * @param : stripe_t
  * @return void
**/
void dump_stripe(stripe_t bande){
  for(int i = 0; i<bande.nblocks; i++){
    for(int j = 0; j < BLOCK_SIZE; j++){
      printf("%d ", bande.stripe[i].data[j]);
    }
    printf("\n");
  }
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
  * Fonction retournant le num de la bande actuelle
  * @param : virtual_disk_t * le systeme
  * @param : int le numéro du block actuel
  * @return : int le numero de la bande
**/
int compute_num_bande(virtual_disk_t *r5,int nbloc){
  return nbloc/r5->ndisk;
}


/** \brief
  * Lit n bytes a partir de StartBandes sur le raid passé en argument et les renvoie
  *dans le buffer
  * @param : virtual_disk_t ,stripe_t ,int
  * @return void
**/
void read_chunk(virtual_disk_t *r5, char *buffer, int n, uint startBande){
  int nbBlocks = compute_nblock(n);
  int nbBandes = compute_nstripe(r5,nbBlocks);
  int parity_index;
  int indice_data = 0;
  stripe_t *mystripe = init_bande(r5);
  for (int i = 0; i < nbBandes; i++) {
    read_stripe(r5, mystripe, startBande + i);
    parity_index = compute_parity_index(r5,startBande + i);
    for (int j = 0; j < r5->ndisk; j++) {
        for (int k = 0; k < BLOCK_SIZE; k++) {
          if (indice_data < n && j != parity_index) {
            buffer[indice_data] = mystripe->stripe[j].data[k];
            indice_data+=1;
          }
        }
      }
    }
  delete_bande(&mystripe);
}



/** \brief
  * fonction de test pour write_chunk
  * @param virtual_disk_t *
**/
void cmd_test1(virtual_disk_t *r5){
  unsigned char buffer[256];
  for(int i = 0; i<r5->ndisk; i++){
    affichageDisque(r5, i);
  }
  for(int i=0;i<256;i++){
    buffer[i]=i;
  }
  write_chunk(r5,buffer,256,0);
  printf("\n\n");
  for(int i = 0; i<r5->ndisk; i++){
    affichageDisque(r5, i);
  }
}


/** \brief
  * Fonction de test pour read_chunk
  * @param : virtual_disk_t *
  * @return void
**/
void cmd_test2(virtual_disk_t *r5){
  unsigned char buffer[256];
  read_chunk(r5,buffer,256,0);
  printf("\nAffichage du Buffer :\n\n");
  for (int i = 0; i < 256; i++) {
    printf("%d ", buffer[i]);
  }
}


void couche2(void){
  //couche1();
  virtual_disk_t *r5d=malloc(sizeof(virtual_disk_t));
  init_disk_raid5("./RAIDFILES",r5d);
  //cmd_test1(r5d);
  printf("\nCMD_TEST2\n");
  cmd_test2(r5d);
  turn_off_disk_raid5(r5d);
}
