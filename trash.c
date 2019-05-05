/** \brief
  * Lecture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
void read_inodes_table(virtual_disk_t *r5Disk, inode_table_t *table){
  printf("read_inodes_table\n");
  stripe_t **bandesLues;
  int nbBandes = compute_nstripe(r5Disk, INODE_SIZE), k, debutTable = startTable(r5Disk);
  char conversion[4];
  char *bufferInode;
  char *buffer = (char*)malloc(sizeof(char) * FILENAME_MAX_SIZE);
  bandesLues = (stripe_t **) malloc(sizeof(stripe_t)*nbBandes);

  for(int i = 0; i<nbBandes; i++){
    bandesLues[i] = init_bande(r5Disk);
  }
 for (int i = 0; i<INODE_TABLE_SIZE; i++){
    //Faire l'inode à chaque tour de i
    /* LECTURE DES BANDES CONSTITUANTS L'INODE */
    /*for (int j = 0; j<nbBandes; j++){ //Ici on lit toutes les bandes
      //BLOCK_SIZE*i*nbBandes*r5->ndisk = taille d'un inode sur disque * i
      //BLOCK_SIZE*j*r5Disk->ndisk = j * taille d'une bande
      read_stripe(r5Disk, bandesLues[j], j+nbBandes*i);
    }*/
    bufferInode=read_chunk(r5Disk, debutTable+i*nbBandes, INODE_OCT);

    /* RECUPERATION DES INFOS DEPUIS LES BANDES */
    //printf("Bande 1:\n");
    //print_stripe(r5Disk , bandesLues[0]);
    //printf("Bande 2:\n");
    //print_stripe(r5Disk , bandesLues[1]);
    //Recup du filename (pas de conversion)
    printf("DEBUG retour : contenu du filename n° %d: \n",i);
    for(k = 0; k < FILENAME_MAX_SIZE; k++){ //k allant de 0 à (2bandes/octets)-1
                                                      //avec BLOCK_SIZE = 4 et ndisk =4; 0<=k<32
      printf("BandesLues = %d, stripe = %d, data = %d, ndisk=%d, k=%d, i = %d\n", k/(BLOCK_SIZE*r5Disk->ndisk), (k/BLOCK_SIZE)%r5Disk->ndisk, k%BLOCK_SIZE, r5Disk->ndisk, k, i);
      buffer[k] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%(r5Disk->ndisk)].data[k%BLOCK_SIZE];
      printf("Modified\n");
      table[i]->filename[k] = buffer[k];
      //table[i]->filename[k] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%(r5Disk->ndisk)].data[k%BLOCK_SIZE];
      //test->stripe = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%(r5Disk->ndisk)];
      //printf("file = bande ? %d\n\n", table[i]->filename[k] == bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%(r5Disk->ndisk)].data[k%BLOCK_SIZE]);
      //k/(BLOCK_SIZE*r5Disk->ndisk) = indice de bande (0 puis 1)
      //(k/BLOCK_SIZE)%r5Disk->ndisk] = indice de block dans la bande (0-4)
      //k%BLOCK_SIZE = indice de l'octet dans le block (0-4)
    }
    printf("\ncontenu du filename n° %d: \n",i);
    for(k = 0; k < FILENAME_MAX_SIZE; k++){
      printf("|%c|",table[i]->filename[k]);
    }
    printf("OUT\n");
    //Recup de la size (conversion int)
    for (k = BLOCK_SIZE*r5Disk->ndisk*2; k < (BLOCK_SIZE*r5Disk->ndisk*2)+4; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    memcpy(&(table[i]->size), conversion, sizeof(table[i]->size));

    //Ou memcpy(&(table[i]->size), bandesLues[BLOCK_SIZE*r5Disk->ndisk*2/(BLOCK_SIZE*r5Disk->ndisk)]->
    //stripe[(BLOCK_SIZE*r5Disk->ndisk*2/BLOCK_SIZE)%r5Disk->ndisk]->data[BLOCK_SIZE*r5Disk->ndisk*2%BLOCK_SIZE],
    //sizeof(table[i]->size));

    //Recup du nblock(conversion int)
    for (k = (BLOCK_SIZE*r5Disk->ndisk*2)+4; k < (BLOCK_SIZE*r5Disk->ndisk*2)+8; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    memcpy(&(table[i]->nblock), conversion, sizeof(table[i]->nblock));

    //Recup du first_byte(conversion int)
    for (k = (BLOCK_SIZE*r5Disk->ndisk*2)+8; k < (BLOCK_SIZE*r5Disk->ndisk*2)+12; k++){
      conversion[k%BLOCK_SIZE] = bandesLues[k/(BLOCK_SIZE*r5Disk->ndisk)]->stripe[(k/BLOCK_SIZE)%r5Disk->ndisk].data[k%BLOCK_SIZE]; //Prob si on change BLOCK_SIZE
    }
    memcpy(&(table[i]->first_byte), conversion, sizeof(table[i]->first_byte));
    printf("memucypyOUT\n");
  }
  for(int i = 0; i<nbBandes; i++){
    delete_bande(&bandesLues[i]);
  }
  free(bandesLues);
  return;
}


