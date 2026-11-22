#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Population.h"

/* --- Fonctions de gestion --- */

Population ajouterIndividuPop(Population pop, Individu ind)
{
    CellulePop *c = malloc(sizeof(CellulePop));
    if (c == NULL) {
        fprintf(stderr, "Erreur allocation mémoire\n");
        exit(EXIT_FAILURE);
    }
    c->indiv = ind;
    c->suivant = pop;
    return c;
}

void libererPopulation(Population pop)
{
    CellulePop *temp;
    while (pop != NULL) {
        temp = pop;
        pop = pop->suivant;
        libererIndividu(temp->indiv);
        free(temp);
    }
}

void afficherPopulation(Population pop)
{
    int i = 1;
    while (pop != NULL) {
        printf("Individu %d | Valeur: %d | Qualité: %f | Bits : ",
               i++, decoder(pop->indiv), qualite(pop->indiv));
        afficherIndividu(pop->indiv);
        pop = pop->suivant;
    }
}

int taillePopulation(Population pop)
{
    int t = 0;
    while (pop != NULL) {
        t++;
        pop = pop->suivant;
    }
    return t;
}

/* --- 1. Initialisation aléatoire --- */

Population initPopulation(int taillePop)
{
    Population pop = NULL;
    for (int i = 0; i < taillePop; i++) {
        Individu ind = initIter(longIndiv); /* ou initRec(longIndiv) */
        pop = ajouterIndividuPop(pop, ind);
    }
    return pop;
}

/* --- 2. Tri par Quicksort (qualité décroissante) --- */

static void partitionner(Population reste, double qPivot,
                         Population *inf, Population *sup)
{
    *inf = NULL;
    *sup = NULL;

    while (reste != NULL) {
        Population suivant = reste->suivant;
        reste->suivant = NULL; /* On détache la cellule de la liste */
        if (qualite(reste->indiv) >= qPivot) {
            reste->suivant = *sup;
            *sup = reste;
        } else {
            reste->suivant = *inf;
            *inf = reste;
        }
        reste = suivant;
    }
}

static Population quicksortPop(Population pop)
{
    if (pop == NULL || pop->suivant == NULL) {
        return pop;
    }

    Population pivot = pop;
    double qPivot = qualite(pivot->indiv);
    Population inf = NULL;
    Population sup = NULL;

    partitionner(pop->suivant, qPivot, &inf, &sup);
    pivot->suivant = NULL;

    sup = quicksortPop(sup);
    inf = quicksortPop(inf);

    /* On concatène les trois listes : sup -> pivot -> inf */
    pivot->suivant = inf;
    if (sup == NULL) {
        return pivot;
    }

    Population courant = sup;
    while (courant->suivant != NULL) {
        courant = courant->suivant;
    }
    courant->suivant = pivot;
    return sup;
}

void trierPopulation(Population *pop)
{
    if (pop != NULL) {
        *pop = quicksortPop(*pop);
    }
}

/* --- 3. Sélection des meilleurs individus --- */

/* On suppose que la population est triée par qualité décroissante */

static Individu copierIndividu(Individu src)
{
    if (src == NULL) {
        return NULL;
    }

    Individu copie = NULL;
    Individu *fin = &copie;
    Individu courant = src;

    while (courant != NULL) {
        Individu cellule = malloc(sizeof(Cellule));
        if (cellule == NULL) {
            libererIndividu(copie);
            return NULL;
        }
        cellule->bit = courant->bit;
        cellule->suivant = NULL;
        *fin = cellule;
        fin = &(cellule->suivant);
        courant = courant->suivant;
    }
    return copie;
}

void selectionnerPopulation(Population *pop, int tSelect)
{
    if (pop == NULL || *pop == NULL || tSelect <= 0) {
        return;
    }

    /* On calcule la taille initiale et repère le tSelect-ième élément */
    int tailleInit = 0;
    Population courant = *pop;
    Population dernierGarde = NULL;

    while (courant != NULL) {
        tailleInit++;
        if (tailleInit == tSelect) {
            dernierGarde = courant;
        }
        courant = courant->suivant;
    }

    /* Si le tSelect-ième élément n'existe pas ou qu'il n'y a rien après, on ne tronque rien */
    if (dernierGarde == NULL || dernierGarde->suivant == NULL) {
        return;
    }

    /* Tronquer après le tSelect-ième élément et libérer les suivants */
    Population aSupprimer = dernierGarde->suivant;
    dernierGarde->suivant = NULL;

    while (aSupprimer != NULL) {
        Population suiv = aSupprimer->suivant;
        libererIndividu(aSupprimer->indiv);
        free(aSupprimer);
        aSupprimer = suiv;
    }

    /* Compléter jusqu'à tailleInit en recopiant les tSelect premiers éléments */
    int tailleActuelle = tSelect;
    Population queue = dernierGarde;
    Population source = *pop;
    int indexSource = 0;

    while (tailleActuelle < tailleInit) {
        if (source == NULL || indexSource >= tSelect) {
            source = *pop;
            indexSource = 0;
        }

        Individu indivCopie = copierIndividu(source->indiv);
        Population newCell = malloc(sizeof(CellulePop));
        if (newCell == NULL) {
            libererIndividu(indivCopie);
            return;
        }

        newCell->indiv = indivCopie;
        newCell->suivant = NULL;

        queue->suivant = newCell;
        queue = newCell;

        source = source->suivant;
        indexSource++;
        tailleActuelle++;
    }
}