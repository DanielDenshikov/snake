#include "Libs.h"
#include "!Snake.h"
#include "MapEditor.h"

//------------------------------------------- КАРТА ----------------------------------------------

//Отрисовка карты в контексте памяти
void LoadMap(void)
{
	PatBlt(hdcMemMap, 0, 0, MapSize * GridSize, MapSize * GridSize, BLACKNESS);
	GridSize = cxClient / 100;
	PatBlt(hdcMemMap, 0, 0, MapSize * GridSize, MapSize * GridSize, BLACKNESS);
	//Отрисовка сетки
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
	hPen = (HPEN)SelectObject(hdcMemMap, hPen);
	int i = 0;
	while (i <= MapSize * GridSize)
	{
		MoveToEx(hdcMemMap, i, 0, NULL);
		LineTo(hdcMemMap, i, MapSize * GridSize);
		i += GridSize;
	}
	i = 0;
	while (i <= MapSize * GridSize)
	{
		MoveToEx(hdcMemMap, 0, i, NULL);
		LineTo(hdcMemMap, MapSize * GridSize, i);
		i += GridSize;
	}
	DeletePen(SelectObject(hdcMemMap, hPen));
	
	Map = new char*[MapSize];
	for (int i = 0; i < MapSize; i++)
	{
		Map[i] = new char[MapSize];
	}
	FILE *FileMap = fopen("Map.mp", "r");
	if (!Map)
	{
		MessageBox(hWnd, "Проверьте наличие файла Map.mp в каталоге с программой!",
			"Ошибка открытия файла карты", MB_OK);
		for (int i = 0; i < MapSize; i++)
		{
			memset(Map[i], 0, MapSize);
		}
	}
	else
	{
		for (int i = 0; i < MapSize; i++)
		{
			fread(Map[i], 1, MapSize, FileMap);
		}
	}
	
	//Отрисовка карты
	RECT MapRect;
	HBRUSH hBrushFill = CreateSolidBrush(RGB(127, 127, 127));
	HBRUSH hBrushFrame = CreateSolidBrush(RGB(202, 202, 202));
	for (int i = 0; i < MapSize; i++)
		for (int j = 0; j < MapSize; j++)
		{
			if (Map[i][j])
			{
				SetRect(&MapRect, i * GridSize, j * GridSize, (i + 1) * GridSize + 1, (j + 1) * GridSize + 1);
				FillRect(hdcMemMap, &MapRect, hBrushFill);
				FrameRect(hdcMemMap, &MapRect, hBrushFrame);
			}
		}
	DeleteObject(hBrushFill);
	DeleteObject(hBrushFrame);
}

//------------------------------------------------------------------------------------------------

//------------------------------------------- ЗМЕЙКА ---------------------------------------------

//Инициализация змейки
void InitSnake(void)
{
	if (StartSnake)
		DeleteSnake();

	StartSnake = new POS;
	StartSnake->x = 20;
	StartSnake->y = 20;
	FinishSnake = StartSnake;
	for (int i = 0; i < 2; i++)
	{
		FinishSnake->Next = new POS;
		FinishSnake->Next->Prev = FinishSnake;
		FinishSnake = FinishSnake->Next;
		FinishSnake->x = FinishSnake->Prev->x;
		FinishSnake->y = FinishSnake->Prev->y - 1;
	}
	SnakeVect = SNAKE_DOWN;
}

