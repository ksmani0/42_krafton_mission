#include "Package.h"

string Package::mStatus = "YOU ###### VS ###### ENEMY";
Info Package::mInfo = { WIDTH / 2, HEIGHT - 2, 6, 0, 2, 6 };
OldInfo Package::mOld = { WIDTH / 2, HEIGHT - 2, 0, 2 };
PlayerShoot Package::mPlayer[MAX] = { 0, };
EnemyShoot Package::mEnemy[MAX] = { 0, };

//������ �и�
void Package::DemageCheck(void)
{
    while (mInfo.playerLife != 0 && mInfo.enemyLife != 0)
    {
        for (int i = 0; i < MAX; ++i)
        {
            if (mPlayer[i].y == 1 && mPlayer[i].x >= mInfo.enemyX &&
                mPlayer[i].x <= mInfo.enemyX + 8)//8
            {
                --mInfo.enemyLife;
                mPlayer[i].y = HEIGHT - 4;
            }
            if (mEnemy[i].y == HEIGHT - 2 && mEnemy[i].x >= mInfo.playerX &&
                mEnemy[i].x <= mInfo.playerX + 8)//8
            {
                --mInfo.playerLife;
                mEnemy[i].y = 4;
            }
        }
    }
}



Package::Package(void)
{
    for (int i = 0; i < MAX; ++i)
    {
        mPlayer[i].x = 0;
        mPlayer[i].y = HEIGHT - 4;
        mEnemy[i].x = 0;
        mEnemy[i].y = 4;
    }
}



//�ܼ� ���� ����
void Package::SetCursor(BOOL bshow)
{//Ŀ���� ���̰� �Ǵ� �� ���̰� �ϴ� �Լ�
    CONSOLE_CURSOR_INFO curInfo;

    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = bshow;//Ŀ�� ���̷��� true, �� ���̷��� false
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}   //������ �ܼ� ȭ�� ������ Ŀ�� ���¸� ����

void Package::SetConsoleSize(int col, int lines)
{
    char setText[CS_SIZE];
    sprintf_s(setText, "mode con cols=%d lines=%d", col, lines);
    //�Ʒ��� �ܼ�â ũ�� �����ϴ� ��. �Ʒ� ���ø� ���� 80, ���� 30
    system(setText);//system(mode con cols=80 lines=30);
}



//��ǥ ����
void Package::GotoXY(int x, int y)
{//�Ʒ��� Ŀ�� ��ġ �����ϴ� ����ü
    COORD Cur = { static_cast<short>(x), static_cast<short>(y) };//�Ʒ��� Ŀ�� ��ġ �̵���Ű�� �Լ�
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
    //GetStdHandle(): �ܼ�â �ڵ�. �ܼ�â�� �̵��� ��ġ�� Ŀ�� �ű��� ��
}

COORD Package::GetXY(void)
{
    COORD cur;
    CONSOLE_SCREEN_BUFFER_INFO curInfo;
    //������ �ܼ� ȭ�� ���� ���� �˻�. ���� �ܼ�â ���� ������ 2�� ���ڿ� ����
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    cur.X = curInfo.dwCursorPosition.X;//�� ���� �ܼ�â�� Ŀ�� ��ġ ������
    cur.Y = curInfo.dwCursorPosition.Y;

    return cur;
}


void Package::Title(void)
{
    GotoXY(0, 0);
    printf("\n\n\n\n");
    printf("    ��    ��                      ��                         ��          \n"); Sleep(80);
    printf("    ��    �� ��         ��    ��  ��      ����   ��        ��  ��    ��\n"); Sleep(80);
    printf("    ��    �� ����� ����      ��     ��    ��  �����  ��   �� ��  \n"); Sleep(80);
    printf("    ��    �� ��    ��   ��    ��  ��     ��    ��  ��    ��  ��     ��   \n"); Sleep(80);
    printf("     ����  ��    ��   ���  ��  ��      ����   ��    ��  ��    ��    \n"); Sleep(80);
    printf("\n\n");
    printf("                                                       ��                ���       \n"); Sleep(80);
    printf("                     ����   ��          ����       ��    ����     ��      ��  \n"); Sleep(80);
    printf("                    ��    ��  �����  ������     ��  ������ ����  ����\n"); Sleep(80);
    printf("                    ��    ��  ��    ��   ��            ��   ��          ��      ��  \n"); Sleep(80);
    printf("                     ����   ��    ��    ����       ��    ����     ��      ��  \n"); Sleep(80);

    printf("\n\n\n\n");
    COORD cur = GetXY();
    GotoXY(WIDTH / 2 - 16, cur.Y);
    printf("press any key to start the game!");
    _getch();
}

void Package::PrintState(void)
{
    int i = 7;
    int j = 3;

    while (--i != 0)
    {
        if (i > mInfo.playerLife)//4~9
            mStatus[++j] = '-';
        else
            mStatus[++j] = '#';
    }
    i = 0;
    j = 13;
    while (++i != 7)
    {
        if (i <= mInfo.enemyLife)
            mStatus[++j] = '#';//14~~19
        else
            mStatus[++j] = '-';
    }
    GotoXY(WIDTH / 2 - 16, 0);
    printf("%s", mStatus.c_str());
}

