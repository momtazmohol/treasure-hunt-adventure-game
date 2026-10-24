#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define TOTAL_ROOMS 10
#define MAX_INVENTORY 5
#define SAVE_FILE "treasure_save.txt"

/* ANSI colours for modern terminals */
#define RESET  "\033[0m"
#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define WHITE  "\033[97m"
#define GOLD   "\033[93m"

/* =========================================================
   ROOM STRUCTURE
   ========================================================= */

typedef struct
{
    int id;
    char name[50];
    char description[300];

    int left;
    int right;
    int forward;
    int back;

    int item;
    int trap;
    int puzzle;
    int locked;

} Room;


/* =========================================================
   PLAYER STRUCTURE
   ========================================================= */

typedef struct
{
    char name[50];

    int health;
    int currentRoom;

    char inventory[MAX_INVENTORY][30];
    int inventoryCount;

    int hasTorch;
    int hasMap;
    int hasKey;
    int hasArtifact;

    int puzzle1Solved;
    int puzzle2Solved;
    int puzzle3Solved;

    int mummyAwake;
    int mummyDefeated;

    int snakeEncountered;
    int trapTriggered;

    int artifactTaken;
    int gameWon;

} Player;


/* =========================================================
   GLOBAL ROOM ARRAY
   ========================================================= */

Room rooms[TOTAL_ROOMS];


/* =========================================================
   FUNCTION PROTOTYPES
   ========================================================= */

void initializeRooms();

void newGame(Player *p);
void gameLoop(Player *p);

void introduction();
void showMainMenu();

void showRoom(Player *p);
void showStatus(Player *p);
void showInventory(Player *p);

void movePlayer(Player *p);
void checkRoom(Player *p);

void addItem(Player *p, const char item[]);
int hasItem(Player *p, const char item[]);

void takeItem(Player *p);

void triggerTrap(Player *p);
void takeDamage(Player *p, int amount);
void snakeEncounter(Player *p);

void mummyEncounter(Player *p);

void gateRiddle(Player *p);
void puzzle1(Player *p);
void puzzle2(Player *p);
void puzzle3(Player *p);

void artifactEvent(Player *p);

void whiteRoom(Player *p);
void alienScene(Player *p);
void secretChamber(Player *p);
void goodSoul(Player *p);
void finalEscape(Player *p);

void gameOver(Player *p);
void gameWon(Player *p);

void saveGame(Player *p);
int loadGame(Player *p);

void pauseScreen();

void printSlowly(char text[]) {
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout);
        Sleep(30);
    }
}


int getChoice(int min, int max);


/* =========================================================
   MAIN
   ========================================================= */

int main()
{
    Player player;
    int choice;

    initializeRooms();

    while (1)
    {
        showMainMenu();

        choice = getChoice(1, 3);

        if (choice == 1)
        {
            newGame(&player);
            introduction();
            gateRiddle(&player);
            gameLoop(&player);
        }

        else if (choice == 2)
        {
            if (loadGame(&player))
            {
                printf("\nGame loaded successfully!\n");
                gameLoop(&player);
            }
            else
            {
                printf("\nNo saved game found.\n");
                pauseScreen();
            }
        }

        else
        {
            printf("\nThank you for playing!\n");
            break;
        }
    }

    return 0;
}


/* =========================================================
   MAIN MENU
   ========================================================= */

void showMainMenu()
{
    printf("\n\n");
    printf("====================================================\n");
    printf("          TREASURE HUNT ADVENTURE GAME\n");
    printf("====================================================\n");
    Sleep(400);
    printf("\n1. New Game\n");
    printf("2. Load Game\n");
    printf("3. Exit\n");

    printf("\nEnter choice: ");
}


/* =========================================================
   INITIALIZE ROOMS
   ========================================================= */

