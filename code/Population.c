#include <stdio.h>
#include <stdlib.h>
#include "Population.h"

/* ---------- Fonctions outils ---------- */

/* Ajoute un individu en tête de population */
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

/* Libère la population et tous ses individus */
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

/* Affiche chaque individu avec sa valeur, sa qualité et ses bits */
void afficherPopulation(Population pop)
{
    int i = 1;
    while (pop != NULL) {
        printf("Individu %d | Valeur: %d | Qualite: %f | Bits : ",
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

/* Copie complète d'un individu (les cellules ne sont pas partagées) */
static Individu copierIndividu(Individu src)
{
    Individu copie = NULL;
    Individu *fin = &copie;
    Individu courant = src;

    while (courant != NULL) {
        Individu cellule = malloc(sizeof(Cellule));
        if (cellule == NULL) {
            fprintf(stderr, "Erreur allocation mémoire\n");
            exit(EXIT_FAILURE);
        }
        cellule->bit = courant->bit;
        cellule->suivant = NULL;
        *fin = cellule;
        fin = &(cellule->suivant);
        courant = courant->suivant;
    }
    return copie;
}

/* ---------- 1. Initialisation aléatoire ---------- */

Population initPopulation(int taillePop)
{
    Population pop = NULL;
    int i;
    for (i = 0; i < taillePop; i++)
        pop = ajouterIndividuPop(pop, initIter(longIndiv));
    return pop;
}

/* ---------- 2. Tri quicksort (qualité décroissante) ---------- */

/* Divise : répartit les cellules entre sup (qualité >= pivot) et inf (qualité < pivot) */
static void partitionner(Population reste, double qPivot,
                         Population *inf, Population *sup)
{
    *inf = NULL;
    *sup = NULL;

    while (reste != NULL) {
        Population suivant = reste->suivant;
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

/* Régner : trie récursivement puis concatène sup -> pivot -> inf */
static Population quicksortPop(Population pop)
{
    if (pop == NULL || pop->suivant == NULL)
        return pop;

    Population pivot = pop;
    double qPivot = qualite(pivot->indiv);
    Population inf, sup;

    partitionner(pop->suivant, qPivot, &inf, &sup);
    pivot->suivant = NULL;

    sup = quicksortPop(sup);
    inf = quicksortPop(inf);

    pivot->suivant = inf;
    if (sup == NULL)
        return pivot;

    Population courant = sup;
    while (courant->suivant != NULL)
        courant = courant->suivant;
    courant->suivant = pivot;
    return sup;
}

void trierPopulation(Population *pop)
{
    if (pop != NULL)
        *pop = quicksortPop(*pop);
}

/* ---------- 3. Croisement de la population ---------- */

/* Construit P2 à partir de P1 : mélange les individus, croise les paires,
   puis remplace P1 par P2. La taille est conservée. */
void croiserPopulation(Population *pop, double pCroisement)
{
    int n, i, j, k;
    Population courant, nouvelle = NULL;
    CellulePop **tab;
    int *ordre;

    if (pop == NULL || *pop == NULL)
        return;

    n = taillePopulation(*pop);
    if (n < 2)
        return;

    tab = malloc(n * sizeof(CellulePop *));
    ordre = malloc(n * sizeof(int));
    if (tab == NULL || ordre == NULL) {
        fprintf(stderr, "Erreur allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    /* Tableau des cellules de P1 */
    courant = *pop;
    for (i = 0; i < n; i++) {
        tab[i] = courant;
        ordre[i] = i;
        courant = courant->suivant;
    }

    /* Mélange de Fisher-Yates : appariement aléatoire */
    for (i = n - 1; i > 0; i--) {
        j = rand() % (i + 1);
        k = ordre[i];
        ordre[i] = ordre[j];
        ordre[j] = k;
    }

    /* Paires croisées : copies des parents, croisement, ajout dans P2 */
    for (i = 0; i + 1 < n; i += 2) {
        Individu a = copierIndividu(tab[ordre[i]]->indiv);
        Individu b = copierIndividu(tab[ordre[i + 1]]->indiv);
        croiser(a, b, pCroisement);
        nouvelle = ajouterIndividuPop(nouvelle, a);
        nouvelle = ajouterIndividuPop(nouvelle, b);
    }

    /* Si la taille est impaire, le dernier individu est recopié sans croisement */
    if (n % 2 == 1)
        nouvelle = ajouterIndividuPop(nouvelle, copierIndividu(tab[ordre[n - 1]]->indiv));

    libererPopulation(*pop);
    *pop = nouvelle;

    free(tab);
    free(ordre);
}

/* ---------- 4. Sélection ---------- */

/* Garde les tSelect premiers individus (population triée), puis complète
   jusqu'à la taille initiale par recopie cyclique de ces tSelect individus. */
void selectionnerPopulation(Population *pop, int tSelect)
{
    Population garde, aSupprimer, queue, source;
    int taille, k, i;

    if (pop == NULL || *pop == NULL || tSelect <= 0)
        return;

    taille = taillePopulation(*pop);
    if (tSelect >= taille)
        return;

    /* Repérer le tSelect-ième élément */
    garde = *pop;
    for (i = 1; i < tSelect; i++)
        garde = garde->suivant;

    /* Tronquer et libérer le reste */
    aSupprimer = garde->suivant;
    garde->suivant = NULL;
    libererPopulation(aSupprimer);

    /* Recopie cyclique des tSelect premiers individus */
    queue = garde;
    source = *pop;
    k = 0;
    for (i = tSelect; i < taille; i++) {
        Population cellule = malloc(sizeof(CellulePop));
        Individu copie = copierIndividu(source->indiv);
        if (cellule == NULL) {
            fprintf(stderr, "Erreur allocation mémoire\n");
            exit(EXIT_FAILURE);
        }
        cellule->indiv = copie;
        cellule->suivant = NULL;
        queue->suivant = cellule;
        queue = cellule;

        source = source->suivant;
        k++;
        if (k == tSelect) {      /* on reprend au début des tSelect premiers */
            source = *pop;
            k = 0;
        }
    }
}

/* ---------- 5. Meilleur individu ---------- */

/* Renvoie l'individu de qualité maximale (sans le copier) */
Individu trouverMeilleurInd(Population pop)
{
    Population best;
    if (pop == NULL)
        return NULL;

    best = pop;
    for (pop = pop->suivant; pop != NULL; pop = pop->suivant) {
        if (qualite(pop->indiv) > qualite(best->indiv))
            best = pop;
    }
    return best->indiv;
}
