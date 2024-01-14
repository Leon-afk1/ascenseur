#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Structure pour représenter un Usager
typedef struct {
    int etage_appel;
    int etage_destination;
} Usager;

typedef struct UsagerNode{
    Usager* usager;
    struct UsagerNode* suivant;  // Pointeur vers le prochain Usager dans la liste
}UsagerNode;

// Liste chaînée d'Usagers
typedef struct {
    UsagerNode* tete;
    int size;
}ListeUsagers;

// Fonction pour créer un nouvel Usager
UsagerNode* creerUsager(Usager* usager) {
    UsagerNode* nouvelUsager = (UsagerNode*)malloc(sizeof(UsagerNode));
    nouvelUsager->usager = usager;
    nouvelUsager->suivant = NULL;
    return nouvelUsager;
}

void supprimerTete(ListeUsagers* liste){
    UsagerNode* nouvelle_tete = liste->tete->suivant;
    free(liste->tete);
    liste->tete = nouvelle_tete;
    liste->size--;
}


void supprimerUsager(ListeUsagers* liste, Usager *usager) {
    // Cas particulier : si la liste est vide
    if (liste->tete == NULL)
        return;

    // Cas particulier : si la valeur à supprimer est dans le premier nœud
    if (liste->tete->usager == usager) {
        UsagerNode* temp = liste->tete;
        liste->tete = liste->tete->suivant;
        free(temp);
        return;
    }

    // Parcours de la liste pour trouver le nœud contenant la valeur à supprimer
    UsagerNode* current = liste->tete;
    UsagerNode* prev = NULL;
    while (current != NULL && current->usager != usager) {
        prev = current;
        current = current->suivant;
    }

    // Si la valeur n'est pas présente dans la liste
    if (current == NULL)
        return;

    // Suppression du nœud contenant la valeur
    prev->suivant = current->suivant;
    free(current);

}


// Fonction pour ajouter un Usager en tête de la liste
void ajouterEnTete(ListeUsagers* liste, Usager* nouvelUsager) {
    UsagerNode* node = creerUsager(nouvelUsager);
    node->suivant = liste->tete;
    liste->tete = node;
    liste->size++;
}

// Fonction pour parcourir la liste et afficher les éléments
void printListe(ListeUsagers* liste) {
    UsagerNode* courant = liste->tete;
    while (courant != NULL) {
        printf("etage appel : %d , etage destination : %d ", courant->usager->etage_appel, courant->usager->etage_destination);
        courant = courant->suivant;
    }
    printf("\n");
}

// Fonction pour retourner l'élément en tête de la liste
Usager* retournerElementEnTete(ListeUsagers* liste) {
    return liste->tete->usager;
}

// Fonction pour retourner l'élément en queue de la liste
Usager* retournerElementEnQueue(ListeUsagers* liste) {
    UsagerNode* courant = liste->tete;
    while (courant != NULL && courant->suivant != NULL) {
        courant = courant->suivant;
    }
    return courant->usager;
}

void libererListe(ListeUsagers* liste){
    UsagerNode* courant = liste->tete;
    UsagerNode* suivant = NULL;
    while (courant != NULL) {
        suivant = courant->suivant;
        free(courant);
        courant = suivant;
    }
}


void ajouterCroissantDestination(ListeUsagers* liste, Usager* nouvelUsager) {
    UsagerNode* node = creerUsager(nouvelUsager);
    UsagerNode* courant = liste->tete;
    UsagerNode* precedent = NULL;
    while (courant != NULL && courant->usager->etage_destination < nouvelUsager->etage_destination) {
        precedent = courant;
        courant = courant->suivant;
    }
    if (precedent == NULL) {
        node->suivant = liste->tete;
        liste->tete = node;
    }
    else {
        precedent->suivant = node;
        node->suivant = courant;
    }
    liste->size++;
}

void ajouterDecroissantDestination(ListeUsagers* liste, Usager* nouvelUsager) {
    UsagerNode* node = creerUsager(nouvelUsager);
    UsagerNode* courant = liste->tete;
    UsagerNode* precedent = NULL;
    while (courant != NULL && courant->usager->etage_destination > nouvelUsager->etage_destination) {
        precedent = courant;
        courant = courant->suivant;
    }
    if (precedent == NULL) {
        node->suivant = liste->tete;
        liste->tete = node;
    }
    else {
        precedent->suivant = node;
        node->suivant = courant;
    }
    liste->size++;
}


void ajouterCroissantAppel(ListeUsagers* liste, Usager* nouvelUsager) {
    UsagerNode* node = creerUsager(nouvelUsager);
    UsagerNode* courant = liste->tete;
    UsagerNode* precedent = NULL;
    while (courant != NULL && courant->usager->etage_appel < nouvelUsager->etage_appel) {
        precedent = courant;
        courant = courant->suivant;
    }
    if (precedent == NULL) {
        node->suivant = liste->tete;
        liste->tete = node;
    }
    else {
        precedent->suivant = node;
        node->suivant = courant;
    }
    liste->size++;
}

void ajouterDecroissantAppel(ListeUsagers* liste, Usager* nouvelUsager) {
    UsagerNode* node = creerUsager(nouvelUsager);
    UsagerNode* courant = liste->tete;
    UsagerNode* precedent = NULL;
    while (courant != NULL && courant->usager->etage_appel > nouvelUsager->etage_appel) {
        precedent = courant;
        courant = courant->suivant;
    }
    if (precedent == NULL) {
        node->suivant = liste->tete;
        liste->tete = node;
    }
    else {
        precedent->suivant = node;
        node->suivant = courant;
    }
    liste->size++;
}

int main() {
    // Initialisation d'une liste vide
    ListeUsagers liste;
    ListeUsagers liste2;
    liste.tete = NULL;
    liste2.tete = NULL;

    // Ajout d'usagers
    Usager usager1 = {1,2};

    ajouterEnTete(&liste, &usager1);
    ajouterEnTete(&liste2, &usager1);

    Usager usager2 = {1,4};
    ajouterEnTete(&liste, &usager2);

    Usager usager3 = {7,9};
    ajouterEnTete(&liste, &usager3);

    printListe(&liste);


    Usager usager4 = {7,6};
    ajouterDecroissantDestination(&liste, &usager4);

    printListe(&liste);
    printListe(&liste2);
    supprimerUsager(&liste,&usager1);
    supprimerUsager(&liste2,&usager1);
    printListe(&liste);
    printListe(&liste2);
    


    // Libération de la mémoire (à faire à la fin du programme)
    libererListe(&liste);

    return 0;
}