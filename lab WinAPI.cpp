// lab WinAPI.cpp : Definiuje punkt wejścia dla aplikacji.
//
#pragma comment(lib, "Msimg32.lib")

#include "framework.h"
#include "lab WinAPI.h"
#include <ctime>
#include <fstream>

#define MAX_LOADSTRING 100
#define ANIMATION_SPEED 25
#define BOARDSIZE 4
#define FIELDSIZE 60
#define BORDER 10
#define STARTX 100
#define STARTY 100

// Zmienne globalne:
HINSTANCE hInst;                                // bieżące wystąpienie
RECT globrc;
HWND hWnd1;
HWND hWnd2;
HWND children1[BOARDSIZE][BOARDSIZE] = { NULL };
HWND children2[BOARDSIZE][BOARDSIZE] = { NULL };
HWND score1;
HWND score2;
HWND end1;
HWND end2;
int valarr[BOARDSIZE][BOARDSIZE] = { 0 };
int offset[BOARDSIZE][BOARDSIZE] = { 0 };
int onboard = 0;
bool playing = false;
int goal = 2048;
int score = 0;
bool read = false;
bool win = false;

// Przekaż dalej deklaracje funkcji dołączone w tym module kodu:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM                MyRegisterChildClass(HINSTANCE hInstance);
ATOM				MyRegisterScoreChildClass(HINSTANCE hInstance);
ATOM				MyRegisterEndChildClass(HINSTANCE hInstance);
LRESULT CALLBACK	ChildProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ScoreProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	EndProc(HWND, UINT, WPARAM, LPARAM);
COLORREF			GetColor(int);
void				Getij(HWND hWnd, int& i, int& j);
void				CreateNew();
bool				CheckBoard();
void				SetMenuGoal(int goal);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: W tym miejscu umieść kod.
	srand(time(NULL));

	// Inicjuj ciągi globalne
	MyRegisterClass(hInstance);
	MyRegisterChildClass(hInstance);
	MyRegisterScoreChildClass(hInstance);
	MyRegisterEndChildClass(hInstance);

	// Wykonaj inicjowanie aplikacji:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABWINAPI));

	MSG msg;

	// Główna pętla komunikatów:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(250, 247, 238)));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LABWINAPI);
	wcex.lpszClassName = L"LABWINAPI";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

ATOM MyRegisterChildClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ChildProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(204, 192, 174)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"Child";
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

ATOM MyRegisterScoreChildClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ScoreProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(204, 192, 174)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"Score";
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

