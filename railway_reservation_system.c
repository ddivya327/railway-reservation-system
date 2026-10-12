
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TRAINS 6
#define MAX_SEATS 40
#define MAX_NAME 50
#define MAX_CITY 30
#define WAIT_CAPACITY 100

/* =========================================================
   RAILWAY RESERVATION AND WAITING LIST MANAGEMENT SYSTEM
   DSA Mini Project - C
   Data Structures:
   1. Arrays       -> trains and seat availability
   2. Linked List  -> confirmed reservations
   3. Queue        -> waiting list (FIFO)
   4. BST          -> PNR indexing/search
   5. Sorting      -> reservation reports
   ========================================================= */

typedef struct {
    int trainNo;
    char trainName[60];
    char source[MAX_CITY];
    char destination[MAX_CITY];
    int totalSeats;
    int availableSeats;
} Train;

typedef struct Booking {
    int pnr;
    int trainNo;
    int seatNo;
    char passengerName[MAX_NAME];
    int age;
    char gender;
    struct Booking *next;
} Booking;

typedef struct WaitPassenger {
    int waitId;
    int trainNo;
    char passengerName[MAX_NAME];
    int age;
    char gender;
    struct WaitPassenger *next;
} WaitPassenger;

typedef struct {
    WaitPassenger *front;
    WaitPassenger *rear;
    int count;
} Queue;

typedef struct PNRNode {
    int pnr;
    Booking *booking;
    struct PNRNode *left;
    struct PNRNode *right;
} PNRNode;

Train trains[MAX_TRAINS] = {
    {101, "Deccan Express", "Mumbai", "Pune", MAX_SEATS, MAX_SEATS},
    {102, "Vidarbha Express", "Nagpur", "Mumbai", MAX_SEATS, MAX_SEATS},
    {103, "Intercity Express", "Pune", "Nashik", MAX_SEATS, MAX_SEATS},
    {104, "Gondwana Express", "Nagpur", "Pune", MAX_SEATS, MAX_SEATS},
    {105, "Maharashtra Express", "Kolhapur", "Gondia", MAX_SEATS, MAX_SEATS},
    {106, "Godavari Express", "Nashik", "Hyderabad", MAX_SEATS, MAX_SEATS}
};

Booking *bookingHead = NULL;
PNRNode *pnrRoot = NULL;
Queue waitQueues[MAX_TRAINS];

int nextPNR = 100001;
int nextWaitId = 1;

/* ---------- Utility ---------- */

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void readLine(char *buffer, int size) {
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

void pauseScreen(void) {
    printf("\nPress Enter to continue...");
    getchar();
}

int findTrainIndex(int trainNo) {
    for (int i = 0; i < MAX_TRAINS; i++) {
        if (trains[i].trainNo == trainNo)
            return i;
    }
    return -1;
}

void initializeQueues(void) {
    for (int i = 0; i < MAX_TRAINS; i++) {
        waitQueues[i].front = NULL;
        waitQueues[i].rear = NULL;
        waitQueues[i].count = 0;
    }
}

/* ---------- Train / Seat Management ---------- */

void displayTrains(void) {
    printf("\n================ AVAILABLE TRAINS ================\n");
    printf("%-8s %-24s %-15s %-15s %-8s\n",
           "Train", "Name", "Source", "Destination", "Seats");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < MAX_TRAINS; i++) {
        printf("%-8d %-24s %-15s %-15s %-8d\n",
               trains[i].trainNo,
               trains[i].trainName,
               trains[i].source,
               trains[i].destination,
               trains[i].availableSeats);
    }
}

void showSeatMap(int trainIndex) {
    printf("\nSeat map for %s (%d)\n", trains[trainIndex].trainName,
           trains[trainIndex].trainNo);
    printf("[O] = Available   [X] = Booked\n\n");

    /* Seat availability is derived from confirmed bookings. */
    for (int seat = 1; seat <= trains[trainIndex].totalSeats; seat++) {
        int booked = 0;
        Booking *cur = bookingHead;

        while (cur != NULL) {
            if (cur->trainNo == trains[trainIndex].trainNo &&
                cur->seatNo == seat) {
                booked = 1;
                break;
            }
            cur = cur->next;
        }

        printf("%02d[%c]  ", seat, booked ? 'X' : 'O');
        if (seat % 5 == 0)
            printf("\n");
    }
    printf("\n");
}

