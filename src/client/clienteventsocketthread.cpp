#include <clienteventsocketthread.h>

EventSocketThread::EventSocketThread(EventSocketThreadCallback *callback, std::queue<msg> &msgQueue, wxCriticalSection &mQcs)
    : callback(callback), msgQueue(msgQueue), mQcs(mQcs) {}
EventSocketThread::~EventSocketThread()
{
    callback->OnEventSocketThreadDestruction();
}

wxThread::ExitCode EventSocketThread::Entry()
{
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << "\n";
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(20565);                 
    serverAddress.sin_addr.s_addr = inet_addr("192.168.152.129");

    if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to server." << "\n";
        closesocket(clientSocket);
        return nullptr;
    }

    while (true)
    {
        msg msg;
        {
            wxCriticalSectionLocker lock(mQcs);
            if (!msgQueue.empty()) 
            {
                msg = msgQueue.front();
                msgQueue.pop();
            }
            else continue; 
        }

        char* msgData = new char[18];
        memcpy(msgData, &msg, 18);

        if (send(clientSocket, (char *)msgData, 18, 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send event." << "\n";
            closesocket(clientSocket);
            delete[] msgData;
            return nullptr;
        }
    }

    closesocket(clientSocket);

    return nullptr;
}