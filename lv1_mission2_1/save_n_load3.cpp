// save_n_load3.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#define _CRT_SECURE_NO_WARNINGS //이거 없이 scanf()를 쓰면 컴파일 불가
#include <iostream>

#define SIZE 124
using namespace std;

typedef struct s_player {
    char name[SIZE];
    int hp;
    int hp_max;
    int life;
    int life_max;
    int check;//해시값 역할. 17로 나눈 후 나머지 값을 저장해둠
} t_player;

void SetPlayer(t_player* player)
{
    cout << "120자 내로 이름을 입력한 후 enter를 눌러주세요!" << endl;
    scanf("%s", player->name);
    player->hp = 10;
    player->hp_max = 10;
    player->life = 1;
    player->life_max = 10;
    player->check = 0;
}

bool SaveGame(t_player* player)
{
    FILE* pFile = NULL;

    player->check = (player->hp + player->hp_max + player->life + player->life_max) % 17;
    fopen_s(&pFile, "player.bin", "wb");
    if (pFile != NULL)
    {
        fwrite(player->name, 1, SIZE, pFile);
        fwrite(&player->hp, 4, 1, pFile);
        fwrite(&player->hp_max, 4, 1, pFile);
        fwrite(&player->life, 4, 1, pFile);
        fwrite(&player->life_max, 4, 1, pFile);
        fwrite(&player->check, 4, 1, pFile);

        fclose(pFile);
        return true;
    }
    return false;
}

bool LoadGame(t_player* player)
{
    FILE* pFile = NULL;

    fopen_s(&pFile, "player.bin", "rb");
    if (pFile != NULL)
    {
        fread(player->name, 1, SIZE, pFile);
        fread(&player->hp, 4, 1, pFile);
        fread(&player->hp_max, 4, 1, pFile);
        fread(&player->life, 4, 1, pFile);
        fread(&player->life_max, 4, 1, pFile);
        fread(&player->check, 4, 1, pFile);

        fclose(pFile);
        return true;
    }
    return false;
}

void CheckStatus(t_player* player)
{
    printf("캐릭터 이름: %s\n", player->name);
    printf("캐릭터 hp: %d\n", player->hp);
    printf("캐릭터 hp_max: %d\n", player->hp_max);
    printf("캐릭터 life: %d\n", player->life);
    printf("캐릭터 life_max: %d\n", player->life_max);
    printf("캐릭터 check: %d\n", player->check);
}

bool IsCheckRight(t_player* player)
{
    int result = 0;

    result = (player->hp + player->hp_max + player->life + player->life_max) % 17;
    if (result != player->check)
        return false;
    return true;
}

void Playing(t_player* player)
{//여기서 게임이 이뤄져야 하지만 save, load 구현이 핵심이니
    player->hp = 8;//간단히 player 내용 수정만 한다
    player->life = 2;

    if (SaveGame(player) == false)//변경한 내용 save
    {
        cout << "save를 실패했습니다!" << endl;
        return;
    }
    cout << "save에 성공했습니다! load시 아래 내용과 같아야 합니다." << endl;
    CheckStatus(player);
}

int main(void)
{
    t_player player;
    int answer = 0;

    while (answer < 1 || answer > 3)
    {
        system("cls");//콘솔 화면에 작업한 내용 지워줌
        cout << "숫자를 눌러 게임 모드를 선택해주세요!\n" << "1: 새 게임\n"
            << "2: 게임 이어하기\n" << "3: 게임 종료\n" << endl;
        cout << "원하는 게임 모드를 선택해주세요!" << endl;
        cin >> answer;
        if (answer < 1 || answer > 3)
            cout << "잘못된 숫자를 눌렀습니다!" << endl;
    }

    system("cls");
    switch (answer)
    {
    case 1:
        SetPlayer(&player);
        Playing(&player);
        break;
    case 2:
        if (LoadGame(&player) == false)
        {
            cout << "파일 로드에 실패했습니다!" << endl;            
            return 0;
        }
        if (IsCheckRight(&player) == false)
        {
            cout << "파일 로드에 실패했습니다.\n로드된 파일의 cheating을 감지했습니다!" << endl;
            return 0;
        }
        CheckStatus(&player);
        break;
    case 3:
        break;
    }
    return 0;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
