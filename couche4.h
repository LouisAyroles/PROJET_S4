#ifndef COUCHE4_H
#define COUCHE4_H

#include "couche3.h"

void write_file(virtual_disk_t *r5Disk, char *NomFichier, file_t fichier);
int read_file(virtual_disk_t *r5Disk, char *NomFichier, file_t fichier);
int delete_file(virtual_disk_t *r5Disk,char *NomFichier);
void load_file_from_host(virtual_disk_t *r5Disk, char *NomFichier);
void store_file_to_host(virtual_disk_t *r5Disk, char *NomFichier);

#endif
