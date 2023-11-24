#include <serversocketthread.h>

SocketThread::SocketThread(SocketThreadCallback *callback, wxImage& capturedImage, wxCriticalSection& cIcs)
    : callback(callback), capturedImage(capturedImage), cIcs(cIcs) {}
SocketThread::~SocketThread()
{
    callback->OnSocketThreadDestruction();
}

wxThread::ExitCode SocketThread::Entry()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return nullptr;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(16165);

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return nullptr;
    }

    listen(serverSocket, SOMAXCONN);

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to accept client connection." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return nullptr;
    }

    closesocket(serverSocket);

    while (true)
    {
        // char response;
        // int bytesRead = recv(clientSocket, &response, sizeof(char), 0);
        // if (bytesRead <= 0)
        // {
        //     std::cerr << "Failed to receive data." << std::endl;
        //     return nullptr;
        // }
        
        unsigned char* bytes = new unsigned char[SIZE];
        {
            wxCriticalSectionLocker lock(cIcs);    
            for (int y = 0; y < 720; ++y)
            {
                for (int x = 0; x < 1280; ++x) {
                    int index = (y * 1280 + x) * 3;
                    bytes[index] = capturedImage.GetRed(x, y);
                    bytes[index + 1] = capturedImage.GetGreen(x, y);
                    bytes[index + 2] = capturedImage.GetBlue(x, y);
                }
            }
        }

        if (send(clientSocket, reinterpret_cast<char *>(bytes), SIZE, 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send image." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return nullptr;
        }

        delete[] bytes;
    }

    closesocket(clientSocket);
    WSACleanup();

    return nullptr;
}