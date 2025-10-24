#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <unistd.h>

struct Pokemon
{
    char name[100];
    char type;
    int level;
    int mana;
    int health;
    int attack;
    int defense;
};

struct turnSkill
{
    char pokemon[100];
    char nameSkill[100];
    int dmg;
    int mp;
};

struct Battle
{
    int turn;
    char pokemon[100];
    char skill[100];
    int mana;
};
//
struct message
{
    short turn;
    int dmg;
};
struct message playerMsg;
struct message opponentMsg;
//
struct Pokemon pokemon[100];
struct turnSkill skill[100];

char player_choice[50];
char opponent_choice[50];
short playerSkill = 0;
short opponentSkill = 0;

char match_code[20];
char pipe_name[50];

short role = 0;
short turn = 1;
short numberOfPokemon = 0;
short numberOfSkill = 0;
short endSkill = 0;
short countSkillOfPokemon = 0;
HANDLE hPipe;
DWORD bytesRead, bytesWritten;

void GetStructPokemon();
void GetStructSkill();
void printPokemonInPack();
void printPokemonStatus();
void printSkillOfPokemon();
void printContent();
void subtractMana(int mana, char *namePokemon);
int getChooseSkill();
void chooseSkill();
char getPokemonType(char *namePokemon);
int getNumberOfPokemon(char *namePokemon);
short checkDefenseEngraved();
void subtractMyHealth(int damage);
void subtractOpponentHealth(int damage);
int calculateDamage(short skill, char *namePokemon);
int choosePokemon();
int createOrJoinGame();

#define LIGHTBLUE "\x1b[94m"
#define BLUE "\x1b[34m"
#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define GREEN "\x1b[32m"
#define ORANGE "\x1b[38;5;208m"
#define BROWN "\x1b[38;5;94m"
#define GRAY "\x1b[90m"

void printFront()
{
    printf(GREEN BOLD "  _______   ______  __    __ ________ __       __  ______  __    __ \n");
    printf(GREEN BOLD " |       \\ /      \\|  \\  /  \\        \\  \\     /  \\/      \\|  \\  |  \\\n");
    printf(GREEN BOLD " | #######\\  ######\\ ## /  ## ######## ##\\   /  ##  ######\\ ##\\ | ##\n");
    printf(GREEN BOLD " | ##__/ ## ##  | ## ##/  ##| ##__   | ###\\ /  ### ##  | ## ###\\| ##\n");
    printf(GREEN BOLD " | ##    ## ##  | ## ##  ## | ##  \\  | ####\\  #### ##  | ## ####\\ ##\n");
    printf(GREEN BOLD " | #######| ##  | ## #####\\ | #####  | ##\\## ## ## ##  | ## ##\\## ##\n");
    printf(GREEN BOLD " | ##     | ##__/ ## ## \\##\\| ##_____| ## \\###| ## ##__/ ## ## \\####\n");
    printf(GREEN BOLD " | ##      \\##    ## ##  \\##\\ ##     \\ ##  \\# | ##\\##    ## ##  \\###\n");
    printf(GREEN BOLD "  \\##       \\###### \\##   \\##\\########\\##      \\## \\###### \\##   \\##  \n");
    printf(RESET "\n");
}

char *inputPipeName()
{
    // input match code
    printf("input match code: ");
    fgets(match_code, 20, stdin);
    match_code[strcspn(match_code, "\n")] = 0;

    // create pipe name based on match code
    snprintf(pipe_name, 50, "\\\\.\\pipe\\match_%s", match_code);
}

