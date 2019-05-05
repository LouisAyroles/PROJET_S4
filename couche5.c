#include "couche1.h"
#include "couche2.h"
#include "couche3.h"
#include "couche4.h"
#include "couche5.h"
#include "raid_defines.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
/**
 * \file couche5.c
 * \brief Programme couche 5 du raid5.
 * \author Groupe14
 * \version 0.1
 * \date 22 avril 2019
 *
 * Programme de la couche 5 du raid5.
 *
 */



/** \brief
  * Recupere la chaine de charactere de stdin
  * @param : commande[] la ou on stock commande en question
  * @return void
**/
void lecture(char cmd[], int taille){
  fgets(cmd,CMD_MAX_SIZE,stdin);
  int tailleCMD = strlen(cmd);
  if (tailleCMD == CMD_MAX_SIZE-1) {
    while (getchar() !='\n');
  }else{
    cmd[tailleCMD-1]='\0';
  }
}

/** \brief
  * Interprete la chaine recupere par fgets
  * @param : commande[] la commande en question
  * @return int -1 si echec
**/
int interpretation(virtual_disk_t* r5Disk, char cmd[CMD_MAX_SIZE]){
  char commande[20];
  char argument[FILENAME_MAX_SIZE];
  int i = 0, j =0;
  int codeCmd;
  //Recup de la commande.
  while(cmd[i] != '\0' && cmd[i] != ' '){
    commande[i] = cmd[i];
    i++;
  }
  commande[i] = '\0';

  //Si la commande est valide
  if((codeCmd=IsCommand(commande)) != 0){
    //Si il y a un argument ou option
    if (cmd[i] != '\0'){
      //On ignore les espaces potentiels en trop
      while(cmd[i] == ' '){
        i++;
      }
      //On copie l'argument/Option
      while(cmd[i] != '\0'){
        argument[j]= cmd[i];
        i++;
        j++;
      }
      argument[j] = '\0';
    }
    //Si il n'y a pas un argument ou option
    else{
      argument[0] = '\0';
    }
    return executer(r5Disk, argument, codeCmd);
  } else{
      printf("\033[31;49mCommande invalide. \033[39;49m help pour les commandes disponibles.\n");
    return -1;
  }
}

/** \brief
  * Execute la commande correspondante au code
  * @param : argument[] et codeCmd
  * @return int
**/
int executer(virtual_disk_t* r5Disk, char argument[], int codeCmd) {
  if (codeCmd == LS) {
    return executerLS(r5Disk,argument);
  }else if (codeCmd == CAT) {
    return executerCAT(r5Disk,argument);
  }else if (codeCmd == RM) {
    return executerRM(r5Disk,argument);
  }else if (codeCmd == CREATE) {
    return executerCREATE(r5Disk,argument);
  }else if (codeCmd == EDIT) {
    return executerEDIT(r5Disk,argument);
  }else if (codeCmd == LOAD) {
    return executerLOAD(r5Disk,argument);
  }else if (codeCmd == STORE) {
    return executerSTORE(r5Disk,argument);
  }else if (codeCmd == QUIT) {
    return executerQUIT(r5Disk,argument);
  }else if (codeCmd == RESET) {
    return executerRESET(r5Disk,argument);
  }else if (codeCmd == HELP) {
    return executerHELP(argument);
  }
}

/** \brief
  * Verifie la commande passee en parametre
  * @param : commande[20] la commande en question
  * @return code commande.
**/
int IsCommand(char commande[20]){
  if(!strcmp(commande, "ls")){
    return LS;
  }else if(!strcmp(commande, "cat")){
    return CAT;
  }else if(!strcmp(commande, "rm")){
    return RM;
  }else if(!strcmp(commande, "create")){
    return CREATE;
  }else if(!strcmp(commande, "edit")){
    return EDIT;
  }else if(!strcmp(commande, "load")){
    return LOAD;
  }else if(!strcmp(commande, "store")){
    return STORE;
  }else if(!strcmp(commande, "quit")){
    return QUIT;
  }else if(!strcmp(commande, "reset")){
    return RESET;
  }else if(!strcmp(commande, "help")){
    return HELP;
  }
  return 0;
}