int getNextAvailableSeat(int trainIndex) {
    for (int seat = 1; seat <= trains[trainIndex].totalSeats; seat++) {
        int occupied = 0;
        Booking *cur = bookingHead;

        while (cur != NULL) {
            if (cur->trainNo == trains[trainIndex].trainNo &&
                cur->seatNo == seat) {
                occupied = 1;
                break;
            }
            cur = cur->next;
        }

        if (!occupied)
            return seat;
    }
    return -1;
}

/* ---------- Linked List: Confirmed Reservations ---------- */

Booking *createBooking(int trainNo, int seatNo, const char *name,
                       int age, char gender) {
    Booking *newNode = (Booking *)malloc(sizeof(Booking));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    newNode->pnr = nextPNR++;
    newNode->trainNo = trainNo;
    newNode->seatNo = seatNo;
    strncpy(newNode->passengerName, name, MAX_NAME - 1);
    newNode->passengerName[MAX_NAME - 1] = '\0';
    newNode->age = age;
    newNode->gender = gender;
    newNode->next = NULL;

    return newNode;
}

void appendBooking(Booking *newNode) {
    if (bookingHead == NULL) {
        bookingHead = newNode;
        return;
    }

    Booking *cur = bookingHead;
    while (cur->next != NULL)
        cur = cur->next;

    cur->next = newNode;
}

/* ---------- BST: PNR Index ---------- */

