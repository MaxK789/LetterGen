//#include <windows.h>
//#include "LetterGen.h"
//#include <vector>
//#include <string>
//
//// Глобальні змінні:
//HINSTANCE hInst;
//LPCTSTR szWindowClass = TEXT("QWERTY");
//LPCTSTR szTitle = TEXT("ПЕРША ПРОГРАМА");
//HWND hwndList1, hwndList2;
//
//// Прототипи функцій:
//ATOM MyRegisterClass(HINSTANCE hInstance);
//BOOL InitInstance(HINSTANCE, int);
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
//// Основна програма WinMain:
//int APIENTRY WinMain(HINSTANCE hInstance,
//    HINSTANCE hPrevInstance,
//    LPSTR lpCmdLine,
//    int nCmdShow)
//{
//    MSG msg;
//
//    MyRegisterClass(hInstance);
//
//    if (!InitInstance(hInstance, nCmdShow))
//    {
//        return FALSE;
//    }
//
//    while (GetMessage(&msg, NULL, 0, 0))
//    {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    return (int)msg.wParam;
//}
//
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//    WNDCLASSEX wcex;
//    wcex.cbSize = sizeof(WNDCLASSEX);
//    wcex.style = CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc = WndProc;
//    wcex.cbClsExtra = 0;
//    wcex.cbWndExtra = 0;
//    wcex.hInstance = hInstance;
//    wcex.hIcon = LoadIcon(NULL, IDI_HAND);
//    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
//    wcex.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
//    wcex.lpszMenuName = NULL;
//    wcex.lpszClassName = szWindowClass;
//    wcex.hIconSm = NULL;
//    return RegisterClassEx(&wcex);
//}
//
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//    HWND hWnd;
//    hInst = hInstance;
//    hWnd = CreateWindow(szWindowClass,
//        szTitle,
//        WS_OVERLAPPEDWINDOW,
//        CW_USEDEFAULT,
//        CW_USEDEFAULT,
//        CW_USEDEFAULT,
//        CW_USEDEFAULT,
//        NULL,
//        NULL,
//        hInstance,
//        NULL);
//
//    if (!hWnd)
//    {
//        return FALSE;
//    }
//
//    CreateWindow(TEXT("BUTTON"),
//        TEXT("Натисни мене"),
//        WS_VISIBLE | WS_CHILD,
//        10,
//        10,
//        100,
//        30,
//        hWnd,
//        (HMENU)1,
//        hInstance,
//        NULL);
//
//    hwndList1 = CreateWindowA("LISTBOX", NULL,
//        WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
//        10, 50, 200, 400,
//        hWnd, (HMENU)2, hInstance, NULL);
//
//    hwndList2 = CreateWindowA("LISTBOX", NULL,
//        WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
//        220, 50, 200, 400,
//        hWnd, (HMENU)3, hInstance, NULL);
//
//    ShowWindow(hWnd, nCmdShow);
//    UpdateWindow(hWnd);
//
//    return TRUE;
//}
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    static std::vector<std::string> unsortedLetters;
//    static std::vector<std::string> sortedLetters;
//
//    switch (message)
//    {
//    case WM_COMMAND:
//    {
//        int wmId = LOWORD(wParam);
//        switch (wmId)
//        {
//        case 1:
//        {
//            const int length = 8;
//            char letters[length + 1] = { 0 };
//            GenerateRandomLetters(letters, length);
//
//            unsortedLetters.push_back(letters);
//            SendMessageA(hwndList1, LB_ADDSTRING, 0, (LPARAM)letters);
//
//            SortLetters(letters, length);
//
//            std::string sortedStr(letters);
//            sortedLetters.push_back(sortedStr);
//
//            std::vector<char*> sortedArray;
//            for (auto& str : sortedLetters) {
//                sortedArray.push_back(const_cast<char*>(str.c_str()));
//            }
//
//            SortStringList(sortedArray.data(), sortedArray.size());
//
//            SendMessageA(hwndList2, LB_RESETCONTENT, 0, 0);
//            for (const auto& str : sortedArray)
//            {
//                SendMessageA(hwndList2, LB_ADDSTRING, 0, (LPARAM)str);
//            }
//        }
//        break;
//        default:
//            return DefWindowProc(hWnd, message, wParam, lParam);
//        }
//    }
//    break;
//    case WM_PAINT:
//    {
//        PAINTSTRUCT ps;
//        HDC hdc = BeginPaint(hWnd, &ps);
//        EndPaint(hWnd, &ps);
//    }
//    break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}
