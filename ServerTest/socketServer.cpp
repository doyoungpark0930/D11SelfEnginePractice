#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define MAXLINE 512
#define PORTNUM 5000

void err_quit(const char* msg)
{
	LPVOID IpMsgBuf;
	FormatMessage( //해당오류에 대해 시스템에서 제공하는 상세 오류 메시지를 가져옴
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)IpMsgBuf);
	LocalFree(IpMsgBuf);
}


int main()
{
	WSADATA wsaData;
	SOCKET sockfd, clientfd;
	int addrlen;
	char buf[MAXLINE + 1];
	int len = 0;
	struct sockaddr_in sockaddr, clientaddr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("dlload error\n");
		return 1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		printf("socket create error\n");
		return 1;
	}

	ZeroMemory(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(PORTNUM); //서버 포트번호 지정
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //모든 네트워크 인터페이스에서 수
	//inet_addr("172.30.1.56");
//htonl(INADDR_ANY); //모든 네트워크 인터페이스에서 수신

	if (::bind(sockfd, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) err_quit("Bind"); //로컬주소를 소켓과 연결
	if (::listen(sockfd, 5) == SOCKET_ERROR) err_quit("Listen");//서버 소켓을 "수신 대기 상태"로 전환


	addrlen = sizeof(clientaddr);
	clientfd = ::accept(sockfd, (SOCKADDR*)&clientaddr, &addrlen); //클라이언트가 서버에 연결할 때까지 대기
	//연결 요청이 오면 이를 수락하고 새로 열린 클라이언트 소켓을 반환

	std::cout << "클라이언트와 연결됨" << std::endl;
	if (clientfd == INVALID_SOCKET)
	{
		err_quit("Accept");
		closesocket(sockfd);
		WSACleanup();
		return 0;
	}
	//inet_ntoa 함수는 (Ipv4) 인터넷 네트워크 주소를 인터넷 표준 점선 소수점 형식의 ASCII 문자열로 변환합니다
	//ntohs는 네트워크 바이트에서 호스트 바이트로 
	printf("[TCP서버] 클라이언트 접속 IP : %s %d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	while (1)
	{
		//데이터받기(고정길이)
		int retVal = recv(clientfd, (char*)&len, sizeof(int), 0);
		if (retVal == SOCKET_ERROR)
		{
			err_quit("Recv");
			break;
		}
		else if (retVal == 0) break;

		//데이터받기(가변길이)
		retVal = recv(clientfd, buf, len, 0);
		if (retVal == SOCKET_ERROR)
		{
			err_quit("Recv");
			break;
		}
		else if (retVal == 0) break;

		//받은 데이터 출력
		buf[retVal] = '\0';
		printf("[TCP/%s : %d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
	}

	closesocket(sockfd);
	printf("소켓 통신 종료");
	WSACleanup();
	return 0;
}
