// lab1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "lab1.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Мои переменные:
const int delta = 5;
RECT wndRect;
HBITMAP hBmp;
BITMAP bmp;
BOOL isMoving = false;
struct figureInfo
{
    int x;
    int y;
    int width;
    int height;
} smile;

// Объявление функций
void InitSmile(figureInfo* smile, int x, int y, int width, int height);

void MoveKeyboard(HWND hWnd, figureInfo* smile, WPARAM wParam, LPARAM lParam, int delta);

void MoveMouse(HWND hWnd, figureInfo* smile, WPARAM wParam, LPARAM lParam);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_WINDOW_NAME, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+2);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_LAB1));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            GetClientRect(hWnd, &wndRect);

            hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
            GetObject(hBmp, sizeof(bmp), &bmp);
            if (hBmp == NULL)
                MessageBox(hWnd, L"Невозможно загрузить изображение!", L"Ошибка", MB_OK | MB_ICONERROR);
          
            InitSmile(&smile, wndRect.right / 2, wndRect.bottom / 2, 100, 100);
        }
        break;

        case WM_PAINT:
        {
            HBITMAP hPrevBmp;
            HDC hMemDc;
            PAINTSTRUCT ps;

            GetClientRect(hWnd, &wndRect);
            HDC hWndDc = BeginPaint(hWnd, &ps);
            hMemDc = CreateCompatibleDC(hWndDc);
            hPrevBmp = (HBITMAP)SelectObject(hMemDc, hBmp);
            smile.width = bmp.bmWidth;
            smile.height = bmp.bmHeight;

            BitBlt(hWndDc, smile.x - smile.width / 2, smile.y - smile.height / 2, bmp.bmWidth, bmp.bmHeight,
                hMemDc, 0, 0, SRCAND);
            SelectObject(hMemDc, hBmp);

            DeleteDC(hMemDc);
            DeleteObject(hPrevBmp);

            EndPaint(hWnd, &ps);
        }
        break;

        case WM_LBUTTONDOWN:
        {
            POINT ptMousePos = { LOWORD(lParam), HIWORD(lParam) };
            isMoving = true;
            InvalidateRgn(hWnd, NULL, TRUE);
        }
        break;

        case WM_MOUSEMOVE:
        {
            MoveMouse(hWnd, &smile, wParam, lParam);
        }
        break;

        case WM_LBUTTONUP:
        {
            isMoving = false;
        }
        break;

        case WM_KEYDOWN:
        {
            MoveKeyboard(hWnd, &smile, wParam, lParam, delta);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_MOUSEWHEEL:
        {
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_DESTROY:
            DeleteObject(hBmp);
            PostQuitMessage(0);

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



void InitSmile(figureInfo *smile, int x, int y, int width, int height)
{
    smile->x = x;
    smile->y = y;
    smile->height = height;
    smile->width = width;
}

void MoveMouse(HWND hWnd, figureInfo* smile, WPARAM wParam, LPARAM lParam)
{
    if (isMoving) {
        smile->x = LOWORD(lParam);
        smile->y = HIWORD(lParam);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void MoveKeyboard(HWND hWnd, figureInfo* smile, WPARAM wParam, LPARAM lParam, int delta)
{
    switch (LOWORD(wParam))
    {
        case VK_LEFT:
            smile->x -= delta;
        break;

        case VK_UP:
            smile->y -= delta;
        break;

        case VK_RIGHT:
            smile->x += delta;
        break;

        case VK_DOWN:
            smile->y += delta;
        break;
    }
}

void MoveMouseWheel()
{

}