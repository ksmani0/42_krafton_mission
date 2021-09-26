// multi_thread_shooting.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
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

struct PlayerShoot
{
    int x = 0;
    int y = HEIGHT - 4;
};

struct EnemyShoot
{
    int x = 0;
    int y = 4;
};

Info gInfo;
OldInfo gOld;
PlayerShoot gPlayer[MAX];
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
    GotoXY(0, 0);
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
    char shoot = 'V';
    //static time_t now = time(NULL);
    
    //if (time(NULL) - now < 1)
      //  return;
    num = num == MAX ? num : num + 1;
    for (int i = 0; i < num; ++i)
    {
        gEnemy[i].x = gEnemy[i].x == 0 ? gInfo.enemyX + 2 : gEnemy[i].x;
        if (gEnemy[i].y != HEIGHT - 2)
        {
            GotoXY(gEnemy[i].x, gEnemy[i].y);
            printf(" ");
            GotoXY(gEnemy[i].x, ++gEnemy[i].y);
            printf("%c", shoot);
        }
        else
        {
            --num;
            GotoXY(gEnemy[i].x, gEnemy[i].y);
            printf(" ");
            gEnemy[i].y = 4;
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

void PrintPlayerShoot(void)
{
    static int num = 0;
    char shoot = '0';

    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        num = num == MAX ? num : num + 1;
    for (int i = 0; i < num; ++i)
    {
        gPlayer[i].x = gPlayer[i].x == 0 ? gInfo.playerX + 2 : gPlayer[i].x;
        if (gPlayer[i].y != 1)
        {
            GotoXY(gPlayer[i].x, gPlayer[i].y);
            printf(" ");
            GotoXY(gPlayer[i].x, --gPlayer[i].y);
            printf("%c", shoot);
        }
        else
        {
            --num;
            GotoXY(gPlayer[i].x, gPlayer[i].y);
            printf(" ");
            gPlayer[i].y = HEIGHT - 4;
            gPlayer[i].x = 0;
        }
    }
}

void PrintState(void)
{
    int i = 7;
    int j = 3;

    for (int k = 0; k < MAX; ++k)
    {
        if (gPlayer[k].y == 2 &&
            (gPlayer[k].x >= gInfo.enemyX && gPlayer[k].x <= gInfo.enemyX + 8))
        {
            --gInfo.enemyLife;
            gPlayer[k].y = 1;
        }
        if (gEnemy[k].y == HEIGHT - 2 && 
            (gEnemy[k].x >= gInfo.playerX && gEnemy[k].x >= gInfo.playerX + 8))
        {
            --gInfo.playerLife;
            gEnemy[k].y = 4;
        }
    }

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
        PrintPlayerShoot();
        MoveNPrintEnemy();
        PrintEnemyShoot();
        Sleep(50);
    }
}

bool ResultDisplay(void)
{
    while (true)
    {
        system("cls");
        GotoXY(WIDTH / 2 - 4, HEIGHT / 2 - 1);
        if (gInfo.playerLife != 0)
            printf("YOU WIN!\n\n");
        else
            printf("ENEMY WIN!\n\n");
        GotoXY(WIDTH / 2 - 34, HEIGHT / 2 + 1);
        printf("Press 'enter' key to restart the game or 'esc' key to quit the game!\n");

        int result = _getch();
        if (result == VK_ESCAPE)//esc
            return false;
        else if (result == VK_RETURN)//enter
            break;
    }

    gInfo.playerLife = 6;
    gInfo.enemyLife = 6;
    gInfo.playerX = WIDTH / 2;
    gInfo.playerY = HEIGHT - 2;
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
        system("cls");
        Title();

        system("cls");
        //thread enemy(EnemyProcess, ref(m));
        //thread playerShoot(PlayerShootThread);
        //thread status(PrintStatusthread, p);

        PlayerProcess();
        //cin.ignore(1024);

        //enemy.join();
        //playerShoot.join();
        //status.join();
        bool result = ResultDisplay();
        if (result == false)
            break;
    }    
    return 0;
}
