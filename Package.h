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
	CS_SIZE = 64,//SIZE�� �ߴ��� ����
	WIDTH = 100,
	HEIGHT = 30
};

/**********/

struct Info
{
	int playerX = WIDTH / 2;//�÷��̾� ���� ��ġ
	int playerY = HEIGHT - 2;
	int playerLife = 6;
	int enemyX = 0;//���ʿ��� ������ �Դٰ���
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
	//������ �и�
	static void DemageCheck(void);

	Package();	

	//�ܼ� ���� ����
	void SetCursor(BOOL bshow);
	void SetConsoleSize(int col, int lines);

	//��ǥ ����
	void GotoXY(int x, int y);
	COORD GetXY(void);

	void Title(void);
	void PrintState(void);
	void PlayerProcess(void);	
	bool ResultDisplay(void);

	//�÷��̾�
	void MovePlayer(void);
	void PrintPlayer(void);
	void PrintPlayerShoot(void);
	
	//��
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