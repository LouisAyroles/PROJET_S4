#ifndef COUCHE3_H
#define COUCHE3_H

#include "couche2.h"

void read_inodes_table(virtual_disk_t *r5Disk, inode_table_t *table);
void write_inodes_table();
void delete_inodes_table();
void get_unused_inodes();
inode_t init_inode(char nomFichier[FILENAME_MAX_SIZE], uint taille, uint start);
void cmd_dump_inode();
void write_super_block();
void read_super_block();
void first_free_byte();

#endif