void initializeRooms()
{
    /* Room 0 */
    rooms[0].id = 0;
    strcpy(rooms[0].name, "Forest Entrance");
    strcpy(rooms[0].description,
           "You are in a mysterious forest. An ancient temple stands before you.");
    rooms[0].left = -1;
    rooms[0].right = -1;
    rooms[0].forward = 1;
    rooms[0].back = -1;
    rooms[0].item = 0;
    rooms[0].trap = 0;
    rooms[0].puzzle = 0;
    rooms[0].locked = 0;


    /* Room 1 */
    rooms[1].id = 1;
    strcpy(rooms[1].name, "Temple Entrance");
    strcpy(rooms[1].description,
           "The rusty temple gate closes behind you. Two ancient doors stand before you.");
    rooms[1].left = 2;
    rooms[1].right = 3;
    rooms[1].forward = -1;
    rooms[1].back = 0;
    rooms[1].item = 0;
    rooms[1].trap = 0;
    rooms[1].puzzle = 0;
    rooms[1].locked = 0;


    /* Room 2 */
    rooms[2].id = 2;
    strcpy(rooms[2].name, "Snake Corridor");
    strcpy(rooms[2].description,
           "A narrow stone corridor filled with ancient carvings. A poisonous snake guards the path.");
    rooms[2].left = -1;
    rooms[2].right = 4;
    rooms[2].forward = -1;
    rooms[2].back = 1;
    rooms[2].item = 1;       /* Torch */
    rooms[2].trap = 0;
    rooms[2].puzzle = 0;
    rooms[2].locked = 0;


    /* Room 3 */
    rooms[3].id = 3;
    strcpy(rooms[3].name, "Dark Chamber");
    strcpy(rooms[3].description,
           "A completely dark chamber. Something seems to be moving in the darkness.");
    rooms[3].left = 4;
    rooms[3].right = -1;
    rooms[3].forward = -1;
    rooms[3].back = 1;
    rooms[3].item = 2;       /* Map */
    rooms[3].trap = 1;
    rooms[3].puzzle = 0;
    rooms[3].locked = 0;


    /* Room 4 */
    rooms[4].id = 4;
    strcpy(rooms[4].name, "Artifact Chamber");
    strcpy(rooms[4].description,
           "A huge chamber filled with ancient statues, gold and mysterious artifacts.");
    rooms[4].left = -1;
    rooms[4].right = -1;
    rooms[4].forward = 5;
    rooms[4].back = 1;
    rooms[4].item = 4;       /* Artifact */
    rooms[4].trap = 0;
    rooms[4].puzzle = 0;
    rooms[4].locked = 0;


    /* Room 5 */
    rooms[5].id = 5;
    strcpy(rooms[5].name, "Mummy Hall");
    strcpy(rooms[5].description,
           "Ancient statues line the walls. The mummy's presence can be felt everywhere.");
    rooms[5].left = 6;
    rooms[5].right = 7;
    rooms[5].forward = -1;
    rooms[5].back = 4;
    rooms[5].item = 0;
    rooms[5].trap = 0;
    rooms[5].puzzle = 1;
    rooms[5].locked = 0;


    /* Room 6 */
    rooms[6].id = 6;
    strcpy(rooms[6].name, "Ancient Library");
    strcpy(rooms[6].description,
           "Dusty books and stone tablets cover the room. A strange riddle is carved into the wall.");
    rooms[6].left = -1;
    rooms[6].right = 8;
    rooms[6].forward = -1;
    rooms[6].back = 5;
    rooms[6].item = 0;
    rooms[6].trap = 0;
    rooms[6].puzzle = 2;
    rooms[6].locked = 0;


    /* Room 7 */
    rooms[7].id = 7;
    strcpy(rooms[7].name, "Trap Hall");
    strcpy(rooms[7].description,
           "A long hall with suspicious stone tiles on the floor.");
    rooms[7].left = 8;
    rooms[7].right = -1;
    rooms[7].forward = -1;
    rooms[7].back = 5;
    rooms[7].item = 0;
    rooms[7].trap = 2;
    rooms[7].puzzle = 0;
    rooms[7].locked = 0;


    /* Room 8 */
    rooms[8].id = 8;
    strcpy(rooms[8].name, "Locked Stone Door");
    strcpy(rooms[8].description,
           "A massive stone door blocks your path. An ancient keyhole is visible.");
    rooms[8].left = -1;
    rooms[8].right = -1;
    rooms[8].forward = 9;
    rooms[8].back = 6;
    rooms[8].item = 3;       /* Key */
    rooms[8].trap = 0;
    rooms[8].puzzle = 3;
    rooms[8].locked = 1;


    /* Room 9 */
    rooms[9].id = 9;
    strcpy(rooms[9].name, "White Room");
    strcpy(rooms[9].description,
           "Everything is completely white. An unknown alien-like figure stands before you.");
    rooms[9].left = -1;
    rooms[9].right = -1;
    rooms[9].forward = -1;
    rooms[9].back = 8;
    rooms[9].item = 0;
    rooms[9].trap = 0;
    rooms[9].puzzle = 0;
    rooms[9].locked = 0;
}


/* =========================================================
   NEW GAME
   ========================================================= */

void newGame(Player *p)
{
    int i;

    strcpy(p->name, "Explorer");

    p->health = 100;
    p->currentRoom = 0;

    p->inventoryCount = 0;

    for (i = 0; i < MAX_INVENTORY; i++)
    {
        strcpy(p->inventory[i], "");
    }

    p->hasTorch = 0;
    p->hasMap = 0;
    p->hasKey = 0;
    p->hasArtifact = 0;

    p->puzzle1Solved = 0;
    p->puzzle2Solved = 0;
    p->puzzle3Solved = 0;

    p->mummyAwake = 0;
    p->mummyDefeated = 0;

    p->snakeEncountered = 0;
    p->trapTriggered = 0;

    p->artifactTaken = 0;
    p->gameWon = 0;
}


/* =========================================================
   INTRODUCTION
   ========================================================= */

void introduction()
{
    printf("\n\n====================================================\n");
    printf("                    THE STORY\n");
    printf("====================================================\n");

    printSlowly("\nYou are an adventurous skydiver exploring a remote area.\n");

    printSlowly("\nDuring a jump, something goes terribly wrong.\n");
    printSlowly("You lose control and land deep inside a mysterious forest.\n");

    printSlowly("\nAfter walking for some time, you discover an old temple.\n");

    printSlowly("\nThe temple is rusty, overgrown and strangely silent.\n");

    printSlowly("\nWith nowhere else to go, you enter the temple.\n\n");
    printf(GREEN);
    printf("                         /\\\n");
    printf("                        /  \\\n");
    printf("                       /____\\\n");
    printf("                      /|    |\\\n");
    printf("                     /_|____|_\\\n");
    printf("                    |  | [] |  |\n");
    printf("              ______|  | || |  |______\n");
    printf("             |       | | || | |       |\n");
    printf("             |  []   | | || | |   []  |\n");
    printf("             |_______|_|_||_|_|_______|\n");
    printf("             |________________________|\n");
    printf(RESET);
    pauseScreen();
}


