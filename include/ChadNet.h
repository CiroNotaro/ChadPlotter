#pragma once

#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <cstdint>
#include <cstring>
#include <iostream>

#define CHAD_DEFAULT_PORT 26969

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct Point {
    float x, y;
    Color color = { 255,255,255 };
};

struct Line {
    Point start, end;
};

enum DataType : uint8_t { CHAD_TYPE_POINT = 0, CHAD_TYPE_LINE};

// Funzioni per il client UDP

// -- Socket
bool ChadConnect(const std::string& ip, int port);
void ChadDisconnect();

// -- Pacchetti
bool ChadSendPoint(float x, float y, Color color = {255,255,255});
bool ChadSendLine(float x1, float y1, float x2, float y2);
