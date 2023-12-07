#include <clientscreensocketthread.h>

wxDEFINE_EVENT(wxEVT_SCREENSOCKETTHREAD_COMPLETED, wxThreadEvent);

ScreenSocketThread::ScreenSocketThread(ScreenSocketThreadCallback *callback, wxEvtHandler *evtHandler, wxString &ip, wxBitmap &bitmap, wxCriticalSection &bcs)
    : callback(callback), evtHandler(evtHandler), ip(ip), bitmap(bitmap), bcs(bcs) {}
ScreenSocketThread::~ScreenSocketThread()
{
    callback->OnScreenSocketThreadDestruction();
}

wxThread::ExitCode ScreenSocketThread::Entry()
{
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSOCKETTHREAD_COMPLETED);
        e->SetString(wxT("Failed to create socket\n"));
        wxQueueEvent(evtHandler, e);
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(16165);                 
    serverAddress.sin_addr.s_addr = inet_addr(ip.mb_str());

    if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSOCKETTHREAD_COMPLETED);
        e->SetString(wxT("Failed to connect to server\n"));
        wxQueueEvent(evtHandler, e);
        closesocket(clientSocket);
        return nullptr;
    }

    while (true)
    {
        if (wxThread::This()->TestDestroy())
        {
            break;
        }
        
        unsigned char* imageData = new unsigned char[SIZE];
        size_t totalReceived = 0;

        while (totalReceived < SIZE)
        {
            int received = recv(clientSocket, (char*)imageData + totalReceived, SIZE - totalReceived, 0);
            if (received <= 0)
            {
                wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSOCKETTHREAD_COMPLETED);
                e->SetString(wxT("Failed to receive image\n"));
                wxQueueEvent(evtHandler, e);
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

        wxBitmap tmpBitmap(image);
        {
            wxCriticalSectionLocker lock(bcs);
            bitmap = tmpBitmap;
        }

        delete[] imageData;
    }

    wxThreadEvent *e = new wxThreadEvent(wxEVT_SCREENSOCKETTHREAD_COMPLETED);
    e->SetString(wxT("Disconnect\n"));
    wxQueueEvent(evtHandler, e);
    closesocket(clientSocket);
    return nullptr;
}