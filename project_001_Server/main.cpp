//==========================================
//
//  �T�[�o�[�v���O����(main.cpp)
//  Author : Tomoya Kanazaki
//
//==========================================
#include <stdio.h>
#include <winsock2.h>
#include "main.h"

#pragma comment (lib, "ws2_32.lib")

//==========================================
//  �}�N����`
//==========================================
#define PORT_NUM (22333) // �|�[�g�ԍ�
#define JOIN_MAX (10) // �ڑ��ő吔

//==========================================
//  �v���g�^�C�v�錾
//==========================================
void Init(WSADATA *wsaData, SOCKET *sock); //����������
void Uninit(SOCKET *sockServer, SOCKET *sock); //�I������
SOCKET SetAddress(SOCKET sock); //�ڑ�����
void Transceiving(SOCKET sockServer); //����M����

//==========================================
//  ���C���֐�
//==========================================
int main(void)
{
	//���[�J���ϐ��錾
	WSADATA wsaData; //�\�P�b�g�̎d�l���
	SOCKET sock;
	
	//����������
	Init(&wsaData, &sock);
	
	//�ڑ�����
	SOCKET sockServer = SetAddress(sock);

	//����M����
	Transceiving(sockServer);

	//�I������
	Uninit(&sockServer, &sock);

	//�L�[���͑҂�
	printf("\n<< Enter >>");
	getchar();
	return 0;
}

//==========================================
//  ����������
//==========================================
void Init(WSADATA *wsaData, SOCKET *sock)
{
	//WinSock�̏�����
	int nErr = WSAStartup(WINSOCK_VERSION, wsaData);

	//NULL�`�F�b�N
	if (nErr != 0)
	{
		//�G���[���b�Z�[�W
		printf("<< �������Ɏ��s >>\n");

		//�L�[���͑҂�
		printf("\n<< Enter >>");
		getchar();
	}
	else
	{
		printf("<< �������ɐ��� >>\n");
	}

	//�\�P�b�g�쐬
	*sock = socket(AF_INET, SOCK_STREAM, 0);

	//NULL�`�F�b�N
	if (*sock == INVALID_SOCKET)
	{
		//�G���[���b�Z�[�W
		printf("<< �\�P�b�g�̍쐬�Ɏ��s >>\n");

		//�L�[���͑҂�
		printf("\n<< Enter >>");
		getchar();
	}
	else
	{
		printf("<< �\�P�b�g�̍쐬�ɐ��� >>\n");
	}
}

//==========================================
//  �I������
//==========================================
void Uninit(SOCKET *sockServer, SOCKET *sock)
{
	//�N���C�A���g�Ƃ̐ڑ������
	closesocket(*sockServer);

	//�ڑ���t�p�\�P�b�g�����
	closesocket(*sock);

	//WinSock�̏I��
	WSACleanup();
}

//==========================================
//  �ڑ�����
//==========================================
SOCKET SetAddress(SOCKET sock)
{
	// �|�[�g�ԍ�,IP�^�C�v�̓���
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUM);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(sock, (struct sockaddr*)&addr, sizeof(addr)); // �|�[�g�̊��蓖��
	listen(sock, JOIN_MAX); // �ڑ���t�̏���

	//�ڑ��҂�
	printf("<< �N���C�A���g����̐ڑ���҂��Ă��܂� >>\n");
	struct sockaddr_in addrClient;
	int nLengthAddr = sizeof(addrClient);
	SOCKET sockServer = accept(sock, (struct sockaddr*)&addrClient, &nLengthAddr);
	printf("<< �N���C�A���g����̐ڑ����m�F >>\n");

	//�l��Ԃ�
	return sockServer;
}

//==========================================
void Transceiving(SOCKET sockServer)
{
	//�f�[�^�̎�M
	char aRecvData[4] = {};
	int nRecvByte = recv(sockServer, &aRecvData[0], sizeof(aRecvData), 0);
	int nData = 0;
	memcpy(&nData, &aRecvData[0], sizeof(int));

	printf("��M�������l >> %d\n", nData);

	//�f�[�^�̑��M
	nData *= 2;
	char aData[4];
	memcpy(&aData[0], &nData, sizeof(int));
	send(sockServer, &aData[0], sizeof(int), 0);

	printf("���M�������l >> %d\n", nData);
}
