#ifndef DXATC_UTILS_SOCKET_H
#define DXATC_UTILS_SOCKET_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define DXATC_UTILS_SOCKET_INVALID INVALID_SOCKET
#else
#define DXATC_UTILS_SOCKET_INVALID -1
#endif

#ifdef _WIN32
    typedef SOCKET DxAtcSocket;
#else
    typedef int DxAtcSocket;
#endif

    
#ifdef _WIN32
#define dxAtcSocketClose(sock) closesocket(sock)
#else
#define dxAtcSocketClose(sock) close(sock)
#endif

#endif
