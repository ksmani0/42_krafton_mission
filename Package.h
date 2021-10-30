#ifndef PACKAGE_H
#define PACKAGE_H

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <thread>
#include <conio.h>
//#pragma once
using namespace std;

enum
{
	MAX = 5,
	STATUS = 28,
	CS_SIZE = 64,//SIZE로 했더니 오류
	WIDTH = 100,
	HEIGHT = 30
};

/**********/

struct Info
{
	int playerX = WIDTH / 2;//플레이어 시작 위치
	int playerY = HEIGHT - 2;
	int playerLife = 6;
	int enemyX = 0;//왼쪽에서 오른쪽 왔다갔다
	int enemyY = 2;
	int enemyLife = 6;
};
struct OldInfo
{
	int playerX = WIDTH / 2;
	int playerY = HEIGHT - 2;
	int enemyX = 0;
	int enemyY = 2;
};

struct PlayerShoot
{
	int x = 0;
	int y = HEIGHT - 4;
};
struct EnemyShoot
{
	int x = 0;
	int y = 4;
};

/**********/

class Package
{
public:
	//스레드 분리
	static void DemageCheck(void);

	Package();	

	//콘솔 설정 관련
	void SetCursor(BOOL bshow);
	void SetConsoleSize(int col, int lines);

	//좌표 관련
	void GotoXY(int x, int y);
	COORD GetXY(void);

	void Title(void);
	void PrintState(void);
	void PlayerProcess(void);	
	bool ResultDisplay(void);

	//플레이어
	void MovePlayer(void);
	void PrintPlayer(void);
	void PrintPlayerShoot(void);
	
	//적
	void MoveNPrintEnemy(void);
	void PrintEnemyShoot(void);	

	~Package() {}

private:
	static string mStatus;//"YOU ###### VS ###### ENEMY";
	static Info mInfo;
	static OldInfo mOld;
	static PlayerShoot mPlayer[MAX];
	static EnemyShoot mEnemy[MAX];

	Package(const Package& ref) {}
	Package& operator=(const Package& ref) {}
};

#endif