/* =========================================================
   GAME LOOP
   ========================================================= */

void gameLoop(Player *p)
{
    int choice;

    while (!p->gameWon)
    {
        showRoom(p);

        printf("\n1. Move\n");
        printf("2. Check Status\n");
        printf("3. Check Inventory\n");
        printf("4. Examine Room\n");
        printf("5. Save Game\n");
        printf("6. Quit to Main Menu\n");

        printf("\nEnter choice: ");

        choice = getChoice(1, 6);

        if (choice == 1)
        {
            movePlayer(p);
        }

        else if (choice == 2)
        {
            showStatus(p);
        }

        else if (choice == 3)
        {
            showInventory(p);
        }

        else if (choice == 4)
        {
            checkRoom(p);
        }

        else if (choice == 5)
        {
            saveGame(p);
        }

        else
        {
            printf("\nReturning to main menu...\n");
            return;
        }
    }
}


/* =========================================================
   SHOW ROOM
   ========================================================= */

void showRoom(Player *p)
{
    Room *r = &rooms[p->currentRoom];

    printf("\n\n====================================================\n");
    printf("                    %s\n", r->name);
    printf("====================================================\n");

    printf("\n%s\n", r->description);

    /* Two ancient doors in the Temple Entrance */
    if (r->id == 1)
    {
        printf(YELLOW);
        printf("\n       .---------.             .---------.\n");
        printf("       |  _____  |             |  _____  |\n");
        printf("       | |     | |             | |     | |\n");
        printf("       | |  o  | |             | |  o  | |\n");
        printf("       | |     | |             | |     | |\n");
        printf("       | |_____| |             | |_____| |\n");
        printf("       |_________|             |_________|\n");
        printf("       |___| |___|             |___| |___|\n");
        printf(RESET);
    }

    printf("\nHealth: %d\n", p->health);

    if (p->mummyAwake && !p->mummyDefeated)
    {
        printSlowly("\nWARNING: The evil mummy is hunting you!\n");
    }
}


/* =========================================================
   MOVE PLAYER
   ========================================================= */

void movePlayer(Player *p)
{
    Room *r = &rooms[p->currentRoom];

    int choice;
    int destination = -1;
    int dest[4], n = 0;

    printf("\n\n---------------- MOVEMENT ----------------\n");

    if (r->left != -1)    { dest[n++] = r->left;    printf("%d. Go Left\n", n); }
    if (r->right != -1)   { dest[n++] = r->right;   printf("%d. Go Right\n", n); }
    if (r->forward != -1) { dest[n++] = r->forward; printf("%d. Go Forward\n", n); }
    if (r->back != -1)    { dest[n++] = r->back;    printf("%d. Go Back\n", n); }

    printf("%d. Stay Here\n", n + 1);

    printf("\nEnter choice: ");

    choice = getChoice(1, n + 1);

    if (choice == n + 1) return;
    destination = dest[choice - 1];


    if (destination == -1)
    {
        printSlowly("\nYou cannot go in that direction.\n");
        return;
    }


    /* Locked door */
    if (destination == 8)
    {
        if (!p->hasKey)
        {
            printSlowly("\nThe stone door is locked.\n");
            printSlowly("You need the ANCIENT KEY.\n");

            puzzle2(p);

            if (!p->hasKey)
                return;
        }
    }


    p->currentRoom = destination;

    printf("\nYou move to: %s\n", rooms[destination].name);

    checkRoom(p);
}


/* =========================================================
   CHECK ROOM
   ========================================================= */

void checkRoom(Player *p)
{
    Room *r = &rooms[p->currentRoom];

    /* Forest */
    if (r->id == 0)
    {
        printSlowly("\nYou can see the temple entrance ahead.\n");
    }


    /* Snake room */
    if (r->id == 2)
    {
        if (!p->snakeEncountered)
        {
            snakeEncounter(p);

            if (p->health <= 0)
                return;
        }

        takeItem(p);
    }


    /* Dark room */
    if (r->id == 3)
    {
        if (!p->hasTorch)
        {
            printSlowly("\nIt is extremely dark here.\n");

            if (!p->trapTriggered)
            {
                printSlowly("\nYou step forward carefully...\n");
                triggerTrap(p);

                if (p->health <= 0)
                    return;
            }
        }
        else
        {
            printSlowly("\nYour torch lights the room.\n");
            printSlowly("You can safely see the surroundings.\n");
        }

        takeItem(p);
    }


    /* Artifact room */
    if (r->id == 4)
    {
        if (!p->artifactTaken)
        {
            artifactEvent(p);

            if (p->health <= 0)
                return;
        }
    }


    /* Mummy hall */
    if (r->id == 5)
    {
        if (p->mummyAwake && !p->mummyDefeated)
        {
            mummyEncounter(p);

            if (p->health <= 0)
                return;
        }

        if (!p->puzzle1Solved)
            puzzle1(p);
    }


    /* Library */
    if (r->id == 6)
    {
        if (p->mummyAwake && !p->mummyDefeated)
        {
            printSlowly("\nThe mummy is getting closer...\n");
        }

        if (!p->puzzle2Solved)
            puzzle2(p);
    }


    /* Trap hall */
    if (r->id == 7)
    {
        if (!p->trapTriggered)
        {
            triggerTrap(p);

            if (p->health <= 0)
                return;
        }
    }


    /* Locked door */
    if (r->id == 8)
    {
        if (!p->hasKey)
        {
            puzzle2(p);
        }

        if (p->hasKey)
        {
            printSlowly("\nYou use the Ancient Key.\n");
            printSlowly("The massive stone door opens.\n");

            if (!p->puzzle3Solved)
                puzzle3(p);
        }
    }


    /* White room */
    if (r->id == 9)
    {
        whiteRoom(p);
    }
}


