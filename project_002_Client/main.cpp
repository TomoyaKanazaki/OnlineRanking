//==========================================
//
//  �N���C�A���g�v���O����(main.cpp)
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
void Uninit(SOCKET *sockClient); //�I������
struct sockaddr_in SetAddress(SOCKET sockClient); //�ڑ�����
void Transceiving(SOCKET sockClient); //����M����

//==========================================
//  ���C���֐�
//==========================================
int main(void)
{
	//���[�J���ϐ��錾
	WSADATA wsaData; //�\�P�b�g�̎d�l���
	SOCKET sockClient; 

	//����������
	Init(&wsaData, &sockClient);

	//�ڑ�����
	struct sockaddr_in addr = SetAddress(sockClient);

	//����M����
	Transceiving(sockClient);

	//�I������
	Uninit(&sockClient);

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
//=========================================
void Uninit(SOCKET *sockClient)
{
	//�\�P�b�g�����
	closesocket(*sockClient);

	//WinSock�̏I��
	WSACleanup();
}

//==========================================
//  �ڑ�����
//==========================================
struct sockaddr_in SetAddress(SOCKET sockClient)
{
	//���[�J���ϐ��錾
	struct sockaddr_in addr;
	const char *pIPAddress = "127.0.0.1";

	//�ڑ���̐ݒ�
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUM);
	addr.sin_addr.S_un.S_addr = inet_addr(pIPAddress);

	//�ڑ�����
	if (connect(sockClient, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		//�G���[���b�Z�[�W
		printf("<< �T�[�o�[�ւ̐ڑ��Ɏ��s >>\n");

		//�L�[���͑҂�
		printf("\n<< Enter >>");
		getchar();
	}
	else
	{
		printf("<< �T�[�o�[�ւ̐ڑ��ɐ��� >>\n");
	}

	//�l��Ԃ�
	return addr;
}

//==========================================
//  ����M����
//==========================================
void Transceiving(SOCKET sockClient)
{
	//���[�J���ϐ��錾
	int nData;
	char aData[4];
	char aRecvData[4] = {};

	//���l�����
	printf("\n���M���鐔�l����� >> ");
	scanf("%d", &nData);
	rewind(stdin);

	//�f�[�^�̑��M
	memcpy(&aData[0], &nData, sizeof(int));
	send(sockClient, &aData[0], sizeof(int), 0);

	printf("���M�������l >> %d\n", nData);

	//�f�[�^�̎�M
	int nRecvByte = recv(sockClient, &aRecvData[0], sizeof(aRecvData), 0);
	memcpy(&nData, &aRecvData[0], sizeof(int));

	printf("��M�������l >> %d\n", nData);
}
