#include "MapEditor.h"

int ClientToGrid(int x)
{
	return ((int)(x / GridEditSize) * GridEditSize);
}

void DrawRect(void)
{
	for (int i = 0; i < MapEditSize; i++)
	{
		MapEdit[i][0] = 1;
		MapEdit[i][MapEditSize - 1] = 1;
		MapEdit[0][i] = 1;
		MapEdit[MapEditSize - 1][i] = 1;
	}
	MapEdit[20][20] = 0;
	MapEdit[20][19] = 0;
	MapEdit[20][18] = 0;
}

void InitMap(FILE *Map)
{
	MapEdit = new char*[MapEditSize];
	for (int i = 0; i < MapEditSize; i++)
	{
		MapEdit[i] = new char[MapEditSize];
	}
	if (Map)
	{
		for (int i = 0; i < MapEditSize; i++)
			fread(MapEdit[i], 1, MapEditSize, Map);
	}
	else
	{
		for (int i = 0; i < MapEditSize; i++)
		{
			memset(MapEdit[i], 0, MapEditSize);
		}
	}
}

void WriteMap(FILE *Map)
{
	if (Map)
	{
		for (int i = 0; i < MapEditSize; i++)
			fwrite(MapEdit[i], 1, MapEditSize, Map);
	}

}

void DrawMap(void)
{
	BitBlt(hdcMemEditMap, 0, 0, cxEditClient, cyEditClient, hdcMemEditGrid, 0, 0, SRCCOPY);
	//Отрисовка карты
	DrawRect();
	RECT MapRect;
	HBRUSH hBrushFill = CreateSolidBrush(RGB(127, 127, 127));
	HBRUSH hBrushFrame = CreateSolidBrush(RGB(202, 202, 202));
	for (int i = 0; i < MapEditSize; i++)
		for (int j = 0; j < MapEditSize; j++)
		{
			if (MapEdit[i][j])
			{
				SetRect(&MapRect, i * GridEditSize, j * GridEditSize, (i + 1) * GridEditSize + 1, (j + 1) * GridEditSize + 1);
				FillRect(hdcMemEditMap, &MapRect, hBrushFill);
				FrameRect(hdcMemEditMap, &MapRect, hBrushFrame);
			}
		}
	DeleteObject(hBrushFill);
	DeleteObject(hBrushFrame);
}

void DeleteMap(void)
{
	if (MapEdit)
	{
		for (int i = 0; i < MapEditSize; i++)
			delete[] MapEdit[i];
		delete[] MapEdit;
		MapEdit = NULL;
	}
}

void DrawGrid(void)
{
	PatBlt(hdcMemEditGrid, 0, 0, cxEditClient, cyEditClient, BLACKNESS);

	//Отрисовка сетки
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
	hPen = (HPEN)SelectObject(hdcMemEditGrid, hPen);
	int i = 0;
	while (i < cxEditClient)
	{
		MoveToEx(hdcMemEditGrid, i, 0, NULL);
		LineTo(hdcMemEditGrid, i, cyEditClient);
		i += GridEditSize;
	}
	i = 0;
	while (i < cyEditClient)
	{
		MoveToEx(hdcMemEditGrid, 0, i, NULL);
		LineTo(hdcMemEditGrid, cxEditClient, i);
		i += GridEditSize;
	}
	DeletePen(SelectObject(hdcMemEditGrid, hPen));
}