/* =========================================================
   ADD ITEM
   ========================================================= */

void addItem(Player *p, const char item[])
{
    if (p->inventoryCount >= MAX_INVENTORY)
    {
        printf("\nYour inventory is full!\n");
        return;
    }

    strcpy(p->inventory[p->inventoryCount], item);
    p->inventoryCount++;

    printf(GREEN "\n%s added to inventory!\n" RESET, item);
}


/* =========================================================
   HAS ITEM
   ========================================================= */

int hasItem(Player *p, const char item[])
{
    int i;

    for (i = 0; i < p->inventoryCount; i++)
    {
        if (strcmp(p->inventory[i], item) == 0)
            return 1;
    }

    return 0;
}


/* =========================================================
   TAKE ITEM
   ========================================================= */

void takeItem(Player *p)
{
    Room *r = &rooms[p->currentRoom];

    if (r->item == 0)
        return;

    if (r->item == 1 && !p->hasTorch)
    {
        printSlowly("\nYou found an ancient TORCH.\n");

        addItem(p, "Torch");
        p->hasTorch = 1;

        r->item = 0;
    }

    else if (r->item == 2 && !p->hasMap)
    {
        printSlowly("\nYou found an ancient MAP.\n");

        addItem(p, "Map");
        p->hasMap = 1;

        r->item = 0;
    }

    else if (r->item == 3 && !p->hasKey)
    {
        printSlowly("\nYou found the ANCIENT KEY.\n");

        addItem(p, "Ancient Key");
        p->hasKey = 1;

        r->item = 0;
    }
}


/* =========================================================
   SNAKE ENCOUNTER
   ========================================================= */

void snakeEncounter(Player *p)
{
    int choice;

    printf("\n\n====================================================\n");
    printf("                 POISONOUS SNAKE!\n");
    printf("====================================================\n");

    printSlowly("\nA huge poisonous snake blocks the corridor!\n\n");
printf(GREEN "       /^\\/^\\\n");
printf(GREEN "     _|__|  O|\n");
printf(GREEN " \\/     /~     \\_/ \\\n");
printf(GREEN "  \\____|__________/  \\\n");
printf(GREEN "         \\_______      \\\n");
printf(GREEN "                 `\\     \\        __           \\\n");
printf(GREEN "                   |     |      /  \\      /     \\\n");
printf(GREEN "                  /     /      /    \\     \\       \\\n");
printf(GREEN "                 /     /      /     /     /      \\\\\n");
printf(GREEN "                /     /      /     /     /        \\\\\n");
printf(GREEN "               /_____/      /      \\____/          \\\\\n" RESET);
    printf("\n1. Run past the snake\n");
    printf("2. Wait silently\n");

    printf("\nEnter choice: ");

    choice = getChoice(1, 2);

    p->snakeEncountered = 1;

    if (choice == 1)
    {
        printSlowly("\nYou run past the snake!\n");
        printSlowly("The snake bites your leg before you escape.\n");

        takeDamage(p, 15);
    }

    else
    {
        printSlowly("\nYou remain completely still.\n");
        printSlowly("The snake moves away slowly.\n");

        printSlowly("\nYou successfully avoid the attack.\n");
    }

    if (p->health <= 0)
        gameOver(p);
}


/* =========================================================
   TRAP
   ========================================================= */

void takeDamage(Player *p, int amount)
{
    p->health -= amount;
    if (p->health < 0)
        p->health = 0;

    printf(RED "\nYou lost %d health. Remaining health: %d\n" RESET,
           amount, p->health);

    /* Show YOU LOSE only when health reaches zero. */
    if (p->health == 0)
    {
        printf(RED "\n==============================\n");
        printf(RED "          YOU LOSE!\n");
        printf(RED "           GAME OVER\n");
        printf(RED "==============================\n" RESET);
    }
}


void triggerTrap(Player *p)
{
    printSlowly("\n\nWARNING!\n");
    printSlowly("You stepped on a hidden ancient trap!\n");

    printSlowly("\nA stone mechanism activates...\n");

    takeDamage(p, 20);

    p->trapTriggered = 1;

    printf("\nYou lost 20 health.\n");
    printf("Current Health: %d\n", p->health);

    if (p->health <= 0)
        gameOver(p);
}


/* =========================================================
   ARTIFACT EVENT
   ========================================================= */

