//#include <windows.h>
//#include "LetterGen.h"
//#include <vector>
//#include <string>
//#include <algorithm> // Для std::sort
//#include <thread>    // Для роботи з потоками
//
//// Глобальні змінні:
//HINSTANCE hInst;
//LPCTSTR szWindowClass = TEXT("QWERTY");
//LPCTSTR szTitle = TEXT("ПЕРША ПРОГРАМА");
//HWND hwndList1, hwndList2;
//std::vector<std::string> unsortedLetters;
//std::vector<std::string> sortedLetters;
//
//// Прототипи функцій:
//ATOM MyRegisterClass(HINSTANCE hInstance);
//BOOL InitInstance(HINSTANCE, int);
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
//// Функції потоків
//void FirstThreadFunc();
//void SecondThreadFunc();
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
//        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
//        10, 50, 200, 400,
//        hWnd, (HMENU)2, hInstance, NULL);
//
//    hwndList2 = CreateWindowA("LISTBOX", NULL,
//        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
//        220, 50, 200, 400,
//        hWnd, (HMENU)3, hInstance, NULL);
//
//    ShowWindow(hWnd, nCmdShow);
//    UpdateWindow(hWnd);
//
//    // Запуск першого потоку
//    std::thread firstThread(FirstThreadFunc);
//    firstThread.detach(); // Відділений потік
//
//    // Запуск другого потоку
//    std::thread secondThread(SecondThreadFunc);
//    secondThread.detach(); // Відділений потік
//
//    return TRUE;
//}
//
//// Функція першого потоку
//void FirstThreadFunc() {
//    for (int i = 0; i < 100; ++i) {
//        const int length = 8;
//        char letters[length + 1] = { 0 };
//        GenerateRandomLetters(letters, length);
//
//        // Отримані набори літер додаються до вектору unsortedLetters
//        unsortedLetters.push_back(letters);
//
//        // Виведення у перший компонент List
//        SendMessageA(hwndList1, LB_ADDSTRING, 0, (LPARAM)letters);
//
//    }
//}
//
//// Функція другого потоку
//void SecondThreadFunc() {
//    while (true) {
//        if (!unsortedLetters.empty()) {
//            // Отримання останнього набору літер
//            std::string lastLetters = unsortedLetters.back();
//            unsortedLetters.pop_back();
//
//            // Сортування набору літер з використанням функції з динамічної бібліотеки
//            //SortLetters(const_cast<char*>(lastLetters.c_str()), lastLetters.length());
//
//            // Додавання сортованого набору літер до вектору sortedLetters
//            sortedLetters.push_back(lastLetters);
//            std::vector<char*> sortedArray;
//            for (auto& str : sortedLetters) {
//                sortedArray.push_back(const_cast<char*>(str.c_str()));
//            }
//
//            SortStringList(sortedArray.data(), sortedArray.size());
//
//            // Очищення вмісту другого компонента List
//            SendMessageA(hwndList2, LB_RESETCONTENT, 0, 0);
//
//            // Додавання відсортованих рядків до другого компонента List
//            for (const auto& str : sortedLetters) {
//                SendMessageA(hwndList2, LB_ADDSTRING, 0, (LPARAM)str.c_str());
//            }
//
//        }
//        else {
//            // Затримка для економії ресурсів процесора
//            Sleep(100);
//        }
//    }
//}
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    case WM_COMMAND:
//    {
//        int wmId = LOWORD(wParam);
//        switch (wmId)
//        {
//        case 1:
//            // Дія при натисканні кнопки
//            // Не потрібно нічого робити тут, бо логіка винесена в потоки
//            break;
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
