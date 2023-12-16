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
        
        size_t width;
        size_t height;
        size_t imageSize;
        unsigned char* imageData;
        {
            wxCriticalSectionLocker lock(ics);
            width = image.GetWidth();
            height = image.GetHeight();
            imageSize = width * height * 3;
            unsigned char* data = image.GetData();
            imageData = new unsigned char[imageSize];
            memmove(imageData, data, imageSize);
        }

        if (send(clientSocket, reinterpret_cast<const char *>(&width), sizeof(width), 0) == SOCKET_ERROR)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSENDTHREAD_COMPLETE);
            e->SetString(wxT("Error: Failed to send width of screen image\n"));
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }

        if (send(clientSocket, reinterpret_cast<const char *>(&height), sizeof(height), 0) == SOCKET_ERROR)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSENDTHREAD_COMPLETE);
            e->SetString(wxT("Error: Failed to send height of screen image\n"));
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }

        if (send(clientSocket, (char *)imageData, imageSize, 0) == SOCKET_ERROR)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSENDTHREAD_COMPLETE);
            e->SetString(wxT("Error: Failed to send image\n"));
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }

        delete[] imageData;
    }

    closesocket(clientSocket);
    return nullptr;
}