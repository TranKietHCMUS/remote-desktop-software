#include <server/screensend.h>

ScreenSendThread::~ScreenSendThread()
{
    callback->OnScreenSendThreadDestruction();
}

wxDEFINE_EVENT(wxEVT_SCREENSENDTHREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_SCREENSENDTHREAD_COMPLETE, wxThreadEvent);

wxThread::ExitCode ScreenSendThread::Entry()
{
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSENDTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to create socket\n"));
        wxQueueEvent(evtHandler, e);
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSENDTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to bind socket\n"));
        wxQueueEvent(evtHandler, e);
        closesocket(serverSocket);
        return nullptr;
    }

    listen(serverSocket, SOMAXCONN);

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSENDTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to accept connection\n"));
        wxQueueEvent(evtHandler, e);
        closesocket(serverSocket);
        return nullptr;
    }

    closesocket(serverSocket);

    wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSENDTHREAD_UPDATE);
    wxQueueEvent(evtHandler, e);

    while (true)
    {   
        if (TestDestroy())
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSENDTHREAD_COMPLETE);
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }
        
        unsigned char* imageData = new unsigned char[SCREEN_SIZE];
        {
            wxCriticalSectionLocker lock(ics);
            unsigned char* data = image.GetData();
            memmove(imageData, data, SCREEN_SIZE);
        }

        if (send(clientSocket, (char *)imageData, SCREEN_SIZE, 0) == SOCKET_ERROR)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSENDTHREAD_COMPLETE);
            if (WSAGetLastError() == WSAECONNRESET) e->SetString(wxT("Disconnect successfully\n"));
            else e->SetString(wxT("Error: Failed to send image\n"));
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }

        delete[] imageData;
    }

    closesocket(clientSocket);
    return nullptr;
}