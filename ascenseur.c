#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "list.c"  
#include <pthread.h>

#define MAX_CAPACITY 10
#define FREQUENCY 2
#define FLOORS 9
#define INT_MAX 2147483647

// Structure to represent an elevator
typedef struct {
    int current_floor;
    UserList *load;  // List of users in the elevator
    User *passengerInCharge;  // User currently in the elevator
    int destination;
} Elevator;

// Global lists to store all users, upward users, and downward users
UserList users;
UserList upwardUsers;
UserList downwardUsers;

// Two elevator instances
Elevator elevator1;
Elevator elevator2;

pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for thread safety

// Function to generate a random user
User getRandomUser() {
    User user;
    // 1/3 chance for each type of user (call from 0, call from a floor, or random call/destination)
    int floor_0 = rand() % 6;
    if (floor_0 == 0) {
        user.floor_call = 0;
        do {
            user.destination_floor = rand() % (FLOORS - 1) + 1;
        } while (user.destination_floor == 0);
        return user;
    } else if (floor_0 == 1) {
        do {
            user.floor_call = rand() % (FLOORS - 1) + 1;
        } while (user.floor_call == 0);
        user.destination_floor = 0;
        return user;
    } else {
        user.floor_call = rand() % (FLOORS);
        user.destination_floor = rand() % (FLOORS);
        while (user.floor_call == user.destination_floor) {
            user.destination_floor = rand() % (FLOORS);
        }
        return user;
    }
}

// Function to determine the direction of a user's movement (1 for up, -1 for down, 0 for no movement)
int userDirection(User *user) {
    if (user == NULL) {
        return 0;
    }
    if ((user->destination_floor - user->floor_call) > 0) {
        return 1;
    } else if ((user->destination_floor - user->floor_call) < 0) {
        return -1;
    } else {
        return 0;
    }
}

// Function to determine the direction of an elevator's movement (1 for up, -1 for down, 0 for no movement)
int elevatorDirection(Elevator elevator, int destination) {
    if ((destination - elevator.current_floor) > 0) {
        return 1;
    } else if ((destination - elevator.current_floor) < 0) {
        return -1;
    } else {
        return 0;
    }
}

// Function to serve users in the elevator
void serveUsers(Elevator *elevator) {
    while (elevator->load->size != 0 && elevator->load->head->user->destination_floor == elevator->current_floor) {
        free(elevator->load->head->user);
        deleteHead(elevator->load);
    }
}

// Function to retrieve users in the same direction as the elevator's destination
void retrieveUsersSameDirection(Elevator *elevator, UserList *users, UserList *upwardUsers, UserList *downwardUsers, int destination) {
    UserList *currentList = elevatorDirection(*elevator, destination) > 0 ? upwardUsers : downwardUsers;

    if (currentList->size == 0) {
        return;
    }

    UserNode *current = currentList->head;
    while (current != NULL && current->user->floor_call == elevator->current_floor) {
        if (elevatorDirection(*elevator, destination) * userDirection(current->user) > 0) {
            addToAscendingDestination(elevator->load, current->user);
        } else {
            printf("Ignore user: Call=%d, Destination=%d\n", current->user->floor_call, current->user->destination_floor);
        }
        deleteHead(currentList);
        deleteUser(users, current->user);
        current = current->next;
    }
}

// Return the desired destination with a fully loaded elevator
int moveFIFO(Elevator *elevator, UserList *upwardUsers, UserList *downwardUsers) {
    if (elevator->current_floor == INT_MAX) {
        return 0;  // Inactive elevator, do not move
    }

    if (elevator->load->size == 0) {
        // If the elevator is empty, choose the destination based on the direction
        if (elevator->load->head != NULL) {
            return (elevator->current_floor + elevator->load->head->user->destination_floor) / 2;
        } else {
            return elevator->current_floor;  // No passengers, stay on the current floor
        }
    }

    // The elevator is already in motion, continue towards the current destination
    if (elevator->load->head != NULL) {
        return elevator->load->head->user->destination_floor;
    } else {
        return elevator->current_floor;  // No passengers, stay on the current floor
    }
}

// Function to determine which floor to go to pick up a user based on the destination direction
User *retrieveFIFO(UserList *users) {
    if (users->size == 0) {
        return NULL;
    }
    User *userDestination = getHeadElement(users);
    return userDestination;
}

// Function to process the elevator's movement and user handling
void elevatorProcess(Elevator *elevator, UserList *users, UserList *upwardUsers, UserList *downwardUsers) {
    if (elevator->load->size == 0) {
        if (elevator->passengerInCharge == NULL) {
            User *user = retrieveFIFO(users);
            if (user == NULL) {
                elevator->destination = elevator->current_floor;
            } else {
                elevator->passengerInCharge = user;
                elevator->destination = user->floor_call;
                deleteUser(users, user);
                deleteUser(upwardUsers, user);
                deleteUser(downwardUsers, user);
            }
        } else if (elevator->current_floor == elevator->destination) {
            addToHead(elevator->load, elevator->passengerInCharge);
            elevator->passengerInCharge = NULL;
        }
    } else {
        elevator->destination = moveFIFO(elevator, upwardUsers, downwardUsers);
        serveUsers(elevator);
        retrieveUsersSameDirection(elevator, users, upwardUsers, downwardUsers, elevator->destination);
    }

    if (elevator->current_floor < elevator->destination) {
        elevator->current_floor += 1;
    } else if (elevator->current_floor > elevator->destination) {
        elevator->current_floor -= 1;
    }
}