/** \brief
  * Fonction d'affichage d'un block qui est dans le disk idDisk et commence a pos
  * @param : virtual_disk_t * , int , int, FILE*
  * @return int
**/
int affichageBlockDecimal(virtual_disk_t *RAID5, int idDisk, uint pos, FILE *output){
  block_t monBloc;
  char nbHexa[BLOCK_SIZE*2];
  int retour;
  retour=read_block(RAID5, &monBloc, pos, idDisk);
  unsigned char shuffle;
  if(retour!=1){
    octetsToHexa(monBloc, nbHexa);
    for(int i=0; i<BLOCK_SIZE*2; i=i+2){
      shuffle=conversionDec(nbHexa[i+1]);
      shuffle=shuffle+16*conversionDec(nbHexa[i]);
      fprintf(output, "[%d]", shuffle);
    }
    return 0;
  }
  else{
    return 1;
  }
}

/** \brief
  * transforme un nombre en son chiffre en decimal
  * @param : nb4bits
  * @return : le chiffre en decimal
**/
int conversionDec(int nb4bits){
  switch(nb4bits){
    case 48:
      return 0;
    case 49:
      return 1;
    case 50:
      return 2;
    case 51:
      return 3;
    case 52:
      return 4;
    case 53:
      return 5;
    case 54:
      return 6;
    case 55:
      return 7;
    case 56:
      return 8;
    case 57:
      return 9;
    case 65:
      return 10;
    case 66:
      return 11;
    case 67:
      return 12;
    case 68:
      return 13;
    case 69:
      return 14;
    case 70:
      return 15;
    default:
      return nb4bits+'0';
  }
}

/** \brief
  * affiche un bloc de donnees en hexadecimal
  * @param : virtual_disk_t
  * @param : integer (n° disk)
  * @param : integer (posit° de ce qu'on veut afficher)
  * @return : void
**/
int affichageBlockHexa(virtual_disk_t *RAID5, int idDisk, uint pos, FILE *output){
  block_t monBloc;
  char nbHexa[BLOCK_SIZE*2];
  int retour;
  retour=read_block(RAID5, &monBloc, pos, idDisk);
  if(retour!=1){
    octetsToHexa(monBloc, nbHexa);
    for(int i=0; i<BLOCK_SIZE*2; i++){
      fprintf(output, "[%c]", nbHexa[i]);
    }
    return 0;
  }
  else{
    return 1;
  }
}

/** \brief
  * Fonction d'affichage du contenu du disk idDisk sur output
  * @param : virtual_disk_t * , int , FILE *
  * @return void
**/
void affichageDisque(virtual_disk_t *RAID5, int idDisk,FILE *output){
  for(int i=0;i<=16;i=i+4){
    affichageBlockHexa(RAID5,idDisk,i,stdout);
    printf("\n");
  }
  printf("\n");
}


