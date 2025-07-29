#include "ChadNet.h"

int main() {
    if (!ChadConnect("127.0.0.1", 26969)) {
        return -1;
    }

    Line l = { {0,0},{10,10} };

    while (true) {
        float x = static_cast<float>(rand() % 200 - 100);
        float y = static_cast<float>(rand() % 200 - 100);

        if (ChadSendPoint(x, y)) {
            std::cout << "Inviato: (" << x << ", " << y << ")" << std::endl;
        }
        else {
            std::cerr << "Errore nell'invio del punto." << std::endl;
        }

#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    ChadDisconnect();
    return 0;
}