void artifactEvent(Player *p)
{
    int choice;

    printf("\n\n====================================================\n");
    printf("              THE CURSED ARTIFACT\n");
    printf("====================================================\n");

    printSlowly("\nYou enter a room filled with ancient artifacts.\n");

    printSlowly("\nGold statues, crowns and mysterious objects cover the walls.\n");

    printSlowly("\nBut one object catches your attention.\n");

    printSlowly("\nA strange golden artifact is glowing on a pedestal.\n\n");
    printf(GOLD);
printf("\033[1;33m                    .-=========-.\n");
printf("\033[1;33m                   /  .-------.  \\\n");
printf("\033[1;33m                  /  /   \033[1;31m* \033[1;34m* \033[1;33m \\   \\\n");
printf("\033[1;33m                 |  |   \033[1;32m* \033[1;31mO \033[1;32m*  \033[1;33m |  |\n");
printf("\033[1;33m                 |  |   \033[1;34m* \033[1;33m| \033[1;34m*  \033[1;33m |  |\n");
printf("\033[1;33m                  \\  \\_________/  /\n");
printf("\033[1;33m                   '._  _____  _.'\n");
printf("\033[1;33m                      |/     \\|\n");
printf("\033[1;33m                      |       |\n");
printf("\033[1;33m                      |   \033[1;31m* \033[1;33m  |\n");
printf("\033[1;33m                      |       |\n");
printf("\033[1;33m                      |  \033[1;34m* \033[1;32m* \033[1;33m |\n");
printf("\033[1;33m                      |       |\n");
printf("\033[1;33m                      |   \033[1;31m* \033[1;33m  |\n");
printf("\033[1;33m                      |       |\n");
printf("\033[1;33m                      |_______|\n");
printf("\033[1;33m                         ||\n");
printf("\033[1;33m                       __||__\n");
printf("\033[1;33m                      /______\\\033[0m\n");
    printf(RESET);
    printf("\n1. Take the artifact\n");
    printf("2. Leave it alone\n");

    printf("\nEnter choice: ");

    choice = getChoice(1, 2);

    /*
       Even if player chooses to leave it,
       the curse causes the artifact to activate.
    */

    if (choice == 2)
    {
        printSlowly("\nYou decide not to touch the artifact.\n");

        printSlowly("\nSuddenly, the room begins shaking!\n");
        printSlowly("The artifact falls from the pedestal.\n");

        printSlowly("\nYou catch it automatically.\n");
    }

    else
    {
        printSlowly("\nYou carefully pick up the artifact.\n");
    }

    p->hasArtifact = 1;
    p->artifactTaken = 1;

    addItem(p, "Cursed Artifact");

    printSlowly("\nThe entire temple begins shaking!\n");

    printSlowly("\nBOOOOOOOM!\n");

    p->mummyAwake = 1;

    printSlowly("\nAn ancient stone coffin opens...\n");

    printSlowly("\nTHE EVIL MUMMY HAS AWAKENED!\n");

    printSlowly("\nMummy:\n");
    printSlowly("\"You have awakened the ancient curse!\"\n");

    pauseScreen();
}


/* =========================================================
   MUMMY ENCOUNTER
   ========================================================= */

void mummyEncounter(Player *p)
{
    printf("\n\n====================================================\n");
    printf("                 THE MUMMY RETURNS\n");
    printf("====================================================\n");

    printSlowly("\nThe evil mummy appears in front of you!\n\n");
printf("\033[1;37m");
printf("                 .-=========-.\n");
printf("                /  .-------.  \\\n");
printf("               /  /         \\  \\\n");
printf("              |  |  .-----.  |  |\n");
printf("              |  | /  \033[1;31mO  O\033[1;37m \\ |  |\n");
printf("              |  | \\   ^   / |  |\n");
printf("              |  |  '-----'  |  |\n");
printf("          ____|  '-----------'  |____\n");
printf("         /    |  =============  |    \\\n");
printf("        /  ===|  =============  |===  \\\n");
printf("       |  ====|  =============  |====  |\n");
printf("       |  ====|  =============  |====  |\n");
printf("       |  ====|  =============  |====  |\n");
printf("        \\  ===|  =============  |===  /\n");
printf("         \\____|  =============  |____/\n");
printf("              |  ===  |  ===  |\n");
printf("              |  ===  |  ===  |\n");
printf("             /|  ===  |  ===  |\\\n");
printf("            / |_______|_______| \\\n");
printf("           /____/           \\____\\\n");
printf("\033[0m");
    printf(RESET);
    printSlowly("\nMummy:\n");
    printSlowly("\"You cannot escape the temple.\"\n");

    printSlowly("\nThe mummy blocks the way.\n");

    if (p->hasTorch)
    {
        printSlowly("\nYou raise your torch.\n");
        printSlowly("The mummy steps back from the light.\n");
    }
    else
    {
        printSlowly("\nYou have no torch to defend yourself.\n");

        takeDamage(p, 10);

        printSlowly("\nThe mummy drains 10 health from you.\n");
        printf("Health: %d\n", p->health);
    }

    if (p->health <= 0)
        gameOver(p);
}


/* =========================================================
   PUZZLE 1
   ========================================================= */

void puzzle1(Player *p)
{
    int answer;

    if (p->puzzle1Solved)
        return;

    printf("\n\n====================================================\n");
    printf("                    RIDDLE 1\n");
    printf("====================================================\n");

    printSlowly("\nMummy:\n");
    printSlowly("\"Answer this riddle and I will let you pass.\"\n");

    printSlowly("\nI have hands but cannot clap.\n");
    printSlowly("I have a face but cannot smile.\n");
    printSlowly("What am I?\n");

    printf("\n1. Clock\n");
    printf("2. Tree\n");
    printf("3. River\n");
    printf("4. Book\n");

    printf("\nEnter answer: ");

    answer = getChoice(1, 4);

    if (answer == 1)
    {
        printf("\nCorrect!\n");

        p->puzzle1Solved = 1;

        printSlowly("\nThe mummy moves away.\n");
    }
    else
    {
        printf("\nWrong answer!\n");

        takeDamage(p, 15);

        printSlowly("\nThe mummy curses you.\n");
        printf("Health: %d\n", p->health);

        if (p->health <= 0)
            gameOver(p);
    }
}