int giveSkill()
{
    short myDamage = 0;
    short opponentDamage = 0;
    memset(&playerMsg, 0, sizeof(playerMsg));
    memset(&opponentMsg, 0, sizeof(opponentMsg));

    if (role == 1)
    {
        printFront();
        printPokemonStatus();
        chooseSkill();
        playerMsg.turn = turn;
        myDamage = calculateDamage(playerSkill, opponent_choice);
        subtractOpponentHealth(myDamage);
        playerMsg.dmg = myDamage;

        if (!WriteFile(hPipe, &playerMsg, sizeof(playerMsg), &bytesWritten, NULL))
        {
            printf("Gui du lieu that bai: %ld\n", GetLastError());
            CloseHandle(hPipe);
            return 1;
        }

        printf(ORANGE "\nBan gay ra %hi dmg.\n", myDamage);
        printf(RESET);
        sleep(1);
        if (pokemon[getNumberOfPokemon(opponent_choice) - 1].health <= 0)
        {
            return 0;
        }
        system("cls");
        printFront();
        printPokemonStatus();
        printf("\ndoi ke dich tan cong...\n");

        if (!ReadFile(hPipe, &opponentMsg, sizeof(opponentMsg), &bytesRead, NULL))
        {
            printf("Nhan du lieu that bai: %ld\n", GetLastError());
            CloseHandle(hPipe);
            return 1;
        }
        turn = opponentMsg.turn;
        opponentDamage = opponentMsg.dmg;
        subtractMyHealth(opponentDamage);

        printf(ORANGE "\nDoi thu gay ra %hi dmg.\n", opponentDamage);
        printf(RESET);
        if (pokemon[getNumberOfPokemon(player_choice) - 1].health <= 0)
        {
            return 0;
        }
        sleep(1);
        system("cls");
    }
    else
    {
        if (turn == 1)
        {
            printFront();
            printPokemonStatus();
            printf("\ndoi ke dich tan cong...\n");
            if (!ReadFile(hPipe, &opponentMsg, sizeof(opponentMsg), &bytesRead, NULL))
            {
                printf("Nhan du lieu that bai: %ld\n", GetLastError());
                CloseHandle(hPipe);
                return 1;
            }
        }
        else
        {
            if (!ReadFile(hPipe, &opponentMsg, sizeof(opponentMsg), &bytesRead, NULL))
            {
                printf("Nhan du lieu that bai: %ld\n", GetLastError());
                CloseHandle(hPipe);
                return 1;
            }
            printFront();
            printPokemonStatus();
            printf("\ndoi ke dich tan cong...\n");
        }

        turn = opponentMsg.turn;
        opponentDamage = opponentMsg.dmg;
        subtractMyHealth(opponentDamage);

        system("cls");
        printFront();
        printPokemonStatus();

        printf(ORANGE "\nDoi thu gay ra %hi dmg.\n", opponentDamage);
        printf(RESET);
        sleep(1);
        if (pokemon[getNumberOfPokemon(player_choice) - 1].health <= 0)
        {
            return 0;
        }

        system("cls");
        printFront();
        printPokemonStatus();

        chooseSkill();
        playerMsg.turn = turn;
        myDamage = calculateDamage(playerSkill, opponent_choice);
        subtractOpponentHealth(myDamage);
        playerMsg.dmg = myDamage;

        if (!WriteFile(hPipe, &playerMsg, sizeof(playerMsg), &bytesWritten, NULL))
        {
            printf("Gui du lieu that bai: %ld\n", GetLastError());
            CloseHandle(hPipe);
            return 1;
        }

        printf(ORANGE "\nBan gay ra %hi dmg.\n", myDamage);
        printf(RESET);
        sleep(1);
        if (pokemon[getNumberOfPokemon(opponent_choice) - 1].health <= 0)
        {
            return 0;
        }
        system("cls");
        printFront();
        printPokemonStatus();
        printf("\ndoi ke dich tan cong...\n");
    }

    return 0;
}

