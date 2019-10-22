#pragma once
#include "Libs.h"
#include "CBot.h"


//------------------------------------------- ОБЩИЕ ----------------------------------------------

static HINSTANCE hInst;								//Дескриптор приложения
static HWND hWnd = NULL;							//Хэндл окна игры
static HWND hMainMenu = NULL;						//Хэндл окна меню
static int cxClient, cyClient;						//Размеры окна
static int X0 = 0, Y0 = 0;							//Новые координаты верхнего левого угла экрана

static bool F_PAUSE = TRUE, F_GAMEOVER = FALSE, F_BOTRESPAWN = FALSE;		//Признаки игры

static int Result = 0;								//Текущий результат
static int BotResult = 0;							//Результат бота
static char szResult[13];							//Строка вывода текущего результата
static int BestResult = 0;							//Рекорд
static char szBestResult[15];						//Строка вывода рекорда

//------------------------------------------------------------------------------------------------

//------------------------------------------- ЕДА ------------------------------------------------

POS *FirstFood = NULL;			//Динамический список еды
int RationSize = 30;			//Размер "порций" еды
int DopRationSize = 0;			//Размер дополнительной еды
static int InRation = 30;		//Остаток от порции
static char szInRation[35];		//Вывод остатка от порции

void InitFood(void);			//Инициализация еды
void LoadFood(void);			//Загрузка еды
void PaintFood(HDC hdc);		//Отрисовка еды
void DeleteFood(void);			//Удаление еды

//------------------------------------------------------------------------------------------------

//------------------------------------------- КАРТА ----------------------------------------------

static HANDLE hBmMap = NULL;	//Битмап для карты
static HDC hdcMemMap = NULL;	//Контекст памяти карты
static int MapSize = 200;		//Размер карты
static int GridSize = 10;		//Исходный размер сетки
char **Map = NULL;				//Массив карты

void LoadMap(void);				//Отрисовка карты в контексте памяти

//------------------------------------------------------------------------------------------------

//------------------------------------------- ЗМЕЙКА ---------------------------------------------

POS *StartSnake = NULL, *FinishSnake = NULL;	//Начало и конец змейки
static char SnakeVect = SNAKE_DOWN;				//Направление змейки
static bool SnakeVectChanged = true;			//Змейка изменила направление
static CRITICAL_SECTION csPaint;				//Секция для согласования потока и WM_PAINT
static CRITICAL_SECTION csGame;					//Секция для согласования потока и WM_PAINT
												
void InitSnake(void);							//Инициализация змейки
VOID SnakeMovingThread(PVOID pvoid);			//Поток управления змейкой
void PaintSnake(HDC hdc);						//Отрисовка змейки
void DeleteSnake(void);							//Удаление змейки

//------------------------------------------------------------------------------------------------

//------------------------------------------- БОТ ------------------------------------------------
CBot Bot;
//------------------------------------------------------------------------------------------------