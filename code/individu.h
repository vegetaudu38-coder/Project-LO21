#ifndef INDIVIDU_H
#define INDIVIDU_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* Par défaut : fonction f1 (longIndiv = 8, A = -1, B = 1).
   Compilé avec -DFONCTION_F2 : fonction f2 (longIndiv = 16, A = 0.1, B = 5). */
#ifdef FONCTION_F2
#define longIndiv 16
#define A 0.1
#define B 5.0
#else
#define longIndiv 8
#define A (-1.0)
#define B (1.0)
#endif

typedef unsigned char Bit;

/* Un individu est une liste chaînée de bits */
typedef struct Cellule {
    Bit bit;
    struct Cellule *suivant;
} Cellule;

typedef Cellule *Individu;

/* --- Outils --- */
Individu ajouterEnTete(Individu ind, Bit b);
void libererIndividu(Individu ind);
void afficherIndividu(Individu ind);

/* --- Initialisation aléatoire --- */
Individu initIter(int n);
Individu initRec(int n);

/* --- Décodage, croisement, qualité --- */
int decoder(Individu ind);
void croiser(Individu ind1, Individu ind2, double pCroise);
double qualite(Individu ind);

#endif
