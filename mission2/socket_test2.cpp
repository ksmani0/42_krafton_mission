// socket_test2.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <winsock2.h>//winsok은 TCP/IP 프로그래밍을 위한 API
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")//자동으로 라이브러리 링크

int main()//서버 소스. 3번, 4번 항목이 클라이언트와 다르다
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    //1.윈속 초기화. 2.2 버전으로 설정
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) printf("Failed WSAStartup() \n");

    //2.소켓 생성. socket() 인자로 소켓 설정
    hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) printf("Failed WSAStartup() \n");

    //3.서버 자신의 주소 셋팅
    //3-1.서버 주소 셋팅
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
    servAddr.sin_port = htons(30002);//서버에서 특정 포트를 지정해줬기 때문에 클라이언트가 이 포트로 들어온다
    if (bind(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)//소켓과 주소 연결해줌
        printf("Binding Error \n");
    if (listen(hSocket, 5) == SOCKET_ERROR)//이 함수로 소켓이 클라이언트로부터 접속을 받을 수 있는 소켓이 됨
        printf("listen Error \n");//앞서 클라이언트에서 만든 소켓은 접속이 목적이었음. 반변 서버에서는
    //bind(), listen()을 통해 받기 위한 소켓을 만듦
    //인자의 5는 동시에 처리할 수 있는 숫자. 연결 큐의 최대 길이

    //3-2.클라이언트 연결 기다리기
    SOCKADDR_IN clientAddr;
    SOCKET clientSocket;
    int sizeClientAddr = sizeof(clientAddr);

    clientSocket = accept(hSocket, (SOCKADDR*)&clientAddr, &sizeClientAddr);
    if (clientSocket == SOCKET_ERROR)//accept()를 통해 클라이언트를 기다린다. 이 함수도 블로킹 함수
        printf("Failed connect() \n");//클라이언트의 connect를 받으면 이 클라이언트의 ip, 포트를 clientAddr에 대입함
    //이후 반환값으로 소켓이 하나 더 나옴. 여기서는 clientSocket. 이게 바로 서버가 갖는 클라이언트 소켓
    //이 clientSocket와 클라이언트의 connect에 사용된 소켓이 연결됨

    //4.클라이언트에게 send. 사실 클라이언트가 먼저 send 해도 됨. 누가 먼저 하든 상관 없음
    char sendData[255] = "hello";
    send(clientSocket, sendData, strlen(sendData) + 1, 0);//클라이언트로 send하면
    //버퍼로 hello가 전송됨. 그럼 send()는 거의 바로 자기 작업 끝내고 다음 코드 실행됨

    //5.소켓 종료 -> 윈속 종료
    closesocket(clientSocket);//클라이언트와 연결하는 데 쓴 소켓부터 먼저 닫음
    closesocket(hSocket);//그 다음 서버의 소켓을 닫음
    WSACleanup();//윈속 종료
    //이후 서버는 빠져나옴. 그럼 클라이언트는 받은 걸 처리하고 빠져나옴
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
