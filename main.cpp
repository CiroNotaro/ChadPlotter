#include <SDL3/SDL.h>
#include <vector>
#include <thread>
#include <atomic>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <iostream>

#include "ChadNet.h"

std::atomic<bool> running(true);
std::vector<Point> points;
std::vector<Line> lines;

void networkListener(int port)
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return;

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return;
    }

    while (running) {
        sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock, &fds);
        struct timeval timeout = { 1, 0 };
        int selectResult = select(sock + 1, &fds, nullptr, nullptr, &timeout);

        if (selectResult > 0) {
            char buffer[sizeof(Line) + 1];
            int received = recvfrom(sock, buffer, sizeof(buffer), 0,
                (struct sockaddr*)&clientAddr, &addrLen);

            if (received >= 1) {
                std::cout << "RECIVED\n";
                DataType type = static_cast<DataType>(buffer[0]);
                if (type == CHAD_TYPE_POINT && received == sizeof(Point) + 1) {
                    Point point;
                    std::memcpy(&point, buffer + 1, sizeof(Point));
                    points.push_back(point);
                }
                else if (type == CHAD_TYPE_LINE && received == sizeof(Line) + 1) {
                    Line line;
                    std::memcpy(&line, buffer + 1, sizeof(Line));
                    lines.push_back(line);
                }
                else {
                    std::cerr << "Errore: pacchetto ricevuto di dimensione errata." << std::endl;
                }
            }
        }
    }

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_CreateWindowAndRenderer("Chad Plotter", 800, 600, 0, &window, &renderer);

    SDL_Event event;
    float zoom = 1.0f;
    float offsetX = 0, offsetY = 0;
    bool dragging = false;
    int lastMouseX, lastMouseY;
    float basePointSize = 5.0f;

    int port = CHAD_DEFAULT_PORT;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            port = std::atoi(argv[i + 1]);
            i++;
        }
    }

    std::thread netThread(networkListener, port);

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    points.push_back({ (event.button.x - offsetX) / zoom, (event.button.y - offsetY) / zoom });
                }
                else if (event.button.button == SDL_BUTTON_RIGHT && !points.empty())
                {
                    points.pop_back();
                }
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    dragging = true;
                    lastMouseX = event.button.x;
                    lastMouseY = event.button.y;
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    dragging = false;
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION && dragging)
            {
                offsetX += event.motion.x - lastMouseX;
                offsetY += event.motion.y - lastMouseY;
                lastMouseX = event.motion.x;
                lastMouseY = event.motion.y;
            }
            else if (event.type == SDL_EVENT_MOUSE_WHEEL)
            {
                float mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                float worldX = (mouseX - offsetX) / zoom;
                float worldY = (mouseY - offsetY) / zoom;

                float zoomFactor = (event.wheel.y > 0) ? 1.1f : 0.9f;
                zoom *= zoomFactor;

                offsetX = mouseX - worldX * zoom;
                offsetY = mouseY - worldY * zoom;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (const auto& line : lines)
        {
            SDL_RenderLine(renderer, line.start.x * (basePointSize * zoom) + offsetX, line.start.y * (basePointSize * zoom) + offsetY,
                line.end.x * (basePointSize * zoom) + offsetX, line.end.y * (basePointSize * zoom) + offsetY);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (const auto& point : points)
        {
            //float drawX = point.x * zoom + offsetX;
            //float drawY = point.y * zoom + offsetY;
            //float pointSize = basePointSize * zoom;
            //SDL_FRect rect = { drawX - pointSize / 2, drawY - pointSize / 2, pointSize, pointSize };
            float drawX = point.x * zoom + offsetX;
            float drawY = point.y * zoom + offsetY;
            float pointSize = 1.0;
            SDL_FRect rect = { drawX , drawY, pointSize, pointSize };
            SDL_SetRenderDrawColor(renderer, point.color.r, point.color.g, point.color.b, 255);
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
    }

    netThread.join();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}