int executerHELP(char arg[]){
  if (arg[0] == '\0') {
    printf("\033[33;49m\tls [-l]\033[39;49m : Liste le contenu du catalogue. Argument optionnel pour un affichage long.\n");
    printf("\033[33;49m\tcat <nom de fichier> \033[39;49m: Affiche à l’ecran le contenu du fichier passé en argument.\n");
    printf("\033[33;49m\trm <nom de fichier> \033[39;49m: Supprime le fichier passé en argumet du Systeme RAID.\n");
    printf("\033[33;49m\tcreate <nom de fichier> \033[39;49m: Crée un nouveau fichier de nom l'argument sur le Systeme RAID.\n");
    printf("\033[33;49m\tedit <nom de fichier> \033[39;49m:  ́Edite le fichier passé en argumet du Systeme RAID.\n");
    printf("\033[33;49m\tload <nom de fichier> \033[39;49m: Copie le contenu du fichier dont le nom est passé en argumet du systeme ”hote” vers le systeme RAID.\n");
    printf("\033[33;49m\tstore <nom  de  fichier> \033[39;49m: Copie le contenu du fichier dont le nom est passé en argumet du systeme RAID vers le systeme ”hote”.\n");
    printf("\033[33;49m\tquit \033[39;49m: Quitte l’interprete de commande et met fin au programme.\n");
    printf("\033[33;49m\treset \033[39;49m: Réinitialisation du systeme.\n");
    return 0;
  }else{
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : help\n");
    return -1;
  }
}

/** \brief
  * Execute un LS sur le systeme
  * @param : option[] de la commande en question
  * @return int
**/
int executerLS(virtual_disk_t* r5Disk, char option[]){
  inode_t maTable[10];
  read_inodes_table(r5Disk, maTable);
  //On execute LS
  if (option[0] == '\0'){
    for (int i = 0; i < get_nb_files(r5Disk); i++) {
      printf("%s\t",maTable[i].filename);
      if (i == get_nb_files(r5Disk)-1) {
        printf("\n");
      }
    }
    return 0;
    //On execute ls -l
  }else if (option[0] == '-' && option[1] == 'l') {
    for (int i = 0; i < get_nb_files(r5Disk); i++) {
      if (maTable[i].size == 0) {
        printf("\033[34;49m%-20s\033[39;49m\t %-5d octets\t\t %-5d blocks\t\t  *eme Bande \n",maTable[i].filename, maTable[i].size, maTable[i].nblock);
      }else{
        printf("\033[34;49m%-20s\033[39;49m\t %-5d octets\t\t %-5d blocks\t\t %deme Bande\n",maTable[i].filename, maTable[i].size, maTable[i].nblock, maTable[i].first_byte);
      }
    }
    return 0;
  //Commande n'est pas valide
  }else{
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : ls [-l]\n");
    return -1;
  }
}

/** \brief
  * Execute un CAT sur le systeme
  * @param : argument[] de la commande en question
  * @return int
**/
int executerCAT(virtual_disk_t* r5Disk, char argument[]){
  inode_t maTable[INODE_TABLE_SIZE];
  file_t fichier;
  read_inodes_table(r5Disk, maTable);
  //Si l'argument est vide, on renvoie une erreur
  if (argument[0] == '\0') {
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : cat <nomFichier>\n");
    return -1;
  }else{
    //Le fichier n'est pas dans le systeme
    if (!read_file(r5Disk, argument, &fichier)) {
      printf("\033[31;49mLe fichier %s n'est pas présent sur le systeme.\033[39;49m\n", argument);
      return -1;
    //Le fichier est présent
    }else{
      printf("%s",fichier.data);
      return 0;
    }
  }
}

/** \brief
  * Execute un RM sur le systeme
  * @param : argument
  * @return int
**/
int executerRM(virtual_disk_t* r5Disk, char argument[]){
  //Si l'argument est vide, on renvoie une erreur
  if (argument[0] == '\0') {
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : rm <nomFichier>\n");
    return -1;
  }else{
    delete_file(r5Disk,argument);
    return 0;
  }
}

