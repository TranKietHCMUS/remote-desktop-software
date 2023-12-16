#include <client/screenrecv.h>

wxDEFINE_EVENT(wxEVT_SCREENRECVTHREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_SCREENRECVTHREAD_COMPLETE, wxThreadEvent);

ScreenRecvThread::~ScreenRecvThread()
{
    callback->OnScreenRecvThreadDestruction();
}

wxThread::ExitCode ScreenRecvThread::Entry()
{
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENRECVTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to create socket\n"));
        e->SetInt(id);
        wxQueueEvent(evtHandler, e);
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);                 
    serverAddress.sin_addr.s_addr = inet_addr(ip.mb_str());

    if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENRECVTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to connect to server\n"));
        e->SetInt(id);
        wxQueueEvent(evtHandler, e);
        closesocket(clientSocket);
        return nullptr;
    }

    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENRECVTHREAD_UPDATE);
        e->SetString(wxT("Connect successfully\n"));
        e->SetId(id);
        wxQueueEvent(evtHandler, e);
    }

    while (true)
    {   
        if (TestDestroy())
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENRECVTHREAD_COMPLETE);
            e->SetInt(id);
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }
        
        size_t width;
        if (recv(clientSocket, reinterpret_cast<char *>(&width), sizeof(width), 0) == SOCKET_ERROR)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENRECVTHREAD_COMPLETE);
            e->SetString(wxT("Error: Failed to receive width of screen image\n"));
            e->SetInt(id);
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }

        size_t height;
        if (recv(clientSocket, reinterpret_cast<char *>(&height), sizeof(height), 0) == SOCKET_ERROR)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENRECVTHREAD_COMPLETE);
            e->SetString(wxT("Error: Failed to receive height of screen image\n"));
            e->SetInt(id);
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }

        size_t imageSize = width * height * 3;
        size_t totalReceived = 0;
        unsigned char* imageData = new unsigned char[imageSize];

        while (totalReceived < imageSize)
        {   
            int received = recv(clientSocket, (char *)imageData + totalReceived, imageSize - totalReceived, 0);
            if (received <= 0)
            {
                wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENRECVTHREAD_COMPLETE);
                e->SetString(wxT("Error: Failed to receive screen image\n"));
                e->SetInt(id);
                wxQueueEvent(evtHandler, e);
                closesocket(clientSocket);
                return nullptr;
            }
            totalReceived += received;
        }

        wxImage image(width, height, true);
        image.SetData(imageData);
        wxBitmap tmpBitmap(image);
        {
            wxCriticalSectionLocker lock(bcs);
            bitmap = tmpBitmap;
        }

        wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENRECVTHREAD_UPDATE);
        wxQueueEvent(evtHandler, e);
    }

    closesocket(clientSocket);
    return nullptr;
}