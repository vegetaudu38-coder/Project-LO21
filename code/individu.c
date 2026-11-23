#include "individu.h"

/* ---------- Outils ---------- */

/* Ajoute un bit en tête de liste et renvoie la nouvelle tête */
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

/* Libère toutes les cellules de l'individu */
void libererIndividu(Individu ind)
{
    Cellule *tmp;
    while (ind != NULL) {
        tmp = ind;
        ind = ind->suivant;
        free(tmp);
    }
}

/* Affiche les bits de gauche à droite */
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

/* Convertit la liste de bits (bit de poids fort en tête) en entier */
int decoder(Individu ind)
{
    int valeur = 0;
    for (; ind != NULL; ind = ind->suivant)
        valeur = 2 * valeur + ind->bit;
    return valeur;
}

/* ---------- Croisement ---------- */

/* Pour chaque position, échange les bits des deux individus avec la probabilité pCroise */
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

/* Mise à l'échelle de x dans [A, B], puis calcul de la qualité */
double qualite(Individu ind)
{
    int x = decoder(ind);
    double X = ((double)x / pow(2.0, longIndiv)) * (B - A) + A;
#ifdef FONCTION_F2
    return -log(X);     /* f2(x) = -ln(X) */
#else
    return -X * X;      /* f1(x) = -X^2 */
#endif
}
