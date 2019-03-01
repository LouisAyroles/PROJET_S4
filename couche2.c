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
  //Il peut y avoir plus de ndisk bandes si le fichier est grand
  //if (numbd % ndisk == 0){
  //  return r5->ndisk-1;
  //}
  //return ((r5->ndisk-1)-(numbd%ndisk);
  return abs(((r5->ndisk)-numbd-1)%(r5->ndisk));
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
stripe_t *init_bande(virtual_disk_t *r5){
  stripe_t *bande = malloc(sizeof(stripe_t));
  bande->nblocks = r5->ndisk;
  bande->stripe = malloc(sizeof(block_t)*r5->ndisk);
  return bande;
}

// ???
void delete_bande(stripe_t *bande){
  free(bande->stripe);
  free(bande);
  bande = NULL;
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
  for(int i = 0; i < nbBandes; i++){   //On parcourt le nbBandes
    pos = compute_parity_index(r5, i); //On recupere l'indice de parite
    printf("%d\n",pos);
    int x=0;
    for(int j = 0; j <= r5->ndisk - 1 ; j++){ //On ajoute à la bande les blocs
        if (j != pos){
          bande->stripe[j]=bloc[(i*3)+j-x]; // ajout de bloc de donnees
        } // Fin IF
        else{
          x=1;
        }
    } // Fin FOR j
    bande->stripe[pos] = compute_parity(r5, bande, pos);
    write_stripe(r5, bande, startBlock*4); // ecriture de la bande sur disque
    startBlock+=1;  // Decalage du block de depart de un block
  } // Fin FOR i
  delete_bande(bande);
}

void afficher_raid(virtual_disk_t *r5){
  for(int z=0;z<=84;z=z+4){
    for(int i=0;i<r5->ndisk;i++){
      affichageBlockHexa(r5,i,z,stdout);
      printf(" ");
    }
    printf("\n");
  }
}

/** \brief
  * fonction de test pour write_chunk
  * NON TESTE
**/
void cmd_test1(virtual_disk_t *r5){
  printf("Test de compute parity\n");
  unsigned char buffer[256];
  afficher_raid(r5);
  for(int i=0;i<256;i++){
    buffer[i]=i;
  }
  write_chunk(r5,buffer,256,0);
  printf("\n\n");
  afficher_raid(r5);
  turn_off_disk_raid5(r5);
}


void read_stripe(virtual_disk_t *r5, stripe_t *lire, uint pos){
  for(int i=0;i<r5->ndisk;i++){
    read_block(r5, &(lire->stripe[i]), pos, i);
  }
}



// PAS FINI, FATIGUE
char *read_chunk(virtual_disk_t *r5, uint start_block, int n){
  uint nbBlocks = compute_nblock(n), pos = 0, nBande;
  char* buffer;
  stripe_t *bande=init_bande(r5);
  buffer = (char*)malloc(sizeof(char)*n);
  for (int i = 0; i < n; i+=4){
    nBande = start_block/r5->ndisk-1;
    pos = compute_parity_index(r5, nBande);
    start_block%r5->ndisk!=pos;
    start_block+=1;
  }
  delete_bande(bande);
}

void main(void){
  //couche1();
  virtual_disk_t *r5d=malloc(sizeof(virtual_disk_t));
  init_disk_raid5("./RAIDFILES",r5d);
  cmd_test1(r5d);
}
