#pragma once
#include "Libs.h"
#include "CBot.h"


//------------------------------------------- ����� ----------------------------------------------

static HINSTANCE hInst;								//���������� ����������
static HWND hWnd = NULL;							//����� ���� ����
static HWND hMainMenu = NULL;						//����� ���� ����
static int cxClient, cyClient;						//������� ����
static int X0 = 0, Y0 = 0;							//����� ���������� �������� ������ ���� ������

static bool F_PAUSE = TRUE, F_GAMEOVER = FALSE, F_BOTRESPAWN = FALSE;		//�������� ����

static int Result = 0;								//������� ���������
static int BotResult = 0;							//��������� ����
static char szResult[13];							//������ ������ �������� ����������
static int BestResult = 0;							//������
static char szBestResult[15];						//������ ������ �������

//------------------------------------------------------------------------------------------------

//------------------------------------------- ��� ------------------------------------------------

POS *FirstFood = NULL;			//������������ ������ ���
int RationSize = 30;			//������ "������" ���
int DopRationSize = 0;			//������ �������������� ���
static int InRation = 30;		//������� �� ������
static char szInRation[35];		//����� ������� �� ������

void InitFood(void);			//������������� ���
void LoadFood(void);			//�������� ���
void PaintFood(HDC hdc);		//��������� ���
void DeleteFood(void);			//�������� ���

//------------------------------------------------------------------------------------------------

//------------------------------------------- ����� ----------------------------------------------

static HANDLE hBmMap = NULL;	//������ ��� �����
static HDC hdcMemMap = NULL;	//�������� ������ �����
static int MapSize = 200;		//������ �����
static int GridSize = 10;		//�������� ������ �����
char **Map = NULL;				//������ �����

void LoadMap(void);				//��������� ����� � ��������� ������

//------------------------------------------------------------------------------------------------

//------------------------------------------- ������ ---------------------------------------------

POS *StartSnake = NULL, *FinishSnake = NULL;	//������ � ����� ������
static char SnakeVect = SNAKE_DOWN;				//����������� ������
static bool SnakeVectChanged = true;			//������ �������� �����������
static CRITICAL_SECTION csPaint;				//������ ��� ������������ ������ � WM_PAINT
static CRITICAL_SECTION csGame;					//������ ��� ������������ ������ � WM_PAINT
												
void InitSnake(void);							//������������� ������
VOID SnakeMovingThread(PVOID pvoid);			//����� ���������� �������
void PaintSnake(HDC hdc);						//��������� ������
void DeleteSnake(void);							//�������� ������

//------------------------------------------------------------------------------------------------

//------------------------------------------- ��� ------------------------------------------------
CBot Bot;
//------------------------------------------------------------------------------------------------