/** \brief
  * Execute un CREATE sur le systeme
  * @param : argument[] de la commande en question
  * @return int
**/
int executerCREATE(virtual_disk_t* r5Disk, char argument[]){
  if (argument[0] == '\0') {
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : create <nomFichier>\n");
    return -1;
  }else{
    add_inode(r5Disk, argument, 0);
    return 0;
  }
}




/** \brief
  * Execute un EDIT sur le systeme
  * @param : argument[] de la commande en question
  * @return int
**/
int executerEDIT(virtual_disk_t* r5, char filename[]){
  if (filename[0] == '\0'){
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : edit <nomFichier>\n");
    return -1;
  }
  file_t fichier;
  int i=0;
  while (i < INODE_TABLE_SIZE && !strcmp(r5->inodes[i].filename,filename)){
    i++;
  }
  if (i != INODE_TABLE_SIZE) {
    if (!read_file(r5,filename,&fichier)) {
      printf("\033[31;49mLe fichier %s n'a pas pu etre ouvert.\033[39;49m\n", filename);
      return -1;
    }
  }else{
    printf("\033[31;49mLe fichier %s n'est pas présent sur le systeme.\033[39;49m\n", filename);
    return -1;
  }
  char *st=fichier.data;
  system("touch f.temp");
  FILE *f = fopen("f.temp", "w");
  fputs(st,f);
  fclose(f);
  pid_t pid;
  switch(pid=fork()){
    case -1:
      perror("Fork");
    case 0:
      execl("/bin/nano","nano","f.temp",(char *)NULL);
      exit(0);
    default:
      wait(NULL);
  }
  f = fopen("./f.temp", "r");
  file_t add;
  char c=getc(f);
  for(i=0;i<MAX_FILE_SIZE;i++){
    if(c=='\0' || c==-1)
      break;
    add.data[i]=c;
    c=getc(f);
  }
  fclose(f);
  add.data[i-1]='\0';
  add.size=i;
  delete_file(r5,filename);
  write_file(r5,filename,add);
  inode_table_t matable[INODE_TABLE_SIZE];
  read_inodes_table(r5,(inode_t *) matable);
  system("rm ./f.temp");
  return 0;
}


/** \brief
  * Supprime la ieme ligne d'un fichier
  * @param : file_t et int
  * @return int
**/
file_t supprimerLignei(file_t fichier, int noligne){
  int debutLigne=0, nbligne=0, tailleLigne=0, finLigne;
  //On se positionne apres le '\n' souhaité dans le tableau data
  while (nbligne < noligne-1 && debutLigne < fichier.size) {
    if (fichier.data[debutLigne] == '\n'){
      nbligne++;
    }
    debutLigne++;
  }
  finLigne = debutLigne;
  //On recupere la taille de la ligne
  while (fichier.data[finLigne] != '\n' && finLigne < fichier.size) {
    tailleLigne++;
    finLigne++;
  }
  for (int i = 0; i < tailleLigne; i++) {
      fichier.data[debutLigne+i] = fichier.data[finLigne+i];
  }
  fichier.size -= tailleLigne;
  return fichier;
}




/** \brief
  * Execute un LOAD sur le systeme
  * @param : argument[] de la commande en question
  * @return int
**/
int executerLOAD(virtual_disk_t* r5Disk, char argument[]){
  if (argument[0] == '\0') {
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : load <nomFichier>\n");
    return -1;
  }else{
    load_file_from_host(r5Disk,argument);
    return 0;
  }
}

/** \brief
  * Execute un LOAD sur le systeme
  * @param : argument[] de la commande en question
  * @return int
**/
int executerSTORE(virtual_disk_t* r5Disk, char argument[]){
  if (argument[0] == '\0') {
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : store <nomFichier>\n");
    return -1;
  }else{
    store_file_to_host(r5Disk, argument);
    return 0;
  }
}


/** \brief
  * Execute un QUIT sur le systeme
  * @param : argument[] de la commande en question
  * @return int
**/
int executerQUIT(virtual_disk_t* r5Disk, char argument[]){
  if (argument[0] == '\0') {
    turn_off_disk_raid5(r5Disk);
    return FIN;
  }else{
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : quit\n");
    return -1;
  }
}