//Поток управления змейкой
VOID SnakeMovingThread(PVOID pvoid)
{
	EnterCriticalSection(&csGame);
	while (!F_GAMEOVER)
	{
		Sleep(500);
		while (!F_GAMEOVER && !F_PAUSE)
		{
			EnterCriticalSection(&csPaint);

			if (!F_GAMEOVER)
			{
				POS *LastSn = FinishSnake;
				POS *LastFd = FirstFood;
				while (LastFd->Next)
				{
					if ((StartSnake->x == LastFd->Next->x) && (StartSnake->y == LastFd->Next->y))
						LastFd->Next->active = false;

					if ((LastSn->x == LastFd->Next->x) && (LastSn->y == LastFd->Next->y))
					{
						if (LastFd->Next->dop)
							DopRationSize--;
						else
							InRation--;

						POS *BUF = LastFd->Next;
						LastFd->Next = LastFd->Next->Next;
						delete BUF;

						BUF = new POS;
						BUF->x = LastSn->x - 1;
						BUF->y = LastSn->y - 1;
						LastSn->Next = BUF;
						BUF->Prev = LastSn;
						FinishSnake = BUF;
						Result++;
						sprintf(szResult, "СЧЕТ %d : %d", Result, BotResult);
						sprintf(szInRation, "ОТ ПОРЦИИ ОСТАЛОСЬ %d КУСКОВ", InRation + DopRationSize);
					}
					if (LastFd->Next)
					{
						if ((Bot.BotStartSnake->x == LastFd->Next->x) && (Bot.BotStartSnake->y == LastFd->Next->y))
							LastFd->Next->active = false;

						if ((Bot.BotFinishSnake->x == LastFd->Next->x) && (Bot.BotFinishSnake->y == LastFd->Next->y))
						{
							if (LastFd->Next->dop)
								DopRationSize--;
							else
								InRation--;

							POS *BUF = LastFd->Next;
							LastFd->Next = LastFd->Next->Next;
							delete BUF;
							Bot.BotSnakeAdd();
							
							BotResult++;
							sprintf(szResult, "СЧЕТ %d : %d", Result, BotResult);
							sprintf(szInRation, "ОТ ПОРЦИИ ОСТАЛОСЬ %d КУСКОВ", InRation + DopRationSize);
						}
						else
							LastFd = LastFd->Next;
					}
				}
				
				LastSn = FinishSnake;

				while (LastSn->Prev)
				{
					LastSn->x = LastSn->Prev->x;
					LastSn->y = LastSn->Prev->y;
					LastSn = LastSn->Prev;
				}

				switch (SnakeVect)
				{
				case SNAKE_UP:
					StartSnake->y--;
					if (StartSnake->y * GridSize - Y0 < cyClient * 0.2)
						Y0 -= GridSize;
					break;
				case SNAKE_DOWN:
					StartSnake->y++;
					if (StartSnake->y * GridSize - Y0 > cyClient - cyClient * 0.2)
						Y0 += GridSize;
					break;
				case SNAKE_LEFT:
					StartSnake->x--;
					if (StartSnake->x * GridSize - X0 < cxClient * 0.2)
						X0 -= GridSize;
					break;
				case SNAKE_RIGTH:
					StartSnake->x++;
					if (StartSnake->x * GridSize - X0 > cxClient - cxClient * 0.2)
						X0 += GridSize;
					break;
				}
				SnakeVectChanged = true;
				Bot.MoveBotSnake(Map, FirstFood, StartSnake);

				LastSn = FinishSnake;

				while (LastSn->Prev)
				{
					if (LastSn->x == StartSnake->x && LastSn->y == StartSnake->y)
						F_GAMEOVER = TRUE;
					if (LastSn->x == Bot.BotStartSnake->x && LastSn->y == Bot.BotStartSnake->y)
						F_BOTRESPAWN = TRUE;
					LastSn = LastSn->Prev;
				}

				LastSn = Bot.BotFinishSnake;

				while (LastSn->Prev)
				{
					if (LastSn->x == StartSnake->x && LastSn->y == StartSnake->y)
						F_GAMEOVER = TRUE;
					if (LastSn->x == Bot.BotStartSnake->x && LastSn->y == Bot.BotStartSnake->y)
						F_BOTRESPAWN = TRUE;
					LastSn = LastSn->Prev;
				}

				if (Map[StartSnake->x][StartSnake->y])
					F_GAMEOVER = TRUE;
				if (Map[Bot.BotStartSnake->x][Bot.BotStartSnake->y])
					F_GAMEOVER = TRUE;
				
				if (F_BOTRESPAWN)
				{
					POS *LastFd = FirstFood;
					while (LastFd->Next)
						LastFd = LastFd->Next;

					POS *StartSn = Bot.BotStartSnake;
					while (StartSn->Next)
					{
						StartSn = StartSn->Next;

						LastFd->Next = new POS;
						DopRationSize++;
						LastFd->Next->dop = true;
						LastFd->Next->x = StartSn->x;
						LastFd->Next->y = StartSn->y;
						LastFd = LastFd->Next;

						sprintf(szInRation, "ОТ ПОРЦИИ ОСТАЛОСЬ %d КУСКОВ", InRation + DopRationSize);

						if (StartSn->Next)
							StartSn = StartSn->Next;
						else
							break;
					}

					Bot.BotSnakeDelete();
					BotResult = 0;
					Bot.BotSnakeInit();
					F_BOTRESPAWN = FALSE;
				}

				if (F_GAMEOVER)
				{
					POS *LastFd = FirstFood;
					while (LastFd->Next)
						LastFd = LastFd->Next;

					POS *StartSn = StartSnake;
					while (StartSn->Next)
					{
						StartSn = StartSn->Next;

						LastFd->Next = new POS;
						DopRationSize++;
						LastFd->Next->dop = true;
						LastFd->Next->x = StartSn->x;
						LastFd->Next->y = StartSn->y;
						LastFd = LastFd->Next;

						sprintf(szInRation, "ОТ ПОРЦИИ ОСТАЛОСЬ %d КУСКОВ", InRation + DopRationSize);

						if (StartSn->Next)
							StartSn = StartSn->Next;
						else
							break;
					}

					DeleteSnake();
					Result = 0;
					InitSnake();
					F_GAMEOVER = FALSE;
					X0 = 0;
					Y0 = 0;
				}

				if (FirstFood->Next == NULL)
				{
					LoadFood();
				}
			}
			
			LeaveCriticalSection(&csPaint);
			InvalidateRect(hWnd, NULL, FALSE);
			Sleep(40);
		}
	}

	PostMessage(hWnd, WM_COMMAND, WM_GAMEOVER, 0);
	LeaveCriticalSection(&csGame);
	_endthread();
	return;
}

