#include <Winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

extern wchar_t HOST[1024];
extern wchar_t PORT[1024];

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
        return errcode;
    }

    int actual_len = 1;
    char recvbuf[512] = { 0 };

    while (strcmp(recvbuf, "ok") != 0 && actual_len > 0)
    {
        if (SOCKET_ERROR == (actual_len = recv(s, (char*)&recvbuf, 512, 0)))
        {
            errcode = WSAGetLastError();
            closesocket(s);
            WSACleanup();
            return errcode;
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