ATOM MyRegisterEndChildClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = EndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0, 255, 0)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"End";
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Przechowuj dojście wystąpienia w naszej zmiennej globalnej

	globrc.left = STARTX;
	globrc.top = STARTY;
	globrc.right = globrc.left + BOARDSIZE * FIELDSIZE + (BOARDSIZE + 1) * BORDER;
	globrc.bottom = globrc.top + (BOARDSIZE + 1) * FIELDSIZE + (BOARDSIZE + 2) * BORDER;
	AdjustWindowRect(&globrc, WS_OVERLAPPEDWINDOW, TRUE);

	int sysWidth = GetSystemMetrics(SM_CXSCREEN);
	int sysHeight = GetSystemMetrics(SM_CYSCREEN);

	hWnd1 = CreateWindowW(L"LABWINAPI", L"2048", WS_OVERLAPPEDWINDOW,
		globrc.left, globrc.top, globrc.right - globrc.left, globrc.bottom - globrc.top, nullptr, nullptr, hInstance, nullptr);

	hWnd2 = CreateWindowW(L"LABWINAPI", L"2048", WS_OVERLAPPEDWINDOW,
		sysWidth - globrc.right, sysHeight - globrc.bottom, globrc.right - globrc.left, globrc.bottom - globrc.top, hWnd1, nullptr, hInstance, nullptr);

	if (!hWnd1 || !hWnd2)
	{
		return FALSE;
	}

	ShowWindow(hWnd1, nCmdShow);
	UpdateWindow(hWnd1);
	ShowWindow(hWnd2, nCmdShow);
	UpdateWindow(hWnd2);

	SetMenuGoal(goal);

	score1 = CreateWindowW(L"Score", nullptr, WS_CHILD | WS_VISIBLE,
		BORDER, BORDER, FIELDSIZE * BOARDSIZE + BORDER * (BOARDSIZE - 1), FIELDSIZE,
		hWnd1, nullptr, hInstance, nullptr);
	score2 = CreateWindowW(L"Score", nullptr, WS_CHILD | WS_VISIBLE,
		BORDER, BORDER, FIELDSIZE * BOARDSIZE + BORDER * (BOARDSIZE - 1), FIELDSIZE,
		hWnd2, nullptr, hInstance, nullptr);

	for (int i = 0; i < BOARDSIZE; i++)
		for (int j = 0; j < BOARDSIZE; j++)
		{
			int xPos = i * BORDER + i * FIELDSIZE, yPos = (j + 1) * BORDER + (j + 1) * FIELDSIZE;
			children1[j][i] = CreateWindowW(L"Child", nullptr, WS_CHILD | WS_VISIBLE,
				xPos, yPos, FIELDSIZE + BORDER * 2, FIELDSIZE + BORDER * 2,
				hWnd1, nullptr, hInstance, nullptr);
			children2[j][i] = CreateWindowW(L"Child", nullptr, WS_CHILD | WS_VISIBLE,
				xPos, yPos, FIELDSIZE + BORDER * 2, FIELDSIZE + BORDER * 2,
				hWnd2, nullptr, hInstance, nullptr);
		}

	end1 = CreateWindowW(L"End", nullptr, WS_CHILD | WS_VISIBLE,
		0, 0, FIELDSIZE * BOARDSIZE + BORDER * (BOARDSIZE + 1), FIELDSIZE * (BOARDSIZE + 1) + BORDER * (BOARDSIZE + 2),
		hWnd1, nullptr, hInstance, nullptr);
	end2 = CreateWindowW(L"End", nullptr, WS_CHILD | WS_VISIBLE,
		0, 0, FIELDSIZE * BOARDSIZE + BORDER * (BOARDSIZE + 1), FIELDSIZE * (BOARDSIZE + 1) + BORDER * (BOARDSIZE + 2),
		hWnd2, nullptr, hInstance, nullptr);

	if (playing)
	{
		ShowWindow(end1, SW_HIDE);
		ShowWindow(end2, SW_HIDE);
	}
	else
	{
		ShowWindow(end1, SW_SHOW);
		ShowWindow(end2, SW_SHOW);
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		if (!read)
		{
			std::string temp;
			std::ifstream file("2048.ini");
			if (file.good())
			{
				file >> temp;
				playing = atoi(temp.c_str());
				file >> temp;
				win = atoi(temp.c_str());
				file >> temp;
				score = atoi(temp.c_str());
				file >> temp;
				goal = atoi(temp.c_str());
				for (int i = 0; i < BOARDSIZE; i++)
					for (int j = 0; j < BOARDSIZE; j++)
					{
						file >> temp;
						valarr[i][j] = atoi(temp.c_str());
					}
				file.close();
			}

			read = true;
		}
	}
	case WM_KEYDOWN:
	{
		if (playing)
		{
			bool spawn = false;
			bool merged[BOARDSIZE][BOARDSIZE];
			for (int i = 0; i < BOARDSIZE; i++)
				for (int j = 0; j < BOARDSIZE; j++)
				{
					merged[i][j] = false;
					offset[i][j] = 0;
				}

			switch (wParam)
			{
			case 'W':
				for (int j = 0; j < 4; j++)
				{
					for (int i = 1; i < 4; i++)
						if (valarr[i][j] != 0)
							for (int k = i - 1; k >= 0; k--)
							{
								if (valarr[k][j] == 0)
								{
									valarr[k][j] = valarr[k + 1][j];
									valarr[k + 1][j] = 0;
									spawn = true;
								}
								else if (valarr[k][j] == valarr[k + 1][j] && !merged[k][j])
								{
									valarr[k][j] *= 2;
									valarr[k + 1][j] = 0;
									onboard--;
									score += valarr[k][j];
									merged[k][j] = true;
									spawn = true;
									if (playing)
									{
										offset[k][j] = 10;
										SetTimer(children1[k][j], k * BOARDSIZE + j, ANIMATION_SPEED, nullptr);
										SetTimer(children2[k][j], k * BOARDSIZE + j, ANIMATION_SPEED, nullptr);
									}

									if (valarr[k][j] == goal)
									{
										playing = false;
										win = true;
										offset[k][j] = 0;
										InvalidateRect(children1[k][j], NULL, TRUE);
										InvalidateRect(children2[k][j], NULL, TRUE);
										ShowWindow(end1, SW_SHOW);
										ShowWindow(end2, SW_SHOW);
									}
									break;	//ponieważ mam odpowiednią kolejność przesuwania to po mergu na pewno nie ma pustych więc mogę breakować, żeby się nie mergowało dalej
								}
								else break;
							}
				}
				break;
			case 'S':
				for (int j = 0; j < 4; j++)
				{
					for (int i = 2; i >= 0; i--)
						if (valarr[i][j] != 0)
							for (int k = i + 1; k < 4; k++)
							{
								if (valarr[k][j] == 0)
								{
									valarr[k][j] = valarr[k - 1][j];
									valarr[k - 1][j] = 0;
									spawn = true;
								}
								else if (valarr[k][j] == valarr[k - 1][j] && !merged[k][j])
								{
									valarr[k][j] *= 2;
									valarr[k - 1][j] = 0;
									onboard--;
									score += valarr[k][j];
									merged[k][j] = true;
									spawn = true;
									if (playing)
									{
										offset[k][j] = 10;
										SetTimer(children1[k][j], k * BOARDSIZE + j, ANIMATION_SPEED, nullptr);
										SetTimer(children2[k][j], k * BOARDSIZE + j, ANIMATION_SPEED, nullptr);
									}

									if (valarr[k][j] == goal)
									{
										playing = false;
										win = true;
										offset[k][j] = 0;
										InvalidateRect(children1[k][j], NULL, TRUE);
										InvalidateRect(children2[k][j], NULL, TRUE);
										ShowWindow(end1, SW_SHOW);
										ShowWindow(end2, SW_SHOW);
									}
									break;
								}
								else break;
							}
				}
				break;
			case 'A':
				for (int i = 0; i < 4; i++)
				{
					for (int j = 1; j < 4; j++)
						if (valarr[i][j] != 0)
							for (int k = j - 1; k >= 0; k--)
							{
								if (valarr[i][k] == 0)
								{
									valarr[i][k] = valarr[i][k + 1];
									valarr[i][k + 1] = 0;
									spawn = true;
								}
								else if (valarr[i][k] == valarr[i][k + 1] && !merged[i][k])
								{
									valarr[i][k] *= 2;
									valarr[i][k + 1] = 0;
									onboard--;
									score += valarr[i][k];
									merged[i][k] = true;
									spawn = true;
									if (playing)
									{
										offset[i][k] = 10;
										SetTimer(children1[i][k], i * BOARDSIZE + k, ANIMATION_SPEED, nullptr);
										SetTimer(children2[i][k], i * BOARDSIZE + k, ANIMATION_SPEED, nullptr);
									}

									if (valarr[i][k] == goal)
									{
										playing = false;
										win = true;
										offset[i][k] = 0;
										InvalidateRect(children1[i][k], NULL, TRUE);
										InvalidateRect(children2[i][k], NULL, TRUE);
										ShowWindow(end1, SW_SHOW);
										ShowWindow(end2, SW_SHOW);
									}
									break;
								}
								else break;
							}
				}
				break;
			case 'D':
				for (int i = 0; i < 4; i++)
				{
					for (int j = 2; j >= 0; j--)
						if (valarr[i][j] != 0)
							for (int k = j + 1; k < 4; k++)
							{
								if (valarr[i][k] == 0)
								{
									valarr[i][k] = valarr[i][k - 1];
									valarr[i][k - 1] = 0;
									spawn = true;
								}
								else if (valarr[i][k] == valarr[i][k - 1] && !merged[i][k])
								{
									valarr[i][k] *= 2;
									valarr[i][k - 1] = 0;
									onboard--;
									score += valarr[i][k];
									merged[i][k] = true;
									spawn = true;
									if (playing)
									{
										offset[i][k] = 10;
										SetTimer(children1[i][k], i * BOARDSIZE + k, ANIMATION_SPEED, nullptr);
										SetTimer(children2[i][k], i * BOARDSIZE + k, ANIMATION_SPEED, nullptr);
									}

									if (valarr[i][k] == goal)
									{
										playing = false;
										win = true;
										offset[i][k] = 0;
										InvalidateRect(children1[i][k], NULL, TRUE);
										InvalidateRect(children2[i][k], NULL, TRUE);
										ShowWindow(end1, SW_SHOW);
										ShowWindow(end2, SW_SHOW);
									}
									break;
								}
								else break;
							}
				}
				break;
			}
			if (spawn)	CreateNew();
			for (int i = 0; i < BOARDSIZE; i++)
				for (int j = 0; j < BOARDSIZE; j++)
				{
					InvalidateRect(children1[i][j], NULL, TRUE);
					InvalidateRect(children2[i][j], NULL, TRUE);
				}
			InvalidateRect(score1, NULL, TRUE);
			InvalidateRect(score2, NULL, TRUE);
		}
		else
		{
			ShowWindow(end1, SW_SHOW);
			ShowWindow(end2, SW_SHOW);
		}
	}
	break;
	case WM_MOVING:
	{
		RECT* rc = (RECT*)lParam;
		RECT thisrc;
		int sysWidth = GetSystemMetrics(SM_CXSCREEN);
		int sysHeight = GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect(hWnd, &thisrc);
		HWND temphWnd = hWnd == hWnd1 ? hWnd2 : hWnd1;

		MoveWindow(temphWnd, sysWidth - rc->right, sysHeight - rc->bottom, thisrc.right - thisrc.left, thisrc.bottom - thisrc.top, TRUE);

		if (thisrc.right > sysWidth / 2 && thisrc.left < sysWidth / 2 && thisrc.bottom > sysHeight / 2 && thisrc.top < sysHeight / 2)
		{
			SetWindowLong(hWnd2, GWL_EXSTYLE, GetWindowLong(hWnd2, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hWnd2, 0, (255 * 50) / 100, LWA_ALPHA);
		}
		else
		{
			SetWindowLong(hWnd2, GWL_EXSTYLE, GetWindowLong(hWnd2, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hWnd2, 0, 255, LWA_ALPHA);
		}
	}
	break;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;

		minMaxInfo->ptMaxSize.x = minMaxInfo->ptMaxTrackSize.x = minMaxInfo->ptMinTrackSize.x = globrc.right - globrc.left;
		minMaxInfo->ptMaxSize.y = minMaxInfo->ptMaxTrackSize.y = minMaxInfo->ptMinTrackSize.y = globrc.bottom - globrc.top;
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		switch (wmId)
		{
		case ID_GAME_NEWGAME:
			for (int i = 0; i < BOARDSIZE; i++)
				for (int j = 0; j < BOARDSIZE; j++)
				{
					valarr[i][j] = 0;
					offset[i][j] = 0;
					InvalidateRect(children1[i][j], NULL, TRUE);
					InvalidateRect(children2[i][j], NULL, TRUE);
				}
			playing = true;
			onboard = 0;
			score = 0;
			InvalidateRect(score1, NULL, TRUE);
			InvalidateRect(score2, NULL, TRUE);
			CreateNew();
			break;
		case ID_GOAL_8:
			SetMenuGoal(8);
			goal = 8;
			break;
		case ID_GOAL_16:
			SetMenuGoal(16);
			goal = 16;
			break;
		case ID_GOAL_64:
			SetMenuGoal(64);
			goal = 64;
			break;
		case ID_GOAL_2048:
			SetMenuGoal(2048);
			goal = 2048;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		if (hWnd == hWnd1)
		{
			std::ofstream file("2048.ini");
			file << playing << '\n' << win << '\n' << score << '\n' << goal << '\n';
			for (int i = 0; i < BOARDSIZE; i++)
			{
				for (int j = 0; j < BOARDSIZE; j++)
					file << valarr[i][j] << '\n';
				file << '\n';
			}
			file.close();
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:
	{
		int i = wParam / BOARDSIZE, j = wParam % BOARDSIZE;
		if (offset[i][j] > 0)
			offset[i][j]--;
		else if (offset[i][j] < 0)
			offset[i][j]++;
		else
			KillTimer(hWnd, wParam);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_PAINT:
	{
		int i, j;
		Getij(hWnd, i, j);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rc;
		GetClientRect(hWnd, &rc);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));
		HRGN rgn = CreateRoundRectRgn(rc.left + BORDER - offset[i][j], rc.top + BORDER - offset[i][j],
			rc.right - BORDER + offset[i][j], rc.bottom - BORDER + offset[i][j], 10, 10);
		HRGN oldrgn = (HRGN)SelectObject(hdc, rgn);
		HPEN pen = CreatePen(PS_SOLID, 1, GetColor(valarr[i][j]));
		HPEN oldPen = (HPEN)SelectObject(hdc, pen);
		HBRUSH brush = CreateSolidBrush(GetColor(valarr[i][j]));
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
		SetWindowRgn(hWnd, rgn, TRUE);
		Rectangle(hdc, 0, 0, FIELDSIZE + 2 * BORDER, FIELDSIZE + 2 * BORDER);
		if (valarr[i][j] > 0)
		{
			HFONT font = CreateFont(
				-MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72),
				0,
				0,
				0,
				FW_BOLD,
				false,
				FALSE,
				0,
				EASTEUROPE_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				L"Comic Sans MS");
			HFONT oldfont = (HFONT)SelectObject(hdc, font);

			TCHAR s[100];
			_stprintf_s(s, TEXT("%d"), valarr[i][j]);
			DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			SelectObject(hdc, oldfont);
			DeleteObject(font);
		}
		SelectObject(hdc, oldPen);
		DeleteObject(pen);
		SelectObject(hdc, oldBrush);
		DeleteObject(brush);
		SelectObject(hdc, oldrgn);
		DeleteObject(rgn);
		EndPaint(hWnd, &ps);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ScoreProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rc;
		GetClientRect(hWnd, &rc);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));
		HRGN rgn = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 10, 10);
		HRGN oldrgn = (HRGN)SelectObject(hdc, rgn);
		SetWindowRgn(hWnd, rgn, TRUE);

		HFONT font = CreateFont(
			-MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72),
			0,
			0,
			0,
			FW_BOLD,
			false,
			FALSE,
			0,
			EASTEUROPE_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			L"Comic Sans MS");
		HFONT oldfont = (HFONT)SelectObject(hdc, font);

		TCHAR s[100];
		_stprintf_s(s, TEXT("%d"), score);
		DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		SelectObject(hdc, oldfont);
		DeleteObject(font);
		SelectObject(hdc, oldrgn);
		DeleteObject(rgn);
		EndPaint(hWnd, &ps);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK EndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
		return 1;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (playing)
		{
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
			ShowWindow(hWnd, SW_HIDE);
		}
		else
		{
			for (int i = 0; i < BOARDSIZE; i++)
				for (int j = 0; j < BOARDSIZE; j++)
				{
					KillTimer(children1[i][j], i * BOARDSIZE + j);
					KillTimer(children2[i][j], i * BOARDSIZE + j);
					offset[i][j] = 0;
				}
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			ShowWindow(hWnd, SW_SHOW);

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255, 255, 255));
			HFONT font = CreateFont(
				-MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72),
				0,
				0,
				0,
				FW_BOLD,
				false,
				FALSE,
				0,
				EASTEUROPE_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				L"Comic Sans MS");
			HFONT oldfont = (HFONT)SelectObject(hdc, font);

			RECT rc;
			GetClientRect(hWnd, &rc);

			HDC hdcTemp = CreateCompatibleDC(hdc);
			HBITMAP hbmpTemp = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
			HBITMAP oldhbmpTemp = (HBITMAP)SelectObject(hdcTemp, hbmpTemp);

			HBRUSH brush;
			HPEN pen;
			TCHAR s[100];

			if (win)
			{
				brush = CreateSolidBrush(RGB(0, 255, 0));
				pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
				_stprintf_s(s, TEXT("Win! :)"));
			}
			else
			{
				brush = CreateSolidBrush(RGB(255, 0, 0));
				pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
				_stprintf_s(s, TEXT("Defeat! :("));
			}

			HBRUSH oldBrush = (HBRUSH)SelectObject(hdcTemp, brush);
			HPEN oldPen = (HPEN)SelectObject(hdcTemp, pen);

			Rectangle(hdcTemp, rc.left, rc.top, rc.right, rc.bottom);

			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255 / 2;
			bf.AlphaFormat = 0;

			AlphaBlend(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcTemp, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, bf);

			DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			SelectObject(hdc, oldfont);
			DeleteObject(font);
			SelectObject(hdcTemp, oldBrush);
			DeleteObject(brush);
			SelectObject(hdcTemp, oldPen);
			DeleteObject(pen);
			SelectObject(hdcTemp, oldhbmpTemp);
			DeleteObject(hbmpTemp);
			ReleaseDC(hWnd, hdcTemp);
			DeleteDC(hdcTemp);
		}
		EndPaint(hWnd, &ps);
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Getij(HWND hWnd, int& i, int& j)
{
	for (i = 0; i < BOARDSIZE; i++)
		for (j = 0; j < BOARDSIZE; j++)
			if (hWnd == children1[i][j] || hWnd == children2[i][j])
				return;
}

