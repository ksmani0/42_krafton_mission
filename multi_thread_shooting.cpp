﻿// multi_thread_shooting.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <conio.h>
#include <ctime>
//#include <cstring>

using namespace std;
#define SIZE 64
#define MAX 5
#define WIDTH 100
#define HEIGHT 30

struct Info
{
    int playerX = WIDTH / 2;//플레이어 시작 위치
    int playerY = HEIGHT - 2;
    int playerLife = 6;
    int enemyX = 0;//왼쪽에서 오른쪽 왔다갔다
    int enemyY = 2;
    int enemyLife = 6;
};

struct OldInfo
{
    int playerX = WIDTH / 2;
    int playerY = HEIGHT - 2;
    int enemyX = 0;
    int enemyY = 2;
};

struct Shoot
{
    int x = 0;
    int y = 0;
};

struct EnemyShoot
{
    int x = 0;
    int y = 4;
};

Info gInfo;
OldInfo gOld;
Shoot gPlayer[MAX];
EnemyShoot gEnemy[MAX];
char gStatus[28] = "YOU ###### VS ###### ENEMY";//28

void SetCursor(BOOL bshow)
{//커서를 보이게 또는 안 보이게 하는 함수
    CONSOLE_CURSOR_INFO curInfo;

    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = bshow;//커서 보이려면 true, 안 보이려면 false
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}   //지정된 콘솔 화면 버퍼의 커서 형태를 설정

void SetConsoleSize(int col, int lines)
{
    char setText[SIZE];
    sprintf_s(setText, "mode con cols=%d lines=%d", col, lines);
    //아래가 콘솔창 크기 변경하는 법. 아래 예시면 가로 80, 세로 30
    system(setText);//system(mode con cols=80 lines=30);
}

void GotoXY(int x, int y)
{//아래는 커서 위치 저장하는 구조체
    COORD Cur = { static_cast<short>(x), static_cast<short>(y) };//아래는 커서 위치 이동시키는 함수
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
    //GetStdHandle(): 콘솔창 핸들. 콘솔창에 이동된 위치로 커서 옮기라는 것
}

COORD GetXY(void)
{
    COORD cur;
    CONSOLE_SCREEN_BUFFER_INFO curInfo;
    //지정된 콘솔 화면 버퍼 정보 검색. 현재 콘솔창 정보 가져와 2번 인자에 대입
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    cur.X = curInfo.dwCursorPosition.X;//즉 현재 콘솔창의 커서 위치 가져옴
    cur.Y = curInfo.dwCursorPosition.Y;

    return cur;
}

void Title(void)
{
    printf("\n\n\n\n");
    printf("    ■    ■              ■  ■                         ■          \n"); Sleep(80);
    printf("    ■    ■    ■    ■  ■  ■      ■■■   ■        ■  ■    ■\n"); Sleep(80);
    printf("    ■    ■  ■■■      ■  ■     ■    ■  ■■■■  ■   ■ ■  \n"); Sleep(80);
    printf("    ■    ■    ■    ■  ■  ■     ■    ■  ■    ■  ■     ■   \n"); Sleep(80);
    printf("     ■■■     ■■  ■  ■  ■      ■■■   ■    ■  ■    ■    \n"); Sleep(80);
    printf("\n\n");
    printf("                                                      ■                ■■       \n"); Sleep(80);
    printf("                    ■■■   ■          ■■■       ■    ■■■     ■      ■  \n"); Sleep(80);
    printf("                   ■    ■  ■■■■  ■■■■■     ■  ■■■■■ ■■■  ■■■\n"); Sleep(80);
    printf("                   ■    ■  ■    ■   ■            ■   ■          ■      ■  \n"); Sleep(80);
    printf("                    ■■■   ■    ■    ■■■       ■    ■■■     ■      ■  \n"); Sleep(80);

    printf("\n\n\n\n");
    COORD cur = GetXY();
    GotoXY(WIDTH / 2 - 16, cur.Y);
    printf("press any key to start the game!");
    _getch();
}

