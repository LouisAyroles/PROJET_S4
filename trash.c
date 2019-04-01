// Fichier où on peut mettre des idées qu'on a rejeté mais qu'on est pas sûr
// de nous.

//DANS COUCHE1.C
/** \brief
  * prend un tableau de 4 octets (char) et le transforme en Hexadecimal
  * @param : block_t (Contient le tableau de bits)
  * @param : char* (Caractere dans lequel on met l'hexa)
  * @return : void
**/
void bitToHexa(block_t monBloc, char* nbHexa){
  char nb = 0;
  if(monBloc.data[BLOCK_SIZE-1]){
      nb+=8;
  }
  if(monBloc.data[BLOCK_SIZE-2]){
      nb+=4;
  }
  if(monBloc.data[BLOCK_SIZE-3]){
      nb+=2;
  }
  if(monBloc.data[BLOCK_SIZE-4]){
      nb+=1;
  }
  switch(nb){
    case 10 : *nbHexa='A';
              break;
    case 11:  *nbHexa='B';
              break;
    case 12 : *nbHexa='C';
              break;
    case 13 : *nbHexa='D';
              break;
    case 14 : *nbHexa='E';
              break;
    case 15 : *nbHexa='F';
              break;
    default:  *nbHexa=nb+'0';
  }
}

void octetsToHexa(block_t monBloc, char* nbHexa){
  char reste, diviseur;
  for (int i = 0; i < BLOCK_SIZE; i++){
    diviseur = monBloc.data[i] / 16;
    reste = monBloc.data[i] % 16;
    nbHexa[(2*BLOCK_SIZE)-(2*i+1)]=conversionHexa(reste);
    nbHexa[(2*BLOCK_SIZE)-(2*i+2)]=conversionHexa(diviseur);
    // QUESTION: Dans quel sens on ecrit dans le tableau d'hexadecimal?
    // Comme je l'ai fait ça ecrit dans la case 7 et 6 l'hexadecimal de l'octet
    // 0, 5 et 4 l'hexadecimal de l'octet 1, etc...
    // Comme ça quand on print de 0 à BLOCK_SIZE*2 (le nombre de chiffre en
    // hexadecimal pour ecrire BLOCK_SIZE octets) ça affiche dans le sens de
    // lecture.
  }
}

/*
char *read_chunk(virtual_disk_t *r5, uint start_block, int n){
  uint nbBlocks = compute_nblock(n);
  unsigned char *buffer=malloc(sizeof(unsigned char)*n);
  int indice_buffer=0;
  int current=start_block;
  block_t *blc=malloc(sizeof(struct block_s));
  buffer = (char*)malloc(sizeof(char)*n);
  char nbHexa[BLOCK_SIZE*2];
  unsigned char shuffle;
  while((uint) current<=start_block+nbBlocks+nbBlocks/3-1){
    if((current-start_block)%r5->ndisk!=compute_parity_index(r5,(current-current%4)/4)){
      if(current%4==0){
        read_block(r5,blc,current,0);
      }
      else if(current%4==1){
        read_block(r5,blc,current-1,1);
      }
      else if(current%4==2){
        read_block(r5,blc,current-2,2);
      }
      else if(current%4==3){
        read_block(r5,blc,current-3,3);
      }
      octetsToHexa(*blc, nbHexa);
      for(int i=0; i<BLOCK_SIZE*2; i=i+2){
        shuffle=conversionDec(nbHexa[i+1]);
        shuffle=shuffle+16*conversionDec(nbHexa[i]);
        buffer[indice_buffer]=shuffle;
        indice_buffer=indice_buffer+1;
      }
    }
    else{
      //printf("Ligne : %d , parité : %d , numbloc : %d\n",(current-current%4)/4,compute_parity_index(r5,(current-current%4)/4),current-start_block);
    }
    current=current+1;
  }
  return buffer;
}
*/

struct dirent *lecture;
char *nomFichiers;
int nbDisk = 0;
nomFichiers = (char*)malloc(sizeof(nomFichiers)*10);
rep = opendir(nomRep);
while((lecture = readdir(rep))){
  nomFichiers[nbDisk] = lecture->d_name;
  nbDisk+=1;
}
r5Disk = (virtual_disk_t*)malloc(sizeof(virtual_disk_t));

  DIR *rep;
