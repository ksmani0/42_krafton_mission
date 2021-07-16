// socket_test.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

//#include <iostream>
#include <stdio.h>
#include <winsock2.h>//winsok은 TCP/IP 프로그래밍을 위한 API
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")//자동으로 라이브러리 링크

int main()//클라이언트 소스
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    //1.윈속 초기화. 2.2 버전으로 설정
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) printf("Failed WSAStartup() \n");
    //이 함수 실행 후 wsaData.wVersion에 0x0202가 들어있는 걸 확인할 수 있음

    //2.소켓 생성. socket() 인자로 소켓 설정
    hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//PF_INET=IPv4 체계(AF_INET), SOCK_STREAM=TCP, IPPROTO_TCP=TCP 프로토콜
    //ip는 랜 카드의 주소. 이 랜 카드를 통해 프로그램이 작동하는데, 프로그램이 가지고 있는 게 포트임
    if (hSocket == INVALID_SOCKET) printf("Failed WSAStartup() \n");//소켓 핸들 유효 여부 검사

    //3.서버 연결
    //3-1.연결할 서버 주소 셋팅
    memset(&servAddr, 0, sizeof(servAddr));//servAddr 쓸 때 오류(bind, connect 안 되는 등) 안 나게 0으로 초기화
    servAddr.sin_family = AF_INET; // 주소 체계로 IPv4를 사용한다고 셋팅. 흔히 쓰는 201.128.xxx.xxx 식의 ip를 쓴다는 것
    //servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//옛날 체계. 이 함수는 이제 컴파일 에러 나서 대신 inet_pton() 쓰면 된다.
    //로컬 ip 127.0.0.1 쓴다는 건 본인 컴퓨터 안에서 처리한다는 뜻
    //참고로 127.0.0.1이 전화번호에 해당. 192 같은 가상 ip나 210로 시작하는 걸 쓰는 경우도 있다
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);//이걸로 문자열로 적은 걸 주소로 바꿔준다. 결국 ip 넣어주는 함수
    servAddr.sin_port = htons(30002);//port를 설정해주는 부분. 근데 왜 htons() 넣어서 대입할까?
    //보통 사람들이 쓰는 건 개인용 pc. 근데 네이버 같은 기업은 대형 pc. 두 pc간 cpu가 다르기 때문에 엔디안이 다름
    //인텔은 리틀엔디안, 대형 pc는 보통 빅엔디안. 대형 pc 기준에 맞춰 차이 없애게 host to network short() 호출

    //3-2.서버에 연결 시도. 현재 구조체 servAddr에는 연결할 ip, port 들어있음
    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        printf("Failed connect() \n");
    //connect()는 블로킹 함수라서 scanf()처럼 처리될 때까지 진행이 멈춤. 만약 서버가 꺼져 있으면?
    //connect()은 연결 시도하다 연결할 수 없다고 뜬다. 결국 연결, 거절, 시간 초과 등 결과 나와야 아래 코드 진행

    //4.서버로부터 통신 대기
    int recvSize;//여기서부터는 서버와 연결됐음. 서버가 send()하게 됨
    char recvData[255];//그래서 클라이언트는 recv()를 하게 됨
    //recv()도 블로킹 함수. 성공, 실패, 종료 중 하나 결정돼야 함. 동기함수인 것
    recvSize = recv(hSocket, recvData, sizeof(recvData), 0);
    //hSocket 소켓으로 recv하겠다, recvData는 recv받을 버퍼 크기
    if (recvSize == -1) printf("recv() Error \n");

    printf("recv %d message: %s \n", recvSize, recvData);

    //5.소켓 종료 -> 윈속 종료
    closesocket(hSocket);//위에서 호출한 socket()과 쌍. 이걸로 소켓 닫는다
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
