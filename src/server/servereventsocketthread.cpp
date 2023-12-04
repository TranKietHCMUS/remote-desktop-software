#include <servereventsocketthread.h>

EventSocketThread::EventSocketThread(EventSocketThreadCallback *callback, std::queue<msg> &msgQueue, wxCriticalSection &mQcs)
    : callback(callback), msgQueue(msgQueue), mQcs(mQcs) {}
EventSocketThread::~EventSocketThread()
{
    callback->OnEventSocketThreadDestruction();
}

wxThread::ExitCode EventSocketThread::Entry()
{
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << "\n";
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(20565);

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket." << "\n";
        closesocket(serverSocket);
        return nullptr;
    }

    listen(serverSocket, SOMAXCONN);

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to accept client connection." << "\n";
        closesocket(serverSocket);
        return nullptr;
    }

    closesocket(serverSocket);

    while (true)
    {
        unsigned char* data = new unsigned char[18];
        size_t totalReceived = 0;
        while (totalReceived < 18)
        {
            int received = recv(clientSocket, (char*)data + totalReceived, 18 - totalReceived, 0);
            if (received == SOCKET_ERROR)
            {
                std::cerr << "Failed to receive event." << "\n";
                closesocket(clientSocket);
                return nullptr;
            }
            totalReceived += received;
        }

        msg msg;
        memcpy(&msg, data, 18);
        {
            wxCriticalSectionLocker lock(mQcs);
            msgQueue.push(msg);
        }
        
        delete[] data;
    }

    closesocket(clientSocket);
    return nullptr;
}