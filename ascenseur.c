#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "list.c"
#include <pthread.h>

#define CAPACITE_MAX 10
#define FREQUENCE 5
#define ETAGES 10

// Structure pour représenter l'ascenseur
typedef struct {
    int etage_actuel;
    ListeUsagers* charge;
} Ascenseur;


ListeUsagers usagers;
ListeUsagers usagers_montants;
ListeUsagers usagers_descendants;
Ascenseur ascenseur;





pthread_mutex_t mutexListe = PTHREAD_MUTEX_INITIALIZER;


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

int usagerDirection(Usager usager){
    if((usager.etage_destination - usager.etage_appel) > 0){
        return 1;
    }else if((usager.etage_destination - usager.etage_appel) < 0){
        return -1;
    }else{
        return 0;
    }
}

int ascenseurDirection(Ascenseur ascenseur, int destination){
    if((destination - ascenseur.etage_actuel) > 0){
        return 1;
    }else if((destination - ascenseur.etage_actuel) < 0){
        return -1;
    }else{
        return 0;
    }
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
        if(ascenseurDirection(*ascenseur,destination) > 0){
            ajouterCroissantDestination(ascenseur->charge, courant->usager);
        }else{
            ajouterDecroissantDestination(ascenseur->charge, courant->usager);
        }
        
        supprimerTete(liste_courante);
        //suprimer également de la liste globale
        supprimerUsager(usagers, courant->usager);
    }
}


// Renvoie la destination souhaitée avec un ascenseur remplis d'usagers
int deplacerFIFO(Ascenseur *ascenseur, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants){
    Usager* usager_destination = retournerElementEnQueue(ascenseur->charge);
    int destination = usager_destination->etage_destination;

    return destination;    
}

// fonction pour savoir a quel etage aller pour récupérer un usager en fonction de la direction de destination
Usager* recupererFIFO(ListeUsagers* usagers){
    Usager* usagerDestination = retournerElementEnTete(usagers);

    return usagerDestination;
}

void processusAscenseur(Ascenseur *ascenseur, ListeUsagers* usagers, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants){
    int destination = 0;

    if(ascenseur->charge->size == 0){
        Usager* usager = recupererFIFO(usagers);
        if(usager == NULL){
            destination = ascenseur->etage_actuel;
        }else{
            //recupere l'usager si l'ascenseur est à sa destination
            if(ascenseur->etage_actuel == destination){
                //supprime de la liste principale
                supprimerUsager(usagers,usager);
                //Et des listes secondaires pour éviter les doublons
                supprimerUsager(usagers_montants,usager);
                supprimerUsager(usagers_descendants,usager);
            }
            destination = usager->etage_destination;
            ajouterEnTete(ascenseur->charge, usager);
        }
    }else{
        destination = deplacerFIFO(ascenseur, usagers_montants, usagers_descendants);
        desservirUsagers(ascenseur);
        recupererUsagersMemeDirection(ascenseur, usagers, usagers_montants, usagers_descendants, destination);
    }
    

    //deplacer l'ascenseur
    sleep(1);  // Temps de déplacement d'une seconde
    ascenseur->etage_actuel += (ascenseur->etage_actuel < destination) ? 1 : -1;
    printf("Ascenseur : À l'étage %d\n", ascenseur->etage_actuel);
}


void* threadAscenseur(void* arg){
    while(1){
        pthread_mutex_lock(&mutexListe);

        //traiter la liste des usagers
        //processusAscenseur(&ascenseur, &usagers, &usagers_montants, &usagers_descendants);
        //printf("oui");

        pthread_mutex_unlock(&mutexListe);
    }
    return NULL;
}


void* threadUsagersFonction(void* arg){

    while(1){
        if(rand() % (FREQUENCE+1) == FREQUENCE){
            Usager random_usager = randomUsager();
            pthread_mutex_lock(&mutexListe);
            ajouterEnTete(&usagers,&random_usager);
            pthread_mutex_unlock(&mutexListe);
        }
        sleep(1);
    }



    return NULL;
}


int main() {
    srand(time(NULL));
    /*    
    for (int i =  0; i < 10; i++) {
        Usager usager = randomUsager();
        printf("Etage appel : %d, Etage destination : %d\n", usager.etage_appel, usager.etage_destination);
    }
    */
    pthread_t pthreadAscenseur;
    pthread_t pthreadUsagers;

    //crétion des threads
    pthread_create(&pthreadAscenseur, NULL, threadAscenseur, NULL);
    pthread_create(&pthreadUsagers, NULL, threadUsagersFonction, NULL);

    //attendre que les threads se terminent
    pthread_join(pthreadAscenseur, NULL);
    pthread_join(pthreadUsagers,NULL);

    return 0;
}
