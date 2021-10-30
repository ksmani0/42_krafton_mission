#include <mutex>
#include "Package.h"

int main(void)
{
    Package package;
    
    SetConsoleTitle(TEXT("Until only one left"));
    //char* 아닌 wchar* 인자 받음. 그래서 매크로 TEXT() 사용
    package.SetCursor(false);
    package.SetConsoleSize(WIDTH, HEIGHT);

    while (true)
    {
        system("cls");
        package.Title();

        system("cls");
        thread TDdemageCheck(Package::DemageCheck);//데미지 연산 게임 중 반복

        package.PlayerProcess();

        TDdemageCheck.join();
        bool result = package.ResultDisplay();
        if (result == false)
            break;
    }
    return 0;
}