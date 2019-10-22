#pragma once

#define _INC_OLE
#include <Windows.h>
#undef _INC_OLE
#include <windowsx.h>
#include <process.h>
#include <stdio.h>
#include <ctime>

#define IDC_NEWGAME			0
#define IDC_CONTINUE		1
#define IDC_CHOOSEMAP		2
#define IDC_HELP			3
#define IDC_EXIT			4

#define WM_GAMEOVER			7

#define SNAKE_UP			0
#define SNAKE_DOWN			1
#define SNAKE_LEFT			2
#define SNAKE_RIGTH			3

struct POS
{
	int x = 0, y = 0;
	POS *Next = NULL, *Prev = NULL;
	bool active = true;
	bool dop = false;
};

#define WS_MENU		WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX