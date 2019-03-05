#ifndef COUCHE3_H
#define COUCHE3_H

#include "couche2.h"

void read_inodes_table();
void write_inodes_table();
void delete_inodes_table();
void get_unused_inodes();
int init_inode(char nomFichier[FILENAME_MAX_SIZE], int taille, int start);
void cmd_dump_inode();
void write_super_block();
void read_super_block();
void first_free_byte();

#endif