PNRNode *insertPNR(PNRNode *root, int pnr, Booking *booking) {
    if (root == NULL) {
        PNRNode *node = (PNRNode *)malloc(sizeof(PNRNode));
        if (node == NULL) {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        node->pnr = pnr;
        node->booking = booking;
        node->left = node->right = NULL;
        return node;
    }

    if (pnr < root->pnr)
        root->left = insertPNR(root->left, pnr, booking);
    else if (pnr > root->pnr)
        root->right = insertPNR(root->right, pnr, booking);

    return root;
}

PNRNode *searchPNR(PNRNode *root, int pnr) {
    if (root == NULL || root->pnr == pnr)
        return root;

    if (pnr < root->pnr)
        return searchPNR(root->left, pnr);

    return searchPNR(root->right, pnr);
}

/* ---------- Queue: Waiting List ---------- */

void enqueueWaiting(int trainIndex, const char *name, int age, char gender) {
    if (waitQueues[trainIndex].count >= WAIT_CAPACITY) {
        printf("\nWaiting list is full for this train.\n");
        return;
    }

    WaitPassenger *node = (WaitPassenger *)malloc(sizeof(WaitPassenger));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    node->waitId = nextWaitId++;
    node->trainNo = trains[trainIndex].trainNo;
    strncpy(node->passengerName, name, MAX_NAME - 1);
    node->passengerName[MAX_NAME - 1] = '\0';
    node->age = age;
    node->gender = gender;
    node->next = NULL;

    if (waitQueues[trainIndex].rear == NULL) {
        waitQueues[trainIndex].front = node;
        waitQueues[trainIndex].rear = node;
    } else {
        waitQueues[trainIndex].rear->next = node;
        waitQueues[trainIndex].rear = node;
    }

    waitQueues[trainIndex].count++;

    printf("\nNo confirmed seat is available.");
    printf("\nPassenger added to WAITING LIST with ID WL%04d.\n", node->waitId);
}

WaitPassenger *dequeueWaiting(int trainIndex) {
    Queue *q = &waitQueues[trainIndex];

    if (q->front == NULL)
        return NULL;

    WaitPassenger *node = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    q->count--;
    node->next = NULL;
    return node;
}

void displayWaitingList(void) {
    int found = 0;

    printf("\n================ WAITING LISTS ================\n");

    for (int i = 0; i < MAX_TRAINS; i++) {
        Queue *q = &waitQueues[i];

        if (q->front != NULL) {
            found = 1;
            printf("\nTrain %d - %s\n", trains[i].trainNo, trains[i].trainName);
            printf("%-10s %-25s %-6s %-8s\n",
                   "Wait ID", "Passenger", "Age", "Gender");
            printf("----------------------------------------------------\n");

            WaitPassenger *cur = q->front;
            while (cur != NULL) {
                printf("WL%-8d %-25s %-6d %-8c\n",
                       cur->waitId, cur->passengerName,
                       cur->age, cur->gender);
                cur = cur->next;
            }
        }
    }

    if (!found)
        printf("No passengers are currently waiting.\n");
}

/* ---------- Booking ---------- */

void bookTicket(void) {
    int trainNo, age, trainIndex;
    char name[MAX_NAME];
    char gender;

    displayTrains();

    printf("\nEnter train number: ");
    scanf("%d", &trainNo);
    clearInputBuffer();

    trainIndex = findTrainIndex(trainNo);

    if (trainIndex == -1) {
        printf("Invalid train number.\n");
        return;
    }

    printf("Passenger name: ");
    readLine(name, MAX_NAME);

    printf("Age: ");
    scanf("%d", &age);
    clearInputBuffer();

    printf("Gender (M/F/O): ");
    scanf(" %c", &gender);
    clearInputBuffer();
    gender = (char)toupper((unsigned char)gender);

    if (age <= 0 || age > 120) {
        printf("Invalid age.\n");
        return;
    }

    if (gender != 'M' && gender != 'F' && gender != 'O') {
        printf("Invalid gender.\n");
        return;
    }

    if (trains[trainIndex].availableSeats > 0) {
        int seatNo = getNextAvailableSeat(trainIndex);

        Booking *newBooking = createBooking(
            trainNo, seatNo, name, age, gender
        );

        appendBooking(newBooking);
        pnrRoot = insertPNR(pnrRoot, newBooking->pnr, newBooking);

        trains[trainIndex].availableSeats--;

        printf("\n========== BOOKING CONFIRMED ==========\n");
        printf("Passenger : %s\n", newBooking->passengerName);
        printf("Train     : %d - %s\n", trainNo, trains[trainIndex].trainName);
        printf("Seat No.  : %d\n", seatNo);
        printf("PNR       : %d\n", newBooking->pnr);
        printf("=======================================\n");
    } else {
        enqueueWaiting(trainIndex, name, age, gender);
    }
}

/* ---------- Reservation Search / Display ---------- */

void printBooking(const Booking *b) {
    int idx = findTrainIndex(b->trainNo);

    printf("\n------------- RESERVATION DETAILS -------------\n");
    printf("PNR            : %d\n", b->pnr);
    printf("Passenger Name  : %s\n", b->passengerName);
    printf("Age             : %d\n", b->age);
    printf("Gender          : %c\n", b->gender);
    printf("Train Number    : %d\n", b->trainNo);

    if (idx != -1) {
        printf("Train Name      : %s\n", trains[idx].trainName);
        printf("Route           : %s -> %s\n",
               trains[idx].source, trains[idx].destination);
    }

    printf("Seat Number     : %d\n", b->seatNo);
    printf("-----------------------------------------------\n");
}

void searchReservation(void) {
    int pnr;

    printf("\nEnter PNR: ");
    scanf("%d", &pnr);
    clearInputBuffer();

    PNRNode *result = searchPNR(pnrRoot, pnr);

    if (result == NULL) {
        printf("No reservation found for PNR %d.\n", pnr);
        return;
    }

    printBooking(result->booking);
}

void displayAllReservations(void) {
    if (bookingHead == NULL) {
        printf("\nNo confirmed reservations.\n");
        return;
    }

    printf("\n================ CONFIRMED RESERVATIONS ================\n");

    Booking *cur = bookingHead;
    while (cur != NULL) {
        printBooking(cur);
        cur = cur->next;
    }
}

/* ---------- Cancellation ---------- */

void transferWaitingPassenger(int trainIndex) {
    WaitPassenger *passenger = dequeueWaiting(trainIndex);

    if (passenger == NULL)
        return;

    int seatNo = getNextAvailableSeat(trainIndex);

    if (seatNo == -1) {
        /* This should not normally happen after a cancellation. */
        printf("Unable to allocate the released seat.\n");
        return;
    }

    Booking *newBooking = createBooking(
        trains[trainIndex].trainNo,
        seatNo,
        passenger->passengerName,
        passenger->age,
        passenger->gender
    );

    appendBooking(newBooking);
    pnrRoot = insertPNR(pnrRoot, newBooking->pnr, newBooking);
    trains[trainIndex].availableSeats--;

    printf("\nWaiting passenger promoted to CONFIRMED status.\n");
    printf("Passenger : %s\n", newBooking->passengerName);
    printf("Seat No.  : %d\n", newBooking->seatNo);
    printf("New PNR   : %d\n", newBooking->pnr);

    free(passenger);
}

void cancelTicket(void) {
    int pnr;

    printf("\nEnter PNR to cancel: ");
    scanf("%d", &pnr);
    clearInputBuffer();

    Booking *cur = bookingHead;
    Booking *prev = NULL;

    while (cur != NULL && cur->pnr != pnr) {
        prev = cur;
        cur = cur->next;
    }

    if (cur == NULL) {
        printf("Reservation with PNR %d not found.\n", pnr);
        return;
    }

    int trainIndex = findTrainIndex(cur->trainNo);

    if (prev == NULL)
        bookingHead = cur->next;
    else
        prev->next = cur->next;

    printf("\nCancellation successful for PNR %d (%s).\n",
           cur->pnr, cur->passengerName);

    free(cur);

    if (trainIndex != -1) {
        /*
           If somebody is waiting, immediately give the released
           seat to the first passenger in FIFO order.
        */
        if (waitQueues[trainIndex].front != NULL) {
            transferWaitingPassenger(trainIndex);
        } else {
            trains[trainIndex].availableSeats++;
            printf("Released seat is now available for new bookings.\n");
        }
    }
}

/* ---------- Sorting ---------- */

int compareBookingsByPNR(const void *a, const void *b) {
    Booking *ba = *(Booking **)a;
    Booking *bb = *(Booking **)b;

    if (ba->pnr < bb->pnr) return -1;
    if (ba->pnr > bb->pnr) return 1;
    return 0;
}

void sortReservationsByPNR(void) {
    int count = 0;
    Booking *cur = bookingHead;

    while (cur != NULL) {
        count++;
        cur = cur->next;
    }

    if (count == 0) {
        printf("\nNo reservations to sort.\n");
        return;
    }

    Booking **array = (Booking **)malloc(count * sizeof(Booking *));
    if (array == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    cur = bookingHead;
    for (int i = 0; i < count; i++) {
        array[i] = cur;
        cur = cur->next;
    }

    qsort(array, count, sizeof(Booking *), compareBookingsByPNR);

    printf("\n========== RESERVATIONS SORTED BY PNR ==========\n");
    printf("%-10s %-25s %-10s %-10s\n",
           "PNR", "Passenger", "Train", "Seat");
    printf("-------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("%-10d %-25s %-10d %-10d\n",
               array[i]->pnr,
               array[i]->passengerName,
               array[i]->trainNo,
               array[i]->seatNo);
    }

    free(array);
}

/* ---------- Reports ---------- */

void displayTrainStatistics(void) {
    printf("\n================ TRAIN STATISTICS ================\n");
    printf("%-8s %-24s %-12s %-12s %-10s\n",
           "Train", "Name", "Total", "Available", "Booked");
    printf("----------------------------------------------------------------\n");

    for (int i = 0; i < MAX_TRAINS; i++) {
        int booked = trains[i].totalSeats - trains[i].availableSeats;
        printf("%-8d %-24s %-12d %-12d %-10d\n",
               trains[i].trainNo,
               trains[i].trainName,
               trains[i].totalSeats,
               trains[i].availableSeats,
               booked);
    }
}

/* ---------- BST Traversal ---------- */

void inorderPNR(PNRNode *root) {
    if (root == NULL)
        return;

    inorderPNR(root->left);
    printf("%d  ", root->pnr);
    inorderPNR(root->right);
}

void displayPNRIndex(void) {
    if (pnrRoot == NULL) {
        printf("\nPNR index is empty.\n");
        return;
    }

    printf("\nPNRs in BST (Inorder Traversal):\n");
    inorderPNR(pnrRoot);
    printf("\n");
}

/* ---------- Cleanup ---------- */

void freeBookings(void) {
    Booking *cur = bookingHead;

    while (cur != NULL) {
        Booking *temp = cur;
        cur = cur->next;
        free(temp);
    }

    bookingHead = NULL;
}

void freeQueue(Queue *q) {
    WaitPassenger *cur = q->front;

    while (cur != NULL) {
        WaitPassenger *temp = cur;
        cur = cur->next;
        free(temp);
    }

    q->front = q->rear = NULL;
    q->count = 0;
}

void freeBST(PNRNode *root) {
    if (root == NULL)
        return;

    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

/* ---------- Menu ---------- */

void showMenu(void) {
    printf("\n\n====================================================\n");
    printf("       RAILWAY RESERVATION MANAGEMENT SYSTEM\n");
    printf("====================================================\n");
    printf("1.  Display All Trains\n");
    printf("2.  Show Seat Map\n");
    printf("3.  Book Ticket\n");
    printf("4.  Search Reservation by PNR\n");
    printf("5.  Cancel Ticket\n");
    printf("6.  Display All Confirmed Reservations\n");
    printf("7.  Display Waiting Lists\n");
    printf("8.  Sort Reservations by PNR\n");
    printf("9.  Display Train Statistics\n");
    printf("10. Display PNR BST Index\n");
    printf("0.  Exit\n");
    printf("====================================================\n");
    printf("Enter your choice: ");
}

int main(void) {
    int choice;
    int trainNo, trainIndex;

    initializeQueues();

    printf("\n****************************************************\n");
    printf("     RAILWAY RESERVATION & WAITING LIST SYSTEM\n");
    printf("              DSA MINI PROJECT - C\n");
    printf("****************************************************\n");

    do {
        showMenu();

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Please enter a valid numeric choice.\n");
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                displayTrains();
                pauseScreen();
                break;

            case 2:
                displayTrains();
                printf("\nEnter train number: ");
                scanf("%d", &trainNo);
                clearInputBuffer();

                trainIndex = findTrainIndex(trainNo);
                if (trainIndex == -1)
                    printf("Invalid train number.\n");
                else
                    showSeatMap(trainIndex);

                pauseScreen();
                break;

            case 3:
                bookTicket();
                pauseScreen();
                break;

            case 4:
                searchReservation();
                pauseScreen();
                break;

            case 5:
                cancelTicket();
                pauseScreen();
                break;

            case 6:
                displayAllReservations();
                pauseScreen();
                break;

            case 7:
                displayWaitingList();
                pauseScreen();
                break;

            case 8:
                sortReservationsByPNR();
                pauseScreen();
                break;

            case 9:
                displayTrainStatistics();
                pauseScreen();
                break;

            case 10:
                displayPNRIndex();
                pauseScreen();
                break;

            case 0:
                printf("\nThank you for using the Railway Reservation System!\n");
                break;

            default:
                printf("\nInvalid choice. Please try again.\n");
        }

    } while (choice != 0);

    for (int i = 0; i < MAX_TRAINS; i++)
        freeQueue(&waitQueues[i]);

    freeBookings();
    freeBST(pnrRoot);

    return 0;
}
