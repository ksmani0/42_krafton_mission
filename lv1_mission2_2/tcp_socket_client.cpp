// tcp_socket_client.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
//#include <windows.h>
#include <winsock2.h>//winsok은 TCP/IP 프로그래밍을 위한 API
#include <WS2tcpip.h>
#include <time.h>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")//자동으로 라이브러리 링크

using namespace std;

#define SIZE 64
#define NUM 2
#define WIDE 80
#define HEIGHT 36

typedef struct s_player
{
    int x = 0;
    int y = 0;
    int oldX = 0;
    int oldY = 0;
} t_player;//내 프로토콜

t_player g_player[NUM];//0=client, 1=server. 얘네가 프로토콜의 type인 것

int g_heightLimit = 0;//이 이상 플레이어가 올라오지 못하게 막음

void GotoXY(int x, int y)
{//아래는 커서 위치 저장하는 구조체
    COORD Cur = { x, y };//아래는 커서 위치 이동시키는 함수
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
    //GetStdHandle(): 콘솔창 핸들. 콘솔창에 이동된 위치로 커서 옮기라는 것
}

void GetXY(int* x, int* y)
{
    CONSOLE_SCREEN_BUFFER_INFO curInfo;
    //지정된 콘솔 화면 버퍼에 대한 정보 검색. 현재 콘솔창에 대한 정보 가져와 2번 인자에 담는다
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    *x = curInfo.dwCursorPosition.X;//즉 현재 콘솔창의 커서 위치를 가져옴
    *y = curInfo.dwCursorPosition.Y;
}

void SetCursor(BOOL bshow)
{//커서를 보이게 또는 안 보이게 하는 함수
    CONSOLE_CURSOR_INFO curInfo;

    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = bshow;//커서 보이려면 true, 안 보이려면 false
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}   //지정된 콘솔 화면 버퍼의 커서 형태를 설정

void MoveClientPlayer()
{
    while(true)
    {
        if (g_player[0].x != g_player[0].oldX || g_player[0].y != g_player[0].oldY)
        {
            GotoXY(g_player[0].oldX, g_player[0].oldY);
            printf("  ");//공백 2개 출력하면 잔상 지울 수 있음
        }
        GotoXY(g_player[0].x, g_player[0].y);
        printf("○");//화면에 출력하는 함수
        g_player[0].oldX = g_player[0].x;
        g_player[0].oldY = g_player[0].y;

        if (GetAsyncKeyState(VK_LEFT) & 0x8000)
            g_player[0].x = g_player[0].x == 0 ? 0 : g_player[0].x - 1;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
            g_player[0].x = g_player[0].x == WIDE ? WIDE : g_player[0].x + 1;
        if (GetAsyncKeyState(VK_UP) & 0x8000)
            g_player[0].y = g_player[0].y == g_heightLimit ? g_heightLimit : g_player[0].y - 1;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
            g_player[0].y = g_player[0].y == HEIGHT ? HEIGHT : g_player[0].y + 1;

        Sleep(65);
        if (GetAsyncKeyState(VK_END) & 0x8000)
            break;//이 키 누르면 서버가 보낼 차례
    }    
}

