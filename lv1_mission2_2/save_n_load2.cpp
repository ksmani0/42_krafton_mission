// save_n_load2.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#define _CRT_SECURE_NO_WARNINGS //이거 없이 strcpy()를 쓰면 컴파일 불가
#include <iostream>
#include <windows.h>
#include <stdlib.h> //srand(), rand() header
#include <time.h>
#include <String>

using namespace std;

#define SIZE 32
#define WIDTH 100
#define STR_HEGITH 10
#define HEIGHT 30
#define MAX 36

typedef struct s_player {
    int x = WIDTH / 2;//플레이어의 시작 위치
    int y = HEIGHT - 2;
    int life = 3;
    int score = 0;
    int hash = 0;
    bool bActive = true;
    char name[SIZE] = { 0, };
} t_player;

typedef struct s_enemy {
    int x = 0;
    int y = 2;
    bool bSpawn = false;//c++에선 바로 초기화 가능
} t_enemy;

typedef struct s_arrow {
    int x = 0;
    int y = 0;
    bool bActive = false;
} t_arrow;

t_player g_player;
t_enemy g_enemy[MAX];
t_arrow g_arrow;

void GotoXY(int x, int y)
{//아래는 커서 위치 저장하는 구조체
    COORD Cur = { x, y };//아래는 커서 위치 이동시키는 함수
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
    //GetStdHandle(): 콘솔창 핸들. 콘솔창에 이동된 위치로 커서 옮기라는 것
}

COORD GetXY(void)
{
    COORD cur;
    CONSOLE_SCREEN_BUFFER_INFO curInfo;
    //지정된 콘솔 화면 버퍼에 대한 정보 검색. 현재 콘솔창에 대한 정보 가져와 2번 인자에 담는다
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    cur.X = curInfo.dwCursorPosition.X;//즉 현재 콘솔창의 커서 위치를 가져옴
    cur.Y = curInfo.dwCursorPosition.Y;

    return cur;
}

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

void PrintState(void)
{
    GotoXY(0, 0);
    printf("Name: %s\nLife: %d    Score: %d", g_player.name, g_player.life, g_player.score);
}

void NewGame(void)
{
    system("cls");
    GotoXY(0, HEIGHT / 2);
    cout << "플레이어 이름을 10글자 내로 적어주세요\n";
    cin >> g_player.name;
    cin.clear(); //이름 잔상 지우기
    cin.ignore(SIZE, '\n'); //입력된 문자들을 버퍼에서 비움
}

char StartMenu(void)
{
    char mode = 0;

    while (true)
    {
        system("cls");//콘솔 화면에 작업한 내용 지워줌
        GotoXY(0, STR_HEGITH);
        cout << "게임 모드를 선택해주세요!\n" << "N : 새 게임\n"
            << "L : 게임 이어하기\n" << "E : 게임 종료\n" << endl;
        cout << "원하는 게임 모드의 영문자를 입력해주세요!" << endl;
        mode = (char)cin.get();
        cin.clear(); //잔상 지우기
        cin.ignore(SIZE, '\n'); //입력된 문자들을 버퍼에서 비움
        if (mode == 'N' || mode == 'L' || mode == 'E')
            break;
    }
    cin.clear(); //잔상 지우기
    cin.ignore(SIZE, '\n'); //입력된 문자들을 버퍼에서 비움
    if (mode == 'N')
        NewGame();

    return mode;
}

bool SaveGame()
{
    FILE* pFile = NULL;

    g_player.hash = (g_player.life + g_player.score) % 11;
    fopen_s(&pFile, "player.bin", "wb");
    if (pFile != NULL)
    {
        fwrite(&g_player.x, 4, 1, pFile);
        fwrite(&g_player.y, 4, 1, pFile);
        fwrite(&g_player.life, 4, 1, pFile);
        fwrite(&g_player.score, 4, 1, pFile);
        fwrite(&g_player.hash, 4, 1, pFile);
        fwrite(&g_player.bActive, 1, 1, pFile);
        fwrite(g_player.name, 1, SIZE, pFile);

        fclose(pFile);
        return true;
    }
    return false;
}

