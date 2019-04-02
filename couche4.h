#ifndef COUCHE4_H
#define COUCHE4_H

#include "couche3.h"

void write_file(virtual_disk_t *r5Disk, char *nomFichier, file_t fichier);
int read_file(virtual_disk_t *r5Disk, char *nomFichier, file_t *fichier);
int delete_file(virtual_disk_t *r5Disk,char *nomFichier);
void load_file_from_host(virtual_disk_t *r5Disk, char *nomFichier);
void store_file_to_host(virtual_disk_t *r5Disk, char *nomFichier);

#endif