int main(void)
{
    WSADATA wsaData;
    SOCKET hSocket;//서버에 연결되고 데이터 주고 받을 소켓
    SOCKADDR_IN servAddr;//클라이언트가 연결할 서버의 ip 주소, 주소 체계, 포트 번호 담아두는 변수


    //0.콘솔창 제목, 크기, 커서 표시 설정
    char setText[SIZE];
    
    SetConsoleTitle(TEXT("seungmki's window: Client"));//char*가 아닌 wchar*를 인자로 받아서 매크로 TEXT() 사용
    sprintf_s(setText, "mode con cols=%d lines=%d", WIDE, HEIGHT);
    system(setText);//콘솔창 크기 변경하는 법
    SetCursor(false);//커서 감추기


    //1.윈속 초기화. 2.2 버전으로 설정
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {//이 함수 실행 후 wsaData.wVersion에 0x0202가 들어있는 걸 확인할 수 있음
        printf("Failed WSAStartup()\n");
        return 1;
    }


    //2.소켓 생성. socket() 인자로 소켓 설정
    //PF_INET=IPv4 체계(AF_INET), SOCK_STREAM=TCP, IPPROTO_TCP=TCP 프로토콜
    //ip는 랜 카드의 주소. 이 랜 카드를 통해 프로그램이 작동하는데, 프로그램마다 가진 게 포트
    hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET)
    {
        printf("Failed socket()\n");
        WSACleanup();
        return 1;
    }


    //3.서버 연결
    //3-1.연결할 서버 주소 셋팅
    memset(&servAddr, 0, sizeof(servAddr));//servAddr 쓸 때 오류(bind, connect 안 되는 등) 안 나게 초기화
    servAddr.sin_family = AF_INET;//주소 체계로 IPv4 사용한다고 설정
    //servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//옛날 방식. 현재 이 함수는 컴파일 에러 나서
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);//대신 이 함수 쓴다
    //로컬 ip 썼으니 서버와 클라이언트가 내 컴퓨터 안에서 주고받겠다는 것
    //1번 인자는 주소 체계, 2번 인자는 문자열로된 주소, 3번은 주소 담을 변수
    servAddr.sin_port = htons(30002);//포트 설정. 그냥 숫자만 대입하지 않고  htons() 쓰는 건
    //개인 pc-대형 pc 간 리틀-빅 엔디안 차이 없애려고. host to network short 호출로 빅 엔디안에 맞춤


    //3-2.서버에 연결 시도
    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {//servAddr에 접속하려는 서버 ip, 포트 정보 있음. 이걸 이용해 hSocket 소켓으로 서버에 접속 시도
        printf("Failed connect()\n");
        closesocket(hSocket);
        WSACleanup();
        return 1;
    }//connect()는 블로킹 함수. 연결, 거절, 시간 초과 등 결과 나오기 전까지 진행 막힘


    ////////// 서버와 함께 플레이어 이동 부분 //////////
    int recvSize = 0;
    int sendSize = 0;
    char recvData[SIZE];//서버와 데이터 주고받을 거라
    char sendData[SIZE];//메시지 버처 2개 준비
    sendData[2] = 0;

    if ((recvSize = recv(hSocket, recvData, sizeof(recvData), 0)) < 0)
    {//블로킹 함수. 성공, 실패, 종료 중 하나 결정돼야 다음 진행됨. 동기 함수.
        printf("Failed recv()\n");
        closesocket(hSocket);
        WSACleanup();
        return 1;
    }

    printf("Server-Client connection success:\nSevre: %s", recvData);
    printf("\n==========서버와 함께 플레이어들을 움직여 보자!==========\n");
    GetXY(&(g_player[0].x), &(g_player[0].y));
    g_heightLimit = g_player[0].y;//여기가 최대로 올라갈 수 있는 행
    g_player[0].oldX = g_player[0].x;
    g_player[0].oldY = g_player[0].y;
    g_player[1].y = g_heightLimit;
    g_player[1].oldY = g_heightLimit;

    while (true)
    {
        MoveClientPlayer();        
        sendData[0] = g_player[0].x;//내 프로토콜에 맞춰 데이터 전달
        sendData[1] = g_player[0].y;
        send(hSocket, sendData, 3, 0);//플레이어 이동은 클라이언트가 먼저 보내고
                
        if ((recvSize = recv(hSocket, recvData, SIZE, 0)) < 0)
        {//이후에 서버가 보낸 패킷에서 서버 플레이어의 이동 좌표 받음
            printf("Failed recv()\n");
            closesocket(hSocket);
            WSACleanup();
            return 1;
        }

        g_player[1].x = recvData[0];
        g_player[1].y = recvData[1];
        if (g_player[1].x != g_player[1].oldX || g_player[1].y != g_player[1].oldY)
        {
            GotoXY(g_player[1].oldX, g_player[1].oldY);
            printf("  ");//이전에 찍힌 서버 플레이어 잔상 지움
        }

        GotoXY(g_player[1].x, g_player[1].y);
        printf("●");//서버 플레이어는 다른 모양으로 출력
        g_player[1].oldX = g_player[1].x;
        g_player[1].oldY = g_player[1].y;

        if (GetAsyncKeyState(VK_ESCAPE))
            break;
    }


    //6.소켓 종료 -> 윈속 종료
    closesocket(hSocket);//서버와 접속하는 데 쓴 소켓 닫음
    WSACleanup();//WSAStartup()과 쌍
    system("pause");
    return 0;
}