/** \brief
  * affiche les disques du raid en valeurs décimales
  * @param : virtual_disk_t *
  * @return int
**/
int afficher_raid_decimal(virtual_disk_t *r5){
  int retour=0;
  int z=0;
  while(retour!=1){
    for(int i=0;i<r5->ndisk;i++){
      retour=affichageBlockDecimal(r5,i,z,stdout);
      if(retour){
        return(retour);
      }
      printf(" ");
    }
    printf("\n");
    z=z+4;
  }
  return 0;
}

int afficher_raid_hexa(virtual_disk_t *r5){
  int retour=0;
  int z=0;
  while(retour!=1){
    for(int i=0;i<r5->ndisk;i++){
      retour=affichageBlockHexa(r5,i,z,stdout);
      if(retour){
        return(retour);
      }
      printf(" ");
    }
    printf("\n");
    z=z+4;
  }
  return 0;
}

void write_chunk(virtual_disk_t *r5, char *buffer, int n, uint startBytes){
  int nbBlocks = compute_nblock(n);
  int nbBandes = compute_nstripe(r5,nbBlocks);
  int pos = 0;
  block_t bloc[nbBlocks];
  for(int i = 0; i<nbBlocks; i++){
    for(int j = 0; j<BLOCK_SIZE; j++){
      if (pos<n){
        bloc[i].data[j] = buffer[pos];
        pos++;
      }
      else{
        bloc[i].data[j] = 0;
      }
    }
  }
  //Pos sert maintenant de numdeParité pour les bandes.
  pos = 0;
  stripe_t *bande=init_bande(r5);
  for(int i = 0; i <= nbBandes; i++){   //On parcourt le nbBandes
    pos = compute_parity_index(r5, startBytes+i); //On recupere l'indice de parite
    int x=0;
    for(int j = 0; j <= r5->ndisk - 1 ; j++){ //On ajoute à la bande les blocs
        if (j != pos){
          bande->stripe[j]=bloc[(i*(r5->ndisk - 1))+j-x]; // ajout de bloc de donnees
        } // Fin IF
        else{
          x=1;
        }
    } // Fin FOR j
    bande->stripe[pos] = compute_parity(r5, bande, pos);
    write_stripe(r5, bande, startBytes+i); // ecriture de la bande sur disque
  } // Fin FOR i
  delete_bande(&bande);
}



/** \brief
  * Fonction de lecture de tableau de char
  * @param : virtual_disk_t * , uint , int
  * @return char *
**/
char *read_chunk(virtual_disk_t *r5, uint start_block, int n){
  unsigned char *buffer=malloc(sizeof(unsigned char)*n);
  int indice_buffer = 0,i=0;
  int current=start_block;
  stripe_t *mystripe = init_bande(r5);
  while(indice_buffer < n){
    read_stripe(r5,mystripe, current);
    for (int j = 0; j < r5->ndisk; j++) {
      int par = compute_parity_index(r5,current);
      if (par != j) {
        i=0;
        while ( i<BLOCK_SIZE && indice_buffer < n){
          buffer[indice_buffer]=mystripe->stripe[j].data[i];
          indice_buffer=indice_buffer+1;
          i++;
        }
      }
    }
    current++;
  }
  delete_bande(&mystripe);
  return buffer;
}



buffer[poswrite] = ((char*)&(inode[i].size))[0];
buffer[poswrite+3] = (inode[i].size>>24) & 0xFF;
buffer[poswrite+2] = (inode[i].size>>16) & 0xFF;
buffer[poswrite+1] = (inode[i].size>>8) & 0xFF;
buffer[poswrite] = inode[i].size & 0xFF;

