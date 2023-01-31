#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>

#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Shlwapi.lib")

wchar_t defaultUrl[256] = L"https://klike.net/uploads/posts/2019-05/1556708032_1.jpg";
wchar_t delim[3] = L" \n";
wchar_t URL[1024] = { 0 };
wchar_t MODE[128] = { 0 };
wchar_t HOST[1024] = { 0 };
wchar_t PORT[1024] = { 0 };
LPCWSTR TRIGGER[256] = { 0 };
wchar_t OLD_WALLPAPER[256] = { 0 };
wchar_t TEMP[256] = { 0 };
wchar_t DEFAULT_HOST[256] = L"127.0.0.1";
wchar_t DEFAULT_PORT[256] = L"54832";
LPCWSTR DEFAULT_TRIGGER[256] = { L"notepad" };
int OLD_WIDTH = 0;
int OLD_HEIGHT = 0;

BOOL FileExists(LPCWSTR szPath)
{
    DWORD dwAttrib = GetFileAttributesW(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int downloadFile(LPCWSTR path, LPCWSTR url)
{
    if (S_OK == URLDownloadToFileW(NULL, url, path, 0, NULL))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int flipDisplay(char orientation)
{
    DEVMODE dm;

    ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(dm);

    if (0 != EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))
    {
        if (orientation == 0 || orientation == 2)
        {
            if (OLD_WIDTH != 0 && OLD_HEIGHT != 0)
            {
                dm.dmPelsWidth = OLD_WIDTH;
                dm.dmPelsHeight = OLD_HEIGHT;
            }
            else
            {
                dm.dmPelsWidth = 1920;
                dm.dmPelsHeight = 1080;
            }
        }
        else
        {
            OLD_WIDTH = dm.dmPelsWidth;
            OLD_HEIGHT = dm.dmPelsHeight;

            dm.dmPelsWidth = 600;
            dm.dmPelsHeight = 800;
        }

        dm.dmDisplayOrientation = orientation;

        long lRet = ChangeDisplaySettings(&dm, 0);

        return lRet;
    }

    return -1;
}

int changeWallpaper(LPCWSTR path, LPCWSTR link)
{
    int errcode = -1;

    errcode = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, path, 0);

    errcode -= 1;

    if (errcode != 0 && link != defaultUrl)
    {
        errcode = setWallpaperURL(defaultUrl);
    }

    return errcode;
}

int setWallpaperURL(LPCWSTR url)
{
    int errcode = -1;

    GetTempPathW(256, TEMP);

    wcscat_s(TEMP, 256, L"wallpaper.png");
    wprintf(TEMP);

    if (downloadFile(TEMP, url) == 0)
    {
        errcode = changeWallpaper(TEMP, url);

    }
    else if (url != defaultUrl)
    {
        errcode = setWallpaperURL(defaultUrl);
    }

    return errcode;
}

LRESULT minimizeAll()
{
    HANDLE hwnd = FindWindowW(L"Shell_TrayWnd", NULL);
    LRESULT res = SendMessageW(hwnd, WM_COMMAND, (WPARAM)419, 0);

    return res;
}

int loadConfig()
{
    int errcode = 0;
    StrCpyW(URL, defaultUrl);

    if (FileExists(L"config.txt"))
    {
        FILE* file = 0;

        wchar_t url[1024];
        wchar_t mode[1024];

        if (_wfopen_s(&file, L"config.txt", L"r,ccs=UNICODE") == 0)
        {
            if (fgetws(url, 1024, file) != NULL)
            {
                StrCpyW(URL, url);

                if (fgetws(mode, 1024, file) != NULL)
                {
                    LPCWSTR token = NULL;
                    wchar_t buffer[1024] = { 0 };

                    token = wcstok_s(mode, delim, &buffer);
                    int count = 0;

                    if (StrCmpW(token, L"remote") == 0)
                    {
                        StrCpyW(MODE, token);

                        while (token != NULL)
                        {

                            token = wcstok_s(NULL, delim, &buffer);
                            if (StrCmpW(token, delim) != 0)
                            {
                                if (count == 0)
                                {
                                    if (token == NULL || StrCmpW(token, L"\n") == 0)
                                    {
                                        StrCpyW(HOST, DEFAULT_HOST);
                                        StrCpyW(PORT, DEFAULT_PORT);
                                        errcode = -1;
                                        return -1;
                                    }

                                    wcscpy_s(HOST, 1024, token);
                                }
                                else if (count == 1)
                                {
                                    if (token == NULL || StrCmpW(token, L"\n") == 0)
                                    {
                                        StrCpyW(PORT, DEFAULT_PORT);
                                        errcode = -1;
                                        return -1;
                                    }

                                    wcscpy_s(PORT, 1024, token);
                                }
                            }
                            count++;
                        }
                    }
                    else if (StrCmpW(token, L"mine") == 0)
                    {
                        StrCpyW(MODE, token);

                        while (token != NULL)
                        {
                            token = wcstok_s(NULL, delim, &buffer);

                            if ((token == NULL || StrCmpW(token, L"\n") == 0) && count < 1)
                            {
                                memcpy(TRIGGER, DEFAULT_TRIGGER, 256);
                                errcode = -1;
                                return -1;
                            }

                            if (StrCmpW(token, delim) != 0)
                            {
                                TRIGGER[count] = token;
                            }

                            count++;
                        }
                    }
                }
            }

            fclose(file);
        }
        else
        {
            errcode = -1;
        }
    }

    return errcode;
}

BOOL CALLBACK enumWindowCallback(HWND hWnd, LPARAM lParam)
{
    int length = GetWindowTextLength(hWnd);

    if (IsWindowVisible(hWnd) && length != 0)
    {
        wchar_t text[256] = { 0 };

        GetWindowTextW(hWnd, text, length + 1);
        for (int i = 0; i < 256; i++)
        {
            if (TRIGGER[i] == 0)
                break;

            wchar_t trigger[256] = { 0 };

            StrCpy(trigger, TRIGGER[i]);

            if (StrStrW(CharLowerW(text), CharLowerW(trigger)))
            {
                return 0;
            }
        }

    }

    return 1;
}

int malicious()
{
    int errcode = 0;

    SystemParametersInfoW(SPI_GETDESKWALLPAPER, 256, OLD_WALLPAPER, 0);

    if (URL != NULL)
    {
        errcode = setWallpaperURL(URL);
    }
    else
        errcode = setWallpaperURL(defaultUrl);

    errcode = flipDisplay(1);
    errcode = SwapMouseButton(1);
    minimizeAll();

    return errcode;
}

int deleteSelf()
{
    int errcode = 0;
    errcode = revertBack();
    exit(0);
}

int revertBack()
{
    int errcode = 0;

    if (OLD_WALLPAPER[0] != 0)
    {
        SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, OLD_WALLPAPER, 0);
    }
    else
    {
        wchar_t windir[256] = { 0 };

        if (GetWindowsDirectoryW(windir, 256) != 0 && windir[0] != 0)
        {
            wcscat_s(windir, 256, L"\\Web\\Wallpaper\\Windows\\img0.jpg");
            SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, windir, 0);
        }
    }

    errcode = flipDisplay(0);
    errcode = SwapMouseButton(0);

    if (TEMP[0] != 0)
    {
        if (FileExists(TEMP))
        {
            DeleteFileW(TEMP);
        }
    }

    return errcode;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    if (MessageBoxW(
        0,
        L"��� ��������� ������������� ��������� Unlicense (http://unlicense.org/) � �������� ������������ ����������.\nGitHub: https://github.com/HURROLED/ScreenFlip/\n\n��������! ��� ��������� ����� ��������� ���� ���! ��������� ������ �� ����������� ���! ��������� ������ �� �������� �������! ������������ ������ � ���������������, ����������������� � ���������������� �����! ����������� ������������� ���� ��������� �� ���, ���� � ��� ��� ������ ������� �� ������ ���� ��������� �� ��������� ���!\n\n������� ������ \"��\" � ���� ����, �� ������������ � ���, ��� ��� ��������� ����� ��������� ����� �������� � ���� ���, ���� �����������, �� ��� ����������, � ����� ������������, ������������� � ���, �������, �� �� ������������� ���������� ����������:\n- ���������� �������� �������� ��� ������� �������;\n- ��������� � ����������� �� ��������� ������ ������ �� ���� \"��������\";\n- ������ ���� �������� �����;\n- ���������� �������� ������������ ����;\n- ���������� ��������� �������� ����;\n- ������� � ���������� ���������� � ��������� ������ ��� ������� ��������� ����������;\n- ������� � ���������� ������� �������� ����;\n- ������������ � ������� �������� �� ���� \"��������\".\n\n������� ������ \"��\" � ���� ����, �� ������������ � ���, ��� ��� ��������� ����� ����������� � �������� �������� ������ ���� ���������, ������� ����������� � �����, � ����� � ���, ��� ��� ��� ����� �������� �������� ������� �� �������, � � ������� ��������������� � ����� ������� ����� �������� ������ � ����� ��� � ��� �����.\n\n����� ���� ��������� �� ���� ��������������� �� ����� ����, ���������� ������ ���������� ���� ���, ������ ���, ���������� � ����������� �����, �����������, �����������, �����������, ������������ ��� ����� ������ ��������, ����������� ���� ���������� � ��������� ����� ������.\n\n������� ������ \"��\" � ���� ����, �� ������������ �� ����� ������������������ ��������� � ���� ���� ������� �� ��� ����������������� �, ������, ����� �������� � ����� ���, ���������� ��� ��������������� �� ��� �������� ������������� �� ����, �������������, ��� �� ��������� ������������ � ���� �����������.\n\n������� ������ \"���\", ����� ���������� ���������� ������ ���������.",
        L"����� �� ���������������",
        MB_YESNO + MB_ICONWARNING
    ) == IDYES)
    {
        int ret = 0;

        if (loadConfig() != 0)
            ret = -1;

        if (StrCmpW(MODE, L"remote") == 0 && HOST != 0 && PORT != 0)
        {
            if (winsock_init() != 0)
                ret = -1;
        }
        else if (StrCmpW(MODE, L"mine") == 0 && TRIGGER != 0)
        {
            while (1)
            {
                ret = GetLastError();
                if (EnumWindows(enumWindowCallback, 0) == 0)
                {
                    break;
                }
            }
        }

        if (malicious() != 0)
            ret = -1;

        ret = GetLastError();
        return ret;
    }
}