/* =========================================================
   PUZZLE 2
   ========================================================= */

void puzzle2(Player *p)
{
    int answer;

    if (p->puzzle2Solved)
        return;

    printf("\n\n====================================================\n");
    printf("                    RIDDLE 2\n");
    printf("====================================================\n");

    printSlowly("\nAn ancient message appears:\n");

    printSlowly("\n\"What has many keys but cannot open a single lock?\"\n");

    printf("\n1. A Door\n");
    printf("2. A Piano\n");
    printf("3. A Map\n");
    printf("4. A Treasure Chest\n");

    printf("\nEnter answer: ");

    answer = getChoice(1, 4);

    if (answer == 2)
    {
        printf("\nCorrect!\n");

        printSlowly("\nA secret compartment opens.\n");

        if (!p->hasKey)
        {
            printSlowly("\nYou found an ANCIENT KEY!\n");

            addItem(p, "Ancient Key");
            p->hasKey = 1;
        }

        p->puzzle2Solved = 1;
    }

    else
    {
        printf("\nWrong answer!\n");

        takeDamage(p, 10);

        printSlowly("\nA magical force hits you.\n");
        printf("Health: %d\n", p->health);

        if (p->health <= 0)
            gameOver(p);
    }
}


/* =========================================================
   PUZZLE 3
   ========================================================= */

void puzzle3(Player *p)
{
    int answer;

    if (p->puzzle3Solved)
        return;

    printf("\n\n====================================================\n");
    printf("                    FINAL RIDDLE\n");
    printf("====================================================\n");

    printSlowly("\nThe stone door speaks:\n");

    printSlowly("\n\"I speak without a mouth and hear without ears.\"\n");
    printSlowly("\"I have no body, but I come alive with wind.\"\n");

    printSlowly("\nWhat am I?\n");

    printf("\n1. Shadow\n");
    printf("2. Echo\n");
    printf("3. Fire\n");
    printf("4. Stone\n");

    printf("\nEnter answer: ");

    answer = getChoice(1, 4);

    if (answer == 2)
    {
        printf("\nCorrect!\n");

        p->puzzle3Solved = 1;

        printSlowly("\nThe stone door opens.\n");

        printSlowly("\nThe mummy suddenly disappears!\n");

        p->mummyDefeated = 1;

        printSlowly("\nMummy:\n");
        printSlowly("\"You have passed my final test...\"\n");
    }

    else
    {
        printf("\nWrong answer!\n");

        takeDamage(p, 20);

        printSlowly("\nThe room shakes violently!\n");
        printf("Health: %d\n", p->health);

        if (p->health <= 0)
            gameOver(p);
    }
}


/* =========================================================
   WHITE ROOM
   ========================================================= */

void whiteRoom(Player *p)
{
    printf("\n\n====================================================\n");
    printf("                    WHITE ROOM\n");
    printf("====================================================\n");

    printSlowly("\nYou walk through the ancient door.\n");

    printSlowly("\nSuddenly everything becomes completely white.\n");

    printSlowly("\nThere are no visible walls.\n");
    printSlowly("No windows.\n");
    printSlowly("No doors.\n\n");
    printf(WHITE);
    printf("        +-----------------------------------+\n");
    printf("        |                                   |\n");
    printf("        |             .------.              |\n");
    printf("        |             |  o o |              |\n");
    printf("        |             |   ^  |              |\n");
    printf("        |             '------'              |\n");
    printf("        |                                   |\n");
    printf("        +-----------------------------------+\n");
    printf(RESET);
    printSlowly("\nA mysterious figure appears before you.\n");

    alienScene(p);
}


/* =========================================================
   ALIEN SCENE
   ========================================================= */

void alienScene(Player *p)
{
    printf("\n\n====================================================\n");
    printf("                 THE MYSTERIOUS ALIEN\n");
    printf("====================================================\n");

    printSlowly("\nAlien:\n");

    printSlowly("\"Do not be afraid. I have been waiting for you.\"\n");

    printSlowly("\nYou look at the cursed artifact.\n");

    printSlowly("\nAlien:\n");

    printSlowly("\"That artifact is the source of the curse.\"\n");

    printSlowly("\n\"The mummy was bound to protect it.\"\n");

    printSlowly("\n\"But the artifact contains a trapped soul.\"\n");

    printSlowly("\nThe artifact begins glowing.\n");

    printSlowly("\nAlien:\n");

    printSlowly("\"Open the secret chamber and free the soul.\"\n");

    secretChamber(p);
}


/* =========================================================
   SECRET CHAMBER
   ========================================================= */

void secretChamber(Player *p)
{
    printf("\n\n====================================================\n");
    printf("                 SECRET CHAMBER\n");
    printf("====================================================\n");

    printSlowly("\nThe artifact opens like a small ancient door.\n");

    printSlowly("\nA powerful light comes from inside.\n");

    printSlowly("\nA peaceful soul slowly emerges.\n");

    printSlowly("\nThe soul looks at you.\n");

    printSlowly("\nSoul:\n");
    printSlowly("\"Thank you for freeing me.\"\n");

    printSlowly("\n\"I have been trapped inside this curse for centuries.\"\n");

    goodSoul(p);
}