/*ecriture du nblock*/
poswrite+=4;
buffer[poswrite+3] = (inode[i].nblock>>24) & 0xFF;
buffer[poswrite+2] = (inode[i].nblock>>16) & 0xFF;
buffer[poswrite+1] = (inode[i].nblock>>8) & 0xFF;
buffer[poswrite] = inode[i].nblock & 0xFF;

/*ecriture du first_byte*/
poswrite+=4;
buffer[poswrite+3] = (inode[i].first_byte>>24) & 0xFF;
buffer[poswrite+2] = (inode[i].first_byte>>16) & 0xFF;
buffer[poswrite+1] = (inode[i].first_byte>>8) & 0xFF;
buffer[poswrite] = inode[i].first_byte & 0xFF;



printf("read_inodes_table\n");
int nbBandes = compute_nstripe(r5Disk, INODE_SIZE),k, debutTable = startTable(r5Disk);
int posread=0;
//char *bufferConversion = malloc(sizeof(char)*sizeof(int));
//printf("Adresse de bufferConversion apres le malloc : %d\n",bufferConversion);
//char *bufferInode = malloc(sizeof(char)*(sizeof(inode_t)));
//printf("Adresse de bufferInode apres le malloc : %d\n", bufferInode);
char bufferConversion[4];
char bufferInode[INODE_OCT];


for (int i = 0; i<INODE_TABLE_SIZE; i++){
  read_chunk(r5Disk, bufferInode, sizeof(inode_t), debutTable+(i*compute_nstripe(r5Disk, compute_nblock(sizeof(inode_t)))));
  /* LECTURE DES BANDES CONSTITUANTS L'INODE */
  //posread = i*(INODE_OCT +sizeof(int));
  posread = 0;
  for(k = 0; k < FILENAME_MAX_SIZE; k++){ //k allant de 0 à (2bandes/octets)-1
    printf("k = %d\tAdresse de table[%d]->filename : %d\n",k,i, table[i]->filename);
    table[i]->filename[k] = bufferInode[k];
  }
  posread+=FILENAME_MAX_SIZE;

  for (k = posread; k < posread+sizeof(int); k++){
    bufferConversion[k-posread] = bufferInode[k];
  }

  printf("i : %d\n",i);
  table[i]->size = uChar_To_Int(bufferConversion);
  posread+= sizeof(int);
  for (k = posread; k < posread+sizeof(int); k++){
    bufferConversion[k-posread] = bufferInode[k];
  }
  table[i]->nblock = uChar_To_Int(bufferConversion);
  posread+= sizeof(int);


  for (k = posread; k < posread+sizeof(int); k++){
    bufferConversion[k-posread] = bufferInode[k];
  }
  table[i]->first_byte = uChar_To_Int(bufferConversion);
  posread+=sizeof(int);
}

for(int z=0;z<INODE_TABLE_SIZE;z++){
  r5Disk->inodes[z]=*(table[z]);
}
printf("Adresse de bufferConversion avant le free : %d\n",bufferConversion);
//free(bufferConversion);
printf("Adresse de bufferInode avant le free : %d\n",bufferInode);
//free(bufferInode);
printf("Coucou\n");


/** \brief
  * Lecture de la table d'inodes
  * @param : virtual_disk_t ,inode_table_t
  * @return : void
**/
  void read_inodes_table(virtual_disk_t *r5Disk, inode_t maTable[INODE_TABLE_SIZE]){
    inode_t Ino;
    for(int i = 0; i< 10; i++){
      read_inode_table_i(r5Disk, &Ino, i);
      for(int j = 0; j < FILENAME_MAX_SIZE; j++){
        maTable[i].filename[j] = Ino.filename[j];
      }
      maTable[i].first_byte = Ino.first_byte;
      maTable[i].size = Ino.size;
      maTable[i].nblock = Ino.nblock;
    }
    printf("\n\n__%d__\n\n", maTable[3].first_byte);
  }