void MoveNPrintEnemy(void)
{
    static bool goLeft = true;

    gOld.enemyX = gInfo.enemyX;
    GotoXY(gOld.enemyX, gInfo.enemyY); printf("        ");
    if (goLeft == true)
    {
        ++gInfo.enemyX;
        GotoXY(gOld.enemyX + 1, gInfo.enemyY + 1); printf("      ");
        GotoXY(gOld.enemyX + 2, gInfo.enemyY + 2); printf("   ");
        GotoXY(gInfo.enemyX, gInfo.enemyY); printf("□□□□");
        GotoXY(gInfo.enemyX + 1, gInfo.enemyY + 1); printf("□□□");
        GotoXY(gInfo.enemyX + 2, gInfo.enemyY + 2); printf(" □");
    }
    else
    {
        --gInfo.enemyX;
        GotoXY(gOld.enemyX + 1, gInfo.enemyY + 1); printf("      ");
        GotoXY(gOld.enemyX + 2, gInfo.enemyY + 2); printf("   ");
        GotoXY(gInfo.enemyX, gInfo.enemyY); printf("□□□□");
        GotoXY(gInfo.enemyX + 1, gInfo.enemyY + 1); printf("□□□");
        GotoXY(gInfo.enemyX + 2, gInfo.enemyY + 2); printf(" □");
    }

    if (gInfo.enemyX == WIDTH - 8)
        goLeft = false;
    else if (gInfo.enemyX == 0)
        goLeft = true;
}

void PrintEnemyShoot(void)
{
    static int num = 0;
    int oldY;
    char shoot = 'V';
    //static time_t now = time(NULL);
    
    //if (time(NULL) - now < 1)
        //return;
    num = num == MAX ? num : num + 1;
    for (int i = 0; i < num; ++i)
    {
        gEnemy[i].x = gEnemy[i].x == 0 ? gInfo.enemyX + 2 : gEnemy[i].x;
        oldY = gEnemy[i].y;
        if (oldY == HEIGHT - 2)
        {
            GotoXY(gEnemy[i].x, oldY);
            printf(" ");
        }
        gEnemy[i].y = gEnemy[i].y == HEIGHT - 2 ? 4 : gEnemy[i].y + 1;
        
        if (gEnemy[i].y != 0)
        {
            GotoXY(gEnemy[i].x, oldY);
            printf(" ");
            GotoXY(gEnemy[i].x, gEnemy[i].y);
            printf("%c", shoot);
        }
        else
        {
            --num;
            gEnemy[i].y = 0;
            gEnemy[i].x = 0;
        }
    }
    //now = time(NULL);
}

void PrintPlayer(void)
{
    GotoXY(gOld.playerX + 2, gInfo.playerY - 2); printf("   ");
    GotoXY(gOld.playerX + 1, gInfo.playerY - 1); printf("      ");
    GotoXY(gOld.playerX, gInfo.playerY); printf("        ");
    GotoXY(gInfo.playerX + 2, gInfo.playerY - 2); printf(" ■");
    GotoXY(gInfo.playerX + 1, gInfo.playerY - 1); printf("■■■");
    GotoXY(gInfo.playerX, gInfo.playerY); printf("■■■■");
}

void MovePlayer(void)
{
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        gOld.playerX = gInfo.playerX;
        gInfo.playerX = gInfo.playerX == 0 ? 0 : gInfo.playerX - 2;
    }        
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        gOld.playerX = gInfo.playerX;
        gInfo.playerX = gInfo.playerX == WIDTH - 8 ? WIDTH - 8 : gInfo.playerX + 2;
    }
}

