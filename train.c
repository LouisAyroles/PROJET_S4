#include <stdio.h>

int main () {
    int N, M;
    int A[100], B[100];
    
    printf("Veuillez rentrer la taille de A ( Max 50 )");
    scanf("%d", &N);
    printf("Veuillez rentrer la taille de B ( Max 50 )");
    scanf("%d", &M);
    printf("*************Remplissage de A : *************\n");
    for(int i = 0; i < N; i++)
    {
        printf("A[%d]=",i);
        scanf("%d",&A[i]);
    }
    printf("*************Remplissage de B : *************\n");
    for(int i = 0; i < M; i++)
    {
        printf("B[%d]=",i);
        scanf("%d",&B[i]);
    }
    printf("*************Affichage de A : *************\n");
    for(int i = 0; i < N; i++)
    {
        printf("%d   ",i);
        printf("%d\n",A[i]);
    }
    printf("*************Affichage de B : *************\n");
    for(int i = 0; i < M; i++)
    {
        printf("%d   ",i);
        printf("%d\n",B[i]);
    }
    
    for(int i = 0; i < M; i++)
    {
        A[N+i]=B[i];
    }
    printf("*************Affichage de A apres la copie : *************\n");
    for(int i = 0; i < N+M; i++)
    {
        printf("%d   ",i);
        printf("%d\n",A[i]);
    }
    
}   

