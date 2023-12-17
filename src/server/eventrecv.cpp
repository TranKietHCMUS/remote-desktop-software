#include <server/eventrecv.h>

wxDEFINE_EVENT(wxEVT_EVENTRECVTHREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_EVENTRECVTHREAD_COMPLETE, wxThreadEvent);

EventRecvThread::~EventRecvThread()
{
    callback->OnEventRecvThreadDestruction();
}

wxThread::ExitCode EventRecvThread::Entry()
{
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTRECVTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to create socket\n"));
        wxQueueEvent(evtHandler, e);
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(51265);

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTRECVTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to bind socket\n"));
        wxQueueEvent(evtHandler, e);
        closesocket(serverSocket);
        return nullptr;
    }

    listen(serverSocket, SOMAXCONN);

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTRECVTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to accept connection\n"));
        wxQueueEvent(evtHandler, e);
        closesocket(serverSocket);
        return nullptr;
    }

    closesocket(serverSocket);

    wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTRECVTHREAD_UPDATE);
    wxQueueEvent(evtHandler, e);
    
    while (true)
    {       
        if (TestDestroy())
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTRECVTHREAD_COMPLETE);
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }
        
        unsigned char* data = new unsigned char[MSG_SIZE];
        size_t totalReceived = 0;
        while (totalReceived < MSG_SIZE)
        {
            int received = recv(clientSocket, (char*)data + totalReceived, MSG_SIZE - totalReceived, 0);
            if (received <= 0)
            {
                wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTRECVTHREAD_COMPLETE);
                if (received < 0) e->SetString(wxT("Error: Failed to receive event\n"));
                else e->SetString(wxT("Disconnect successfully\n"));
                wxQueueEvent(evtHandler, e);
                closesocket(clientSocket);
                return nullptr;
            }
            totalReceived += received;
        }

        msg msg;
        memcpy(&msg, data, MSG_SIZE);

        if (msg.type != -1)
        {
            wxScreenDC screenDC;

            int screenWidth = screenDC.GetSize().GetWidth();
            int screenHeight = screenDC.GetSize().GetHeight();

            msg.x = (double)msg.x / SCREEN_WIDTH * screenWidth;
            msg.y = (double)msg.y / SCREEN_HEIGHT * screenHeight;
        }
        
        {
            wxCriticalSectionLocker lock(mQcs);
            if (msg.type != -1) msgQueue.push(msg);
        }
        
        delete[] data;
    }

    closesocket(clientSocket);
    return nullptr;
}