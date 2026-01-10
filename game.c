#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "game.h"
#include "utils.h"




static void displayRoomLegend(GameState *g);
static void displayMap(GameState* g);
static Room* moveRoom(GameState *g, Room *currentRoom);
static int attackMonster(Player *player, Monster *monster);


static void displayBag(Player *player);
static void displayDefeated(Player *player);


static void freePlayer(Player *player);
static void freeRooms(Room *root);

// Map display functions
static void displayMap(GameState* g) {
    if (!g->rooms) return;
    
    // Find bounds
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (Room* r = g->rooms; r; r = r->next) {
        if (r->x < minX) minX = r->x;
        if (r->x > maxX) maxX = r->x;
        if (r->y < minY) minY = r->y;
        if (r->y > maxY) maxY = r->y;
    }
    
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    
    // Create grid
    int** grid = malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        grid[i] = malloc(width * sizeof(int));
        for (int j = 0; j < width; j++) grid[i][j] = -1;
    }
    
    for (Room* r = g->rooms; r; r = r->next)
        grid[r->y - minY][r->x - minX] = r->id;
    
    printf("=== SPATIAL MAP ===\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] != -1) printf("[%2d]", grid[i][j]);
            else printf("    ");
        }
        printf("\n");
    }
    
    for (int i = 0; i < height; i++) free(grid[i]);
    free(grid);
}









// Monster functions
void freeMonster(void* data)
{
    if (!data) return;

    Monster *monster = (Monster *)data;
    free(monster->name);
    free(monster);
}

cmpValue compareMonsters(void* a, void* b)
{
    Monster *monsterA = (Monster *)a;
    Monster *monsterB = (Monster *)b;

    // Compare names
    int nameCmp = strcmp(monsterA->name, monsterB->name);

    if (nameCmp < 0) // a < b
        return SMALLER;
    else if (nameCmp > 0) // a > b
        return BIGGER;    

    // Compare attack
    if (monsterA->attack < monsterB->attack)
        return SMALLER;
    else if (monsterA->attack > monsterB->attack)
        return BIGGER;

    // Compare hp
    if (monsterA->maxHp < monsterB->maxHp)
        return SMALLER;
    else if (monsterA->maxHp > monsterB->maxHp)
        return BIGGER;

    //Compare Type
    if (monsterA->type < monsterB->type)
        return SMALLER;
    else if (monsterA->type > monsterB->type)
        return BIGGER;

    // They are equal
    return EQUAL;
}

void printMonster(void* data)
{
    Monster *monster = (Monster *)data;
    char *typeName;
    switch (monster->type)
    {
        case PHANTOM:
            typeName = "Phantom";
            break;
        case SPIDER:
            typeName = "Spider";
            break;
        case DEMON:
            typeName = "Demon";
            break;
        case GOLEM:
            typeName = "Golem";
            break;
        case COBRA:
            typeName = "Cobra";
            break;
        default:
            exit(EXIT_FAILURE);
    }

    printf("[%s] Type: %s, Attack: %d, HP: %d\n", monster->name, typeName, monster->attack, monster->maxHp);
}



void freeItem(void* data)
{
    if (!data) return;

    Item *item = (Item*)data;
    free(item->name);
    free(item);
}

cmpValue compareItems(void* a, void* b)
{
    Item *itemA = (Item*)a;
    Item *itemB = (Item*)b;


    // Compare names
    int nameCmp = strcmp(itemA->name, itemB->name);

    if (nameCmp < 0) // a < b
        return SMALLER;
    else if (nameCmp > 0) // a > b
        return BIGGER;
    
    // Compare values
    if (itemA->value < itemB->value)
        return SMALLER;
    else if (itemA->value > itemB->value)
        return BIGGER;


    //Compare Type
    if (itemA->type < itemB->type)
        return SMALLER;
    else if (itemA->type > itemB->type)
        return BIGGER;

    // They are equal
    return EQUAL;
}

void printItem(void* data)
{
    Item *item = (Item*)data;
    char *typeName = item->type == ARMOR ? "ARMOR" : "SWORD";
    printf("[%s] %s - Value: %d\n", typeName, item->name, item->value);
}


Room* findRoomById(GameState *g, int id)
{
    if (g == NULL) return NULL;
    
    Room *index = g->rooms;
    while (index)
    {
        if (index->id == id)
            return index;

        index = index->next;
    }

    return NULL; // No match found
}

Room* findRoomByPos(GameState *g, int x, int y)
{
    if (g == NULL) return NULL;
    
    Room *index = g->rooms;
    while (index)
    {
        if (index->x == x && index->y == y)
            return index;

        index = index->next;
    }

    return NULL; // No match found
}