COLORREF GetColor(int val)
{
	switch (val)
	{
	case 0:
		return RGB(204, 192, 174);
	case 2:
		return RGB(238, 228, 198);
	case 4:
		return RGB(239, 225, 218);
	case 8:
		return RGB(243, 179, 124);
	case 16:
		return RGB(246, 153, 100);
	case 32:
		return RGB(246, 125, 98);
	case 64:
		return RGB(247, 93, 60);
	case 128:
		return RGB(237, 206, 116);
	case 256:
		return RGB(239, 204, 98);
	case 512:
		return RGB(243, 201, 85);
	case 1024:
		return RGB(238, 200, 72);
	case 2048:
		return RGB(239, 192, 47);
	default:
		return RGB(255, 0, 255);
	}
}

void CreateNew()
{
	int newx, newy;
	do
	{
		newx = rand() % BOARDSIZE;
		newy = rand() % BOARDSIZE;
	} while (valarr[newy][newx] != 0);

	valarr[newy][newx] = 2;
	onboard++;

	if (onboard == BOARDSIZE * BOARDSIZE)
		playing = CheckBoard();

	if (playing)
	{
		offset[newy][newx] = -10;
		SetTimer(children1[newy][newx], newy * BOARDSIZE + newx, ANIMATION_SPEED, nullptr);
		SetTimer(children2[newy][newx], newy * BOARDSIZE + newx, ANIMATION_SPEED, nullptr);
	}
}

