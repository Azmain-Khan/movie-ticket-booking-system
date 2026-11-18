#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 5
#define COLS 10

int seats[ROWS][COLS] = {0}; // 0 = empty, 1 = occupied
char seatOwners[ROWS][COLS][50] = {""}; // stores owner names
char currentUser[50] = ""; // logged-in user

// ANSI Colors
#define RESET   "\033[0m"
#define BLUE    "\033[1;34m"
#define GREEN   "\033[1;32m"
#define CYAN    "\033[1;36m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"

#define CONSOLE_WIDTH 80
#define BOX_WIDTH 60

// ------------------- UTILS -------------------
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printBoxLine(char borderChar) {
    int padding = (CONSOLE_WIDTH - BOX_WIDTH) / 2;
    for (int i = 0; i < padding; i++) printf(" ");
    for (int i = 0; i < BOX_WIDTH; i++) printf("%c", borderChar);
    printf("\n");
}

void printBoxText(const char *text, const char *color) {
    int padding = (CONSOLE_WIDTH - BOX_WIDTH) / 2;
    printf("%*s", padding, "");
    printf("| %s%-*s%s |\n", color, BOX_WIDTH - 4, text, RESET);
}

void waitForEnter() {
    int ch;
    printf(YELLOW "\nPress Enter to continue..." RESET);
    while ((ch = getchar()) != '\n' && ch != EOF);
}

// ------------------- WELCOME -------------------
void welcomeScreen() {
    clearScreen();
    printBoxLine('=');
    printBoxText("WELCOME TO THE SHONAR BANGLA SEATING SERVICE", GREEN);
    printBoxLine('=');
    printBoxText("Effortless reservations, reliable service.", CYAN);
    printBoxText("Book your seat with ease.", CYAN);
    printBoxLine('=');
    waitForEnter();
}

// ------------------- LOGIN SYSTEM -------------------
int createAccount() {
    char fullName[50], password[50];
    FILE *fp = fopen("users.txt", "a");
    if (!fp) {
        printf(RED "Error accessing database!\n" RESET);
        waitForEnter();
        return 0;
    }

    printf(YELLOW "Enter Full Name: " RESET);
    fgets(fullName, sizeof(fullName), stdin);
    fullName[strcspn(fullName, "\n")] = 0;

    printf(YELLOW "Enter Password: " RESET);
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    fprintf(fp, "%s|%s\n", fullName, password);
    fclose(fp);

    printf(GREEN "Account created successfully!\n" RESET);
    waitForEnter();
    return 1;
}

int login() {
    char fullName[50], password[50], line[200];
    FILE *fp;

    printf(YELLOW "Enter Full Name: " RESET);
    fgets(fullName, sizeof(fullName), stdin);
    fullName[strcspn(fullName, "\n")] = 0;

    printf(YELLOW "Enter Password: " RESET);
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    fp = fopen("users.txt", "r");
    if (!fp) {
        printf(RED "No users found. Please create an account first.\n" RESET);
        waitForEnter();
        return 0;
    }

    int authenticated = 0;
    while (fgets(line, sizeof(line), fp)) {
        char *savedName = strtok(line, "|");
        char *savedPass = strtok(NULL, "\n");
        if (savedName && savedPass && strcmp(fullName, savedName) == 0 && strcmp(password, savedPass) == 0) {
            authenticated = 1;
            strcpy(currentUser, fullName);
            break;
        }
    }
    fclose(fp);

    if (!authenticated) {
        printf(RED "Invalid credentials!\n" RESET);
        waitForEnter();
        return 0;
    }

    printf(GREEN "Login successful! Welcome %s\n" RESET, fullName);
    waitForEnter();
    return 1;
}