int createOrJoinGame()
{
    int choose = 0, again = 0;

    // choose role
    printf("Chon: 1 (create game) \nChon: 2 (join game) \n");
    printf("chon: ");
    while (scanf("%hi", &role) != 1 || (role != 1 && role != 2))
    {
        getchar();
        printf("INVALID INPUT.\nPlease enter 1 or 2: ");
    }
    getchar();

    if (role == 1)
    {
        // Player 1: create pipe
        printf("\n");
        inputPipeName();
        hPipe = CreateNamedPipe(pipe_name, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 100, 100, 0, NULL);
        if (hPipe == INVALID_HANDLE_VALUE)
        {
            printf("Tao pipe that bai: %ld\n", GetLastError());
            return 1;
        }

        printf("Player 1 san sang, doi Player 2...\n");
        // wait for Player 2 to connect
        if (!ConnectNamedPipe(hPipe, NULL))
        {
            printf("Ket noi pipe that bai: %ld\n", GetLastError());
            CloseHandle(hPipe);
            return 1;
        }
    }
    else
    {
        // Player 2: connect to pipe
        do
        {
            printf("\n");
            inputPipeName();
            again = 0;
            hPipe = CreateFile(pipe_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
            if (hPipe == INVALID_HANDLE_VALUE)
            {
                printf("Ket noi pipe that bai: %ld\n", GetLastError());
                again = 1;
            }
        } while (again == 1);
    }

    printf("\nKet noi thanh cong!\n");
    printf("VUI LONG CHO TRONG GIAY LAT...\n");
    sleep(1);
    system("cls");

    if (role == 1)
    {
        // input pokemon choice
        printFront();
        printf("\n");
        printContent();
        choose = choosePokemon();
        strcpy(player_choice, pokemon[choose - 1].name);
        player_choice[strcspn(player_choice, "\n")] = 0;

        if (!WriteFile(hPipe, player_choice, strlen(player_choice) + 1, &bytesWritten, NULL))
        {
            printf("Gui du lieu that bai: %ld\n", GetLastError());
            CloseHandle(hPipe);
            return 1;
        }
        printf("\ndoi ke dich chon pokemon...\n");

        // give opponent choice
        if (!ReadFile(hPipe, opponent_choice, 50, &bytesRead, NULL))
        {
            printf("Nhan du lieu that bai: %ld\n", GetLastError());
            CloseHandle(hPipe);
            return 1;
        }
    }
    else
    {
        printFront();
        printf("\ndoi ke dich chon pokemon...\n");
        // give opponent choice
        if (!ReadFile(hPipe, opponent_choice, 50, &bytesRead, NULL))
        {
            printf("Nhan du lieu that bai: %ld\n", GetLastError());
            CloseHandle(hPipe);
            return 1;
        }

        system("cls");
        printFront();
        printf("\n");
        printContent();

        // input pokemon choice
        choose = choosePokemon();
        strcpy(player_choice, pokemon[choose - 1].name);
        player_choice[strcspn(player_choice, "\n")] = 0;

        if (!WriteFile(hPipe, player_choice, strlen(player_choice) + 1, &bytesWritten, NULL))
        {
            printf("Gui du lieu that bai: %ld\n", GetLastError());
            CloseHandle(hPipe);
            return 1;
        }
    }
    ////////////////////
    if (strcmp(player_choice, opponent_choice) == 0)
    {
        int originalId = getNumberOfPokemon(player_choice) - 1;

        char newName[100];
        snprintf(newName, sizeof(newName), "%s_2", pokemon[originalId].name);

        if (numberOfPokemon < 100)
        {
            pokemon[numberOfPokemon] = pokemon[originalId];
            strcpy(pokemon[numberOfPokemon].name, newName);
            pokemon[numberOfPokemon].name[sizeof(pokemon[numberOfPokemon].name) - 1] = '\0';
            strcpy(opponent_choice, pokemon[numberOfPokemon].name);
            numberOfPokemon++;
        }
        else
        {
            strncat(opponent_choice, "_2", sizeof(opponent_choice) - strlen(opponent_choice) - 1);
        }
    }
    ////////////////////
    printf("\n");
    printf(RED "\t\t\t   TRAN CHIEN BAT DAU!\n");
    printf(RESET);
    sleep(1);
    system("cls");

    return 1;
}
// end

// Function to calculate damage
// defenseEngraved: 1 -> increase attack
// defenseEngraved: 0 -> normal attack
// defenseEngraved: -1 -> decrease attack

char getPokemonType(char *namePokemon)
{
    int i = 0;
    for (; i < numberOfPokemon; i++)
    {
        if (strcmp(namePokemon, pokemon[i].name) == 0)
        {
            break;
        }
    }

    return pokemon[i].type;
}

int getNumberOfPokemon(char *namePokemon)
{
    int i = 0;
    for (; i < numberOfPokemon; i++)
    {
        if (strcmp(namePokemon, pokemon[i].name) == 0)
        {
            break;
        }
    }

    return i + 1;
}

// function to calculate damage

void subtractMana(int mana, char *namePokemon)
{
    pokemon[getNumberOfPokemon(namePokemon) - 1].mana -= mana;
}

void subtractMyHealth(int damage)
{
    pokemon[getNumberOfPokemon(player_choice) - 1].health -= damage;
}

void subtractOpponentHealth(int damage)
{
    pokemon[getNumberOfPokemon(opponent_choice) - 1].health -= damage;
}

short checkDefenseEngraved()
{
    char playerType = getPokemonType(player_choice);
    char opponentType = getPokemonType(opponent_choice);

    if ((playerType == 'f' && opponentType == 'l') ||
        (playerType == 'w' && opponentType == 'f') ||
        (playerType == 'l' && opponentType == 'e') ||
        (playerType == 'e' && opponentType == 'w'))
    {
        return 1; // increate attack
    }
    else if ((playerType == 'f' && opponentType == 'w') ||
             (playerType == 'w' && opponentType == 'e') ||
             (playerType == 'l' && opponentType == 'f') ||
             (playerType == 'e' && opponentType == 'l'))
    {
        return -1; // decrease attack
    }
    else
    {
        return 0; // normal attack
    }
}
// sai khi role 2 nhan dame vao
int calculateDamage(short typeSkill, char *namePokemon)
{
    int damage = 0;
    short playerEngraved = 0;
    //
    playerEngraved = checkDefenseEngraved();
    //

    for (int i = 0; i < numberOfSkill; i += 3)
    {
        if (typeSkill - 1 == i)
        {
            damage = skill[typeSkill - 1].dmg - pokemon[getNumberOfPokemon(namePokemon) - 1].defense;
            return damage;
        }
    }

    if (playerEngraved == 1)
    {
        damage = skill[typeSkill - 1].dmg * 1.5 - pokemon[getNumberOfPokemon(namePokemon) - 1].defense;
    }
    else if (playerEngraved == 0)
    {
        damage = skill[typeSkill - 1].dmg - pokemon[getNumberOfPokemon(namePokemon) - 1].defense;
    }
    else if (playerEngraved == -1)
    {
        damage = skill[typeSkill - 1].dmg * 0.5 - pokemon[getNumberOfPokemon(namePokemon) - 1].defense;
    }

    return damage;
}
// end

void printContent()
{
    printf("\nBan chi co the chon 1 pokemon trong tui de chien dau \n");
    printf(RED "HAY CAN THAN KHI CHON POKEMON!\n");
    printf(RESET);
    sleep(1);
}

void getStructPokemon()
{
    FILE *fptr;
    fptr = fopen("onePokemon.txt", "r");

    if (fptr == NULL)
    {
        printf("Error! Could not open file\n");
        return;
    }

    while (fscanf(fptr, "%99[^,],%c,%d,%d,%d,%d,%d\n", pokemon[numberOfPokemon].name, &pokemon[numberOfPokemon].type, &pokemon[numberOfPokemon].level, &pokemon[numberOfPokemon].mana, &pokemon[numberOfPokemon].health, &pokemon[numberOfPokemon].attack, &pokemon[numberOfPokemon].defense) == 7)
    {
        numberOfPokemon++;
    }

    fclose(fptr);
}

void getStructSkill()
{
    FILE *fptr;
    fptr = fopen("chieuPokemon.txt", "r");

    if (fptr == NULL)
    {
        printf("Error! Could not open file\n");
        return;
    }

    while (fscanf(fptr, "%99[^,],%99[^,],%d,%d\n", skill[numberOfSkill].pokemon, skill[numberOfSkill].nameSkill, &skill[numberOfSkill].dmg, &skill[numberOfSkill].mp) == 4)
    {
        numberOfSkill++;
    }

    fclose(fptr);
}

void printPokemonInPack()
{
    for (int i = 0; i < numberOfPokemon; i++)
    {
        printf("%d. %s \n", i + 1, pokemon[i].name);
    }
}

void printPokemonStatus()
{
    int stayHP = 0, stayMP = 0;
    int subtractHP = 0, subtractMP = 0;
    printf(ORANGE "\t\t\t   BATTLE\n");
    printf(RESET);
    printf("===========================Turn %hi===========================\n\n", turn);
    switch (pokemon[getNumberOfPokemon(opponent_choice) - 1].type)
    {
    case 'f':
        printf(ORANGE "\t\t\t\t%s", opponent_choice);
        printf(RESET);
        break;
    case 'w':
        printf(LIGHTBLUE "\t\t\t\t%s", opponent_choice);
        printf(RESET);
        break;
    case 'l':
        printf(GREEN "\t\t\t\t%s", opponent_choice);
        printf(RESET);
        break;
    case 'e':
        printf(BROWN "\t\t\t\t%s", opponent_choice);
        printf(RESET);
        break;
    }

    printf("\n\t\t\t\tHP: \t");
    stayHP = pokemon[getNumberOfPokemon(opponent_choice) - 1].health / 5;
    subtractHP = 20 - stayHP;
    for (int i = 0; i < stayHP; i++)
    {
        printf(RED "#");
        printf(RESET);
    }
    for (int i = 0; i < subtractHP; i++)
    {
        printf(RED "-");
        printf(RESET);
    }

    printf("\n\n");
    switch (pokemon[getNumberOfPokemon(player_choice) - 1].type)
    {
    case 'f':
        printf(ORANGE "%s", player_choice);
        printf(RESET);
        break;
    case 'w':
        printf(LIGHTBLUE "%s", player_choice);
        printf(RESET);
        break;
    case 'l':
        printf(GREEN "%s", player_choice);
        printf(RESET);
        break;
    case 'e':
        printf(BROWN "%s", player_choice);
        printf(RESET);
        break;
    }
    printf("\nHP: \t");
    stayHP = pokemon[getNumberOfPokemon(player_choice) - 1].health / 5;
    subtractHP = 20 - stayHP;
    for (int i = 0; i < stayHP; i++)
    {
        printf(RED "#");
        printf(RESET);
    }
    for (int i = 0; i < subtractHP; i++)
    {
        printf(RED "-");
        printf(RESET);
    }

    printf("\nMP: \t");
    stayMP = pokemon[getNumberOfPokemon(player_choice) - 1].mana / 5;
    subtractMP = 20 - stayMP;
    for (int i = 0; i < stayMP; i++)
    {
        printf(BLUE "#");
        printf(RESET);
    }
    for (int i = 0; i < subtractMP; i++)
    {
        printf(BLUE "-");
        printf(RESET);
    }
    printf("\n");
}

// Function to choose skill
void printSkillOfPokemon()
{
    short i = 0, j = 1;
    countSkillOfPokemon = 0;
    endSkill = 0;

    printf("\n");
    printf(YELLOW "skill of %s: \n", player_choice);
    printf(RESET);
    for (i = 0; i < numberOfSkill; i++)
    {
        if (strcmp(player_choice, skill[i].pokemon) == 0)
        {
            if (skill[i].mp > pokemon[getNumberOfPokemon(player_choice) - 1].mana)
            {
                printf(GRAY "%d. %s (dmg: %d, mp: %d)\n", j, skill[i].nameSkill, skill[i].dmg, skill[i].mp);
                printf(RESET);
                j++;
                endSkill = i;
                countSkillOfPokemon++;
            }
            else
            {
                printf("%d. %s (dmg: %d, mp: %d)\n", j, skill[i].nameSkill, skill[i].dmg, skill[i].mp);
                j++;
                endSkill = i;
                countSkillOfPokemon++;
            }
        }
    }
}

int getChooseSkill()
{
    int choose = 0;
    short count = 0;

    printSkillOfPokemon();
    do
    {
        if (count == 0)
        {
            printf("\nnhap skill ban muon su dung: ");
        }
        else
        {
            printf("\nban khong du mana, vui long chon skill khac: ");
        }
        while (scanf("%d", &choose) != 1 || choose < 0 || choose > countSkillOfPokemon)
        {
            getchar();
            printf("INVALID INPUT.\nnhap lai skill ban muon su dung: ");
        }

        choose = choose + (endSkill - countSkillOfPokemon + 1);
        count++;
    } while (pokemon[getNumberOfPokemon(player_choice) - 1].mana < skill[choose - 1].mp);

    subtractMana(skill[choose - 1].mp, player_choice);

    return choose;
}

void chooseSkill()
{
    short i = 0;

    playerSkill = getChooseSkill();
    system("cls");
    printFront();
    printPokemonStatus();
    printf("\n");
    printf("%s - %s\n", player_choice, skill[playerSkill - 1].nameSkill);
}
// end

int choosePokemon()
{
    int choose = 0;

    printf("\n");
    printf(YELLOW "pokemon in pack: \n");
    printf(RESET);
    printPokemonInPack();

    printf("\nban muon chon pokemon nao: ");
    while (scanf("%d", &choose) != 1 || choose < 0 || choose > numberOfPokemon)
    {
        getchar();
        printf("INVALID INPUT.\nPlease enter a number between 1 and %d: ", numberOfPokemon);
    }

    return choose;
}

int main()
{
    getStructPokemon(); //
    getStructSkill();   //
    createOrJoinGame(); //

    while (pokemon[getNumberOfPokemon(opponent_choice) - 1].health > 0 && pokemon[getNumberOfPokemon(player_choice) - 1].health > 0)
    {
        giveSkill();

        if (pokemon[getNumberOfPokemon(opponent_choice) - 1].health <= 0)
        {
            system("cls");
            printf(GREEN "\n\t\t\t   BAN DA THANG!\n");
            printf(RESET);
            break;
        }
        else if (pokemon[getNumberOfPokemon(player_choice) - 1].health <= 0)
        {
            system("cls");
            printf(RED "\n\t\t\t   BAN DA THUA CUOC!\n");
            printf(RESET);
            break;
        }

        turn++;
    }

    CloseHandle(hPipe);
    return 0;
}
// D:\c++\c\deadline\pokemon_MinhHoang\pokemonPlay_MinhHoang.exe

// them di chuyen con tro trong terminal như trong test.c