void Package::PlayerProcess(void)
{
    while (mInfo.playerLife != 0 && mInfo.enemyLife != 0)
    {
        PrintState();
        MovePlayer();
        PrintPlayer();
        PrintPlayerShoot();
        MoveNPrintEnemy();
        PrintEnemyShoot();
        Sleep(50);
    }
}

bool Package::ResultDisplay(void)
{
    while (true)
    {
        system("cls");
        GotoXY(WIDTH / 2 - 4, HEIGHT / 2 - 1);
        if (mInfo.playerLife != 0)
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

    mInfo.playerLife = 6;
    mInfo.enemyLife = 6;
    mInfo.playerX = WIDTH / 2;
    mInfo.playerY = HEIGHT - 2;
    return true;
}



//�÷��̾�
void Package::MovePlayer(void)
{
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)//0x8000
    {
        mOld.playerX = mInfo.playerX;
        mInfo.playerX = mInfo.playerX == 0 ? 0 : mInfo.playerX - 2;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        mOld.playerX = mInfo.playerX;
        mInfo.playerX = mInfo.playerX == WIDTH - 8 ? WIDTH - 8 : mInfo.playerX + 2;
    }
}

void Package::PrintPlayer(void)
{
    GotoXY(mOld.playerX + 2, mInfo.playerY - 2); printf("   ");
    GotoXY(mOld.playerX + 1, mInfo.playerY - 1); printf("      ");
    GotoXY(mOld.playerX, mInfo.playerY); printf("        ");
    GotoXY(mInfo.playerX + 2, mInfo.playerY - 2); printf(" ��");
    GotoXY(mInfo.playerX + 1, mInfo.playerY - 1); printf("����");
    GotoXY(mInfo.playerX, mInfo.playerY); printf("�����");
}//���ۿ� ��ü �׸� �׸��� ����ϴ� �� �ƴϸ� ������ ���� �� �ܻ� ���� ��

void Package::PrintPlayerShoot(void)
{
    static int num = 0;
    char shoot = '0';

    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        num = num == MAX ? num : num + 1;
    for (int i = 0; i < num; ++i)
    {
        mPlayer[i].x = mPlayer[i].x == 0 ? mInfo.playerX + 2 : mPlayer[i].x;
        if (mPlayer[i].y > 1)
        {
            GotoXY(mPlayer[i].x, mPlayer[i].y);
            printf(" ");
            GotoXY(mPlayer[i].x, --mPlayer[i].y);
            printf("%c", shoot);
        }
        else
        {
            if (mPlayer[i].x >= mInfo.enemyX && mPlayer[i].x <= mInfo.enemyX + 8)//8
            {
                GotoXY(mPlayer[i].x, mPlayer[i].y);
                printf(" ");
            }

            --num;
            GotoXY(mPlayer[i].x, mPlayer[i].y);
            printf(" ");
            mPlayer[i].y = HEIGHT - 4;
            mPlayer[i].x = 0;
        }
    }
}



//��
void Package::MoveNPrintEnemy(void)
{
    static bool goLeft = true;

    mOld.enemyX = mInfo.enemyX;
    GotoXY(mOld.enemyX, mInfo.enemyY); printf("        ");
    if (goLeft == true)
    {
        ++mInfo.enemyX;
        GotoXY(mOld.enemyX + 1, mInfo.enemyY + 1); printf("      ");
        GotoXY(mOld.enemyX + 2, mInfo.enemyY + 2); printf("   ");
        GotoXY(mInfo.enemyX, mInfo.enemyY); printf("�����");
        GotoXY(mInfo.enemyX + 1, mInfo.enemyY + 1); printf("����");
        GotoXY(mInfo.enemyX + 2, mInfo.enemyY + 2); printf(" ��");
    }
    else
    {
        --mInfo.enemyX;
        GotoXY(mOld.enemyX + 1, mInfo.enemyY + 1); printf("      ");
        GotoXY(mOld.enemyX + 2, mInfo.enemyY + 2); printf("   ");
        GotoXY(mInfo.enemyX, mInfo.enemyY); printf("�����");
        GotoXY(mInfo.enemyX + 1, mInfo.enemyY + 1); printf("����");
        GotoXY(mInfo.enemyX + 2, mInfo.enemyY + 2); printf(" ��");
    }

    if (mInfo.enemyX == WIDTH - 8)
        goLeft = false;
    else if (mInfo.enemyX == 0)
        goLeft = true;
}

void Package::PrintEnemyShoot(void)
{
    static int num = 0;
    char shoot = 'V';

    num = num == MAX ? num : num + 1;
    for (int i = 0; i < num; ++i)
    {
        mEnemy[i].x = mEnemy[i].x == 0 ? mInfo.enemyX + 2 : mEnemy[i].x;
        if (mEnemy[i].y != HEIGHT - 2)
        {
            GotoXY(mEnemy[i].x, mEnemy[i].y);
            printf(" ");
            GotoXY(mEnemy[i].x, ++mEnemy[i].y);
            printf("%c", shoot);
        }
        else
        {
            --num;
            GotoXY(mEnemy[i].x, mEnemy[i].y);
            printf(" ");
            mEnemy[i].y = 4;
            mEnemy[i].x = 0;
        }
    }
}