bool LoadGame(void)
{
    FILE* pFile = NULL;
    int result = 0;

    fopen_s(&pFile, "player.bin", "rb");
    if (pFile != NULL) {
        fread(&g_player.x, 4, 1, pFile);
        fread(&g_player.y, 4, 1, pFile);
        fread(&g_player.life, 4, 1, pFile);
        fread(&g_player.score, 4, 1, pFile);
        fread(&g_player.hash, 4, 1, pFile);
        fread(&g_player.bActive, 1, 1, pFile);
        fread(g_player.name, 1, SIZE, pFile);

        fclose(pFile);
        result = (g_player.life + g_player.score) % 11;
        if (result != g_player.hash)
        {
            GotoXY(WIDTH / 2, HEIGHT / 2);
            cout << "\n\n!!!로드된 파일에서 cheat를 감지했습니다!!!" << endl;
            Sleep(700);
            return false;
        }
        return true;
    }
    GotoXY(WIDTH / 2, HEIGHT / 2);
    cout << "저장된 파일이 없습니다!" << endl;
    return false;
}

void SpawnEnemy(void)
{
    for (int i = 0; i < MAX; ++i) {
        if (!g_enemy[i].bSpawn) {
            g_enemy[i].x = rand() % (WIDTH / 2) * 2;
            g_enemy[i].y = 2;
            g_enemy[i].bSpawn = true;
            break;//별 하나 등장시키면 탈출. 안 그러면 화면 가득 별이 나온다.
        }
    }
}

void MovePlayer(void)
{
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        g_player.x = g_player.x == 0 ? 0 : g_player.x - 2;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        g_player.x = g_player.x == WIDTH - 2 ? WIDTH - 2 : g_player.x + 2;
    if (GetAsyncKeyState(VK_UP) & 0x8000)
        g_player.y = g_player.y == 2 ? 2 : g_player.y - 1;
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        g_player.y = g_player.y == HEIGHT - 2 ? HEIGHT - 2 : g_player.y + 1;
}

void PrintEnemyCheckCrash(void)
{
    for (int i = 0; i < MAX; ++i) {
        if (g_enemy[i].bSpawn == false)
            continue;//break에서 변경했더니 종종 적이 사라지는 현상 해결
        GotoXY(g_enemy[i].x, g_enemy[i].y);
        printf("☆");
        g_enemy[i].y++;

        if (g_enemy[i].y == g_arrow.y && g_enemy[i].x == g_arrow.x && g_arrow.bActive == true) {
            g_enemy[i].bSpawn = false;
            g_arrow.bActive = false;
            g_player.score++;
        }
        if (g_enemy[i].y == g_player.y && g_enemy[i].x == g_player.x) {
            g_enemy[i].bSpawn = false;
            g_player.life = g_player.life == 0 ? 0 : g_player.life - 1;
        }
        g_player.bActive = g_player.life == 0 ? false : true;
        if (g_player.bActive == false)
            break; //플레이어가 죽으면 게임 끝
        if (g_enemy[i].y > HEIGHT)
            g_enemy[i].bSpawn = false;
    }//별 중 몇 개가 참인지 모르니 여기선 break 없이 다 확인 필요
}

void RenderPlayer(void)
{
    GotoXY(g_player.x, g_player.y);
    if (g_player.bActive == true)
        printf("▲");
    else {
        printf("X");
        Sleep(800);
    }
}

void ShootArrow(void)
{
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (!g_arrow.bActive) {//스페이스 누르면 화살을 화면에 그리도록 불값 변경
            g_arrow.x = g_player.x;//플레이어에서 화살이 나가야 하니
            g_arrow.y = g_player.y - 1;//플레이어 기준 좌표값을 대입
            g_arrow.bActive = true;
        }//이 조건문 없으면 화살이 날아가다 스페이스 눌리면 사라지고 다시 플레이어 위치에서 날아감
    }
    if (g_arrow.bActive) {
        GotoXY(g_arrow.x, g_arrow.y);//화살이 그려질 위치로 커서 이동
        printf("†");//이동한 위치에 그림
        g_arrow.y--;//위로 날아가니 y좌표값을 계속 줄임
        if (g_arrow.y < 0)//천장 닿으면 화살 화면에서 지움
            g_arrow.bActive = false;
    }
}

char ResultMenu(void)
{
    char mode = 0;

    while (true)
    {
        system("cls");
        GotoXY(0, STR_HEGITH);
        printf("Your score: %d\n", g_player.score);

        cout << "\n게임 모드를 선택해주세요!\n" << "N : 새 게임\n"
            << "S : 게임 저장하기\n" << "L : 게임 불러오기\n" << "E : 게임 종료\n" << endl;
        cout << "원하는 게임 모드의 영문자를 눌러주세요!\n" <<
            "단 life가 0인 상태로 게임이 종료됐을 땐 게임이 저장되지 않습니다." << endl;
        mode = (char)cin.get();
        cin.clear(); //잔상 지우기
        cin.ignore(SIZE, '\n'); //입력된 문자들을 버퍼에서 비움
        if (mode == 'N' || (mode == 'S' && g_player.life != 0) || mode == 'L' || mode == 'E')
            break;
    }
    cin.clear(); //잔상 지우기
    cin.ignore(SIZE, '\n'); //입력된 문자들을 버퍼에서 비움
    
    if (mode == 'N')
    {
        NewGame();
        mode = 0;
    }

    return mode;
}

