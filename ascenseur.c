#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "list.c"
#include <pthread.h>

#define CAPACITE_MAX 10
#define FREQUENCE_MAX 5
#define ETAGES 10

// Structure pour représenter l'ascenseur
typedef struct {
    int etage_actuel;
    ListeUsagers* charge;
} Ascenseur;

pthread_mutex_t mutexListe = PTHREAD_MUTEX_INITIALIZER;


int ascenseurDirection(Ascenseur ascenseur, int destination);

// Fonction pour faire avancer l'ascenseur d'un étage
void deplacerAscenseur(Ascenseur *ascenseur, int destination) {

    printf("Ascenseur : Déplacement de l'étage %d à l'étage %d\n", ascenseur->etage_actuel, destination);

    // Simulation du déplacement de l'ascenseur
    while (ascenseur->etage_actuel != destination) {
        sleep(1);  // Temps de déplacement d'une seconde
        ascenseur->etage_actuel += (ascenseur->etage_actuel < destination) ? 1 : -1;
        printf("Ascenseur : À l'étage %d\n", ascenseur->etage_actuel);
    }
}

// Fonction pour le code du processus usager
void processusUsager(Usager *usager, int tube_ascenseur[2]) {
    close(tube_ascenseur[0]);  // Fermer le côté de lecture du tube

    // Appel initial de l'ascenseur
    printf("Usager : Appelle l'ascenseur à l'étage %d\n", usager->etage_appel);
    write(tube_ascenseur[1], usager, sizeof(Usager));

    // Attendre que l'ascenseur desserve l'usager
    read(tube_ascenseur[1], usager, sizeof(Usager));

    // Le processus usager se termine
    exit(0);
}

Usager randomUsager() {
    Usager usager;
    // 1/3 
    int etage_0 = rand() % 6;
    printf("etage 0 : %d\n", etage_0);
    if (etage_0 == 0) {
        usager.etage_appel = 0;
        usager.etage_destination = rand() % (ETAGES - 1) + 1;
        return usager;
    }else if (etage_0 == 1) {
        usager.etage_appel = rand() % (ETAGES - 1) + 1;
        usager.etage_destination = 0;
        return usager;
    }else{
        usager.etage_appel = rand() % (ETAGES);
        usager.etage_destination = rand() % (ETAGES);
        while (usager.etage_appel == usager.etage_destination) {
            usager.etage_destination = rand() % (ETAGES);
        }
        return usager;
    }
}

void threadUsagers(ListeUsagers* usagers, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants){

}

// Fonction pour desservir un usager
void desservirUsagers(Ascenseur* ascenseur) {
    while(ascenseur->charge->tete->usager->etage_destination == ascenseur->etage_actuel){
        //supprime l'usager de la liste
        supprimerTete(ascenseur->charge);
        printf("Ascenseur : Dessert l'usager à l'étage %d\n", ascenseur->etage_actuel);
    }
}

void recupererUsagersMemeDirection(Ascenseur *ascenseur, ListeUsagers* usagers, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants, int destination){

    ListeUsagers* liste_courante = ascenseurDirection(*ascenseur,destination) > 0 ? usagers_montants : usagers_descendants;

    UsagerNode* courant = liste_courante->tete;

    while(ascenseur->charge->size < CAPACITE_MAX && courant != NULL && courant->usager->etage_appel == ascenseur->etage_actuel){
        ajouterCroissantDestination(ascenseur->charge, courant->usager);
        supprimerTete(liste_courante);
        //suprimer également de la liste globale
        supprimerUsager(usagers, courant->usager);
    }
}

int usagerDirection(Usager usager){
    if((usager.etage_destination - usager.etage_appel) > 0){
        return 1;
    }else{
        return 0;
    }
}

int ascenseurDirection(Ascenseur ascenseur, int destination){
    if((destination - ascenseur.etage_actuel) > 0){
        return 1;
    }else{
        return -1;
    }
}

// Renvoie la destination souhaitée avec un ascenseur remplis d'usagers
int deplacerFIFO(Ascenseur *ascenseur, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants){
    Usager* usager_destination = retournerElementEnQueue(&ascenseur->charge);
    int destination = usager_destination->etage_destination;

    return destination;    
}
// fonction pour savoir a quel etage aller pour récupérer un usager en fonction de la direction de destination
Usager* recupererFIFO(ListeUsagers* usagers){
    Usager* usagerDestination = retournerElementEnTete(usagers);

    return usagerDestination;
}

void threadAscenseur(Ascenseur *ascenseur, ListeUsagers* usagers, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants){

}

void processusAscenseur(Ascenseur *ascenseur, ListeUsagers* usagers, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants){
    int destination;

    if(ascenseur->charge->size == 0){
        Usager* usager = recupererFIFO(usagers);
        //recupere l'usager si l'ascenseur est à sa destination
        if(ascenseur->etage_actuel == destination){
            //supprime de la liste principale
            supprimerUsager(usagers,usager);
            //Et des listes secondaires pour éviter les doublons
            supprimerUsager(usagers_montants,usager);
            supprimerUsager(usagers_descendants,usager);
        }
        destination = usager->etage_destination;
        ajouterCroissantDestination(ascenseur->charge, usager);
    }else{
        destination = deplacerFIFO(&ascenseur, usagers_montants, usagers_descendants);
        desservirUsagers(&ascenseur);
        recupererUsagersMemeDirection(ascenseur, usagers, usagers_montants, usagers_descendants, destination);
    }
    

    //deplacer l'ascenseur
    sleep(1);  // Temps de déplacement d'une seconde
    ascenseur->etage_actuel += (ascenseur->etage_actuel < destination) ? 1 : -1;
    printf("Ascenseur : À l'étage %d\n", ascenseur->etage_actuel);
}


void* threadAscenseur1(void* arg){
    while(1){
        pthread_mutex_lock(&mutexListe);

        //traiter la liste des usagers
        processusAscenseur(&ascenseur1, &usagers, &usagers_montants, &usagers_descendants);

        pthread_mutex_unlock(&mutexListe);
    }
    return NULL;
}


int main() {
    for (int i =  0; i < 10; i++) {
        Usager usager = randomUsager();
        printf("Etage appel : %d, Etage destination : %d\n", usager.etage_appel, usager.etage_destination);
    }

    pthread_t threadAscenseur1;

    //crétion des threads
    pthread_create(&threadAscenseur1, NULL, threadAscenseur1, NULL);

    //attendre que les threads se terminent
    pthread_join(threadAscenseur1, NULL);

    return 0;
}
