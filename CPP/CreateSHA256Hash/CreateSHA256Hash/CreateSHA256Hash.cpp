// CreateSHA256Hash.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "CreateSHA256Hash.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), nullptr, &WndProc);


    return 0;
}


OPENFILENAME myOpenFileSettings;


INT_PTR CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DWORD error;
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam == IDC_BUTTON1))
        {
            wchar_t fileName[1024] = {};
            memset(&myOpenFileSettings, 0, sizeof(OPENFILENAME));
            myOpenFileSettings.hwndOwner = hWnd;
            myOpenFileSettings.hInstance = hInst;
            myOpenFileSettings.lStructSize = sizeof(OPENFILENAME);
            myOpenFileSettings.nMaxFile = 1024;
            myOpenFileSettings.lpstrFile = fileName;
            if (GetOpenFileName(&myOpenFileSettings) != 0)
            {
                SetDlgItemText(hWnd, IDC_EDIT1, myOpenFileSettings.lpstrFile);
            }
            else
            {
                error = CommDlgExtendedError();
            }
        }
        break;

    }
    return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}