void addRoom(GameState* g)
{
    int x = 0;
    int y = 0;

    if (g->roomCount > 0)
    {
        displayMap(g);
        displayRoomLegend(g);

        int attachToId = getInt("Attach to room ID: ");
        Room *attachTo = findRoomById(g, attachToId); 

        x = attachTo->x;
        y = attachTo->y;

        int attachDir = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");

        // Adjust new room's position based on the direction to attach
        switch (attachDir)
        {
            case 0: //Up
                y--;
                break;
            case 1: //Down
                y++;
                break;
            case 2: //Left
                x--;
                break;
            case 3: //Right
                x++;
                break;
            default: //Error
                exit(EXIT_FAILURE);
        }


        // Check if it is already taken
        if (findRoomByPos(g, x, y))
        {
            printf("Room exists there\n");
            return;
        }
    }
    
    // Initalize a new room
    Room *myRoom = (Room*)malloc(sizeof(Room));

    // The id of the new room should be the next free int starting from 0.
    myRoom->id = g->roomCount;

    myRoom->x = x;
    myRoom->y = y;
    myRoom->visited = 0;

    int isMonster = getInt("Add monster? (1=Yes, 0=No): ");

    if (isMonster == 1)
    {
        char *monsterName = getString("Monster name: ");
        int monsterType = getInt("Type (0-4): ");
        int monsterHp = getInt("HP: ");
        int monsterAttack = getInt("Attack: ");

        Monster *myMonster = (Monster*)malloc(sizeof(Monster));
        myMonster->name = monsterName;
        myMonster->type = monsterType;
        myMonster->hp = myMonster->maxHp = monsterHp;
        myMonster->attack = monsterAttack;

        myRoom->monster = myMonster;
    }
    else
        myRoom->monster = NULL;



    int isItem = getInt("Add item? (1=Yes, 0=No): ");

    if (isItem == 1)
    {
        char *itemName = getString("Item name: ");
        int itemType = getInt("Type (0=Armor, 1=Sword): ");
        int itemValue = getInt("Value: ");

        Item *myItem = (Item*)malloc(sizeof(Item));
        myItem->name = itemName;
        myItem->type = itemType;
        myItem->value = itemValue;

        myRoom->item = myItem;

    }
    else
        myRoom->item = NULL;
    
    // Add myRoom to the linked list
    myRoom->next = g->rooms;
    g->rooms = myRoom;

    g->roomCount++;

    printf("Created room %d at (%i,%i)", myRoom->id, x, y);
}

static void displayRoomLegend(GameState *g)
{
    printf("=== ROOM LEGEND ===\n");

    Room *index = g->rooms;
    while (index)
    {
        char hasMonster = index->monster ? 'V' : 'X';
        int hasItem = index->item ? 'V' : 'X';

        printf("ID %d: [M:%c] [I:%c]\n", index->id, hasMonster, hasItem);

        index = index->next;
    }

    printf("===================\n");
}




void initPlayer(GameState* g)
{
    if (g->roomCount < 1)
    {
        printf("Create rooms first\n");
        return;
    }

    Player *myPlayer = (Player*)malloc(sizeof(Player));

    myPlayer->hp = myPlayer->maxHp = g->configMaxHp;
    myPlayer->baseAttack = g->configBaseAttack;

    myPlayer->bag = createBST(compareItems, printItem, freeItem);
    myPlayer->defeatedMonsters = createBST(compareMonsters, printMonster, freeMonster);

    myPlayer->currentRoom = findRoomById(g, 0);
    assert(myPlayer->currentRoom); // Should pass if roomCount isnt wrong


    if (g->player) // free if we are replacing a player
        freePlayer(g->player);
    g->player = myPlayer;
}




