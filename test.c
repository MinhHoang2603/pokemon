#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

#define UP 72
#define DOWN 80
#define ENTER 13

#define MAX_CODE 20
#define MAX_INPUT 512

const char *choices[] = {"Keo", "Bua", "Bao", "Thoat"};

void gotoxy(int x, int y)
{
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void highlight(int x, int y, const char *text)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    gotoxy(x, y);
    printf("%s", text);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void print_choice(int player)
{
    gotoxy(0, 0);
    printf("Ban da chon: %s", choices[player]);
}

int main()
{
    int choice = 0;
    int key;

    system("cls");
    gotoxy(10, 0);
    printf("=== Chon Kéo - Búa - Bao ===");

    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            if (i == choice)
                highlight(10, 5 + i, choices[i]);
            else
            {
                gotoxy(10, 5 + i);
                printf("%s", choices[i]);
            }
        }

        key = getch();
        if (key == 0 || key == 224)
        {
            key = getch();
            if (key == UP && choice > 0)
                choice--;
            else if (key == DOWN && choice < 3)
                choice++;
        }
        else if (key == ENTER)
        {
            break;
        }

        // Xóa lựa chọn cũ để cập nhật lại
        for (int i = 0; i < 4; i++)
        {
            gotoxy(10, 5 + i);
            printf("                    ");
        }
    }

    system("cls");
    if (choice == 3)
    {
        gotoxy(0, 0);
        printf("Ban da chon Thoat. Ket thuc chuong trinh.");
    }
    else
    {
        print_choice(choice);
        gotoxy(0, 1);
        printf("Cam on ban da tham gia!");
    }

    return 0;
}