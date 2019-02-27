#include "couche1.h"
#include "couche2.h"
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
   //Dans le cas du raid5 , une bande = n-1 blocs avec n le nombre de disques Nop ?
   //une bande = n blocs, avec n-1 blocs de données et 1 bloc de parité
   //Une bande contient donc n-1 bloc des n blocs de données à stocker
   return (nblocks/((r5Disk->ndisk)-1))+(nblocks%((r5Disk->ndisk)-1) == 0);
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
  * @param : virtual_disk_t , int
  * @return int
**/
int compute_parity_index(virtual_disk_t *r5,int numbd){
  //Il peut y avoir plus de ndisk bandes si le fichier est grand
  //if (numbd % ndisk == 0){
  //  return 0;
  //}
  //return ((r5->ndisk)-(numbd%ndisk);
  return ((r5->ndisk)-numbd-1);
}

/** \brief
  * Ecrit une stripe a la position passée en argument sur le raid passé en argument
  * @param : virtual_disk_t ,stripe_t ,int
  * @return void
**/
void write_stripe(virtual_disk_t *r5,stripe_t *ecrire,uint pos){
  for(int i=0;i<r5->ndisk;i++){
    write_block(r5, &(ecrire->stripe[i]), pos, i);
  }
}

// Bande gérée dynamiquement ????
void init_bande(stripe_t *bande){
  bande = malloc(sizeof(stripe_t));
  bande->nblocks = NB_DISK;
  bande->stripe = malloc(sizeof(block_t)*NB_DISK);
  return bande;
}

// ???
void delete_bande(stripe_t **bande){
  free(*bande->stripe);
  free(*bande);
  *bande = NULL;
}

/** \brief
  * Ecrit une stripe a la position passée en argument sur le raid passé en argument
  * @param : virtual_disk_t ,stripe_t ,int
  * @return void
**/
void write_chunk(virtual_disk_t *r5, char *buffer, int n, uint startBlock){
  int nbBlocks = compute_nblock(n);
  int nbBandes = compute_nstripe(nbBlocks);
  int pos = 0;
  block_t bloc[nbBlocks];
  stripe_t bande;
  for(int i = 0; i<nbBlocks; i++){
    for(int j = 0; j<BLOCK_SIZE; j++){
      if (pos<n){
        bloc[i][j] = char[pos];
        pos++;
      }
      else{
        block[i][j] = 0;
      }
    }
  }
  //Pos sert maintenant de numdeParité pour les bandes.
  pos = 0;
  init_bande(&bande);
  for(int i = 0; i < nbBandes; i++){
    pos = compute_parity_index(r5, i);
    for(int j = 0; j < r5->ndisk - 1){
        if (j != pos){
          bande->stripe[j]=bloc[(i*3)+j];
        }
        else{
          band->stripe[j] = compute_parity(r5, bande, pos);
        }
    }
  }
}



void main(void){
  couche1();
}
