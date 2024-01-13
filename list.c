#include <stdio.h>
#include <stdlib.h>

// Structure pour représenter un Usager
typedef struct{
    int etage_appel;
    int etage_destination;
    struct Usager* suivant;  // Pointeur vers le prochain Usager dans la liste
}Usager;

// Liste chaînée d'Usagers
typedef struct {
    Usager* tete;
    int size;
}ListeUsagers;

// Fonction pour créer un nouvel Usager
struct Usager* creerUsager(int etage_appel, int etage_destination) {
    Usager* nouvelUsager = (Usager*)malloc(sizeof(Usager));
    nouvelUsager->etage_appel=etage_appel;
    nouvelUsager->etage_destination=etage_destination;
    nouvelUsager->suivant = NULL;
    return nouvelUsager;
}

// Fonction pour ajouter un Usager en tête de la liste
void ajouterEnTete(ListeUsagers* liste, Usager* nouvelUsager) {
    nouvelUsager->suivant = liste->tete;
    liste->tete = nouvelUsager;
    liste->size++;
}

// Fonction pour parcourir la liste et afficher les éléments
void parcourirListe(ListeUsagers* liste) {
    Usager* courant = liste->tete;
    while (courant != NULL) {
        printf("etage appel : %d , etage destination : %d", courant->etage_appel, courant->etage_destination);
        courant = courant->suivant;
    }
}

// Fonction pour retourner l'élément en tête de la liste
Usager* retournerElementEnTete(ListeUsagers* liste) {
    return liste->tete;
}

// Fonction pour retourner l'élément en queue de la liste
Usager* retournerElementEnQueue(ListeUsagers* liste) {
    Usager* courant = liste->tete;
    while (courant != NULL && courant->suivant != NULL) {
        courant = courant->suivant;
    }
    return courant;
}

int main() {
    // Initialisation d'une liste vide
    ListeUsagers liste;
    liste.tete = NULL;

    // Ajout d'usagers
    Usager* usager1 = creerUsager(1, "John Doe");
    ajouterEnTete(&liste, usager1);

    Usager* usager2 = creerUsager(2, "Jane Doe");
    ajouterEnTete(&liste, usager2);

    Usager* usager3 = creerUsager(3, "Alice");
    ajouterEnTete(&liste, usager3);

    // Libération de la mémoire (à faire à la fin du programme)
    free(usager1);
    free(usager2);
    free(usager3);

    return 0;
}