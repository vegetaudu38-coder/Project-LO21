#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Population.h"

/* Demande un entier et le redemande tant qu'il est hors des bornes */
static int lireEntier(const char *msg, int min, int max)
{
    int v;
    do {
        printf("%s (%d-%d) : ", msg, min, max);
        if (scanf("%d", &v) != 1) {
            fprintf(stderr, "Entree invalide\n");
            exit(EXIT_FAILURE);
        }
    } while (v < min || v > max);
    return v;
}

int main(void)
{
    int taillePop, tauxSelection, nGen, tSelect, g;
    double pCroise = 0.5;
    Population pop;
    Individu meilleur;
    int x;
    double X;

    srand((unsigned int)time(NULL));

    taillePop     = lireEntier("Taille de la population", 20, 200);
    tauxSelection = lireEntier("Taux de selection en %", 10, 90);
    nGen          = lireEntier("Nombre de generations", 20, 200);

    tSelect = taillePop * tauxSelection / 100;

    /* Initialiser la population */
    pop = initPopulation(taillePop);

    /* Répéter nGen fois : croiser, trier, sélectionner */
    for (g = 0; g < nGen; g++) {
        croiserPopulation(&pop, pCroise);
        trierPopulation(&pop);
        selectionnerPopulation(&pop, tSelect);
    }

    /* Afficher le meilleur individu */
    trierPopulation(&pop);
    meilleur = trouverMeilleurInd(pop);

    x = decoder(meilleur);
    X = ((double)x / pow(2.0, longIndiv)) * (B - A) + A;

    printf("\nMeilleur individu :\n");
    printf("Bits   : ");
    afficherIndividu(meilleur);
    printf("Valeur : %d\n", x);
    printf("X      : %f\n", X);
    printf("Qualite: %f\n", qualite(meilleur));

    libererPopulation(pop);
    return 0;
}
