#ifndef POPULATION_H
#define POPULATION_H

#include "individu.h"

/* Une cellule de la liste de population contient un individu */
typedef struct CellulePop {
    Individu indiv;
    struct CellulePop *suivant;
} CellulePop;

typedef CellulePop *Population;

/* --- Fonctions outils --- */
Population ajouterIndividuPop(Population pop, Individu ind);
void libererPopulation(Population pop);
void afficherPopulation(Population pop);
int taillePopulation(Population pop);

/* --- Opérations sur les populations --- */
Population initPopulation(int taillePop);
void trierPopulation(Population *pop);
void selectionnerPopulation(Population *pop, int tSelect);
void croiserPopulation(Population *pop, double pCroisement);
Individu trouverMeilleurInd(Population pop);

#endif