bool CheckBoard()
{
	for (int i = 0; i < BOARDSIZE; i++)
		for (int j = 0; j < BOARDSIZE; j++)
			if ((i > 0 && valarr[i][j] == valarr[i - 1][j]) ||
				(i < BOARDSIZE - 1 && valarr[i][j] == valarr[i + 1][j]) ||
				(j > 0 && valarr[i][j] == valarr[i][j - 1]) ||
				(j < BOARDSIZE - 1 && valarr[i][j] == valarr[i][j + 1]))
				return true;

	win = false;
	return false;
}

void SetMenuGoal(int goal)
{
	HMENU hmenu1 = GetMenu(hWnd1);
	HMENU hmenu2 = GetMenu(hWnd2);

	CheckMenuItem(hmenu1, ID_GOAL_8, MF_UNCHECKED);
	CheckMenuItem(hmenu1, ID_GOAL_16, MF_UNCHECKED);
	CheckMenuItem(hmenu1, ID_GOAL_64, MF_UNCHECKED);
	CheckMenuItem(hmenu1, ID_GOAL_2048, MF_UNCHECKED);
	CheckMenuItem(hmenu2, ID_GOAL_8, MF_UNCHECKED);
	CheckMenuItem(hmenu2, ID_GOAL_16, MF_UNCHECKED);
	CheckMenuItem(hmenu2, ID_GOAL_64, MF_UNCHECKED);
	CheckMenuItem(hmenu2, ID_GOAL_2048, MF_UNCHECKED);

	switch (goal)
	{
	case 8:
		CheckMenuItem(hmenu1, ID_GOAL_8, MF_CHECKED);
		CheckMenuItem(hmenu2, ID_GOAL_8, MF_CHECKED);
		break;
	case 16:
		CheckMenuItem(hmenu1, ID_GOAL_16, MF_CHECKED);
		CheckMenuItem(hmenu2, ID_GOAL_16, MF_CHECKED);
		break;
	case 64:
		CheckMenuItem(hmenu1, ID_GOAL_64, MF_CHECKED);
		CheckMenuItem(hmenu2, ID_GOAL_64, MF_CHECKED);
		break;
	case 2048:
		CheckMenuItem(hmenu1, ID_GOAL_2048, MF_CHECKED);
		CheckMenuItem(hmenu2, ID_GOAL_2048, MF_CHECKED);
		break;
	}
}