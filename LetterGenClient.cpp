#define _WINSOCKAPI_
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "LetterGen.h"
#include <vector>
#include <string>
#include <algorithm>
#include <thread>
#include <map>
#include <sstream>
#pragma comment(lib, "ws2_32.lib")

// Глобальні змінні:
HINSTANCE hInst;
LPCTSTR szWindowClass = TEXT("QWERTY");
LPCTSTR szTitle = TEXT("ПЕРША ПРОГРАМА");
HWND hwndList1, hwndList2;
std::vector<std::string> unsortedLetters;
std::vector<std::string> sortedLetters;
std::map<char, int> letterFrequency;

// Прототипи функцій:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void FirstThreadFunc(std::vector<std::string>& unsortedLetters, std::map<char, int>& letterFrequency);
void SecondThreadFunc(std::vector<std::string>& unsortedLetters, std::vector<std::string>& sortedLetters);
void SendDataOverTCP(const std::vector<std::string>& sortedLetters);
void SaveTopLettersToRegistry(const std::map<char, int>& letterFrequency);
void UpdateLetterFrequency(const std::string& letters, std::map<char, int>& letterFrequency);
void SaveToRegistry(const std::wstring& keyName, const std::wstring& valueName, const std::wstring& value);
std::wstring LoadFromRegistry(const std::wstring& keyName, const std::wstring& valueName);

// Основна програма WinMain:
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    std::wstring topLetters = LoadFromRegistry(L"SOFTWARE\\MyApp", L"TopLetters");
    if (!topLetters.empty()) {
        std::wstringstream wss(topLetters);
        std::wstring letter;
        while (std::getline(wss, letter, L' ')) {
            if (!letter.empty()) {
                std::wstringstream pairStream(letter);
                std::wstring letterStr, frequencyStr;
                std::getline(pairStream, letterStr, L':');
                std::getline(pairStream, frequencyStr, L':');
                char ch = static_cast<char>(std::stoi(std::wstring(frequencyStr)));
                letterFrequency[ch] = std::stoi(std::wstring(frequencyStr));
            }
        }
    }

    std::thread firstThread(FirstThreadFunc, std::ref(unsortedLetters), std::ref(letterFrequency));
    std::thread secondThread(SecondThreadFunc, std::ref(unsortedLetters), std::ref(sortedLetters));

    firstThread.detach();
    secondThread.detach();

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_HAND);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = NULL;
    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    CreateWindow(TEXT("BUTTON"), TEXT("Send to Server"), WS_VISIBLE | WS_CHILD, 10, 10, 120, 30, hWnd, (HMENU)1, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Save to Registry"), WS_VISIBLE | WS_CHILD, 140, 10, 120, 30, hWnd, (HMENU)2, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Load from Registry"), WS_VISIBLE | WS_CHILD, 270, 10, 150, 30, hWnd, (HMENU)3, hInstance, NULL);

    hwndList1 = CreateWindowA("LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY, 10, 50, 200, 400, hWnd, (HMENU)4, hInstance, NULL);

    hwndList2 = CreateWindowA("LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY, 220, 50, 200, 400, hWnd, (HMENU)5, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// Функція для збереження топ-літер у реєстр
void SaveTopLettersToRegistry(const std::map<char, int>& letterFrequency) {
    std::vector<std::pair<char, int>> freqVector(letterFrequency.begin(), letterFrequency.end());

    std::sort(freqVector.begin(), freqVector.end(),
        [](const std::pair<char, int>& a, const std::pair<char, int>& b) {
            return b.second < a.second;
        });

    std::ostringstream oss;
    int count = 0;
    for (const auto& pair : freqVector) {
        if (count >= 5) break;
        oss << pair.first << ":" << pair.second << " ";
        count++;
    }

    std::string topLetters = oss.str();
    int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, topLetters.c_str(), -1, NULL, 0);
    std::wstring wideTopLetters(wideCharLength, 0);
    MultiByteToWideChar(CP_UTF8, 0, topLetters.c_str(), -1, &wideTopLetters[0], wideCharLength);

    SaveToRegistry(L"SOFTWARE\\MyApp", L"TopLetters", wideTopLetters);
}

// Оновлення частоти літер
void UpdateLetterFrequency(const std::string& letters, std::map<char, int>& letterFrequency) {
    for (char letter : letters) {
        letterFrequency[letter]++;
    }
}

// Функція першого потоку
void FirstThreadFunc(std::vector<std::string>& unsortedLetters, std::map<char, int>& letterFrequency) {
    for (int i = 0; i < 100; ++i) {
        const int length = 8;
        char letters[length + 1] = { 0 };
        GenerateRandomLetters(letters, length);
        unsortedLetters.push_back(letters);

        UpdateLetterFrequency(letters, letterFrequency);

        SaveTopLettersToRegistry(letterFrequency);

        SendMessageA(hwndList1, LB_ADDSTRING, 0, (LPARAM)letters);
    }
}

// Функція другого потоку
void SecondThreadFunc(std::vector<std::string>& unsortedLetters, std::vector<std::string>& sortedLetters) {
    while (true) {
        if (!unsortedLetters.empty()) {
            std::string lastLetters = unsortedLetters.back();
            unsortedLetters.pop_back();

            //SortLetters(const_cast<char*>(lastLetters.c_str()), static_cast<int>(lastLetters.length()));
            sortedLetters.push_back(lastLetters);

            std::vector<char*> sortedArray;
            for (auto& str : sortedLetters) {
                sortedArray.push_back(const_cast<char*>(str.c_str()));
            }

            SortStringList(sortedArray.data(), static_cast<int>(sortedArray.size()));

            SendMessageA(hwndList2, LB_RESETCONTENT, 0, 0);

            for (const auto& str : sortedLetters) {
                SendMessageA(hwndList2, LB_ADDSTRING, 0, (LPARAM)str.c_str());
            }

        }
        else {
            Sleep(100);
        }
    }
}

void SendDataOverTCP(const std::vector<std::string>& sortedLetters) {
    WSADATA wsaData;
    SOCKET SendSocket = INVALID_SOCKET;
    sockaddr_in RecvAddr;
    int Port = 27015;
    std::string dataToSend;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        MessageBox(NULL, TEXT("WSAStartup failed"), TEXT("Error"), MB_OK);
        return;
    }

    SendSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SendSocket == INVALID_SOCKET) {
        MessageBox(NULL, TEXT("Socket creation failed"), TEXT("Error"), MB_OK);
        WSACleanup();
        return;
    }

    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(Port);
    if (InetPton(AF_INET, TEXT("127.0.0.1"), &RecvAddr.sin_addr) <= 0) {
        MessageBox(NULL, TEXT("Invalid address"), TEXT("Error"), MB_OK);
        closesocket(SendSocket);
        WSACleanup();
        return;
    }

    if (connect(SendSocket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr)) == SOCKET_ERROR) {
        MessageBox(NULL, TEXT("Connection failed"), TEXT("Error"), MB_OK);
        closesocket(SendSocket);
        WSACleanup();
        return;
    }

    for (const auto& str : sortedLetters) {
        dataToSend += str;
        dataToSend += '\n';
    }

    int bytesSent = send(SendSocket, dataToSend.c_str(), static_cast<int>(dataToSend.size()), 0);
    if (bytesSent == SOCKET_ERROR) {
        MessageBox(NULL, TEXT("Send failed"), TEXT("Error"), MB_OK);
    }
    else {
        MessageBox(NULL, TEXT("Data sent successfully"), TEXT("Success"), MB_OK);
    }

    closesocket(SendSocket);
    WSACleanup();
}

