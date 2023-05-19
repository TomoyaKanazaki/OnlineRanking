//==========================================
//
//  クライアントプログラム(main.cpp)
//  Author : Tomoya Kanazaki
//
//==========================================
#include <stdio.h>
#include <winsock2.h>
#include "main.h"

#pragma comment (lib, "ws2_32.lib")

//==========================================
//  マクロ定義
//==========================================
#define PORT_NUM (22333) // ポート番号
#define JOIN_MAX (10) // 接続最大数

//==========================================
//  プロトタイプ宣言
//==========================================
void Init(WSADATA *wsaData, SOCKET *sock); //初期化処理
void Uninit(SOCKET *sockClient); //終了処理
struct sockaddr_in SetAddress(SOCKET sockClient); //接続処理
void Transceiving(SOCKET sockClient); //送受信処理
char *GetIPAdddress(void); //IPアドレス取得処理

//==========================================
//  メイン関数
//==========================================
int main(void)
{
	//ローカル変数宣言
	WSADATA wsaData; //ソケットの仕様情報
	SOCKET sockClient; 

	//初期化処理
	Init(&wsaData, &sockClient);

	//接続処理
	struct sockaddr_in addr = SetAddress(sockClient);

	//送受信処理
	Transceiving(sockClient);

	//終了処理
	Uninit(&sockClient);

	//キー入力待ち
	printf("\n<< Enter >>");
	getchar();
	return 0;
}

//==========================================
//  初期化処理
//==========================================
void Init(WSADATA *wsaData, SOCKET *sock)
{
	//WinSockの初期化
	int nErr = WSAStartup(WINSOCK_VERSION, wsaData);

	//NULLチェック
	if (nErr != 0)
	{
		//エラーメッセージ
		printf("<< 初期化に失敗 >>\n");

		//キー入力待ち
		printf("\n<< Enter >>");
		getchar();
	}
	else
	{
		printf("<< 初期化に成功 >>\n");
	}

	//ソケット作成
	*sock = socket(AF_INET, SOCK_STREAM, 0);

	//NULLチェック
	if (*sock == INVALID_SOCKET)
	{
		//エラーメッセージ
		printf("<< ソケットの作成に失敗 >>\n");

		//キー入力待ち
		printf("\n<< Enter >>");
		getchar();
	}
	else
	{
		printf("<< ソケットの作成に成功 >>\n");
	}
}

//==========================================
//  終了処理
//=========================================
void Uninit(SOCKET *sockClient)
{
	//ソケットを閉じる
	closesocket(*sockClient);

	//WinSockの終了
	WSACleanup();
}

//==========================================
//  接続処理
//==========================================
struct sockaddr_in SetAddress(SOCKET sockClient)
{
	//ローカル変数宣言
	struct sockaddr_in addr;
	const char *pIPAddress = GetIPAdddress();
	
	//接続先の設定
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUM);
	addr.sin_addr.S_un.S_addr = inet_addr(pIPAddress);

	//接続する
	if (connect(sockClient, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		//エラーメッセージ
		printf("<< サーバーへの接続に失敗 >>\n");

		//キー入力待ち
		printf("\n<< Enter >>");
		getchar();
	}
	else
	{
		printf("<< サーバーへの接続に成功 >>\n");
	}

	//値を返す
	return addr;
}

//==========================================
//  送受信処理
//==========================================
void Transceiving(SOCKET sockClient)
{
	//ローカル変数宣言
	int nData;
	char aData[4];
	char aRecvData[4] = {};

	//数値を入力
	printf("\n送信する数値を入力 >> ");
	scanf("%d", &nData);
	rewind(stdin);

	//データの送信
	memcpy(&aData[0], &nData, sizeof(int));
	send(sockClient, &aData[0], sizeof(int), 0);

	printf("送信した数値 >> %d\n", nData);

	//データの受信
	int nRecvByte = recv(sockClient, &aRecvData[0], sizeof(aRecvData), 0);
	memcpy(&nData, &aRecvData[0], sizeof(int));

	printf("受信した数値 >> %d\n", nData);
}

//==========================================
//  IPアドレス取得処理
//==========================================
char *GetIPAdddress(void)
{
	//ローカル変数宣言
	FILE *pFile; //ファイルポインタ
	char aString[128]; //文字列廃棄用
	char aIPAddress[128]; //IPアドレスの取得用

	//ファイルを開く
	pFile = fopen("Address.txt", "r");

	//NULLチェック
	if (pFile != NULL)
	{
		//見出しを取得
		for(int nCnt = 0; nCnt < 3; nCnt++)
		{
			fscanf(pFile, "%s", &aString[0]);
		}

		//アドレスを取得
		fscanf(pFile, "%s", &aIPAddress[0]);

		//ファイルを閉じる
		fclose(pFile);

		//成功表示
		printf("IPアドレスの取得に成功 >> %s\n", &aIPAddress[0]);
	}
	else
	{
		printf("IPアドレスの取得に失敗しました");
	}

	//取得したIPアドレスを返す
	return &aIPAddress[0];
}