void loginInterface() {
    char input[10];
    int choice;
    while (1) {
        clearScreen();
        printBoxLine('=');
        printBoxText("Login or Create Account", BLUE);
        printBoxLine('-');
        printBoxText("1. Create New Account", GREEN);
        printBoxText("2. Login", GREEN);
        printBoxText("3. Exit", GREEN);
        printBoxLine('=');

        printf(YELLOW "\nEnter your choice: " RESET);
        fgets(input, sizeof(input), stdin);
        choice = atoi(input);

        switch (choice) {
            case 1: createAccount(); break;
            case 2: if (login()) return; break;
            case 3: printf(GREEN "Exiting... Thank you for choosing us. Goodbye!\n" RESET); exit(0);
            default: printf(RED "Invalid choice! Try again.\n" RESET); waitForEnter();
        }
    }
}

// ------------------- SEATING -------------------
void showStatisticsInline() {
    int totalSeats = ROWS * COLS, occupied = 0;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (seats[r][c]) occupied++;
    int remaining = totalSeats - occupied;

    printBoxLine('=');
    printBoxText("Seat Details", BLUE);
    printBoxLine('-');
    char line[100];
    sprintf(line, "Total Seats: %d", totalSeats); printBoxText(line, CYAN);
    sprintf(line, "Occupied Seats: %d", occupied); printBoxText(line, RED);
    sprintf(line, "Remaining Seats: %d", remaining); printBoxText(line, GREEN);
    printBoxLine('=');
}

void displaySeats() {
    clearScreen();
    printBoxLine('=');
    printBoxText("Seating Arrangement", BLUE);
    printBoxLine('-');

    int padding = (CONSOLE_WIDTH - BOX_WIDTH) / 2;

    printf("%*s|", padding, "");
    printf("     ");
    for (int c = 0; c < COLS; c++) {
        printf("S%-3d", c+1);
    }
    printf("%*s|\n", BOX_WIDTH - 6 - (COLS * 4), "");

    for (int r = 0; r < ROWS; r++) {
        printf("%*s|", padding, "");
        printf(" R%-2d ", r+1);
        for (int c = 0; c < COLS; c++) {
            printf(seats[r][c] ? RED " X  " RESET : GREEN " O  " RESET);
        }
        printf("%*s|\n", BOX_WIDTH - 6 - (COLS * 4), "");
    }

    printBoxLine('=');
    showStatisticsInline();

    // Show booked seat owners
    printf(YELLOW "\nBooked Seats with Owners:\n" RESET);
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (seats[r][c]) {
                printf("Row %d Seat %d -> %s\n", r+1, c+1, seatOwners[r][c]);
            }
        }
    }
}

void addSeat() {
    char buf[10]; int row, col;
    displaySeats();

    printf(YELLOW "Enter Row (1-%d): " RESET, ROWS);
    fgets(buf, sizeof(buf), stdin);
    row = atoi(buf);

    printf(YELLOW "Enter Seat (1-%d): " RESET, COLS);
    fgets(buf, sizeof(buf), stdin);
    col = atoi(buf);

    if (row<1||row>ROWS||col<1||col>COLS) {
        printf(RED "Invalid position!\n" RESET);
        waitForEnter();
        return;
    }
    if (seats[row-1][col-1]) {
        printf(RED "Seat already occupied by %s!\n" RESET, seatOwners[row-1][col-1]);
    }
    else {
        seats[row-1][col-1]=1;
        strcpy(seatOwners[row-1][col-1], currentUser);
        printf(GREEN "Seat booked successfully for %s!\n" RESET, currentUser);
    }
    waitForEnter();
}

void removeSeat() {
    char buf[10]; int row, col;
    displaySeats();

    printf(YELLOW "Enter Row (1-%d): " RESET, ROWS);
    fgets(buf, sizeof(buf), stdin); row = atoi(buf);

    printf(YELLOW "Enter Seat (1-%d): " RESET, COLS);
    fgets(buf, sizeof(buf), stdin); col = atoi(buf);

    if (row<1||row>ROWS||col<1||col>COLS) {
        printf(RED "Invalid position!\n" RESET);
        waitForEnter();
        return;
    }
    if (!seats[row-1][col-1]) {
        printf(RED "Seat is already empty!\n" RESET);
    }
    else {
        printf(GREEN "Seat cancelled for %s\n" RESET, seatOwners[row-1][col-1]);
        seats[row-1][col-1]=0;
        strcpy(seatOwners[row-1][col-1], "");
    }
    waitForEnter();
}