/** \brief
  * Execute un RESET sur le systeme
  * @param : argument[] de la commande en question
  * @return int
**/
int executerRESET(virtual_disk_t* r5Disk, char argument[]){
  if (argument[0] == '\0') {
    reinit_systeme(r5Disk);
    return 0;
  }else{
    printf("\033[31;49mCommande invalide. \033[39;49mUsage : reset\n");
    return -1;
  }
}


/** \brief
  * Affiche le disque réparé
  * @param : virtual_disk_t*, int
  * @return int
**/
void demo_repair(virtual_disk_t *r5,int idDisk){
  stripe_t *stripe = init_bande(r5);
  block_t repaired;
  printf("\033[31;49m    Disque 0                Disque 1                Disque 2                Disque 3                Disque 1 réparé\n\033[39;49m");
  int i=0;
  while(!read_stripe(r5,stripe,i)) {
    repaired=block_repair(r5,i,idDisk);
    printf("\033[34;49m %-3d\033[39;49m",i);
    dump_stripe(*stripe);
    printf("   [");
    for(int z=0;z<BLOCK_SIZE;z++){
      printf(" %d",repaired.data[z]);
    }
    printf(" ]");
    printf("\n");
    i++;
  }
  delete_bande(&stripe);
}



/** \brief
  * Repare le disque
  * @param : virtual_disk_t*, int
  * @return int
**/
void repair_disk(virtual_disk_t *r5,int idDisk){
  char cmd[100];cmd[0]='\0';
  strncat(cmd,"cp ./RAIDFILES/d",16);
  char repair[20];
  snprintf(repair, 10,"%d ",(idDisk+1)%r5->ndisk);
  strncat(cmd,repair,sizeof(repair));
  snprintf(repair, 20,"./RAIDFILES/d%d",idDisk);
  strncat(cmd,repair,sizeof(repair));
  printf("Commande : %s\n",cmd);
  system(cmd);
  r5->storage[idDisk]=fopen(repair,"r+b");
  block_t repaired;
  block_t test_lecture;
  int i=0;
  while(!read_block(r5,&test_lecture,i,(idDisk-1)%r5->ndisk)){
    repaired=block_repair(r5,i,idDisk);
    write_block(r5,&repaired,i,idDisk);
    i++;
  }
  update_super_block(r5);
}

/** \brief
  * Repare le disque
  * @param : virtual_disk_t*, int
  * @return int
**/
void cmd_repair(const char *path,int idDisk, int nbDisque){
  virtual_disk_t *r5=init_disk_raid5(path, nbDisque);
  char cmd[100];cmd[0]='\0';
  strncat(cmd,"cp ./RAIDFILES/d",16);
  char repair[20];
  snprintf(repair, 10,"%d ",(idDisk+1)%r5->ndisk);
  strncat(cmd,repair,sizeof(repair));
  snprintf(repair, 20,"./RAIDFILES/d%d",idDisk);
  strncat(cmd,repair,sizeof(repair));
  printf("Commande : %s\n",cmd);
  system(cmd);
  r5->storage[idDisk]=fopen(repair,"r+b");
  block_t repaired;
  block_t test_lecture;
  int i=0;
  while(!read_block(r5,&test_lecture,i,(idDisk-1)%r5->ndisk)){
    repaired=block_repair(r5,i,idDisk);
    write_block(r5,&repaired,i,idDisk);
    i++;
  }
  update_super_block(r5);
}

/** \brief
  * Retourne -1 si tout les disques sont présents ou le numero du disque absent
  * @param : virtual_disk_t*, int
  * @return int
**/
int detect_missing_disk(virtual_disk_t *r5,char *repertoire){
  FILE *buffer;
  size_t lengthRep = strlen(repertoire);
  char *nomDisque = malloc(sizeof(char)*(lengthRep+10));  /*Creation d'une chaine pouvant contenir [repertoire]+10 caracteres*/
  add_finChemin(repertoire, nomDisque, lengthRep);
  for (int i = 0; i < r5->ndisk; i++){
      nomDisque[lengthRep+2] = i + '0';
      buffer=fopen(nomDisque,"r+b");
      if(!buffer){
        return i;
      }
  }
  return -1;
}

