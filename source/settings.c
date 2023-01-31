#include "ScreenFlip.h"

#include <windows.h>

wchar_t TEMP[256] = { 0 };
wchar_t OLD_WALLPAPER[256] = { 0 };
INT OLD_WIDTH = 0;
INT OLD_HEIGHT = 0;

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

LONG flipDisplay(char orientation)
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

        LONG lRet = ChangeDisplaySettingsW(&dm, 0);
        if (DISP_CHANGE_SUCCESSFUL != lRet)
        {
            printf("Display settings change was not successful.");
        }

        return lRet;
    }

    return -1;
}

int changeWallpaper(LPCWSTR path, LPCWSTR link)
{
    int errcode = -1;

    errcode = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, path, 0);

    errcode -= 1;

    if (errcode != 0 && link != DEFAULT_URL)
    {
        errcode = setWallpaperURL(DEFAULT_URL);
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
    else if (url != DEFAULT_URL)
    {
        errcode = setWallpaperURL(DEFAULT_URL);
    }

    return errcode;
}

LRESULT minimizeAll()
{
    HANDLE hwnd = FindWindowW(L"Shell_TrayWnd", NULL);
    LRESULT res = SendMessageW(hwnd, WM_COMMAND, (WPARAM)419, 0);

    return res;
}

int revertBack()
{
    int errcode = 0;
	
	wchar_t windir[256] = { 0 };

	if (GetWindowsDirectoryW(windir, 256) != 0 && windir[0] != 0)
	{
		wchar_t path[256] = { 0 };
		wcscat_s(windir, 256, L"\\Web\\Wallpaper\\Windows\\img0.jpg");
		SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, OLD_WALLPAPER, 0);
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

    minimizeAll();

    return errcode;
}

int changeSettings()
{
    int errcode = 0;

    SystemParametersInfoW(SPI_GETDESKWALLPAPER, 256, OLD_WALLPAPER, 0);

    if (URL != NULL)
    {
        errcode = setWallpaperURL(URL);
    }
    else
        errcode = setWallpaperURL(DEFAULT_URL);

    errcode = flipDisplay(1);
    errcode = SwapMouseButton(1);
    minimizeAll();

    return errcode;
}