void SaveToRegistry(const std::wstring& keyName, const std::wstring& valueName, const std::wstring& value) {
    HKEY hKey;
    DWORD dwDisposition;
    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, keyName.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, &dwDisposition);
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, TEXT("Error creating/opening registry key"), TEXT("Error"), MB_OK);
        return;
    }

    result = RegSetValueEx(hKey, valueName.c_str(), 0, REG_SZ, (BYTE*)value.c_str(), (DWORD)(value.size() + 1) * sizeof(wchar_t));
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, TEXT("Error setting registry value"), TEXT("Error"), MB_OK);
    }

    RegCloseKey(hKey);
}

std::wstring LoadFromRegistry(const std::wstring& keyName, const std::wstring& valueName) {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, keyName.c_str(), 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, TEXT("Error opening registry key"), TEXT("Error"), MB_OK);
        return L"";
    }

    wchar_t buffer[1024];
    DWORD bufferSize = sizeof(buffer);
    result = RegQueryValueEx(hKey, valueName.c_str(), NULL, NULL, (LPBYTE)buffer, &bufferSize);
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, TEXT("Error reading registry value"), TEXT("Error"), MB_OK);
        RegCloseKey(hKey);
        return L"";
    }

    RegCloseKey(hKey);

    return std::wstring(buffer);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case 1:
            // Відправка даних по TCP/IP
            SendDataOverTCP(sortedLetters);
            break;
        case 2:
            // Збереження в реєстр
            SaveTopLettersToRegistry(letterFrequency);
            MessageBox(NULL, TEXT("Top letters saved to registry"), TEXT("Success"), MB_OK);
            break;
        case 3:
            // Завантаження з реєстру
            MessageBox(NULL, LoadFromRegistry(L"SOFTWARE\\MyApp", L"TopLetters").c_str(), TEXT("Top Letters from Registry"), MB_OK);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
