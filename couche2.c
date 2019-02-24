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



void main(void){
  couche1();
}
