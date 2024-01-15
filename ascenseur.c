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
#define ETAGES 9
#define INT_MAX 2147483647

// Structure pour représenter l'ascenseur
typedef struct {
    int etage_actuel;
    ListeUsagers* charge;
} Ascenseur;


ListeUsagers usagers;
ListeUsagers usagers_montants;
ListeUsagers usagers_descendants;
Ascenseur ascenseur1;
Ascenseur ascenseur2;


// Function prototype for Render
void Render(Ascenseur *ascenseur, ListeUsagers* usagers,int destination);



pthread_mutex_t mutexListe = PTHREAD_MUTEX_INITIALIZER;


Usager randomUsager() {
    Usager usager;
    // 1/3 
    int etage_0 = rand() % 6;
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
    while (ascenseur->charge->size != 0 && ascenseur->charge->tete->usager->etage_destination == ascenseur->etage_actuel) {
        free(ascenseur->charge->tete->usager);
        supprimerTete(ascenseur->charge);
    }
}

void recupererUsagersMemeDirection(Ascenseur* ascenseur, ListeUsagers* usagers, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants, int destination) {
    ListeUsagers* liste_courante = ascenseurDirection(*ascenseur, destination) > 0 ? usagers_montants : usagers_descendants;

    if (liste_courante->size == 0) {
        return;
    }

    UsagerNode* courant = liste_courante->tete;
    while (courant != NULL && courant->usager->etage_appel == ascenseur->etage_actuel) {
        if (ascenseurDirection(*ascenseur, destination) * usagerDirection(*courant->usager) > 0) {
            printf("Ajoute l'usager à la charge : A=%d, D=%d\n", courant->usager->etage_appel, courant->usager->etage_destination);
            ajouterCroissantDestination(ascenseur->charge, courant->usager);
            ascenseur->usagerPrisEnCharge = courant->usager;
        } else {
            printf("Ignore l'usager : A=%d, D=%d\n", courant->usager->etage_appel, courant->usager->etage_destination);
        }
        supprimerTete(liste_courante);
        supprimerUsager(usagers, courant->usager);
        courant = courant->suivant;
    }
}



// Renvoie la destination souhaitée avec un ascenseur remplis d'usagers
int deplacerFIFO(Ascenseur *ascenseur, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants) {
    if (ascenseur->etage_actuel == INT_MAX) {
        return 0;  // Ascenseur inactif, ne pas se déplacer
    }

    if (ascenseur->charge->size == 0) {
        // Si l'ascenseur est vide, choisir la destination basée sur la direction
        return (ascenseur->etage_actuel + ascenseur->charge->tete->usager->etage_destination) / 2;
    }

    // L'ascenseur est déjà en mouvement, continuez vers la destination actuelle
    return ascenseur->charge->tete->usager->etage_destination;
}

// fonction pour savoir a quel etage aller pour récupérer un usager en fonction de la direction de destination
Usager* recupererFIFO(ListeUsagers* usagers){
    if(usagers->size == 0){
        return NULL;
    }
    Usager* usagerDestination = retournerElementEnTete(usagers);

    return usagerDestination;
}

void processusAscenseur(Ascenseur *ascenseur, ListeUsagers* usagers, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants) {
    int destination = 0;

    if (ascenseur->charge->size == 0) {
        Usager* usager = recupererFIFO(usagers);

        if (usager == NULL) {
            destination = ascenseur->etage_actuel;
        } else {
            destination = usager->etage_appel;

            if (ascenseur->etage_actuel == destination) {
                supprimerUsager(usagers, usager);
                supprimerUsager(usagers_montants, usager);
                supprimerUsager(usagers_descendants, usager);

                ajouterEnTete(ascenseur->charge, usager);
                ascenseur->usagerPrisEnCharge = usager;
            }
        }

    } else {
        destination = deplacerFIFO(ascenseur, usagers_montants, usagers_descendants);
        desservirUsagers(ascenseur);
        //recupererUsagersMemeDirection(ascenseur, usagers, usagers_montants, usagers_descendants, destination);
    }

    if (ascenseur->etage_actuel < destination) {
        ascenseur->etage_actuel += 1;
    } else if (ascenseur->etage_actuel > destination) {
        ascenseur->etage_actuel += -1;
    }

    Render(ascenseur, usagers, destination);
}