//Отрисовка змейки
void PaintSnake(HDC hdc)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(213, 69, 0));
	POS *X = Bot.BotFinishSnake;
	RECT SnakeRect;

	while (X)
	{
		SetRect(&SnakeRect, X->x * GridSize - X0, X->y * GridSize - Y0, (X->x + 1) * GridSize + 1 - X0, (X->y + 1) * GridSize + 1 - Y0);
		FillRect(hdc, &SnakeRect, hBrush);
		FrameRect(hdc, &SnakeRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		X = X->Prev;
	}
	FillRect(hdc, &SnakeRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	DeleteObject(hBrush);

	hBrush = CreateSolidBrush(RGB(0, 69, 213));
	X = FinishSnake;
	
	while (X)
	{
		SetRect(&SnakeRect, X->x * GridSize - X0, X->y * GridSize - Y0, (X->x + 1) * GridSize + 1 - X0, (X->y + 1) * GridSize + 1 - Y0);
		FillRect(hdc, &SnakeRect, hBrush);
		FrameRect(hdc, &SnakeRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		X = X->Prev;
	}
	FillRect(hdc, &SnakeRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	DeleteObject(hBrush);
}

//Удаление змейки
void DeleteSnake(void)
{
	POS *X;
	if (StartSnake)
		while (StartSnake)
		{
			X = StartSnake->Next;
			delete StartSnake;
			StartSnake = X;
		}
	StartSnake = NULL;
	FinishSnake = NULL;
}
//------------------------------------------------------------------------------------------------

//-------------------------------------------- ЕДА -----------------------------------------------

//Инициализация еды
void InitFood(void)
{
	if (FirstFood)
		DeleteFood();
	FirstFood = new POS;
}

//Загрузка еды
void LoadFood(void)
{
	InRation = 30;
	sprintf(szInRation, "ОТ ПОРЦИИ ОСТАЛОСЬ %d КУСКОВ", InRation);
	srand(time(NULL));
	bool F_OK = FALSE;
	POS *X = NULL;
	for (int i = 0; i < RationSize; i++)
	{
		F_OK = TRUE;
		int x, y;
		while (F_OK)
		{
			X = FirstFood;
			F_OK = FALSE;
			x = rand() % MapSize;
			y = rand() % MapSize;
			if (Map[x][y]) F_OK = TRUE;
			else
			{
				while (X->Next)
				{
					if ((x == X->x) && (y = X->y))
					{
						F_OK = TRUE;
						break;
					}
					X = X->Next;
				}
				X = StartSnake;
				while (X)
				{
					if ((x == X->x) && (y = X->y))
					{
						F_OK = TRUE;
						break;
					}
					X = X->Next;
				}
			}
		}
		X = FirstFood;
		while (X->Next) X = X->Next;
		X->Next = new POS;
		X = X->Next;
		X->x = x;
		X->y = y;
	}

	return;
}

//Отрисовка еды
void PaintFood(HDC hdc)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 64));
	RECT FoodRect;

	POS *X = FirstFood->Next;
	while (X)
	{
		SetRect(&FoodRect, X->x * GridSize - X0 - 1, X->y * GridSize - Y0 - 1, (X->x + 1) * GridSize + 2 - X0, (X->y + 1) * GridSize + 2 - Y0);
		FillRect(hdc, &FoodRect, hBrush);
		FrameRect(hdc, &FoodRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		X = X->Next;
	}

	DeleteObject(hBrush);
}

//Удаление еды
void DeleteFood(void)
{
	POS *X;
	if(FirstFood)
		while (FirstFood)
		{
			X = FirstFood->Next;
			delete FirstFood;
			FirstFood = X;
		}
}

//------------------------------------------------------------------------------------------------

//-------------------------------------------- ОКНА ----------------------------------------------

//Окно главного меню
LRESULT CALLBACK MainMenuWndProc(HWND hMainMenuWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInst;																		//Дескриптор приложения

	HWND hMapEditorWnd;																			//Хэндл редактора
	static int cxMainMenu, cyMainMenu, cxButton, cyButton;										//Размеры меню

	static HWND hMainMenuButtons[5];
	char ButtonIDC[] = { IDC_NEWGAME, IDC_CONTINUE, IDC_CHOOSEMAP, IDC_HELP, IDC_EXIT };		//ID кнопок
	char *ButtonName[] = { "Новая игра", "Продолжить", "Изменить карту", "Помощь", "Выход" };	//Имена кнопок
	
	LPDRAWITEMSTRUCT dis;

	switch (iMsg)
	{
	case WM_CREATE:
		//Создание кнопок
		for (int i = 0; i < 5; i++)
		{
			hMainMenuButtons[i] = CreateWindow("button", ButtonName[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0,
				hMainMenuWnd, (HMENU)ButtonIDC[i], hInst, NULL);
		}
		break;
	case WM_SIZE:
		//Изменение размеров окна меню
		cxMainMenu = cxClient / 4;
		cyMainMenu = cyClient / 2;
		cyButton = cyMainMenu / 8;
		cxButton = cyButton * 3;
		break;
	case WM_PAINT:
		//Перемещение меню и кнопок
		MoveWindow(hMainMenuWnd, cxClient * 3 / 8, cyClient * 3 / 8 - cyButton, cxMainMenu, cyMainMenu + cyButton, TRUE);
		for (int i = 0; i < 5; i++)
			MoveWindow(hMainMenuButtons[i], cxMainMenu / 2 - cxButton / 2, cyButton / 2 + cyButton * i * 1.5, cxButton, cyButton, TRUE);
		break;
	case WM_COMMAND:
		switch(wParam)
		{
		case IDC_NEWGAME:
			//Пункт меню "Новая игра"
			F_PAUSE = FALSE;
			PostMessage(hWnd, WM_COMMAND, wParam, lParam);
			DestroyWindow(hMainMenuWnd);
			break;
		case IDC_CONTINUE:
			F_PAUSE = FALSE;
			PostMessage(hWnd, WM_COMMAND, wParam, lParam);
			DestroyWindow(hMainMenuWnd);
			break;
		case IDC_CHOOSEMAP:
			if (!RegisterMapEditorClass(hInst))
			{
				hMapEditorWnd = CreateWindow("MapEditor", "MAP EDITOR", WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_MAXIMIZE, 20, 20, 600, 600,
					NULL, NULL, hInst, NULL);
				ShowWindow(hMapEditorWnd, SW_SHOW);
				UpdateWindow(hMapEditorWnd);
				PostMessage(hWnd, WM_COMMAND, wParam, lParam);
			}
			break;
		case IDC_HELP:
			MessageBox(hMainMenuWnd, "Игра змейка. Управление осуществляется клавишами стрелками.\n"
				"Имеется встроенный редактор карты\n\n"
				"Данная игра составлена в учебных целях.\n"
				"Автор: Деньщиков Данил\n"
				"Никакие права не защищены. Ижевск, 2016", "Помощь", MB_OK | MB_DEFBUTTON1);
			break;
		case IDC_EXIT:
			F_PAUSE = FALSE;
			PostMessage(hWnd, WM_DESTROY, 0, 0);
			break;
		}
		break;
	case WM_CHAR:
		if (wParam != 27) break;
	case WM_DESTROY:
		F_PAUSE = FALSE;
		DestroyWindow(hMainMenuWnd);
		break;
	}
	return DefWindowProc(hMainMenuWnd, iMsg, wParam, lParam);
}

//Окно игры
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	
	static HDC hdc;								//Контекст устройства
	
	static HBITMAP hBmMem;						//Двойная буферизация - битмап
	static HDC hdcMem;							//Двойная буферизация - контекст
	
	switch (iMsg)
	{
	case WM_CREATE:
		//Открываем меню
		F_PAUSE = TRUE;
		hMainMenu = CreateWindow("MainMenu", "Главное меню", WS_CAPTION | WS_BORDER, 0, 0, 0, 0,
			hWnd, NULL, hInst, NULL);
		ShowWindow(hMainMenu, SW_SHOW);
		UpdateWindow(hMainMenu);
		
		GridSize = GetSystemMetrics(SM_CXFULLSCREEN) / 100;

		//Готовим контекст устройства для рисования
		hdc = GetDC(hWnd);
		hBmMap = CreateCompatibleBitmap(hdc, MapSize * 20 + 1, MapSize * 20 + 1);
		hdcMemMap = CreateCompatibleDC(hdc);
		SelectObject(hdcMemMap, hBmMap);

		hBmMem = CreateCompatibleBitmap(hdc, MapSize * 20 + 1, MapSize * 20 + 1);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmMem);
		
		InitializeCriticalSection(&csPaint);
		InitializeCriticalSection(&csGame);

		InitSnake();
		Bot.BotSnakeInit();
		InitFood();

		break;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		if (cxClient && cyClient)
		{
			LoadMap();
		}
		if (F_PAUSE) PostMessage(hMainMenu, WM_SIZE, 0, 0);
		
		break;
	case WM_CHAR:
		if(wParam == 27)
		{
			//Открываем меню по ESC
			F_PAUSE = TRUE;
			hMainMenu = CreateWindow("MainMenu", "Главное меню", WS_CAPTION | WS_BORDER, 0, 0, 0, 0,
				hWnd, NULL, hInst, NULL);
			ShowWindow(hMainMenu, SW_SHOW);
			UpdateWindow(hMainMenu);
		}
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_UP:
			if (SnakeVect != SNAKE_DOWN && SnakeVectChanged)
			{
				SnakeVect = SNAKE_UP;
				SnakeVectChanged = false;
			}
			break;
		case VK_DOWN:
			if (SnakeVect != SNAKE_UP && SnakeVectChanged)
			{
				SnakeVect = SNAKE_DOWN;
				SnakeVectChanged = false;
			}
			break;
		case VK_LEFT:
			if (SnakeVect != SNAKE_RIGTH && SnakeVectChanged)
			{
				SnakeVect = SNAKE_LEFT;
				SnakeVectChanged = false;
			}
			break;
		case VK_RIGHT:
			if (SnakeVect != SNAKE_LEFT && SnakeVectChanged)
			{
				SnakeVect = SNAKE_RIGTH;
				SnakeVectChanged = false;
			}
			break;
		}
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_NEWGAME:
			F_PAUSE = TRUE;
			if (!F_GAMEOVER)
			{
				F_GAMEOVER = TRUE;
				EnterCriticalSection(&csGame);
				F_GAMEOVER = FALSE;
				LeaveCriticalSection(&csGame);
			}
			else F_GAMEOVER = FALSE;
			F_PAUSE = FALSE;
			X0 = 0;
			Y0 = 0;
			InitSnake();
			Bot.BotSnakeInit();
			InitFood();
			Result = 0;
			BotResult = 0;
			sprintf(szResult, "СЧЕТ %d : %d", Result, BotResult);
			sprintf(szBestResult, "РЕКОРД: %d", BestResult);
			_beginthread(SnakeMovingThread, NULL, NULL);
			break;
		case IDC_CONTINUE:
			F_PAUSE = FALSE;
			break;
		case WM_GAMEOVER:
			if (F_GAMEOVER) MessageBox(hWnd, szResult, 
				(Result > BestResult) ? "Рекорд побит!" : "Дерзай!", MB_OK);
			if (Result > BestResult) BestResult = Result;
			F_PAUSE = TRUE;
			hMainMenu = CreateWindow("MainMenu", "Главное меню", WS_CAPTION | WS_BORDER, 0, 0, 0, 0,
				hWnd, NULL, hInst, NULL);
			ShowWindow(hMainMenu, SW_SHOW);
			UpdateWindow(hMainMenu);
			break;
		}
	case WM_PAINT:
		EnterCriticalSection(&csPaint);
		PatBlt(hdcMem, 0, 0, cxClient, cyClient, BLACKNESS);
		BitBlt(hdcMem, 0, 0, cxClient, cyClient, hdcMemMap, X0, Y0, SRCCOPY);
		PaintFood(hdcMem);
		PaintSnake(hdcMem);
		TextOut(hdcMem, 5, 5, szResult, strlen(szResult));
		TextOut(hdcMem, cxClient / 2 - 15, 5, szInRation, strlen(szInRation));
		TextOut(hdcMem, cxClient - 100, 5, szBestResult, strlen(szBestResult));
		BitBlt(hdc, 0, 0, cxClient, cyClient, hdcMem, 0, 0, SRCCOPY);
		LeaveCriticalSection(&csPaint);
		if (F_PAUSE) PostMessage(hMainMenu, WM_PAINT, 0, 0);
		break;
	case WM_DESTROY:
		if(F_PAUSE) PostMessage(hMainMenu, WM_DESTROY, 0, 0);
		
		DeleteDC(hdcMemMap);
		hdcMemMap = NULL;
		DeleteObject(hBmMap);
		hBmMap = NULL;

		ReleaseDC(hWnd, hdc);

		DeleteCriticalSection(&csPaint);
		DeleteCriticalSection(&csGame);

		DeleteSnake();
		Bot.BotSnakeDelete();
		DeleteFood();

		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

//Регистрация классов окон
bool RegisterMyWindowsClass(HINSTANCE hInst)
{
	WNDCLASS wnd;

	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wnd.hInstance = hInst;
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = "FirstProject";
	wnd.lpszMenuName = NULL;
	wnd.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wnd);

	wnd.lpszClassName = "MainMenu";
	wnd.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wnd.lpfnWndProc = MainMenuWndProc;

	RegisterClass(&wnd);
	return FALSE;
}

//Головная функция
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	hInst = hInstance;
	if (RegisterMyWindowsClass(hInstance)) return 0;
	
	hWnd = CreateWindow("FirstProject", "Snake", WS_VISIBLE | WS_MENU | WS_MAXIMIZE | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
		NULL, NULL, hInstance, NULL);
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}