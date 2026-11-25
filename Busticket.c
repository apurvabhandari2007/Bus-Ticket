#include<stdio.h>
#include <string.h>

#define MAX_ROUTES   4
#define SEATS        20      
#define MAX_BOOKINGS 200
#define FILE_NAME    "bookings.txt"

/* ---------- STRUCTURE 1: one bus route ---------- */
struct Route {
    int   id;
    char  source[20];
    char  dest[20];
    char  time[10];
    float fare;
};

/* ---------- STRUCTURE 2: one booking record ---------- */
struct Booking {
    int   id;          /* unique booking id */
    char  name[30];
    int   age;
    int   routeId;
    int   seatNo;
    float fare;
};

/* ---------- ARRAYS: our "tables" in memory ---------- */
struct Route routes[MAX_ROUTES] = {
    {1, "Delhi",  "Shimla",   "08:00", 650.0},
    {2, "Delhi",  "Manali",   "18:30", 1200.0},
    {3, "Baddi",  "Chandigarh","07:15", 150.0},
    {4, "Shimla", "Chandigarh","14:00", 300.0}
};

struct Booking bookings[MAX_BOOKINGS];
int bookingCount = 0;    /* how many bookings exist */
int nextId = 1;          /* id for the next booking */

/* seats[r][s] = 0 means free, 1 means booked
   (route index r, seat number s; index 0 is unused) */
int seats[MAX_ROUTES][SEATS + 1];

/* ---------- FILE HANDLING: save all bookings to file ---------- */
void saveBookings() {
    FILE *fp = fopen(FILE_NAME, "w");      /* "w" = write (overwrite) */
    if (fp == NULL) {
        printf("Error: could not open file!\n");
        return;
    }
    for (int i = 0; i < bookingCount; i++) {
        fprintf(fp, "%d %s %d %d %d %.2f\n",
                bookings[i].id, bookings[i].name, bookings[i].age,
                bookings[i].routeId, bookings[i].seatNo, bookings[i].fare);
    }
    fclose(fp);
}

/* ---------- FILE HANDLING: load bookings when program starts ---------- */
void loadBookings() {
    FILE *fp = fopen(FILE_NAME, "r");      /* "r" = read */
    if (fp == NULL) return;                /* first run: no file yet */

    struct Booking b;
    while (fscanf(fp, "%d %s %d %d %d %f",
                  &b.id, b.name, &b.age, &b.routeId, &b.seatNo, &b.fare) == 6) {
        bookings[bookingCount++] = b;
        seats[b.routeId - 1][b.seatNo] = 1;   /* mark seat as booked */
        if (b.id >= nextId) nextId = b.id + 1;
    }
    fclose(fp);
}

/* ---------- Show all routes ---------- */
void displayRoutes() {
    printf("\n%-4s %-10s %-12s %-8s %-8s\n", "No.", "From", "To", "Time", "Fare");
    printf("------------------------------------------------\n");
    for (int i = 0; i < MAX_ROUTES; i++) {
        printf("%-4d %-10s %-12s %-8s Rs.%-.0f\n",
               routes[i].id, routes[i].source, routes[i].dest,
               routes[i].time, routes[i].fare);
    }
}

/* ---------- Show seats of one route ---------- */
void displaySeats(int r) {
    printf("\nSeats for %s -> %s  ([XX] = booked)\n",
           routes[r].source, routes[r].dest);
    for (int s = 1; s <= SEATS; s++) {
        if (seats[r][s] == 1)
            printf("[XX] ");
        else
            printf("[%2d] ", s);
        if (s % 4 == 0) printf("\n");      /* 4 seats per row */
    }
}

/* ---------- Book a ticket ---------- */
void bookTicket() {
    int r, s;
    struct Booking b;

    displayRoutes();
    printf("Choose route number: ");
    scanf("%d", &r);
    if (r < 1 || r > MAX_ROUTES) {
        printf("Invalid route!\n");
        return;
    }
    r--;                                   /* number -> array index */

    displaySeats(r);
    printf("Choose seat number (1-%d): ", SEATS);
    scanf("%d", &s);
    if (s < 1 || s > SEATS) {
        printf("Invalid seat number!\n");
        return;
    }
    if (seats[r][s] == 1) {
        printf("Sorry, seat %d is already booked!\n", s);
        return;
    }
    if (bookingCount >= MAX_BOOKINGS) {
        printf("Booking list is full!\n");
        return;
    }

    printf("Enter passenger name (no spaces, use _): ");
    scanf("%s", b.name);
    printf("Enter age: ");
    scanf("%d", &b.age);

    b.id      = nextId++;
    b.routeId = routes[r].id;
    b.seatNo  = s;
    b.fare    = routes[r].fare;

    bookings[bookingCount++] = b;          /* add to array */
    seats[r][s] = 1;                       /* mark seat taken */
    saveBookings();                        /* write to file */

    printf("\n===== TICKET CONFIRMED =====\n");
    printf("Booking ID : %d\n", b.id);
    printf("Passenger  : %s (age %d)\n", b.name, b.age);
    printf("Route      : %s -> %s\n", routes[r].source, routes[r].dest);
    printf("Time       : %s\n", routes[r].time);
    printf("Seat No.   : %d\n", b.seatNo);
    printf("Fare       : Rs.%.2f\n", b.fare);
}

/* ---------- View all booking records ---------- */
void viewBookings() {
    if (bookingCount == 0) {
        printf("No bookings yet.\n");
        return;
    }
    printf("\n%-6s %-15s %-5s %-18s %-5s %-8s\n",
           "ID", "Name", "Age", "Route", "Seat", "Fare");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < bookingCount; i++) {
        int r = bookings[i].routeId - 1;
        char routeName[40];
        sprintf(routeName, "%s-%s", routes[r].source, routes[r].dest);
        printf("%-6d %-15s %-5d %-18s %-5d %-8.2f\n",
               bookings[i].id, bookings[i].name, bookings[i].age,
               routeName, bookings[i].seatNo, bookings[i].fare);
    }
}

/* ---------- Cancel a booking ---------- */
void cancelTicket() {
    int id, found = 0;
    printf("Enter Booking ID to cancel: ");
    scanf("%d", &id);

    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].id == id) {
            seats[bookings[i].routeId - 1][bookings[i].seatNo] = 0;  /* free seat */
            for (int j = i; j < bookingCount - 1; j++)               /* shift left */
                bookings[j] = bookings[j + 1];
            bookingCount--;
            saveBookings();
            printf("Booking %d cancelled.\n", id);
            found = 1;
            break;
        }
    }
    if (!found)
        printf("Booking ID not found.\n");
}

/* ---------- MAIN: menu ---------- */
int main() {
    int choice;
    loadBookings();        /* read old bookings from file first */

    do {
        printf("\n===== BUS TICKET BOOKING SYSTEM =====\n");
        printf("1. View Routes\n");
        printf("2. View Seats of a Route\n");
        printf("3. Book Ticket\n");
        printf("4. View All Bookings\n");
        printf("5. Cancel Ticket\n");
        printf("6. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {   /* user typed a letter */
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            if (c == EOF) break;
            choice = 0;
        }

        switch (choice) {
            case 1: displayRoutes(); break;
            case 2: {
                int r;
                displayRoutes();
                printf("Choose route number: ");
                scanf("%d", &r);
                if (r >= 1 && r <= MAX_ROUTES) displaySeats(r - 1);
                else printf("Invalid route!\n");
                break;
            }
            case 3: bookTicket();   break;
            case 4: viewBookings(); break;
            case 5: cancelTicket(); break;
            case 6: printf("Thank you!\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 6);

    return 0;
}