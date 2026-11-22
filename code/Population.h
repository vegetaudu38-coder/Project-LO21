#ifndef POPULATION_H
#define POPULATION_H

#include "***.h" /* à adapter selon le nom exact du .h de votre binôme */

typedef struct CellulePop {
    Individu indiv;
    struct CellulePop *suivant;
} CellulePop;

typedef CellulePop *Population;

/* --- Fonctions outils de gestion du type Population --- */

Population ajouterIndividuPop(Population pop, Individu ind);
void libererPopulation(Population pop);
void afficherPopulation(Population pop);
int taillePopulation(Population pop);

/* --- Fonctions d'opérations sur les populations --- */

Population initPopulation(int taillePop);
void trierPopulation(Population *pop);
void selectionnerPopulation(Population *pop, int tSelect);
void croiserPopulation(Population *pop, double pCroisement);
Individu trouverMeilleurInd(Population pop);

#endif