#include "ChadNet.h"

static int sock = -1;
static sockaddr_in serverAddr{};

#ifdef _WIN32
static WSADATA wsaData;
#endif

bool ChadConnect(const std::string& ip, int port)
{
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Errore inizializzazione Winsock." << std::endl;
        return false;
    }
#endif

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "Errore creazione socket." << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        std::cerr << "Errore nell'interpretazione dell'IP." << std::endl;
        ChadDisconnect();
        return false;
    }

    return true;
}

void ChadDisconnect() {
    if (sock >= 0)
    {
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        sock = -1;
    }
}

bool ChadSendPoint(float x, float y, Color color)
{
    if (sock < 0)
    {
        std::cerr << "Errore: socket non inizializzato, chiama ChadConnect()." << std::endl;
        return false;
    }

    char buffer[sizeof(DataType) + sizeof(Point)];
    buffer[0] = CHAD_TYPE_POINT;

    Point point = { x, y, color};
    std::memcpy(buffer + 1, &point, sizeof(Point));

    int sent = sendto(sock, buffer, sizeof(buffer), 0,
        (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    return (sent == sizeof(buffer));
}

bool ChadSendLine(float x1, float y1, float x2, float y2)
{
    if (sock < 0)
    {
        std::cerr << "Errore: socket non inizializzato, chiama ChadConnect()." << std::endl;
        return false;
    }

    char buffer[sizeof(DataType) + sizeof(Line)];
    buffer[0] = CHAD_TYPE_LINE;

    Line line = { {x1, y1}, {x2, y2} };
    std::memcpy(buffer + 1, &line, sizeof(Line));

    int sent = sendto(sock, buffer, sizeof(buffer), 0,
        (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    return (sent == sizeof(buffer));
}
