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
int GetOrder(const int nScore); //���ʎ擾����
int Descending(const void * n0, const void * n1); //�\�[�g�p

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

	//����̏��ʂ��擾����
	nData = GetOrder(nData);

	//�f�[�^�̑��M
	char aData[4];
	memcpy(&aData[0], &nData, sizeof(int));
	send(sockServer, &aData[0], sizeof(int), 0);

	printf("���M�������l >> %d\n", nData);
}

//==========================================
//  ���ʎ擾����
//==========================================
int GetOrder(const int nScore)
{
	//���[�J���ϐ��錾
	FILE *pFile;
	int nOrder = 0; //����
	int nNumData = 0; //�g�p����f�[�^��
	int *apRanking = NULL;

	//�t�@�C�����J��
	pFile = fopen("Ranking.bin", "rb");

	//NULL�`�F�b�N
	if (pFile != NULL)
	{
		//�ۑ�����Ă���f�[�^�����擾����
		fread(&nNumData, sizeof(int), 1, pFile);

		//(�ۑ�����Ă���f�[�^�� + 1) * 4�o�C�g�̃��������m�ۂ���
		if (apRanking == NULL)
		{
			apRanking = new int[nNumData + 1];
		}
		else
		{
			printf("�����L���O�f�[�^�p�̃��������m�ۂł��܂���ł���\n");
		}

		//���������m�ۂ�����ۑ�����Ă���f�[�^���擾����
		if (apRanking != NULL)
		{
			fread(apRanking, sizeof(int), nNumData, pFile);
		}

		//�t�@�C�������
		fclose(pFile);
	}
	else
	{
		printf("�����L���O�f�[�^�̓ǂݍ��݂Ɏ��s\n");
	}

	//NULL�`�F�b�N
	if (apRanking != NULL)
	{
		//�Ō�̃������Ɏ�M�����f�[�^��ۑ�����
		apRanking[nNumData] = nScore;

		//�ۑ�����Ă���f�[�^����1���₷
		nNumData++;

		//�����L���O�f�[�^���\�[�g����
		qsort(apRanking, nNumData, sizeof(int), Descending);

		//��M�����X�R�A�̏��ʂ��擾����
		for (int nCnt = 0; nCnt < nNumData; nCnt++)
		{
			if (apRanking[nCnt] == nScore)
			{
				nOrder = nCnt + 1;
				break;
			}
		}
	}

	//�t�@�C�����J��
	pFile = fopen("Ranking.bin", "wb");

	//NULL�`�F�b�N
	if (pFile != NULL)
	{
		//NULL�`�F�b�N
		if (apRanking != NULL)
		{
			//�ۑ�����f�[�^����ۑ�����
			fwrite(&nNumData, sizeof(int), 1, pFile);

			//�����L���O��ۑ�����
			fwrite(apRanking, sizeof(int), nNumData, pFile);

			//�t�@�C�������
			fclose(pFile);
		}
	}
	else
	{
		printf("�����L���O�f�[�^�̏����o���Ɏ��s\n");
	}

	//���������J������
	if (apRanking != NULL)
	{
		delete[] apRanking;
		apRanking = NULL;
	}

	//���ʂ�Ԃ�
	return nOrder;
}

//==========================================
//  �\�[�g�p
//==========================================
int Descending(const void * n0, const void * n1)
{
	//n0��n1�̒l���r�������ʂɂ���ĕԂ�l�����肷��
	if (*(int *)n0 < *(int *)n1)
	{
		return 1;
	}
	else if (*(int *)n0 > *(int *)n1)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
