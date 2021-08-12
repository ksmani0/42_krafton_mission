// echoServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <winsock2.h>//winsok은 TCP/IP 프로그래밍을 위한 API
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")//자동으로 라이브러리 링크

int main(void)//서버 소스
{//클라이언트가 종료되면 해당 클라이언트 처리 끝내고 다시 accept()로 대기하게 구현
    WSADATA wsaData;//접속 연결 대기 -> 접속 연결 -> 처리(recv한 data 그대로 send)
    SOCKET listenSocket;// -> 해당 소켓 종료 시 -> 접속 연결 대기(반복)
    SOCKADDR_IN servAddr;

    //1.윈속 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Failed WSAStartup() \n");
        return 1;
    }

    //2.소켓 생성
    listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //클라이언트의 접속 시도 기다릴 소켓 생성. 클라이언트와 데이터
    //주고받을 때 쓰는 소켓 아님! 딱 connect()의 접속 시도만 받는다
    if (listenSocket == INVALID_SOCKET)
    {
        WSACleanup();
        printf("Failed socket() \n");
        return 1;
    }

    //3.서버 자신의 주소를 셋팅
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //INADDR_ANY=자동으로 이 컴퓨터에 존재하는 랜카드 중 사용가능한 랜카드의 IP 쓰라는 것
    //실제로 선언된 매크로로 가보면 0x00000000로 돼있음. 0.0.0.0로 선언한 것
    //이 주소 선언하면 자기 호스트로 들어오는 패킷 무조건 수신한다는 뜻
    //그래서 127.0.0.1로 접속 시도하는 클라이언트와 연결될 수 있음
    //inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
    servAddr.sin_port = htons(30002);//서버에서 이렇게 특정 포트를 정해줘서 클라이언트가 이 포트로 접속
    if (bind(listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {//클라이언트 접속 시도 받을 소켓과 서버의 주소 정보 연결해줌
        printf("Binding Error \n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    if (listen(listenSocket, 5) == SOCKET_ERROR)
    {//이 함수로 소켓이 클라이언트로부터 접속을 받을 수 있는 소켓됨
        printf("listen Error \n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server Start \n");

    SOCKADDR_IN clientAddr;
    SOCKET clientSocket;//이게 클라이언트와 데이터 주고받을 소켓
    int sizeClientAddr = sizeof(clientAddr);
    char sendData[255] = "Hi";
    char recvByte;
    char recvData[255];
    int recvCount = 0;

    //4.접속 시도된 클라이언트 연결과 통신 처리
    while (1)
    {
        //4-1.클라이언트 연결 기다리기
        clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &sizeClientAddr);
        //accept()도 블로킹 함수. listenSocket 소켓으로 클라이언트의 connect()를 기다림
        //클라이언트의 connect를 받으면 이 클라이언트의 ip, 포트를 clientAddr에 대입
        //반환값으로 나온 새로운 소켓(여기서는 clientSocket)이 바로 서버가 갖는 클라이언트 소켓
        //이 clientSocket와 클라이언트의 connect에 사용된 소켓이 연결돼 데이터 주고받음
        if (clientSocket == INVALID_SOCKET) printf("Failed connect() \n");
        recvCount = 0;

        printf("accept client socket: %d \n", clientSocket);
        printf("%d.%d.%d.%d : %d \n", clientAddr.sin_addr.S_un.S_un_b.s_b1,
            clientAddr.sin_addr.S_un.S_un_b.s_b2, clientAddr.sin_addr.S_un.S_un_b.s_b3,
            clientAddr.sin_addr.S_un.S_un_b.s_b4, clientAddr.sin_port);
        //연결된 클라이언트의 ip와 포트 출력. 클라이언트 껐다 켤 때마다 포트 달라지는 거 확인 가능

        //4-2.연결된 클라이언트에 send
        send(clientSocket, sendData, strlen(sendData) + 1, 0);
        //누가 먼저 보내든 상관 없다. 여기선 서버가 먼저 보냄

        //4-3.접속한 클라이언트가 접속 끊을 때까지 recv 대기 반복
        printf("recv count: ");
        while ((recvByte = recv(clientSocket, recvData, sizeof(recvData), 0)) > 0)
        {//클라이언트가 접속을 정상적으로 끊으면 recv() 반환값=0으로 반복문 탈출
            printf("[%d]%d ", ++recvCount, recvData[0]);
            send(clientSocket, sendData, strlen(sendData) + 1, 0);
        }//받은 걸 그대로 보냄

        //4-4.접속이 끊긴 처리
        printf("\n");//tcp/ip는 연결 지속 방식이라 한번 connect되면 지속적으로
        closesocket(clientSocket);//데이터 주고 받음. 연결 종료면 send, recv가 안 되니
        printf("closed socket: %d \n", clientSocket);//접속 유지 여부 판단 기중 중 하나가 되는 것
    }

    //5.소켓 종료 -> 윈속 종료
    closesocket(listenSocket);//클라이언트와의 연결 소켓 clientSocket부터 먼저 닫고
    //클라이언트의 접속 시도 받는 데 쓴 소켓도 닫음
    WSACleanup();
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