/* =========================================================
   GOOD SOUL
   ========================================================= */

void goodSoul(Player *p)
{
    int choice;

    printf("\n\n====================================================\n");
    printf("                    THE GOOD SOUL\n");
    printf("====================================================\n");

    printSlowly("\nSoul:\n");
    printSlowly("\"You have freed me, so I will grant you one wish.\"\n");

    printf("\n1. I want to return safely home.\n");
    printf("2. I want unlimited treasure.\n");
    printf("3. I want to stay here.\n");

    printf("\nEnter choice: ");

    choice = getChoice(1, 3);

    if (choice == 1)
    {
        printSlowly("\nThe soul smiles.\n");
        Sleep(500);
        printSlowly("\n\"Your wish is granted.\"\n");
    }

    else if (choice == 2)
    {
        printSlowly("\nThe soul looks at the artifact.\n");

        printSlowly("\n\"Treasure created this curse.\"\n");
        printSlowly("\"You have learned that freedom is more valuable.\"\n");

        printSlowly("\nThe soul grants you another chance.\n");
    }

    else
    {
        printSlowly("\nYou decide to remain in the temple.\n");

        printSlowly("\nThe temple begins collapsing!\n");

        printSlowly("\nYou quickly realize that staying is not a good idea.\n");
    }

    finalEscape(p);
}


/* =========================================================
   FINAL ESCAPE
   ========================================================= */

void finalEscape(Player *p)
{
    printf("\n\n====================================================\n");
    printf("                    FINAL ESCAPE\n");
    printf("====================================================\n");

    printSlowly("\nA bright light surrounds you.\n");

    printSlowly("\nThe ancient temple begins disappearing.\n");

    printSlowly("\nYou close your eyes...\n");

    printSlowly("\nWhen you open them again,\n");
    printSlowly("you are safely back in the real world.\n");

    printSlowly("\nThe mysterious forest is gone.\n");

    printSlowly("\nThe cursed artifact is gone.\n");

    printSlowly("\nThe good soul has finally been freed.\n");

    p->gameWon = 1;

    gameWon(p);
}


/* =========================================================
   STATUS
   ========================================================= */

void showStatus(Player *p)
{
    printf("\n\n====================================================\n");
    printf("                     STATUS\n");
    printf("====================================================\n");

    printf("\nHealth: %d / 100\n", p->health);

    printf("Current Room: %s\n",
           rooms[p->currentRoom].name);

    printf("\nPuzzles solved: ");

    if (p->puzzle1Solved)
        printf("1 ");

    if (p->puzzle2Solved)
        printf("2 ");

    if (p->puzzle3Solved)
        printf("3 ");

    if (!p->puzzle1Solved &&
        !p->puzzle2Solved &&
        !p->puzzle3Solved)
    {
        printf("None");
    }

    printf("\n");
}


/* =========================================================
   INVENTORY
   ========================================================= */

void showInventory(Player *p)
{
    int i;

    printf("\n\n====================================================\n");
    printf("                   INVENTORY\n");
    printf("====================================================\n");

    if (p->inventoryCount == 0)
    {
        printf("\nInventory is empty.\n");
        return;
    }

    for (i = 0; i < p->inventoryCount; i++)
    {
        printf("%d. %s\n",
               i + 1,
               p->inventory[i]);
    }
}


/* =========================================================
   SAVE GAME
   ========================================================= */

