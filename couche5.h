#ifndef LOUIS_H
#define LOUIS_H

#include "couche4.h"

void lecture(char cmd[], int taille);
int interpretation(virtual_disk_t* r5Disk, char cmd[CMD_MAX_SIZE]);
int executer(virtual_disk_t* r5Disk, char argument[], int codeCmd);
int IsCommand(char commande[20]);
int executerLS(virtual_disk_t* r5Disk, char option[]);
int executerCAT(virtual_disk_t* r5Disk, char argument[]);
int executerRM(virtual_disk_t* r5Disk, char argument[]);
int executerCREATE(virtual_disk_t* r5Disk, char argument[]);
int executerEDIT(virtual_disk_t* r5Disk, char argument[]);
int executerLOAD(virtual_disk_t* r5Disk, char argument[]);
int executerSTORE(virtual_disk_t* r5Disk, char argument[]);
int executerQUIT(virtual_disk_t* r5Disk, char argument[]);
int executerHELP(char argument[]);
int executerRESET(virtual_disk_t* r5Disk, char argument[]);
file_t supprimerLignei(file_t fichier, int noligne);
void printEditionfichier();
void printFichier(file_t fichier, char nomFichier[]);
void repair_disk(virtual_disk_t *r5,int idDisk);
void cmd_format(char rep[], int nbdisk, int disksize);
void format(char *dirname, int size, int diskid);
void interpreteur(char *rep);
#endif