void read_inode_table_i(virtual_disk_t *r5Disk, inode_t *Ino, int indice){
  int depart = startTable(r5Disk), posread = 0, k;
  uchar buffer[50];
  uchar conversion[4];
  read_chunk(r5Disk, buffer, 44, depart+(indice*4));
  for(k = 0; k<FILENAME_MAX_SIZE; k++){
    Ino->filename[k] = buffer[k];
  }
  printf("\n");
  posread+=FILENAME_MAX_SIZE;
  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->size = uChar_To_Int(conversion);
  printf("--%d--\n", uChar_To_Int(conversion));

  posread+= sizeof(int);
  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->nblock = uChar_To_Int(conversion);
  posread+= sizeof(int);

  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->first_byte = uChar_To_Int(conversion);
}


void write_inode_table_i(virtual_disk_t *r5Disk, inode_t Ino, int indice){
  int depart = startTable(r5Disk), poswrite = 0;
  uchar buffer[50];
  uchar conversion[4];

  printf("AFFICHAGE INODE:\nIno.filename = %s\n", Ino.filename);
  printf("Ino.size = %d\n", Ino.size);
  printf("Ino.nblock = %d\n", Ino.nblock);
  printf("Ino.first_byte = %d\n", Ino.first_byte);
  for (int i = 0; i < FILENAME_MAX_SIZE; i++) {
    buffer[i]= Ino.filename[i];
  }
  poswrite+=FILENAME_MAX_SIZE;


  int_To_uChar(Ino.size,&conversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = conversion[i];
  }
  poswrite+=sizeof(int);


  int_To_uChar(Ino.nblock, &conversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = conversion[i];
  }
  poswrite+=sizeof(int);


  int_To_uChar(Ino.first_byte, &conversion);
  for (int i = 0; i < sizeof(int); i++) {
    buffer[poswrite+i] = conversion[i];
  }
  printf("poswrite = %d && |_%d_|\n", poswrite, depart+(indice*4));
  write_chunk(r5Disk,buffer, 44, depart+(indice*4));
}

/**********************************TEST COUCHE 3 *************************/
stripe_t *bande = init_bande(r5d);
inode_t table[10];
inode_t Ino;
char test[FILENAME_MAX_SIZE];
for(int i = 0; i < r5d->ndisk; i++){
  for(int j = 0; j < BLOCK_SIZE; j++){
    if(j == 0){
      bande->stripe[i].data[j] = 2;
    }else{
      bande->stripe[i].data[j] = 0;
    }
  }
}
for(int i = 0; i < r5d->ndisk; i++){
  printf("[ ");
  for(int j = 0; j < BLOCK_SIZE; j++){
    printf("%d, ", bande->stripe[i].data[j]);
  }
  printf("]\n");
}
for(int i = 0; i < 42; i++){
  write_stripe(r5d, bande, i);
}
read_inodes_table(r5d, table);
printf("\ntest : %d--\n", table[3].first_byte);
test[0] = 'T';
test[1] = 'e';
test[2] = 'x';
test[3] = 't';
test[4] = 'e';
test[5] = '\0';
Ino = init_inode(test,54,42);
printf("\ntest : %s--\n", Ino.filename);
for (int i = 0; i < 10; i++) {
  table[i] = Ino;
}
table[3].size = 42314;
write_inodes_table(r5d,table);
affichageSysteme(r5d);
// for(int i = 0; i< 10; i++){
//   read_inode_table_i(r5d, &Ino, 0);
// }
delete_bande(&bande);
/*****************************************************************************/


void read_inode_table_i(virtual_disk_t *r5Disk, inode_t *Ino, int indice){
  int depart = startTable(r5Disk), posread = 0, k;
  uchar buffer[50];
  uchar conversion[4];
  read_chunk(r5Disk, buffer, 44, depart+(indice*4));
  for(k = 0; k<FILENAME_MAX_SIZE; k++){
    Ino->filename[k] = buffer[k];
  }
  printf("\n");
  posread+=FILENAME_MAX_SIZE;
  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->size = uChar_To_Int(conversion);
  printf("--%d--\n", uChar_To_Int(conversion));

  posread+= sizeof(int);
  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->nblock = uChar_To_Int(conversion);
  posread+= sizeof(int);

  for (k = posread; k < posread+sizeof(int); k++){
    conversion[k-posread] = buffer[k];
  }
  Ino->first_byte = uChar_To_Int(conversion);
}



