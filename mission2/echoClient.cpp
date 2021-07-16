// echoClient.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <winsock2.h>//winsok은 TCP/IP 프로그래밍을 위한 API
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")//자동으로 라이브러리 링크

int main(void)//클라이언트 소스
{//클라이언트 실행 -> 연결 -> send 후 같은 내용 recv. 이걸 5번 반복 -> 실행 종료(이 과정 반복)
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    //1.윈속 초기화. 2.2 버전으로 설정
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {//이 함수 실행 후 wsaData.wVersion에 0x0202가 들어있는 걸 확인할 수 있음
        printf("Failed WSAStartup() \n");
        return 1;
    }

    //2.소켓 생성. socket() 인자로 소켓 설정
    //PF_INET=IPv4 체계(AF_INET), SOCK_STREAM=TCP, IPPROTO_TCP=TCP 프로토콜
    //ip는 랜 카드의 주소. 이 랜 카드를 통해 프로그램이 작동하는데, 프로그램마다 가진 게 포트
    hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET)
    {
        printf("Failed socket() \n");
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
    servAddr.sin_port = htons(30002);//포트 설정. 그냥 숫자만 대입하지 않고  htons() 쓰는 건
    //개인 pc-대형 pc 간 리틀-빅 엔디안 차이 없애려고. host to network short 호출로
    //빅 엔디안에 맞춤

    //3-2.서버에 연결 시도
    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {//servAddr에 접속하려는 서버 ip, 포트 정보 있음. 이걸 이용해 hSocket 소켓으로 서버에 접속 시도
        printf("Failed connect() \n");
        closesocket(hSocket);
        WSACleanup();
        return 1;
    }//connect()는 블로킹 함수. 연결, 거절, 시간 초과 등 결과 나오기 전까지 진행 막힘

    //4.서버로부터 통신 대기. 여기서부터 서버와 접속됨
    int recvSize;
    char recvData[255];//서버와 데이터 주고받을 거라
    char sendData[255];//메시지 버처 2개 준비
    
    //서버가 우선 send()하게 짰으니 클라이언트는 recv()로 받을 준비
    recvSize = recv(hSocket, recvData, sizeof(recvData), 0);
    //recv()도 블로킹 함수. 성공, 실패, 종료 중 하나 결정돼야 다음 진행됨. 동기 함수.
    //hSocket 소켓으로 recv하겠다, recvData는 recv받을 버퍼
    if (recvSize == -1) printf("recv() Error \n");
    printf("recv %d message: %s \n", recvSize, recvData);//전달 받은 메시지 출력

    //5.연결된 서버와 통신
    int count = 5;
    while(count--)
    {//서버와 데이터 통신 5회 반복
        sendData[0] = count;
        send(hSocket, sendData, 1, 0);//이번엔 클라이언트가 보낸다
        printf("send data: %d \n", sendData[0]);
        recvSize = recv(hSocket, recvData, sizeof(recvData), 0);
        if (recvSize == SOCKET_ERROR)//서버가 보낸 같은 메시지 받아서 출력
        {
            printf("recv() Error \n");
            break;
        }
        printf("recv Data: ");
    }

    //6.소켓 종료 -> 윈속 종료
    closesocket(hSocket);//서버와 접속하는 데 쓴 소켓 닫음
    WSACleanup();//WSAStartup()과 쌍
    system("pause");
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
