#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <list.c>

#define CAPACITE_MAX 10
#define FREQUENCE_MAX 5
#define ETAGES 10

// Structure pour représenter l'ascenseur
typedef struct {
    int etage_actuel;
    ListeUsagers* charge;
} Ascenseur;

// Structure pour représenter un usager
typedef struct {
    int etage_appel;
    int etage_destination;
} Usager;

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

/*
void ordonanceurAscenseur(Ascenseur ascenseur, int tube_ascenseur[2]){
    Usager usagers[100];
    int size_usagers = 0;

    // Le processus principal gère les déplacements de l'ascenseur
    for (int i = 0; i < 2; i++) {
        read(tube_ascenseur[0], &usagers[size_usagers], sizeof(Usager));

        // Déplacement de l'ascenseur vers l'étage d'appel de l'usager
        deplacerAscenseur(&ascenseur, usagers[size_usagers].etage_appel);

        // Déplacement de l'ascenseur vers l'étage de destination de l'usager
        deplacerAscenseur(&ascenseur, usagers[size_usagers].etage_destination);

        // Desservir l'usager
        desservirUsager(&usagers[size_usagers]);
        
        // Informer le processus usager que l'ascenseur a desservi l'usager
        write(tube_ascenseur[1], &usagers[size_usagers], sizeof(Usager));
        size_usagers++;
    }
}
*/



Usager randomUsager() {
    // 1/3 
    Usager usager;
    usager.etage_appel = rand() % ETAGES;
    usager.etage_destination = rand() % ETAGES;
    if (usager.etage_appel == usager.etage_destination) {
        usager.etage_destination = (usager.etage_destination + 1) % ETAGES;
    }
    return usager;
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
        return 0;
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


int main() {
    // Initialisation de la graine pour la génération aléatoire basée sur le temps actuel
    srand((unsigned int)time(NULL));

    Ascenseur ascenseur;
    ascenseur.etage_actuel = 0;

    int tube_ascenseur[2];
    pipe(tube_ascenseur);

    // Exemple d'appel d'ascenseur avec deux usagers
    Usager usager1;
    usager1.etage_appel = rand() % 10;             // Étage d'appel aléatoire entre 0 et 9
    usager1.etage_destination = rand() % 10;       // Étage de destination aléatoire entre 0 et 9

    Usager usager2;
    usager2.etage_appel = rand() % 10;             // Étage d'appel aléatoire entre 0 et 9
    usager2.etage_destination = rand() % 10;       // Étage de destination aléatoire entre 0 et 9

    // Créer un processus fils pour le premier usager
    pid_t pid1 = fork();

    if (pid1 == 0) {
        // Code du processus fils pour le premier usager
        processusUsager(&usager1, tube_ascenseur);
    }

    // Créer un processus fils pour le deuxième usager
    pid_t pid2 = fork();

    if (pid2 == 0) {
        // Code du processus fils pour le deuxième usager
        processusUsager(&usager2, tube_ascenseur);
    }
    close(tube_ascenseur[1]);  // Fermer le côté d'écriture du tube dans le processus principal

    //threads 
    processusAscenseur(Ascenseur *ascenseur, ListeUsagers* usagers, ListeUsagers* usagers_montants, ListeUsagers* usagers_descendants);

    // Attendre que les processus fils se terminent
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