void Render(Ascenseur *ascenseur, ListeUsagers* usagers,int destination){
    for(int y = 0;y<ETAGES + 1;y++){
        printf("\33[2K\r");
        printf("\033[A");
    }

    for(int i = 0;i<ETAGES + 1;i++){
        
        if(destination == ETAGES-i){
            printf("► | ");
        }else{
            printf("  | ");
        }
        if(ascenseur->etage_actuel == ETAGES-i){
            printf("█%d ",ascenseur->charge->size);
        }else{
            printf("   ");
        }
        UsagerNode* curseur = usagers->tete;
        int up = 0;
        int down = 0;
        for(int x = 0;x<usagers->size;x++){
            if(curseur->usager->etage_appel == ETAGES-i){
                up += usagerDirection(*curseur->usager) > 0 ? 1 : 0;
                down += usagerDirection(*curseur->usager) < 0 ? 1 : 0;
            }
            curseur = curseur->suivant;
        }
        if(up == 0){
            printf(" | ▲ -");
        }else{
            printf(" | ▲ %d",up);
        }
        if(down == 0){
            printf(" | ▼ -");
        }else{
            printf(" | ▼ %d",down);
        }
        printf("\n");

}
}



void* threadAscenseur(void* arg) {
    Ascenseur* ascenseur = (Ascenseur*)arg;

    while (1) {
        pthread_mutex_lock(&mutexListe);
        processusAscenseur(ascenseur, &usagers, &usagers_montants, &usagers_descendants);
        pthread_mutex_unlock(&mutexListe);
        sleep(1);
    }

    return NULL;
}


void* threadUsagersFonction(void* arg){
    if(FREQUENCE == 0){
        exit(0);
    }
    while(1){
        pthread_mutex_lock(&mutexListe);

        Usager* random_usager = (Usager*)malloc(sizeof(Usager));
        *random_usager = randomUsager();
        ajouterEnQueue(&usagers,random_usager);
        if(usagerDirection > 0)
            ajouterCroissantDestination(&usagers_montants,random_usager);
        else
            ajouterDecroissantDestination(&usagers_descendants,random_usager);
        //printf("Usager : Appelle l'ascenseur à l'étage %d, vers %d\n", random_usager->etage_appel,random_usager->etage_destination);
        //printListe(&usagers);
        pthread_mutex_unlock(&mutexListe);

        float tts = 1 + (float)rand()/INT_MAX  * FREQUENCE * 2;
        sleep(tts);
    }

    return NULL;
}


int main() {
    srand(time(NULL));
    usagers.tete = NULL;
    usagers.size = 0;
    usagers_montants.tete = NULL;
    usagers_montants.size = 0;
    usagers_descendants.tete = NULL;
    usagers_descendants.size = 0;


    ListeUsagers charge;
    charge.tete = NULL;
    charge.size = 0;

    ascenseur1.charge = &charge;
    ascenseur1.etage_actuel = 0;
    ascenseur1.usagerPrisEnCharge = NULL;

    ascenseur2.charge = &charge;
    ascenseur2.etage_actuel = 0;
    ascenseur2.usagerPrisEnCharge = NULL;

    pthread_t pthreadAscenseur;
    pthread_t pthreadUsagers;
    pthread_t pthreadAscenseur2;


    //crétion des threads
    pthread_create(&pthreadAscenseur, NULL, threadAscenseur, (void*)&ascenseur1);
    pthread_create(&pthreadAscenseur2, NULL, threadAscenseur, (void*)&ascenseur2);
    pthread_create(&pthreadUsagers, NULL, threadUsagersFonction, NULL);



    //attendre que les threads se terminent
    pthread_join(pthreadAscenseur, NULL);
    pthread_join(pthreadUsagers,NULL);
    pthread_join(pthreadAscenseur2,NULL);

    return 0;
}
