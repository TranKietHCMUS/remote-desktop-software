#include <clientscreensocketthread.h>

ScreenSocketThread::ScreenSocketThread(ScreenSocketThreadCallback *callback, wxImage &screenImage, wxCriticalSection &sIcs)
    : callback(callback), screenImage(screenImage), sIcs(sIcs) {}
ScreenSocketThread::~ScreenSocketThread()
{
    callback->OnScreenSocketThreadDestruction();
}

wxThread::ExitCode ScreenSocketThread::Entry()
{
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << "\n";
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(16165);                 
    serverAddress.sin_addr.s_addr = inet_addr("192.168.152.129");

    if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to server." << "\n";
        closesocket(clientSocket);
        return nullptr;
    }

    while (true)
    {
        unsigned char* imageData = new unsigned char[SIZE];
        size_t totalReceived = 0;
        while (totalReceived < SIZE)
        {
            int received = recv(clientSocket, (char*)imageData + totalReceived, SIZE - totalReceived, 0);
            if (received == SOCKET_ERROR)
            {
                std::cerr << "Failed to receive image." << "\n";
                closesocket(clientSocket);
                return nullptr;
            }
            totalReceived += received;
        }

        wxImage image(1280, 720, true);
        for (int y = 0; y < 720; ++y)
        {
            for (int x = 0; x < 1280; ++x)
            {
                int index = (y * 1280 + x) * 3;
                unsigned char red = imageData[index];
                unsigned char green = imageData[index + 1];
                unsigned char blue = imageData[index + 2];
                image.SetRGB(x, y, red, green, blue);
            }
        }
        
        delete[] imageData;
        
        {
            wxCriticalSectionLocker lock(sIcs);
            screenImage = image;
        }
    }

    closesocket(clientSocket);
    return nullptr;
}