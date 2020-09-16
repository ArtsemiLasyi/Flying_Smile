/*
    Лабораторная работа 1
    Изучение событийной архитекторы Windows-приложений, механизма обработки сообщений, 
        механизма перерисовки окна.

    - Разработать программу, позволяющую передвигать с помощью клавиатуры и мыши спрайт 
        (окрашенный прямоугольник или эллипс) внутри рабочей области окна.
    - Обеспечить работу колесика мыши. Прокручивание двигает спрайт по вертикали. 
        С удерживаемой клавишей Shift прокручивание колесика двигает спрайт по горизонтали.
    - Заменить спрайт на картинку с непрямоугольным контуром.
    - Придать спрайту движение с отскоком от границ окна.
*/

#include "framework.h"
#include "lab1.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Мои переменные:
const int delta = 20;
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

void MoveMouseWheel(HWND hWnd, figureInfo* smile, WPARAM wParam, LPARAM lParam, int delta);

void CorrectSmile(figureInfo* smile, RECT* wndRect, int delta);

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
            hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
            GetObject(hBmp, sizeof(bmp), &bmp);
            if (hBmp == NULL)
                MessageBox(hWnd, L"Невозможно загрузить изображение!", L"Ошибка", MB_OK | MB_ICONERROR);
         
            GetClientRect(hWnd, &wndRect);
            InitSmile(&smile, wndRect.right / 2, wndRect.bottom / 2, 0, 0);
        }
        break;

        case WM_PAINT:
        {
            HBITMAP hPrevBmp;
            HDC hMemDc;
            PAINTSTRUCT ps;

            HDC hWndDc = BeginPaint(hWnd, &ps);
            hMemDc = CreateCompatibleDC(hWndDc);
            hPrevBmp = (HBITMAP)SelectObject(hMemDc, hBmp);
           
            smile.width = bmp.bmWidth;
            smile.height = bmp.bmHeight;
            GetClientRect(hWnd, &wndRect);
            CorrectSmile(&smile, &wndRect, delta);
           
            BitBlt(hWndDc, smile.x, smile.y, bmp.bmWidth, bmp.bmHeight,
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
            InvalidateRect(hWnd, NULL, TRUE);
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
            MoveMouseWheel(hWnd, &smile, wParam, lParam, delta);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_DESTROY:
        {
            DeleteObject(hBmp);
            PostQuitMessage(0);
        }
        break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void CorrectSmile(figureInfo *smile, RECT *wndRect, int delta)
{
    delta *= 2;
    BOOL condition1 = (smile->x + smile->width) >= wndRect->right;
    BOOL condition2 = smile->x <= 0;
    BOOL condition3 = (smile->y + smile->height) >= wndRect->bottom;
    BOOL condition4 = smile->y <= 0;
    if (condition1)
        smile->x -= delta;
    if (condition2)
        smile->x += delta;
    if (condition3)
        smile->y -= delta;
    if (condition4)
        smile->y += delta;
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

void MoveMouseWheel(HWND hWnd, figureInfo* smile, WPARAM wParam, LPARAM lParam, int delta)
{
    int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    int sign;
    if (wheelDelta > 0)
        sign = 1;
    else
        sign = -1;
    if (LOWORD(wParam) != MK_SHIFT)
        smile->y -= sign * delta;
    else
        smile->x -= sign * delta;
}