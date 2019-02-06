#ifndef COUCHE1_H
#define COUCHE1_H

void add_finChemin(const char * repertoire, char * nomDisque, size_t lengthRep);
void init_disk_raid5(const char * repertoire);
void turn_off_disk_raid5(const char * repertoire);
void info_disque();
int compute_nblock(int n);
void write_block(virtual_disk_t *RAID5, block_t entrant, uint pos, int idDisk);
int read_block(virtual_disk_t RAID5, block_t *recup, uint pos, int idDisk);
void block_repair(virtual_disk_t RAID5, uint pos, int idDisk);
void affichageBlockHexa(block_t donnees, FILE *output);

#endif
