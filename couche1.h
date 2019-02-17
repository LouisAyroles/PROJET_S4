#ifndef COUCHE1_H
#define COUCHE1_H

#include "raid_defines.h"

void add_finChemin(const char * repertoire, char * nomDisque, size_t lengthRep);
void init_disk_raid5(const char * repertoire, virtual_disk_t *r5Disk);
void turn_off_disk_raid5(const char * repertoire, virtual_disk_t *r5Disk);
void info_disque(virtual_disk_t *r5Disk);
int compute_nblock(int n);
void write_block(virtual_disk_t *RAID5, block_t *entrant, uint pos, int idDisk);
int read_block(virtual_disk_t *RAID5, block_t *recup, uint pos, int idDisk);
void block_repair(virtual_disk_t *RAID5, uint pos, int idDisk);
void octetsToHexa(block_t monBloc, char* nbHexa);
void affichageBlockHexa(virtual_disk_t *RAID5, int idDisk, uint pos, FILE *output);
char conversionHexa(char nb4bits);

#endif