void playGame(GameState* g)
{
    while (1)
    {

    if (!g->player)
    {
        printf("Init player first\n");
        return;
    }

    Player *player = g->player;
    Room *currentRoom = player->currentRoom;
    Monster *currentMonster = currentRoom->monster;
    Item *currentItem = currentRoom->item;
    

    currentRoom->visited = 1;

    displayMap(g);
    displayRoomLegend(g);

    // Print current room info
    printf("--- Room %d ---\n", currentRoom->id);
    if (currentRoom->monster)
        printf("Monster: %s (HP:%d)\n", currentMonster->name, currentMonster->hp);
    if (currentRoom->item)
        printf("Item: %s\n", currentItem->name);

    //Player hp
    printf("HP: %d/%d\n", player->hp, player->maxHp);


    int choice = getInt("1.Move 2.Fight 3.Pickup 4.Bag 5.Defeated 6.Quit\n");

    switch (choice)
    {
        case 1: // Move
            if (currentRoom->monster) // Player must defeat the monster in the room first
            {
                printf("Kill monster first\n");
                break;
            }

            Room *nextRoom = moveRoom(g, currentRoom);
            if (!nextRoom) // Player chose an invalid move
            {
                printf("No room there\n");
                break;
            }

            player->currentRoom = nextRoom;

            break;
        case 2: // Fight
            if (!currentMonster)
            {
                printf("No monster\n");
                break;
            }

            int isAlive = attackMonster(player, currentMonster);

            if (!isAlive)
            {
                printf("--- YOU DIED ---\n");
                freeGame(g);
                exit(EXIT_SUCCESS);
            }
            else
            {
                printf("Monster defeated!\n");
                bstInsert(&player->defeatedMonsters->root, currentMonster, player->defeatedMonsters->compare);

                currentRoom->monster = NULL;
            }

            break;
        case 3: // Pick up
            if (currentMonster) // Player must defeat the monster in the room first
            {
                printf("Kill monster first\n");
                break;
            }


            if (!currentItem)
            {
                printf("No item here\n");
                break;
            }

            if (bstFind(player->bag->root, currentItem, player->bag->compare))
            {
                printf("Duplicate item.\n");
                break;
            }

            
            printf("Picked up %s\n", currentItem->name);
            bstInsert(&player->bag->root, currentItem, player->bag->compare);

            currentRoom->item = NULL;
            break;
        case 4: // Bag
            displayBag(player);
            break;
        case 5: // Defeated
            displayDefeated(player);
            break;
        case 6: // Quit
            return;
    }

    // Win check
    Room *index = g->rooms;
    while (index)
    {
        if (!index->visited || index->monster) // Game isnt completed
            break;

        index = index->next;
    }

    if (index == NULL) // We checked all rooms
    {
        printf("***************************************\n");
        printf("             VICTORY!                  \n");
        printf(" All rooms explored. All monsters defeated. \n");
        printf("***************************************\n");

        freeGame(g);
        exit(EXIT_SUCCESS);
    }

    }
}



static void displayBag(Player *player)
{
    printf("=== INVENTORY ===\n");
    int choice = getInt("1.Preorder 2.Inorder 3.Postorder\n");

    switch (choice)
    {
        case 1:
            bstPreorder(player->bag->root, player->bag->print);
            break;
        case 2:
            bstInorder(player->bag->root, player->bag->print);
            break;
        case 3:
            bstPostorder(player->bag->root, player->bag->print);
            break;
    }
}

static void displayDefeated(Player *player)
{
    printf("=== DEFEATED MONSTERS ===\n");
    int choice = getInt("1.Preorder 2.Inorder 3.Postorder\n");

    switch (choice)
    {
        case 1:
            bstPreorder(player->defeatedMonsters->root, player->defeatedMonsters->print);
            break;
        case 2:
            bstInorder(player->defeatedMonsters->root, player->defeatedMonsters->print);
            break;
        case 3:
            bstPostorder(player->defeatedMonsters->root, player->defeatedMonsters->print);
            break;
    }
}




// Return 1 if player won, 0 if lost
static int attackMonster(Player *player, Monster *monster)
{
    while (1)
    {
        monster->hp -= player->baseAttack;
        if (monster->hp < 0)
            monster->hp = 0;
        printf("You deal %d damage. Monster HP: %d\n", player->baseAttack, monster->hp);

        if (monster->hp <= 0)
            return 1;

        player->hp -= monster->attack;
        if (player->hp < 0)
            player->hp = 0;

        printf("Monster deals %d damage. Your HP: %d\n", monster->attack, player->hp);

        if (player->hp <= 0)
            return 0;
    }
}


static Room* moveRoom(GameState *g, Room *currentRoom)
{
    int x = currentRoom->x;
    int y = currentRoom->y;


    int moveDir = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");
    switch (moveDir)
    {
        case 0: //Up
            y--;
            break;
        case 1: //Down
            y++;
            break;
        case 2: //Left
            x--;
            break;
        case 3: //Right
            x++;
            break;
        default: //Error
            exit(EXIT_FAILURE);
     }


    return findRoomByPos(g, x, y);
}




static void freePlayer(Player *player)
{
    bstFree(player->bag->root, player->bag->freeData);
    free(player->bag);

    bstFree(player->defeatedMonsters->root, player->defeatedMonsters->freeData);
    free(player->defeatedMonsters);


    free(player);
}

static void freeRooms(Room *root)
{
    if (root == NULL) return;

    freeRooms(root->next);

    // Post recursion free
    // If NULL free will skip them
    freeMonster(root->monster);
    freeItem(root->item);

    free(root);
}

void freeGame(GameState* g)
{
    freePlayer(g->player);
    freeRooms(g->rooms);
}