char RunGame(void)
{
    while (true) {
        system("cls");//한 프레임 전의 상하 이동시 남는 잔상을 이게 지움
        if (g_player.bActive == false || GetAsyncKeyState(VK_ESCAPE))
            break;//GetAsyncKeyState(VK_ESCAPE) 이면 지금 눌렸거나 이전에 눌린 거 다 처리 가능

        PrintState();
        SpawnEnemy();
        MovePlayer();
        ShootArrow();
        PrintEnemyCheckCrash();
        RenderPlayer();

        Sleep(50);//1000이면 1초
    }
    cin.clear(); //잔상 지우기

    return ResultMenu();
}

int main(void)
{
    SetConsoleTitle(TEXT("seungmki's game"));//이 함수는 char*가 아닌 wchar*를 인자로 받음
    //그래서 매크로 TEXT() 사용
    srand(time(NULL));
    SetConsoleSize(WIDTH, HEIGHT);//콘솔 사이즈 설정
    SetCursor(false);//커서 감추기

    char mode = 'N';
    while (true)
    {
        if (mode == 'N')
            mode = StartMenu();
        if (mode == 'L') {
            if (LoadGame() == false) {
                cout << "파일 불러오기에 실패했습니다!" << endl;
                Sleep(700);
                break;
            }
        }
        if (mode == 'S') {
            if (SaveGame() == false) {
                cout << "파일 저장에 실패했습니다!" << endl;
                Sleep(700);
                break;
            }
        }
        if (mode == 'E')
            break;

        mode = RunGame();
    }

    system("cls");//종료 화면만 깨끗이 보이도록 앞서 지우기
    GotoXY(0, HEIGHT / 2);
    cout << "게임을 종료합니다." << endl;
    system("pause");//바로 꺼지지 않고 아무 키나 누르라고 함
    return 0;
}

/*
char StartMenu(void)
{
    char mode = 0;

    while (true)
    {
        system("cls");//콘솔 화면에 작업한 내용 지워줌
        GotoXY(0, STR_HEGITH);
        cout << "게임 모드를 선택해주세요!\n" << "N : 새 게임\n"
            << "L : 게임 이어하기\n" << "E : 게임 종료\n" << endl;
        cout << "원하는 게임 모드의 영문자를 눌러주세요!" << endl;
        if (GetAsyncKeyState(0X4e) & 0x8000)
            mode = 'N';
        if (GetAsyncKeyState(0X4c) & 0x8000)
            mode = 'L';
        if (GetAsyncKeyState(0x45) & 0x8000)
            mode = 'E';
        if (mode == 'N' || mode == 'L' || mode == 'E')
            break;
    }
    if (mode == 'N')
        NewGame();

    return mode;
}

char ResultMenu(void)
{
    char mode = 0;

    while (true)
    {
        system("cls");
        GotoXY(0, STR_HEGITH);
        printf("Your score: %d\n", g_player.score);

        cout << "\n게임 모드를 선택해주세요!\n" << "N : 새 게임\n"
            << "S : 게임 저장하기\n" << "L : 게임 불러오기\n" << "E : 게임 종료\n" << endl;
        cout << "원하는 게임 모드의 영문자를 눌러주세요!\n" <<
            "단 life가 0인 상태로 게임이 종료됐을 땐 게임이 저장되지 않습니다." << endl;
        if (GetAsyncKeyState(0X4e) & 0x8000)
            mode = 'N';
        if (GetAsyncKeyState(0x53) & 0x8000 && g_player.life != 0)
            mode = 'S';//목숨이 0이면 저장 불가
        if (GetAsyncKeyState(0X4c) & 0x8000)
            mode = 'L';
        if (GetAsyncKeyState(0x45) & 0x8000)
            mode = 'E';
        if (mode == 'N' || mode == 'S' || mode == 'L' || mode == 'E')
            break;
    }

    if (mode == 'N')
    {
        NewGame();
        mode = 0;
    }

    return mode;
}
*/