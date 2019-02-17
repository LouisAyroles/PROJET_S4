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
