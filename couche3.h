#ifndef COUCHE3_H
#define COUCHE3_H

#include "couche2.h"

void read_inodes_table(virtual_disk_t *r5Disk, inode_t maTable[10]);
void write_inodes_table(virtual_disk_t *r5Disk, inode_table_t inode);
void delete_inode(virtual_disk_t *r5Disk, int numInode);
int get_unused_inodes(inode_table_t tab);
int get_nb_files(inode_table_t tab);
inode_t init_inode(char nomFichier[FILENAME_MAX_SIZE], uint taille, uint start);
void cmd_dump_inode(char *nomRep, virtual_disk_t *r5Disk);
void write_super_block(virtual_disk_t *r5Disk, super_block_t superblock);
void read_super_block(virtual_disk_t *r5Disk, super_block_t *superblock);
void first_free_byte(virtual_disk_t *r5Disk);
void read_inode_table_i(virtual_disk_t *r5Disk, inode_t *Ino, int indice);

#endif
