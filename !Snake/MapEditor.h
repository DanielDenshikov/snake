#pragma once
#define _INC_OLE
#include <Windows.h>
#undef _INC_OLE
#include <windowsx.h>
#include <stdio.h>
#include "MapEditorMenu.h"

static int cxEditClient, cyEditClient;

static HANDLE hBmEditGrid = NULL;		//������ ��� �����
static HDC hdcMemEditGrid = NULL;		//�������� ������ �����

static HANDLE hBmEditMap = NULL;		//������ ��� �����
static HDC hdcMemEditMap = NULL;		//�������� ������ �����
static int MapEditSize = 200;			//������ �����
static int GridEditSize = 10;			//�������� ������ �����
static char **MapEdit = NULL;					//������ �����

static char Instrument = IDC_POINT;

LRESULT CALLBACK MapEditorWnd(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

bool RegisterMapEditorClass(HINSTANCE hInst);