void saveGame(Player *p)
{
    FILE *file;
    int i;

    file = fopen(SAVE_FILE, "w");

    if (file == NULL)
    {
        printf("\nError: Could not save the game.\n");
        return;
    }

    fprintf(file, "%s\n", p->name);

    fprintf(file, "%d\n", p->health);
    fprintf(file, "%d\n", p->currentRoom);

    fprintf(file, "%d\n", p->inventoryCount);

    for (i = 0; i < p->inventoryCount; i++)
    {
        fprintf(file, "%s\n", p->inventory[i]);
    }

    fprintf(file, "%d\n", p->hasTorch);
    fprintf(file, "%d\n", p->hasMap);
    fprintf(file, "%d\n", p->hasKey);
    fprintf(file, "%d\n", p->hasArtifact);

    fprintf(file, "%d\n", p->puzzle1Solved);
    fprintf(file, "%d\n", p->puzzle2Solved);
    fprintf(file, "%d\n", p->puzzle3Solved);

    fprintf(file, "%d\n", p->mummyAwake);
    fprintf(file, "%d\n", p->mummyDefeated);

    fprintf(file, "%d\n", p->snakeEncountered);
    fprintf(file, "%d\n", p->trapTriggered);

    fprintf(file, "%d\n", p->artifactTaken);
    fprintf(file, "%d\n", p->gameWon);

    for (i = 0; i < TOTAL_ROOMS; i++)
    {
        fprintf(file, "%d\n", rooms[i].item);
    }

    fclose(file);

    printf("\nGame saved successfully!\n");
}
int loadGame(Player *p)
{
    FILE *file;
    int i;

    file = fopen(SAVE_FILE, "r");

    if (file == NULL)
    {
        return 0;
    }

    fscanf(file, "%49s", p->name);

    fscanf(file, "%d", &p->health);
    fscanf(file, "%d", &p->currentRoom);

    fscanf(file, "%d", &p->inventoryCount);

    for (i = 0; i < p->inventoryCount; i++)
    {
        fscanf(file, "%29s", p->inventory[i]);
    }

    fscanf(file, "%d", &p->hasTorch);
    fscanf(file, "%d", &p->hasMap);
    fscanf(file, "%d", &p->hasKey);
    fscanf(file, "%d", &p->hasArtifact);

    fscanf(file, "%d", &p->puzzle1Solved);
    fscanf(file, "%d", &p->puzzle2Solved);
    fscanf(file, "%d", &p->puzzle3Solved);

    fscanf(file, "%d", &p->mummyAwake);
    fscanf(file, "%d", &p->mummyDefeated);

    fscanf(file, "%d", &p->snakeEncountered);
    fscanf(file, "%d", &p->trapTriggered);

    fscanf(file, "%d", &p->artifactTaken);
    fscanf(file, "%d", &p->gameWon);

    for (i = 0; i < TOTAL_ROOMS; i++)
    {
        fscanf(file, "%d", &rooms[i].item);
    }

    fclose(file);

    return 1;
}
void gameOver(Player *p)
{
    printf(RED "\n\n====================================================\n");
    printf("                    YOU LOSE!\n");
    printf("                    GAME OVER\n");
    printf("====================================================\n" RESET);

    printf("\nYour health has reached zero.\n");

    printf("\nThe ancient temple has defeated you.\n");

    printf("\nYou can start a new game and try again.\n");

    p->gameWon = 0;

    exit(0);
}
void gameWon(Player *p)
{
    printf("\n\n====================================================\n");
    printf("                 CONGRATULATIONS!\n");
    printf("====================================================\n");

    printSlowly("\nYou survived the mysterious temple.\n");

    printSlowly("\nYou survived the poisonous snake.\n");
    printSlowly("You found the ancient items.\n");
    printSlowly("You solved the mummy's riddles.\n");
    printSlowly("You discovered the truth behind the artifact.\n");
    printSlowly("You freed the trapped soul.\n");

    printSlowly("\nAnd finally...\n");

    printSlowly("\nYOU RETURNED HOME SAFELY!\n");

    printf(GREEN "\n");
    printf("             ___________\n");
    printf("            '._==_==_=_.'\n");
    printf("            .-\\\\:      /-.\n");
    printf("           | (|:.     |) |\n");
    printf("            '-|:.     |-'\n");
    printf("              \\\\::.    /\n");
    printf("               '::. .'\n");
    printf("                 ) (\n");
    printf("               .'   '.\n");
    printf("              /_______\\\n");
    printf(RESET);
    printf(GREEN "\n====================================================\n");
    printf("                     YOU WIN!\n");
    printf("====================================================\n" RESET);
    exit(0);
}
int getChoice(int min, int max)
{
    int choice;
    int result;

    while (1)
    {
        result = scanf("%d", &choice);

        if (result == 1 && choice >= min && choice <= max)
        {
            return choice;
        }

        printf("\nInvalid input! Enter a number between %d and %d: ",
               min, max);

        while (getchar() != '\n');
    }
}
void pauseScreen()
{
    printf("\nPress ENTER to continue...");

    getchar();
    getchar();
}


/* =========================================================
   GATE RIDDLE (first challenge, before entering the temple)
   ========================================================= */

void gateRiddle(Player *p)
{
    char answer[100];
    int attempts = 0;
    int maxAttempts = 3;
    int i, j;

    printf("\n\n====================================================\n");
    printf("                  THE SEALED GATE\n");
    printf("====================================================\n");

    printSlowly("\nThe temple gate is shut tight. Carved into the stone is a message:\n");
    printSlowly("\n\"Only a clever mind may enter. Answer me, traveler.\"\n");

    printf(GOLD);
    printSlowly("\nI follow you all day long,\n");
    printSlowly("but when the night or darkness comes, I am gone.\n");
    printSlowly("What am I?\n");
    printf(RESET);

    while (attempts < maxAttempts)
    {
        printf("\nType your answer (%d attempt(s) left): ", maxAttempts - attempts);

        if (fgets(answer, sizeof(answer), stdin) == NULL)
            break;

        /* make the answer lowercase and remove the newline */
        for (i = 0, j = 0; answer[i] != '\0'; i++)
        {
            if (answer[i] != '\n')
                answer[j++] = (char)tolower((unsigned char)answer[i]);
        }
        answer[j] = '\0';

        if (strstr(answer, "shadow") != NULL)
        {
            printf(GREEN "\nCorrect!\n" RESET);
            printSlowly("\nThe stone gate rumbles and slowly slides open.\n");
            printSlowly("A cold wind rushes out of the temple...\n");
            pauseScreen();
            return;
        }

        attempts++;
        printf(RED "\nWrong answer! The gate stays shut." RESET "\n");

        if (attempts < maxAttempts)
        {
            takeDamage(p, 5);

            if (attempts == 2)
            {
                printSlowly("\nA voice whispers: \"Look at the ground when the sun is out.\"\n");
            }
        }
    }

    /* Out of attempts: the gate still opens, but it costs health. */
    printSlowly("\nThe gate cracks and breaks open. The answer was: SHADOW.\n");
    takeDamage(p, 10);
    pauseScreen();
}