void updateSeat() {
    char buf[10]; int row,col,newRow,newCol;
    displaySeats();

    printf(YELLOW "Enter Current Row (1-%d): " RESET, ROWS);
    fgets(buf, sizeof(buf), stdin);
    row = atoi(buf);

    printf(YELLOW "Enter Current Seat (1-%d): " RESET, COLS);
    fgets(buf, sizeof(buf), stdin);
    col = atoi(buf);

    if (row<1||row>ROWS||col<1||col>COLS||!seats[row-1][col-1]) {
        printf(RED "Invalid current seat!\n" RESET);
        waitForEnter(); return;
    }

    printf(YELLOW "Enter New Row (1-%d): " RESET, ROWS);
    fgets(buf, sizeof(buf), stdin);
    newRow = atoi(buf);

    printf(YELLOW "Enter New Seat (1-%d): " RESET, COLS);
    fgets(buf, sizeof(buf), stdin);
    newCol = atoi(buf);

    if (newRow<1||newRow>ROWS||newCol<1||newCol>COLS||seats[newRow-1][newCol-1]) {
        printf(RED "New seat invalid or occupied!\n" RESET);
        waitForEnter();
        return;
    }

    strcpy(seatOwners[newRow-1][newCol-1], seatOwners[row-1][col-1]);
    seats[row-1][col-1]=0; strcpy(seatOwners[row-1][col-1], "");
    seats[newRow-1][newCol-1]=1;
    printf(GREEN "Seat updated successfully!\n" RESET);
    waitForEnter();
}

void saveSeats() {
    FILE *fp=fopen("seats.txt","w");
    if(!fp){
        printf(RED "Error saving file!\n" RESET);
        return;
    }

    for(int r=0;r<ROWS;r++){
        for(int c=0;c<COLS;c++){
            fprintf(fp,"%d|%s\n", seats[r][c], seatOwners[r][c]);
        }
    }
    fclose(fp);

    printf(GREEN "Seating arrangement saved!\n" RESET);
    waitForEnter();
}

void loadSeats() {
    FILE *fp=fopen("seats.txt","r");
    if(!fp){
        printf(RED "No saved file found!\n" RESET);
        waitForEnter();
        return;
    }
    for(int r=0;r<ROWS;r++){
        for(int c=0;c<COLS;c++){
            char owner[50];
            fscanf(fp,"%d|%[^\n]\n", &seats[r][c], owner);
            strcpy(seatOwners[r][c], owner);
        }
    }
    fclose(fp);

    printf(GREEN "Seating arrangement loaded!\n" RESET);
    waitForEnter();
}

// ------------------- MAIN -------------------
int main() {
    char buf[10]; int choice;
    clearScreen();
    welcomeScreen();

    loginInterface();

    while (1) {
        clearScreen();
        printBoxLine('=');
        printBoxText("Main Menu", BLUE);
        printBoxLine('-');
        printBoxText("1. Add a Seat", GREEN);
        printBoxText("2. Remove a Seat", GREEN);
        printBoxText("3. Update a Seat", GREEN);
        printBoxText("4. Display Seating Arrangement", GREEN);
        printBoxText("5. Save Seating Arrangement", GREEN);
        printBoxText("6. Load Seating Arrangement", GREEN);
        printBoxText("7. Exit", GREEN);
        printBoxLine('=');

        printf(YELLOW "\nEnter your choice: " RESET);
        fgets(buf, sizeof(buf), stdin);
        choice = atoi(buf);

        switch (choice) {
            case 1: addSeat(); break;
            case 2: removeSeat(); break;
            case 3: updateSeat(); break;
            case 4: displaySeats(); waitForEnter(); break;
            case 5: saveSeats(); break;
            case 6: loadSeats(); break;
            case 7: printf(GREEN "Exiting... Thank you for choosing us. Goodbye!\n" RESET); exit(0);
            default: printf(RED "Invalid choice!\n" RESET); waitForEnter();
        }
    }
}
