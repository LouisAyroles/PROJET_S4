#include <stdio.h>
#include <stdlib.h>


// typedef struct virtual_disk_s {
//     int number_of_files;
//     super_block_t super_block;
//     inode_table_t inodes;// tableau
//     int ndisk;
//     enum raid raidmode; // type de RAID
//     FILE **storage; //tab[NUMBER_OF_DISKS];
// } virtual_disk_t;

void init_disk_raid5(virtual_disk_t *diskRaid)

///\brief calcule le nombre de blocs pour stocker n octets
int compute_nblock(int n, int nbDisk)
{
    return (n/4+(!(n%4 == 0)));
}

void write_block(virtual_disk_t RAID5, block_t entrant, uint pos, int idDisk)

///\brief renvoie un code d'erreur si echec de lecture
int read_block(virtual_disk_t RAID5, block_t *recup, uint pos, int idDisk)

///\brief utiliser "fseek", "fwrite" et "fread"
void block_repair(virtual_disk_t RAID5, uint pos, int idDisk)

void affichageBlockHexa(block_t donnees, FILE *output)

