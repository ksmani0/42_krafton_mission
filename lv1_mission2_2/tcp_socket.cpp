// tcp_socket.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <winsock2.h>//winsok은 TCP/IP 프로그래밍을 위한 API
#include <WS2tcpip.h>
#include <time.h>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")//자동으로 라이브러리 링크

#define SIZE 256

using namespace std;

#define SIZE 128
#define NUM 2
#define WIDE 80
#define HEIGHT 36

typedef struct s_player
{
    int x = 0;
    int y = 0;
    int oldX = 0;
    int oldY = 0;
} t_player;

t_player g_player[NUM];//0=client, 1=server. 얘네가 프로토콜의 type

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

void MoveServerPlayer()
{
    while (true)
    {
        if (g_player[1].x != g_player[1].oldX || g_player[1].y != g_player[1].oldY)
        {
            GotoXY(g_player[1].oldX, g_player[1].oldY);
            printf("  ");//화면에 출력하는 함수
        }
        GotoXY(g_player[1].x, g_player[1].y);
        printf("●");//화면에 출력하는 함수
        g_player[1].oldX = g_player[1].x;
        g_player[1].oldY = g_player[1].y;

        if (GetAsyncKeyState(VK_LEFT) & 0x8000)
            g_player[1].x = g_player[1].x == 0 ? 0 : g_player[1].x - 1;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
            g_player[1].x = g_player[1].x == WIDE ? WIDE : g_player[1].x + 1;
        if (GetAsyncKeyState(VK_UP) & 0x8000)
            g_player[1].y = g_player[1].y == g_heightLimit ? g_heightLimit : g_player[1].y - 1;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
            g_player[1].y = g_player[1].y == HEIGHT ? HEIGHT : g_player[1].y + 1;

        Sleep(65);
        if (GetAsyncKeyState(VK_END) & 0x8000)
            break;//이 키 누르면 서버가 보낼 차례
    }    
}

