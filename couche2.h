#include "couche1.h"

#ifndef COUCHE2_H
#define COUCHE2_H

int compute_nstripe(int nblocks);
block_t compute_parity(virtual_disk_t *r5,stripe_t *tocompute,int indice_parite);
int compute_parity_index(virtual_disk_t *r5,int numbd);
void write_stripe(virtual_disk_t *r5,stripe_t *ecrire,uint pos);
void write_chunk(virtual_disk_t *r5, char *buffer, int n, uint startBlock);
char *read_chunk(virtual_disk_t *r5, uint start_block, int n);
void delete_bande(stripe_t **bande);
void init_bande(stripe_t *bande);
void read_stripe(virtual_disk_t *r5, stripe_t *lire, uint pos)

#endif