/************************************************************************************/
file_t fichier;
int nboctets=0;
read_file(r5Disk, argument, &fichier);
char buffer[MAX_FILE_SIZE];
for (int i = 0; i < fichier.size; i++) {
  printf("%c",fichier.data[i]);
}
do {
  fgets(buffer,MAX_FILE_SIZE-fichier.size,stdin);
  nboctets = strlen(buffer);
  for (int i = 0; i < nboctets; i++) {
    fichier.data[fichier.size] = buffer[i];
    fichier.size+=1;
  }
} while(buffer[0] != '\n');
write_file(r5Disk,argument,fichier);
return 0;


/**************************************************************************/

if (argument[0] == '\0') {
  return -1;
}else{
  file_t fichier;
  int nboctets=0, nblignes = 0, noligne;
  char buffer[MAX_FILE_SIZE];
  read_file(r5Disk, argument, &fichier);
  char cmd[50];
  char ligne[200];
  printEditionfichier();
  printf("Voulez vous rentrer en mode insertion ou suppresion?[i -> insertion / s-> suppresion] : ");
  lecture(cmd, 50);
  while(cmd[0] != 'i' && cmd[0] != 's'){
    printf("\nSaisie invalide![i -> insertion / s-> suppresion] : ");
    lecture(cmd,50);
  }
  // Mode INSERTION
  if (cmd[0] == 'i' ) {
    nboctets=0;
    printFichier(fichier, argument);
    do {
      fgets(buffer,MAX_FILE_SIZE-fichier.size,stdin);
      nboctets = strlen(buffer);
      for (int i = 0; i < nboctets; i++) {
        fichier.data[fichier.size] = buffer[i];
        fichier.size+=1;
      }
    } while(buffer[0] != '\n');
    write_file(r5Disk,argument,fichier);
    //Mode SUPRESSION
    }else{
      //On calcule le nombre de ligne du fichier
      for (int i = 0; i < fichier.size; i++) {
        if (fichier.data[i] == '\n') {
          nblignes+=1;
        }
      }
      nblignes+=1;
      printf("\nQuelle ligne voulez vous supprimer? [1-%d]",nblignes);
      fscanf(stdin, "%d", &noligne);
      while(noligne < 0 || noligne > nblignes){
        printf("\nSaisie invalide! Quelle ligne voulez vous supprimer? [1-%d] : ", nblignes);
        fscanf(stdin, "%d", &noligne);
      }
      fichier = supprimerLignei(fichier, noligne);
      printFichier(fichier, argument);
      write_file(r5Disk, argument, fichier);
    }
  }


  /********************************************************************/
  /** \brief
    * Fonction Auxiliaire qui affiche un cartouche pour l'edition
    * @param :
    * @return void
  **/
  void printEditionfichier(){
    printf(" ________________________________________\n");
    printf("|                                        |\n");
    printf("|                                        |\n");
    printf("|");
    printf("\033[31;49m           Edition de fichier\033[39;49m");
    printf("           |\n");
    printf("|                                        |\n");
    printf("|                                        |\n");
    printf("------------------------------------------\n");
  }

  /** \brief
    * Fonction Auxiliaire qui affiche le nom et le contenu d'un fichier
    * @param :
    * @return void
  **/
  void printFichier(file_t fichier, char nomFichier[]){
    printf("\n\n\n\t\t\t\033[31;49m %s\033[39;49m\n", nomFichier);
    for (int i = 0; i < fichier.size; i++) {
      printf("%c",fichier.data[i]);
    }
  }
