#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define longIndiv 8
#define A (-1.0)
#define B (1.0)

typedef unsigned char Bit;

typedef struct Cellule {
    Bit bit;
    struct Cellule *suivant;
} Cellule;

typedef Cellule *Individu;   /* liste chaînée de bits */

/* ---------- Outils ---------- */

Individu ajouterEnTete(Individu ind, Bit b)
{
    Cellule *c = (Cellule *)malloc(sizeof(Cellule));
    if (c == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        exit(EXIT_FAILURE);
    }
    c->bit = b;
    c->suivant = ind;
    return c;
}

void libererIndividu(Individu ind)
{
    Cellule *tmp;
    while (ind != NULL) {
        tmp = ind;
        ind = ind->suivant;
        free(tmp);
    }
}

void afficherIndividu(Individu ind)
{
    for (; ind != NULL; ind = ind->suivant)
        printf("%d", ind->bit);
    printf("\n");
}

/* ---------- Initialisation aléatoire ---------- */

/* Version itérative */
Individu initIter(int n)
{
    Individu ind = NULL;
    int i;
    for (i = 0; i < n; i++)
        ind = ajouterEnTete(ind, (Bit)(rand() % 2));
    return ind;
}

/* Version récursive */
Individu initRec(int n)
{
    if (n == 0)
        return NULL;
    return ajouterEnTete(initRec(n - 1), (Bit)(rand() % 2));
}

/* ---------- Décodage ---------- */

int decoder(Individu ind)
{
    int valeur = 0;
    for (; ind != NULL; ind = ind->suivant)
        valeur = 2 * valeur + ind->bit;
    return valeur;
}

/* ---------- Croisement ---------- */

void croiser(Individu ind1, Individu ind2, double pCroise)
{
    Bit tmp;
    double tirage;
    while (ind1 != NULL && ind2 != NULL) {
        tirage = (double)rand() / RAND_MAX;   /* réel dans [0,1] */
        if (tirage < pCroise) {
            tmp = ind1->bit;
            ind1->bit = ind2->bit;
            ind2->bit = tmp;
        }
        ind1 = ind1->suivant;
        ind2 = ind2->suivant;
    }
}

/* ---------- Qualité ---------- */

double qualite(Individu ind)
{
    int x = decoder(ind);
    double X = ((double)x / pow(2.0, longIndiv)) * (B - A) + A;
    return -X * X;
}

/* ---------- Programme de test ---------- */

int main(void)
{
    Individu i1, i2;
    double pCroise = 0.5;

    srand((unsigned int)time(NULL));

    i1 = initIter(longIndiv);
    i2 = initRec(longIndiv);

    printf("Individu 1 : "); afficherIndividu(i1);
    printf("Individu 2 : "); afficherIndividu(i2);
    printf("Valeur 1 = %d, qualite 1 = %f\n", decoder(i1), qualite(i1));
    printf("Valeur 2 = %d, qualite 2 = %f\n", decoder(i2), qualite(i2));

    croiser(i1, i2, pCroise);

    printf("\nApres croisement (pCroise = %.2f) :\n", pCroise);
    printf("Individu 1 : "); afficherIndividu(i1);
    printf("Individu 2 : "); afficherIndividu(i2);
    printf("Valeur 1 = %d, qualite 1 = %f\n", decoder(i1), qualite(i1));
    printf("Valeur 2 = %d, qualite 2 = %f\n", decoder(i2), qualite(i2));

    libererIndividu(i1);
    libererIndividu(i2);
    return 0;
}