int main(void)
{
    WSADATA wsaData;
    SOCKET listenSocket;//클라이언트의 접속 시도를 받는 소켓
    SOCKADDR_IN servAddr;//서버의 ip 주소나 주소 체계, 포트 번호 담아두는 변수


    //0.콘솔창 제목, 크기, 커서 표시 설정
    char setText[SIZE];

    SetConsoleTitle(TEXT("seungmki's window: Server"));//char*가 아닌 wchar*를 인자로 받아서 매크로 TEXT() 사용
    sprintf_s(setText, "mode con cols=%d lines=%d", WIDE, HEIGHT);
    system(setText);//콘솔창 크기 변경하는 법
    SetCursor(false);//커서 감추기

    //1.윈속 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Failed WSAStartup() \n");
        return 1;
    }


    //2.클라이언트의 접속 시도를 받는 소켓 생성
    listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //클라이언트와 데이터 주고받을 때 쓰는 소켓 아님! 딱 connect()의 접속 시도만 받는다
    if (listenSocket == INVALID_SOCKET)
    {
        WSACleanup();
        printf("Failed socket() \n");
        return 1;
    }


    //3.서버 자신의 주소를 설정
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY=자동으로 이 컴퓨터에 존재하는 랜카드 중 사용 가능한 랜카드의 IP 쓰라는 것
    //실제로 선언된 매크로로 가보면 0x00000000로 돼있음. 0.0.0.0로 선언한 것
    //이 주소 선언하면 자기 호스트로 들어오는 패킷 무조건 수신한다는 뜻. 그래서 127.0.0.1로 접속 시도하는 클라이언트와 연결될 수 있음
    //inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);는 옛날에 쓰던 방식
    servAddr.sin_port = htons(30002);//서버에서 이렇게 특정 포트를 정해줘서 클라이언트가 이 포트로 접속
    if (bind(listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {//클라이언트 접속 시도 받을 소켓과 서버의 주소 정보 연결해줌
        printf("Binding Error\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    if (listen(listenSocket, 5) == SOCKET_ERROR)//2번 인자는 연결 대기열 크기로 연결할 수 있는 클라이언트 수.
    {//이 함수로 소켓이 클라이언트로부터 접속을 받을 수 있는 소켓됨
        printf("listen Error \n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    printf("Server Start!\n");//이제 제대로 클라이언트와 연결될 수 있는 서버 준비 완료

       
    SOCKADDR_IN clientAddr;//클라이언트 정보 담아둘 구조체 변수
    SOCKET clientSocket;//이게 클라이언트와 데이터 주고받을 소켓
    int sizeClientAddr = sizeof(clientAddr);
    char sendData[SIZE] = "Hi! Shall we start?";
    char recvData[SIZE];
    int recvByte;


    //4.접속 시도한 클라이언트와 연결 및 통신 처리
    //4-1.클라이언트 연결 기다리기
    clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &sizeClientAddr);
    //accept()도 블로킹 함수. listenSocket 소켓으로 클라이언트의 connect()를 기다림
    //클라이언트의 connect를 받으면 이 클라이언트의 ip, 포트를 clientAddr에 대입
    //반환값으로 나온 새로운 소켓(clientSocket)이 바로 서버가 갖는 클라이언트 소켓
    //이 clientSocket와 클라이언트의 connect에 사용된 소켓이 연결돼 데이터 주고받음
    if (clientSocket == INVALID_SOCKET)
    {
        printf("Failed connect()\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server : Client connected\nClient : %d", clientSocket);//클라이언트와 연결된 소켓 정보 출력
    

    //4-2.연결된 클라이언트에 첫 메시지 send
    send(clientSocket, sendData, strlen(sendData) + 1, 0);

    ////////// 클라이언트와 함께 플레이어 이동 부분 //////////
    printf("\n==========서버와 함께 플레이어들을 움직여 보자!==========\n");
    GetXY(&(g_player[1].x), &(g_player[1].y));
    g_heightLimit = g_player[1].y;//여기가 최대로 올라갈 수 있는 행
    g_player[1].oldX = g_player[1].x;
    g_player[1].oldY = g_player[1].y;
    g_player[0].y = g_heightLimit;
    g_player[0].oldY = g_heightLimit;

    while (true)
    {
        if ((recvByte = recv(clientSocket, recvData, SIZE, 0)) < 0)
        {//클라이언트가 보낸 패킷을 recvData에 담음
            printf("Failed recv()\n");
            closesocket(clientSocket);
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        g_player[0].x = recvData[0];
        g_player[0].y = recvData[1];
        if (g_player[0].x != g_player[0].oldX || g_player[0].y != g_player[0].oldY)
        {
            GotoXY(g_player[0].oldX, g_player[0].oldY);
            printf("  ");//이전에 찍힌 클라이언트 플레이어 잔상 지움
        }

        GotoXY(g_player[0].x, g_player[0].y);
        printf("○");//화면에 클라이언트 플레이어 출력
        g_player[0].oldX = g_player[0].x;
        g_player[0].oldY = g_player[0].y;
        
        MoveServerPlayer();
        sendData[0] = g_player[1].x;
        sendData[1] = g_player[1].y;
        send(clientSocket, sendData, strlen(sendData) + 1, 0);
        //write(clientSocket, sendData, strlen(sendData));

        if (GetAsyncKeyState(VK_ESCAPE))
            break;
    }


    //5.서버의 소켓과 윈속 종료
    closesocket(clientSocket);
    //tcp/ip는 연결 지속 방식이라 한번 connect되면 지속적으로 데이터 주고 받음
    //연결 종료면 send, recv가 안 되니 접속 유지 여부 판단 기중 중 하나가 됨
    closesocket(listenSocket);//클라이언트와의 연결 소켓 clientSocket부터 먼저 닫고
    //클라이언트의 접속 시도 받는 데 쓴 소켓도 닫음
    WSACleanup();

    return 0;
}
