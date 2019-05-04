#ifndef COUCHE3_H
#define COUCHE3_H

#include "couche2.h"

void read_inodes_table(virtual_disk_t *r5Disk, inode_t maTable[10]);
void write_inodes_table(virtual_disk_t *r5Disk, inode_t maTable[10]);
void delete_inode(virtual_disk_t *r5Disk, int numInode);
int get_unused_inodes(virtual_disk_t *r5Disk);
int get_nb_files(virtual_disk_t *r5Disk);
inode_t init_inode(char nomFichier[FILENAME_MAX_SIZE], uint taille, uint start);
void cmd_dump_inode(char *nomRep, virtual_disk_t *r5Disk);
void write_super_block(virtual_disk_t *r5Disk, super_block_t superblock);
void read_super_block(virtual_disk_t *r5Disk, super_block_t *superblock);
int first_free_byte(virtual_disk_t *r5Disk);
void read_inode_table_i(virtual_disk_t *r5Disk, inode_t *Ino, int indice);
void inode_table_dump(inode_table_t t);
void inode_dump(inode_t i);
void super_block_dump(super_block_t s);
void update_super_block(virtual_disk_t* r5Disk);
void add_inode(virtual_disk_t *r5Disk, char nomFICHIER[50], int size);
void reinit_systeme(virtual_disk_t* r5Disk);

#endif
