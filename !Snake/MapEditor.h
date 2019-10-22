#pragma once
#define _INC_OLE
#include <Windows.h>
#undef _INC_OLE
#include <windowsx.h>
#include <stdio.h>
#include "MapEditorMenu.h"

static int cxEditClient, cyEditClient;

static HANDLE hBmEditGrid = NULL;		//Битмап для сетки
static HDC hdcMemEditGrid = NULL;		//Контекст памяти сетки

static HANDLE hBmEditMap = NULL;		//Битмап для карты
static HDC hdcMemEditMap = NULL;		//Контекст памяти карты
static int MapEditSize = 200;			//Размер карты
static int GridEditSize = 10;			//Исходный размер сетки
static char **MapEdit = NULL;					//Массив карты

static char Instrument = IDC_POINT;

LRESULT CALLBACK MapEditorWnd(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

bool RegisterMapEditorClass(HINSTANCE hInst);