/*
int main(void)
{
    WSADATA wsaData;
    SOCKET hSocket;//서버에 연결되고 데이터 주고 받을 소켓
    SOCKADDR_IN servAddr;//클라이언트가 연결할 서버의 ip 주소, 주소 체계, 포트 번호 담아두는 변수

    int s, n, len_in, a;
    int totalCount = 0;
    const char* exitChar = "exit\n";
    //


    //1.윈속 초기화. 2.2 버전으로 설정
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {//이 함수 실행 후 wsaData.wVersion에 0x0202가 들어있는 걸 확인할 수 있음
        printf("Failed WSAStartup()\n");
        return 1;
    }


    //2.소켓 생성. socket() 인자로 소켓 설정
    //PF_INET=IPv4 체계(AF_INET), SOCK_STREAM=TCP, IPPROTO_TCP=TCP 프로토콜
    //ip는 랜 카드의 주소. 이 랜 카드를 통해 프로그램이 작동하는데, 프로그램마다 가진 게 포트
    hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET)
    {
        printf("Failed socket()\n");
        WSACleanup();
        return 1;
    }


    //3.서버 연결
    //3-1.연결할 서버 주소 셋팅
    memset(&servAddr, 0, sizeof(servAddr));//servAddr 쓸 때 오류(bind, connect 안 되는 등) 안 나게 초기화
    servAddr.sin_family = AF_INET;//주소 체계로 IPv4 사용한다고 설정
    //servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//옛날 방식. 현재 이 함수는 컴파일 에러 나서
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);//대신 이 함수 쓴다
    //로컬 ip 썼으니 서버와 클라이언트가 내 컴퓨터 안에서 주고받겠다는 것
    //1번 인자는 주소 체계, 2번 인자는 문자열로된 주소, 3번은 주소 담을 변수
    servAddr.sin_port = htons(30002);//포트 설정. 그냥 숫자만 대입하지 않고  htons() 쓰는 건
    //개인 pc-대형 pc 간 리틀-빅 엔디안 차이 없애려고. host to network short 호출로 빅 엔디안에 맞춤


    //3-2.서버에 연결 시도
    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {//servAddr에 접속하려는 서버 ip, 포트 정보 있음. 이걸 이용해 hSocket 소켓으로 서버에 접속 시도
        printf("Failed connect()\n");
        closesocket(hSocket);
        WSACleanup();
        return 1;
    }//connect()는 블로킹 함수. 연결, 거절, 시간 초과 등 결과 나오기 전까지 진행 막힘


    ////////// 서버와 게임 진행 부분 //////////
    printf("==================================================\n");
    printf("숫자 야구게임을 시작합니다!\n종료하고 싶다면 exit를 입력하고 엔터를 눌러주세요.\n \
        \n0을 제외한 서로 다른 숫자를 3개 입력하세요!\n예로 285를 입력한 후 엔터를 누르시면 됩니다.\n");
    printf("==================================================\n");

    int recvSize = 0;
    int sendSize = 0;
    char recvData[SIZE];//서버와 데이터 주고받을 거라
    char sendData[SIZE];//메시지 버처 2개 준비
    while (true)
    {
        if (fgets(sendData, SIZE - 1, stdin))//1번=읽어올 버퍼, 2번=읽을 개수, 3번=fd
        {//최대 SIZE - 1 - 1 개수만큼 읽고 끝에 널 문자 넣음
            sendSize = strlen(sendData);
        }
        else
        {
            printf("Failed fgets()\n");
            closesocket(hSocket);
            WSACleanup();
            return 1;
        }

        if (!strcmp(sendData, exitChar))
            break;

        if (sendData[0] != '0' && sendData[1] != '0' && sendData[2] != '0' && sendData[0] != sendData[1] \
            && sendData[1] != sendData[2] && sendData[0] != sendData[2])
        {
            if (sendSize == 4 && isdigit(sendData[0]) && isdigit(sendData[1]) && isdigit(sendData[2]))
                send(hSocket, sendData, sendSize, 0);//서버에 전달
            else
            {
                printf("0을 제외한 서로 다른 숫자를 3개 입력하세요!\n");
                continue;
            }
        }

        totalCount++;//제대로 된 숫자가 입력됐으니 아래 진행

    }

    //
    return 0;
}
*/