// Function to render the state of elevators and users
void Render() {
    for (int y = 0; y < FLOORS + 1; y++) {
        printf("\33[2K\r");
        printf("\033[A");
    }
    for (int i = 0; i < FLOORS + 1; i++) {
        printf("%d  ", FLOORS - i);
        if (elevator1.destination == FLOORS - i || elevator2.destination == FLOORS - i) {
            printf("► | ");
        } else {
            printf("  | ");
        }

        if (elevator1.current_floor == FLOORS - i) {
            printf("█%d ", elevator1.load->size);
        } else {
            printf("   ");
        }

        if (elevator2.current_floor == FLOORS - i) {
            printf("█%d ", elevator2.load->size);
        } else {
            printf("   ");
        }

        UserNode *cursor = users.head;
        int up = 0;
        int down = 0;
        for (int x = 0; x < users.size; x++) {
            if (cursor->user->floor_call == FLOORS - i) {
                up += userDirection(cursor->user) > 0 ? 1 : 0;
                down += userDirection(cursor->user) < 0 ? 1 : 0;
            }
            cursor = cursor->next;
        }
        if (elevator1.passengerInCharge != NULL && elevator1.passengerInCharge->floor_call == FLOORS - i) {
            if (userDirection(elevator1.passengerInCharge) < 0)
                down++;
            if (userDirection(elevator1.passengerInCharge) > 0)
                up++;
        }
        if (elevator2.passengerInCharge != NULL && elevator2.passengerInCharge->floor_call == FLOORS - i) {
            if (userDirection(elevator2.passengerInCharge) < 0)
                down++;
            if (userDirection(elevator2.passengerInCharge) > 0)
                up++;
        }

        if (up == 0) {
            printf(" | ▲ -");
        } else {
            printf(" | ▲ %d", up);
        }
        if (down == 0) {
            printf(" | ▼ -");
        } else {
            printf(" | ▼ %d", down);
        }
        printf("\n");
    }
}

// Function for the display thread to continuously render the state
void *displayThread(void *arg) {
    while (1) {
        Render();
        sleep(1);
    }

    return NULL;
}

// Function for the elevator thread to process elevator movements
void *elevatorThread(void *arg) {
    Elevator *elevator = (Elevator *)arg;

    while (1) {
        pthread_mutex_lock(&listMutex);
        elevatorProcess(elevator, &users, &upwardUsers, &downwardUsers);
        pthread_mutex_unlock(&listMutex);
        sleep(1);
    }

    return NULL;
}

// Function for the user thread to generate random users
void *userThreadFunction(void *arg) {
    if (FREQUENCY == 0) {
        exit(0);
    }
    while (1) {
        pthread_mutex_lock(&listMutex);

        User *randomUser = (User *)malloc(sizeof(User));
        *randomUser = getRandomUser();
        addToEnd(&users, randomUser);
        if (userDirection(randomUser) > 0)
            addToAscendingDestination(&upwardUsers, randomUser);
        else
            addToDescendingDestination(&downwardUsers, randomUser);
        pthread_mutex_unlock(&listMutex);

        float tts = 1 + (float)rand() / INT_MAX * FREQUENCY * 2;
        sleep(tts);
    }

    return NULL;
}

int main() {
    printf("\n");
    srand(time(NULL));

    // Initialize user lists, elevators, and mutex
    users.head = NULL;
    users.size = 0;
    upwardUsers.head = NULL;
    upwardUsers.size = 0;
    downwardUsers.head = NULL;
    downwardUsers.size = 0;

    UserList load1, load2;
    load1.head = NULL;
    load1.size = 0;
    load2.head = NULL;
    load2.size = 0;

    elevator1.load = &load1;
    elevator1.current_floor = 0;
    elevator1.passengerInCharge = NULL;
    elevator1.destination = 0;

    elevator2.load = &load2;
    elevator2.current_floor = 0;
    elevator2.passengerInCharge = NULL;
    elevator2.destination = 0;

    // Create threads
    pthread_t pthreadUsers;
    pthread_t pthreadElevator1;
    pthread_t pthreadElevator2;
    pthread_t pthreadDisplay;

    pthread_create(&pthreadUsers, NULL, userThreadFunction, NULL);
    pthread_create(&pthreadElevator1, NULL, elevatorThread, (void *)&elevator1);
    pthread_create(&pthreadElevator2, NULL, elevatorThread, (void *)&elevator2);
    pthread_create(&pthreadDisplay, NULL, displayThread, NULL);

    // Wait for threads to finish
    pthread_join(pthreadUsers, NULL);
    pthread_join(pthreadElevator1, NULL);
    pthread_join(pthreadElevator2, NULL);
    pthread_join(pthreadDisplay, NULL);

    return 0;
}
