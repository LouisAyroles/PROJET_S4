#include "couche1.h"
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
  return (nblocks/(r5Disk->ndisk))+1;
}

/** \brief
  * Calcule le bloc de parité d'une stripe
  * @param : virtual_disk_t *, stripe_t *,int
  * @return block_t
**/
block_t compute_parity(virtual_disk_t *r5,stripe_t *tocompute,int indice_parité){
  block_t retour=tocompute->stripe[(indice_parité+1)%(tocompute->nblocks)];
  for(int i=0;i<tocompute->nblocks;i++){
    if(i!=indice_parité && i!=((indice_parité+1)%(tocompute->nblocks))){
      retour=xorbl(&retour,&(tocompute->stripe[i]),&retour);
      }
    }
  }
}


void main(void){
  couche1();
}