void interpreteur(char *rep) {
  char cmd[CMD_MAX_SIZE];
  virtual_disk_t *r5d;
  char logfile[20];logfile[0]='\0';
  int disqueavant;
  int nbdisque;
  strcat(logfile, rep);
  strcat(logfile,"/log");
  FILE* fd;
  fd = fopen(logfile,"a");
  fclose(fd);
  fd = fopen(logfile,"r+");
  fseek(fd, 0, SEEK_END);
  int taille = ftell(fd);
  fseek(fd,0,SEEK_SET);
  if (taille == 0) {
    printf("\033[32;49m[RAID5]:\033[39;49m");
    printf("Combien de disques ? [3-10] : ");
    fscanf(stdin, "%d", &nbdisque);
    while(nbdisque < 3 || nbdisque > 10){
      while (getchar() != '\n');
      printf("Saisie invalide! Combien de disques ? : [3-10]\n");
      fscanf(stdin, "%d", &nbdisque);
    }
    while (getchar() != '\n');
    virtual_disk_t *r5d=init_disk_raid5(rep,nbdisque);
    fwrite(&(r5d->ndisk),sizeof(int),1,fd);
    update_super_block(r5d);
    system("clear");
    do{
      printf("\033[32;49m[RAID5]:\033[39;49m");
      lecture(cmd, CMD_MAX_SIZE);
    }while ( interpretation(r5d,cmd) != FIN );
  }else{
    fread(&disqueavant,sizeof(int),1,fd);
    printf("Nombre de disques de la derniere utilisation du Systeme : %d\n",disqueavant);
    printf("Combien de disques ? [3-10] : ");
    fscanf(stdin, "%d", &nbdisque);
    while(nbdisque < 3 || nbdisque > 10){
      while (getchar() != '\n');
      printf("Saisie invalide! Combien de disques ? : [3-10]\n");
      fscanf(stdin, "%d", &nbdisque);
    }
    while (getchar() != '\n');
    if (nbdisque != disqueavant) {
      cmd_format(rep, nbdisque, 5000);
      r5d=init_disk_raid5(rep,nbdisque);
      fseek(fd,0 , SEEK_SET);
      fwrite(&(r5d->ndisk),sizeof(int),1,fd);
      update_super_block(r5d);
      system("clear");
      do{
        printf("\033[32;49m[RAID5]:\033[39;49m");
        lecture(cmd, CMD_MAX_SIZE);
      }while ( interpretation(r5d,cmd) != FIN );
    }else{
      r5d=init_disk_raid5(rep,nbdisque);
      fseek(fd,0 , SEEK_SET);
      fwrite(&(r5d->ndisk),sizeof(int),1,fd);
      update_super_block(r5d);
      system("clear");
      do{
        printf("\033[32;49m[RAID5]:\033[39;49m");
        lecture(cmd, CMD_MAX_SIZE);
      }while ( interpretation(r5d,cmd) != FIN );
    }
  }
  fclose(fd);
}

void format(char *dirname, int size, int diskid){
  char filename[strlen(dirname)+4];
  snprintf(filename, strlen(dirname)+4, "%s/d%d", dirname, diskid);
  printf("%s\n", filename);
  FILE *fp = fopen(filename, "w+");
  unsigned char zero=0;
  for(int i=0; i<size; i++) {
    int nb_write = fwrite(&zero, 1, 1, fp);
  }
  fclose(fp);
}

void cmd_format(char rep[], int nbdisk, int disksize) {
    for(int diskid=0; diskid<nbdisk; diskid++) {
	     format(rep, disksize, diskid);
    }
}


int main(int argc, char const *argv[]) {
  if (argc !=2) {
    printf("Usage : %s [NomRep]\n",argv[0]);
  }else{
    system("clear");
    interpreteur((char*)argv[1]);
  }
  return 0;
}
