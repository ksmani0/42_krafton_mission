// key_input_test.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _CRT_NONSTDC_NO_WARNINGS
#include <iostream>
#include <Windows.h> //GetAsyncKeyState() header
#include <conio.h> //_putch() header

void gotoxy(int x, int y)
{
    COORD Cur;//커서 위치를 저장하는 구조체
    Cur.X = x;
    Cur.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
    //SetConsoleCursorPosition(): 커서 위치를 이동시키는 함수
    //GetStdHandle(): 콘솔창 핸들
}

int main()
{
    int x = 0;
    int y = 0;
    int c = '@';

    while (true) {
        gotoxy(x, y);
        putch(c);//화면에 출력하는 함수

        //if (GetAsyncKeyState(VK_UP) & GetAsyncKeyState(VK_UP) & 0x8000) y-=2;
        //else if (GetAsyncKeyState(VK_DOWN) & GetAsyncKeyState(VK_DOWN) & 0x8000) y+=2;
        //else if (GetAsyncKeyState(VK_LEFT) & GetAsyncKeyState(VK_LEFT) & 0x8000) x-=2;
        //else if (GetAsyncKeyState(VK_RIGHT) & GetAsyncKeyState(VK_RIGHT) & 0x8000) x+=2;
        if (GetAsyncKeyState(VK_LCONTROL) & GetAsyncKeyState(0x52) & 0x8000)
            c = c == '@' ? '#' : '@';//여기서 화면에 찍히는 모양을 바꾼다
        if (GetAsyncKeyState(VK_UP) & 0x8000) y--;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000) y++;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) x--;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) x++;

        Sleep(65);
        if (GetAsyncKeyState(VK_ESCAPE))
            break;//esc 만나면 탈출
    }
    return 0;
}

/*
        if (GetAsyncKeyState(VK_UP) & 0x8000) y--;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000) y++;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) x--;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) x++;

        //if (GetAsyncKeyState(VK_UP) & 0x0001)
        //    y -= 2;
        //if (GetAsyncKeyState(VK_DOWN) & 0x0001)
        //    y += 2;
        //if (GetAsyncKeyState(VK_LEFT) & 0x0001)
        //    x -= 2;
        //if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
        //    x += 2;
*/


// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
