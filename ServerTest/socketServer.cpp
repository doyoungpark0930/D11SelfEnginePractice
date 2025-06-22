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
	FormatMessage( //�ش������ ���� �ý��ۿ��� �����ϴ� �� ���� �޽����� ������
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
	sockaddr.sin_port = htons(PORTNUM); //���� ��Ʈ��ȣ ����
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //��� ��Ʈ��ũ �������̽����� ��
	//inet_addr("172.30.1.56");
//htonl(INADDR_ANY); //��� ��Ʈ��ũ �������̽����� ����

	if (::bind(sockfd, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) err_quit("Bind"); //�����ּҸ� ���ϰ� ����
	if (::listen(sockfd, 5) == SOCKET_ERROR) err_quit("Listen");//���� ������ "���� ��� ����"�� ��ȯ


	addrlen = sizeof(clientaddr);
	clientfd = ::accept(sockfd, (SOCKADDR*)&clientaddr, &addrlen); //Ŭ���̾�Ʈ�� ������ ������ ������ ���
	//���� ��û�� ���� �̸� �����ϰ� ���� ���� Ŭ���̾�Ʈ ������ ��ȯ

	std::cout << "Ŭ���̾�Ʈ�� �����" << std::endl;
	if (clientfd == INVALID_SOCKET)
	{
		err_quit("Accept");
		closesocket(sockfd);
		WSACleanup();
		return 0;
	}
	//inet_ntoa �Լ��� (Ipv4) ���ͳ� ��Ʈ��ũ �ּҸ� ���ͳ� ǥ�� ���� �Ҽ��� ������ ASCII ���ڿ��� ��ȯ�մϴ�
	//ntohs�� ��Ʈ��ũ ����Ʈ���� ȣ��Ʈ ����Ʈ�� 
	printf("[TCP����] Ŭ���̾�Ʈ ���� IP : %s %d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	while (1)
	{
		//�����͹ޱ�(��������)
		int retVal = recv(clientfd, (char*)&len, sizeof(int), 0);
		if (retVal == SOCKET_ERROR)
		{
			err_quit("Recv");
			break;
		}
		else if (retVal == 0) break;

		//�����͹ޱ�(��������)
		retVal = recv(clientfd, buf, len, 0);
		if (retVal == SOCKET_ERROR)
		{
			err_quit("Recv");
			break;
		}
		else if (retVal == 0) break;

		//���� ������ ���
		buf[retVal] = '\0';
		printf("[TCP/%s : %d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
	}

	closesocket(sockfd);
	printf("���� ��� ����");
	WSACleanup();
	return 0;
}
