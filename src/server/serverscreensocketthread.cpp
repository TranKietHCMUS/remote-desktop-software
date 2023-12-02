#include <serverscreensocketthread.h>

ScreenSocketThread::ScreenSocketThread(ScreenSocketThreadCallback *callback, wxImage& capturedImage, wxCriticalSection& cIcs)
    : callback(callback), capturedImage(capturedImage), cIcs(cIcs) {}
ScreenSocketThread::~ScreenSocketThread()
{
    callback->OnScreenSocketThreadDestruction();
}

wxThread::ExitCode ScreenSocketThread::Entry()
{
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << "\n";
        WSACleanup();
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(16165);

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket." << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return nullptr;
    }

    listen(serverSocket, SOMAXCONN);

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to accept client connection." << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return nullptr;
    }

    closesocket(serverSocket);

    while (true)
    {
        unsigned char* imageData = new unsigned char[SIZE];
        {
            wxCriticalSectionLocker lock(cIcs);    
            for (int y = 0; y < 720; ++y)
            {
                for (int x = 0; x < 1280; ++x) {
                    int index = (y * 1280 + x) * 3;
                    imageData[index] = capturedImage.GetRed(x, y);
                    imageData[index + 1] = capturedImage.GetGreen(x, y);
                    imageData[index + 2] = capturedImage.GetBlue(x, y);
                }
            }
        }

        if (send(clientSocket, (char *)imageData, SIZE, 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send image." << "\n";
            closesocket(clientSocket);
            WSACleanup();
            delete[] imageData;
            return nullptr;
        }

        delete[] imageData;
    }

    closesocket(clientSocket);
    WSACleanup();

    return nullptr;
}