LRESULT CALLBACK MapEditorWnd(HWND hMapEditorWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
	static int X0, Y0, X1, Y1;
	static HBRUSH hBrushFill, hBrushFrame;
	static HBITMAP hBmMem;						//Двойная буферизация - битмап
	static HDC hdcMem;							//Двойная буферизация - контекст
	RECT rect;
	static FILE *Map;

	switch (iMsg)
	{
	case WM_CREATE:
		X0 = 0;
		Y0 = 0;
		GridEditSize = GetSystemMetrics(SM_CYFULLSCREEN) / MapEditSize;
		cxEditClient = MapEditSize * GridEditSize + 1;
		cyEditClient = MapEditSize * GridEditSize + 1;
		
		//Готовим контекст устройства для рисования
		hdc = GetDC(hMapEditorWnd);

		hBmEditGrid = CreateCompatibleBitmap(hdc, MapEditSize * GridEditSize + 1, MapEditSize * GridEditSize + 1);
		hdcMemEditGrid = CreateCompatibleDC(hdc);
		SelectObject(hdcMemEditGrid, hBmEditGrid);

		hBmEditMap = CreateCompatibleBitmap(hdc, MapEditSize * GridEditSize + 1, MapEditSize * GridEditSize + 1);
		hdcMemEditMap = CreateCompatibleDC(hdc);
		SelectObject(hdcMemEditMap, hBmEditMap);
		/*
		hBmMem = CreateCompatibleBitmap(hdc, MapEditSize * GridEditSize + 1, MapEditSize * GridEditSize + 1);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmMem);
		*/
		hBrushFill = CreateSolidBrush(RGB(127, 127, 127));
		hBrushFrame = CreateSolidBrush(RGB(202, 202, 202));

		InitMap(0);
		DrawGrid();
		DrawMap();
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_OPENMAP:
			if (Map)
			{
				fclose(Map);
				Map = NULL;
			}

			Map = fopen("Map.mp", "r");
			if (!Map)
			{
				MessageBox(hMapEditorWnd, "Проверьте наличие файла Map.mp в каталоге с программой!",
					"Ошибка открытия файла карты", MB_OK);
			}
			else
			{
				DeleteMap();
				InitMap(Map);
			}
			DrawMap();
			InvalidateRect(hMapEditorWnd, NULL, FALSE);
			break;
		case IDC_SAVEMAP:
			if (Map)
			{
				fclose(Map);
				Map = NULL;
			}

			Map = fopen("Map.mp", "w");
			if (!Map)
			{
				MessageBox(hMapEditorWnd, "Проверьте наличие файла Map.mp в каталоге с программой!",
					"Ошибка записи в файл карты", MB_OK);
			}
			else
			{
				WriteMap(Map);
				fclose(Map);
				Map = NULL;
				MessageBox(hMapEditorWnd, "Сохранение прошло успешно!",
					"Сохранить карту", MB_OK);
			}
			break;
		case IDC_POINT:
		case IDC_RECT:
		case IDC_ELL:
			Instrument = wParam;
			break;
		case IDC_D_ALL:
			for (int i = 0; i < MapEditSize; i++)
				for (int j = 0; j < MapEditSize; j++)
					MapEdit[i][j] = 1;
			DrawMap();
			InvalidateRect(hMapEditorWnd, NULL, FALSE);
			break;
		case IDC_C_ALL:
			for (int i = 0; i < MapEditSize; i++)
				for (int j = 0; j < MapEditSize; j++)
					MapEdit[i][j] = 0;
			DrawMap();
			InvalidateRect(hMapEditorWnd, NULL, FALSE);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		X0 = LOWORD(lParam);
		Y0 = HIWORD(lParam);
		if (X0 / GridEditSize >= MapEditSize) X0 = MapEditSize * GridEditSize - 1;
		if (Y0 / GridEditSize >= MapEditSize) Y0 = MapEditSize * GridEditSize - 1;

		if (Instrument == IDC_POINT)
		{
			MapEdit[X0 / GridEditSize][Y0 / GridEditSize] = 1;
			DrawMap();
			InvalidateRect(hMapEditorWnd, NULL, FALSE);
		}
		
		break;
	case WM_RBUTTONDOWN:
		X0 = LOWORD(lParam);
		Y0 = HIWORD(lParam);
		if (X0 / GridEditSize >= MapEditSize) X0 = MapEditSize * GridEditSize - 1;
		if (Y0 / GridEditSize >= MapEditSize) Y0 = MapEditSize * GridEditSize - 1;

		if (Instrument == IDC_POINT)
		{
			MapEdit[X0 / GridEditSize][Y0 / GridEditSize] = 0;
			DrawMap();
			InvalidateRect(hMapEditorWnd, NULL, FALSE);
		}
		
		break;
	case WM_LBUTTONUP:
		X1 = LOWORD(lParam);
		Y1 = HIWORD(lParam);
		if (X1 / GridEditSize >= MapEditSize) X1 = MapEditSize * GridEditSize - 1;
		if (Y1 / GridEditSize >= MapEditSize) Y1 = MapEditSize * GridEditSize - 1;
		
		switch (Instrument)
		{
		case IDC_POINT:
			MapEdit[X1 / GridEditSize][Y1 / GridEditSize] = 1;
			break;
		case IDC_RECT:
			if (X1 < X0)
			{
				int b = X1;
				X1 = X0;
				X0 = b;
			}
			if (Y1 < Y0)
			{
				int b = Y1;
				Y1 = Y0;
				Y0 = b;
			}

			for (int i = X0 / GridEditSize; i < X1 / GridEditSize; i++)
				for (int j = Y0 / GridEditSize; j < Y1 / GridEditSize; j++)
					MapEdit[i][j] = 1;
			break;
		}
		
		DrawMap();
		InvalidateRect(hMapEditorWnd, NULL, FALSE);
		break;
	case WM_RBUTTONUP:
		X1 = LOWORD(lParam);
		Y1 = HIWORD(lParam);
		if (X1 / GridEditSize >= MapEditSize) X1 = MapEditSize * GridEditSize - 1;
		if (Y1 / GridEditSize >= MapEditSize) Y1 = MapEditSize * GridEditSize - 1;

		switch (Instrument)
		{
		case IDC_POINT:
			MapEdit[X1 / GridEditSize][Y1 / GridEditSize] = 0;
			break;
		case IDC_RECT:
			if (X1 < X0)
			{
				int b = X1;
				X1 = X0;
				X0 = b;
			}
			if (Y1 < Y0)
			{
				int b = Y1;
				Y1 = Y0;
				Y0 = b;
			}
			
			for (int i = X0 / GridEditSize; i < X1 / GridEditSize; i++)
				for (int j = Y0 / GridEditSize; j < Y1 / GridEditSize; j++)
					MapEdit[i][j] = 0;
			break;
		}

		DrawMap();
		InvalidateRect(hMapEditorWnd, NULL, FALSE);
		break;
	case WM_MOUSEMOVE:
		if ((wParam & MK_LBUTTON) || (wParam & MK_RBUTTON))
		{
			X1 = LOWORD(lParam);
			Y1 = HIWORD(lParam);
			if (X1 / GridEditSize >= MapEditSize) X1 = MapEditSize * GridEditSize - 1;
			if (Y1 / GridEditSize >= MapEditSize) Y1 = MapEditSize * GridEditSize - 1;

			switch (Instrument)
			{
			case IDC_POINT:
				MapEdit[X1 / GridEditSize][Y1 / GridEditSize] = 2 - wParam;
				DrawMap();
				InvalidateRect(hMapEditorWnd, NULL, FALSE);
				break;
			case IDC_RECT:
				SetRect(&rect, ClientToGrid((X0 < X1) ? X0 : X1), ClientToGrid((Y0 < Y1) ? Y0 : Y1),
					ClientToGrid((X0 < X1) ? X1 : X0), ClientToGrid((Y0 < Y1) ? Y1 : Y0));
				SendMessage(hMapEditorWnd, WM_PAINT, 0, 0);
				DrawMap();
				FillRect(hdcMemEditMap, &rect, (wParam & MK_LBUTTON) ? hBrushFill : (HBRUSH)GetStockObject(BLACK_BRUSH));
				FrameRect(hdcMemEditMap, &rect, (wParam & MK_LBUTTON) ? hBrushFrame : (HBRUSH)GetStockObject(BLACK_BRUSH));
				InvalidateRect(hMapEditorWnd, NULL, FALSE);
				break;
			}
		}

		break;
	case WM_PAINT:
		BitBlt(hdc, 0, 0, cxEditClient, cyEditClient, hdcMemEditMap, 0, 0, SRCCOPY);
		//BitBlt(hdc, 0, 0, cxEditClient, cyEditClient, hdcMem, 0, 0, SRCCOPY);
		break;
	case WM_DESTROY:
		DeleteDC(hdcMemEditGrid);
		hdcMemEditGrid = NULL;
		DeleteObject(hBmEditGrid);
		hBmEditGrid = NULL;

		DeleteDC(hdcMemEditMap);
		hdcMemEditMap = NULL;
		DeleteObject(hBmEditMap);
		hBmEditMap = NULL;

		DeleteObject(hBrushFill);
		DeleteObject(hBrushFrame);

		DeleteMap();

		ReleaseDC(hMapEditorWnd, hdc);
		DestroyWindow(hMapEditorWnd);
		break;
	}
	return DefWindowProc(hMapEditorWnd, iMsg, wParam, lParam);
}

bool RegisterMapEditorClass(HINSTANCE hInst)
{
	WNDCLASS wnd;

	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wnd.hInstance = hInst;
	wnd.lpfnWndProc = MapEditorWnd;
	wnd.lpszClassName = "MapEditor";
	wnd.lpszMenuName = "MapEditorMenu";
	wnd.style = 0;

	RegisterClass(&wnd);
	return FALSE;
}