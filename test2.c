#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
typedef struct Personne Personne;

struct Personne
{
    int age;
    char nom[20];
    char prenom[20];

};


int main(int argc, char const *argv[])
{
    srand(time(NULL));
    int nbAmis;
    Personne* invite;

    printf("Combien d'amis allez vous inviter a votre anniversaire?");
    scanf("%d", &nbAmis);
    if ( nbAmis > 0 ) {
        invite = malloc( nbAmis * sizeof(Personne));
        if (invite==NULL) {
            exit(0);
        }


        for( int i = 0; i < nbAmis; i++)
        {
            char nom[20] = "nom";
            char prenom[20] = "prenom";
            int j=-1;
            do
            {
                j++;
                invite[i].nom[j] = nom[j];
            }while (nom[j] != '\0');
            j=-1;
            do
            {
                j++;
                invite[i].prenom[j] = prenom[j];
            }while (prenom[j] != '\0');
            invite[i].age = 20 + rand()%11;
        }


        for(int i = 0; i < nbAmis; i++)
        {
            printf("Quel est le prenom de votre ami?");
            scanf("%s",invite[i].prenom);
            printf("Quel est le nom de %s?",invite[i].prenom);
            scanf("%s",invite[i].nom);
            printf("Quel est l'age de %s %s ?", invite[i].prenom, invite[i].nom);
            scanf("%d",&(invite[i].age));
        }

        printf("\nListes des invités\n");
        for(int i = 0; i < nbAmis; i++)
        {
            printf("%s %s %d\n",invite[i].nom,invite[i].prenom,invite[i].age);
        }
return 0;
}