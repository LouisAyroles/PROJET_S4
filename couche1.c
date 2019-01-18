#include <stdio.h>
#include <stdlib.h>


void init_disk_raid5(virtual_disk_t *diskRaid)

///\brief calcule le nombre de blocs pour coder "n" octets
int compute_nblock(int n)
{
    return (n/4+(n%4!=0));
}

void write_block(virtual_disk_t RAID5, block_t entrant, uint pos, int idDisk)

///\brief renvoie un code d'erreur si echec de lecture
int read_block(virtual_disk_t RAID5, block_t *recup, uint pos, int idDisk)

///\brief utiliser "fseek", "fwrite" et "fread"
void block_repair(virtual_disk_t RAID5, uint pos, int idDisk)

void affichageBlockHexa(block_t donnees, FILE *output)

