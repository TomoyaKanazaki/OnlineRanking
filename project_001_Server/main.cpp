//==========================================
//
//  サーバープログラム(main.cpp)
//  Author : Tomoya Kanazaki
//
//==========================================
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
int GetOrder(const int nScore); //順位取得処理
void LoadData(int *apRanking, int &nNumData);
void SaveData(int *apRanking, int nNumData);
void SaveText(int *apRanking, int nNumData);
int Descending(const void * n0, const void * n1); //ソート用

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

	//今回の順位を取得する
	nData = GetOrder(nData);

	//データの送信
	char aData[4];
	memcpy(&aData[0], &nData, sizeof(int));
	send(sockServer, &aData[0], sizeof(int), 0);

	printf("送信した数値 >> %d\n", nData);
}

//==========================================
//  順位取得処理
//==========================================
int GetOrder(const int nScore)
{
	int nOrder = 0; //順位
	int nNumData = 0; //使用するデータ数
	int *apRanking = NULL;

	//読み込み処理
	LoadData(apRanking, nNumData);

	//NULLチェック
	if (apRanking != NULL)
	{
		//最後のメモリに受信したデータを保存する
		apRanking[nNumData] = nScore;

		//保存されているデータ数の1増やす
		nNumData++;

		//ランキングデータをソートする
		qsort(apRanking, nNumData, sizeof(int), Descending);

		//受信したスコアの順位を取得する
		for (int nCnt = 0; nCnt < nNumData; nCnt++)
		{
			if (apRanking[nCnt] == nScore)
			{
				nOrder = nCnt + 1;
				break;
			}
		}
	}

	//書き出し処理
	SaveData(apRanking, nNumData);
	SaveText(apRanking, nNumData);

	//メモリを開放する
	if (apRanking != NULL)
	{
		delete[] apRanking;
		apRanking = NULL;
	}

	//順位を返す
	return nOrder;
}

//==========================================
//  読み込み処理
//==========================================
void LoadData(int *apRanking, int &nNumData)
{
	//ローカル変数宣言
	FILE *pFile = NULL;

	//ファイルを開く
	pFile = fopen("Ranking.bin", "rb");

	//NULLチェック
	if (pFile != NULL)
	{
		//保存されているデータ数を取得する
		fread(&nNumData, sizeof(int), 1, pFile);

		//(保存されているデータ数 + 1) * 4バイトのメモリを確保する
		if (apRanking == NULL)
		{
			apRanking = new int[nNumData + 1];
		}
		else
		{
			printf("ランキングデータ用のメモリが確保できませんでした\n");
		}

		//メモリを確保したら保存されているデータを取得する
		if (apRanking != NULL)
		{
			fread(apRanking, sizeof(int), nNumData, pFile);
		}

		//ファイルを閉じる
		fclose(pFile);
	}
	else
	{
		printf("ランキングデータの読み込みに失敗\n");
	}
}

//==========================================
//  書き出し処理
//==========================================
void SaveData(int *apRanking, int nNumData)
{
	//ローカル変数宣言
	FILE *pFile = NULL;

	//ファイルを開く
	pFile = fopen("Ranking.bin", "wb");

	//NULLチェック
	if (pFile != NULL)
	{
		//保存するデータ数を保存する
		fwrite(&nNumData, sizeof(int), 1, pFile);

		//ランキングを保存する
		fwrite(apRanking, sizeof(int), nNumData, pFile);

		//ファイルを閉じる
		fclose(pFile);
	}
	else
	{
		printf("ランキングデータの書き出しに失敗\n");
	}
}

//==========================================
//  テキストファイルへの書き出し
//==========================================
void SaveText(int *apRanking, int nNumData)
{
	//ローカル変数宣言
	FILE *pFile = NULL;

	//ファイルを開く
	pFile = fopen("Ranking.txt", "w");

	//NULLチェック
	if (pFile != NULL)
	{
		//保存数
		fprintf(pFile, "保存されたデータ数 : %d", nNumData);

		//各スコアの保存
		for (int nCntData = 0; nCntData < nNumData; nCntData++, apRanking++)
		{
			fprintf(pFile, "%d : %d", nCntData + 1, *apRanking);
		}

		//ファイルを閉じる
		fclose(pFile);
	}
	else
	{
		printf("ランキングデータの書き出しに失敗\n");
	}
}

//==========================================
//  ソート用
//==========================================
int Descending(const void * n0, const void * n1)
{
	//n0とn1の値を比較した結果によって返り値を決定する
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