void PlayerShoot(void)
{
    char shoot = '0';
    int oldY = 0;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        for (int i = 0; i < MAX; ++i)
        {
            gPlayer[i].x = gInfo.playerX + 2;
            oldY = gPlayer[i].y;
            gPlayer[i].y = gPlayer[i].y == 0 ? HEIGHT - 3 : gPlayer[i].y - 1;
            if (gPlayer[i].y >= 1)
            {
                GotoXY(gPlayer[i].x, oldY);
                printf(" ");
                GotoXY(gPlayer[i].x, gPlayer[i].y);
                printf("%c", shoot);
            }
            else
            {
                gPlayer[i].y = 0;
            }
        }
    }
}

void PrintState(void)
{
    int i = 7;
    int j = 3;

    while (--i != 0)
    {
        if (i > gInfo.playerLife)//4~9
            gStatus[++j] = '-';
        else
            gStatus[++j] = '#';
    }
    i = 0;
    j = 13;
    while (++i != 7)
    {
        if (i <= gInfo.enemyLife)
            gStatus[++j] = '#';//14~~19
        else
            gStatus[++j] = '-';
    }
    GotoXY(WIDTH / 2 - 16, 0);
    printf("%s", gStatus);
}

void PlayerProcess(void)
{
    while (gInfo.playerLife != 0 && gInfo.enemyLife != 0)
    {
        //system("cls");
        PrintState();
        MovePlayer();
        PrintPlayer();
        PlayerShoot();
        MoveNPrintEnemy();
        PrintEnemyShoot();
        Sleep(50);
    }
}

bool ResultDisplay(void)
{
    GotoXY(WIDTH - 4, HEIGHT / 2 - 1);
    if (gInfo.playerLife != 0)
        printf("YOU WIN!\n");
    else
        printf("ENEMY WIN!\n");
    printf("If you retry the game,\nplase enter nay key except ESC!");

    if (_getch() == VK_ESCAPE)//esc
        return false;

    gInfo.playerLife = 6;
    gInfo.enemyLife = 6;
    return true;
}

int main(void)
{
    SetConsoleTitle(TEXT("Until only one left"));//char* 아닌 wchar* 인자 받음
    //그래서 매크로 TEXT() 사용
    SetCursor(false);
    SetConsoleSize(WIDTH, HEIGHT);

    //mutex m;
    
    while (true)
    {
        Title();

        system("cls");
        //thread enemy(EnemyProcess, ref(m));
        //thread playerShoot(PlayerShootThread);
        //thread status(PrintStatusthread, p);

        PlayerProcess();

        //enemy.join();
        //playerShoot.join();
        //status.join();
        if (!ResultDisplay)
            break;
    }    
    return 0;
}

/*
void PlayerShootThread(void)
{
    char shoot = '0';
    int oldY = 0;

    while (gInfo.playerLife != 0 && gInfo.enemyLife != 0)
    {
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            for (int i = 0; i < MAX; ++i)
            {
                gPlayer[i].x = gInfo.playerX + 2;
                oldY = gPlayer[i].y;
                gPlayer[i].y = gPlayer[i].y == 0 ? HEIGHT - 3 : gPlayer[i].y - 1;
                if (gPlayer[i].y >= 1)
                {
                    GotoXY(gPlayer[i].x, oldY);
                    printf(" ");
                    GotoXY(gPlayer[i].x, gPlayer[i].y);
                    printf("%c", shoot);
                }
                else
                    gPlayer[i].y = 0;
            }
        }
    }
}

//////
void PrintStatusthread(char* status)
{
    while (gInfo.playerLife != 0 && gInfo.enemyLife != 0)
    {
        int i = 7;
        int j = 3;

        while (--i != 0)
        {
            if (i > gInfo.playerLife)//4~9
                status[++j] = '#';
            else
                status[++j] = '-';
        }
        i = 0;
        j = 13;
        while (++i != 7)
        {
            if (i <= gInfo.enemyLife)
                status[++j] = '#';//14~~19
            else
                status[++j] = '-';
        }
        GotoXY(WIDTH / 2 - 16, 0);
        printf("%s", status);
    }
}

//////
//if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)//
        //{//
            //gInfo.enemyLife = 0;//
            //break;//스레드 검사용
        //}//
*/