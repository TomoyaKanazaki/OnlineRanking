//==========================================
//
//  サーバープログラム(main.cpp)
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
void Uninit(SOCKET *sockServer, SOCKET *sock); //終了処理
SOCKET SetAddress(SOCKET sock); //接続処理
void Transceiving(SOCKET sockServer); //送受信処理

//==========================================
//  メイン関数
//==========================================
int main(void)
{
	//ローカル変数宣言
	WSADATA wsaData; //ソケットの仕様情報
	SOCKET sock;
	
	//初期化処理
	Init(&wsaData, &sock);
	
	//接続処理
	SOCKET sockServer = SetAddress(sock);

	//送受信処理
	Transceiving(sockServer);

	//終了処理
	Uninit(&sockServer, &sock);

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
//==========================================
void Uninit(SOCKET *sockServer, SOCKET *sock)
{
	//クライアントとの接続を閉じる
	closesocket(*sockServer);

	//接続受付用ソケットを閉じる
	closesocket(*sock);

	//WinSockの終了
	WSACleanup();
}

//==========================================
//  接続処理
//==========================================
SOCKET SetAddress(SOCKET sock)
{
	// ポート番号,IPタイプの入力
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUM);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(sock, (struct sockaddr*)&addr, sizeof(addr)); // ポートの割り当て
	listen(sock, JOIN_MAX); // 接続受付の準備

	//接続待ち
	printf("<< クライアントからの接続を待っています >>\n");
	struct sockaddr_in addrClient;
	int nLengthAddr = sizeof(addrClient);
	SOCKET sockServer = accept(sock, (struct sockaddr*)&addrClient, &nLengthAddr);
	printf("<< クライアントからの接続を確認 >>\n");

	//値を返す
	return sockServer;
}

//==========================================
void Transceiving(SOCKET sockServer)
{
	//データの受信
	char aRecvData[4] = {};
	int nRecvByte = recv(sockServer, &aRecvData[0], sizeof(aRecvData), 0);
	int nData = 0;
	memcpy(&nData, &aRecvData[0], sizeof(int));

	printf("受信した数値 >> %d\n", nData);

	//データの送信
	nData *= 2;
	char aData[4];
	memcpy(&aData[0], &nData, sizeof(int));
	send(sockServer, &aData[0], sizeof(int), 0);

	printf("送信した数値 >> %d\n", nData);
}
