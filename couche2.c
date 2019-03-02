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
void delete_bande(stripe_t *bande){
  free(bande->stripe);
  free(bande);
  bande = NULL;
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

int afficher_raid_hexa(virtual_disk_t *r5){
  int retour=0;
  int z=0;
  while(retour!=1){
    for(int i=0;i<r5->ndisk;i++){
      retour=affichageBlockHexa(r5,i,z,stdout);
      if(retour==1){
        return(1);
      }
      printf(" ");
    }
    printf("\n");
    z=z+4;
  }
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
      if(retour==1){
        return(1);
      }
      printf(" ");
    }
    printf("\n");
    z=z+4;
  }
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
  afficher_raid_decimal(r5);
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
    }
  }
}

/** \brief
  * Fonction de test pour read_chunk
  * @param : virtual_disk_t *
  * @return void
**/
void cmd_test2(virtual_disk_t *r5){
  unsigned char buffer[256];
  afficher_raid_hexa(r5);
  read_chunk(r5,0,256);
  printf("\n\n");
  turn_off_disk_raid5(r5);
}


char *read_chunk(virtual_disk_t *r5, uint start_block, int n){
  uint nbBlocks = compute_nblock(n);
  unsigned char *buffer=malloc(sizeof(unsigned char)*n);
  int indice_buffer=0;
  int current=start_block;
  block_t *blc=malloc(sizeof(struct block_s));
  buffer = (char*)malloc(sizeof(char)*n);
  char nbHexa[BLOCK_SIZE*2];
  unsigned char shuffle;
  while((uint) current<=start_block+nbBlocks+nbBlocks/3-1){
    if((current-start_block)%r5->ndisk!=compute_parity_index(r5,(current-current%4)/4)){
      if(current%4==0){
        read_block(r5,blc,current,0);
      }
      else if(current%4==1){
        read_block(r5,blc,current-1,1);
      }
      else if(current%4==2){
        read_block(r5,blc,current-2,2);
      }
      else if(current%4==3){
        read_block(r5,blc,current-3,3);
      }
      octetsToHexa(*blc, nbHexa);
      for(int i=0; i<BLOCK_SIZE*2; i=i+2){
        shuffle=conversionDec(nbHexa[i+1]);
        shuffle=shuffle+16*conversionDec(nbHexa[i]);
        buffer[indice_buffer]=shuffle;
        indice_buffer=indice_buffer+1;
      }
    }
    else{
      //printf("Ligne : %d , parité : %d , numbloc : %d\n",(current-current%4)/4,compute_parity_index(r5,(current-current%4)/4),current-start_block);
    }
    current=current+1;
  }
  return buffer;
}

void main(void){
  //couche1();
  virtual_disk_t *r5d=malloc(sizeof(virtual_disk_t));
  init_disk_raid5("./RAIDFILES",r5d);
  cmd_test1(r5d);
  printf("\nCMD_TEST2\n");
  cmd_test2(r5d);
}
