#ifndef COUCHE2_H
#define COUCHE2_H

#include "couche1.h"

int compute_nstripe(virtual_disk_t *r5Disk,int nblocks);
block_t compute_parity(virtual_disk_t *r5, stripe_t *tocompute, int indice_parite);
int compute_parity_index(virtual_disk_t *r5,int numbd);
void write_stripe(virtual_disk_t *r5, stripe_t *ecrire, uint pos);
stripe_t *init_bande(virtual_disk_t *r5);
void delete_bande(stripe_t **bande);
void print_stripe(virtual_disk_t *r5,stripe_t *stripe);
void write_chunk(virtual_disk_t *r5, char *buffer, int n, uint startBytes);
void cmd_test1(virtual_disk_t *r5);
int read_stripe(virtual_disk_t *r5, stripe_t *lire, uint pos);
void cmd_test2(virtual_disk_t *r5);
int compute_num_bande(virtual_disk_t *r5,int nbloc);
void read_chunk(virtual_disk_t *r5, char *buffer, int n, uint startBande);
void dump_stripe(stripe_t bande);
void affichageSysteme(virtual_disk_t *RAID5);

#endif
