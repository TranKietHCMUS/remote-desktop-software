#include <client/eventsend.h>

wxDEFINE_EVENT(wxEVT_EVENTSENDTHREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_EVENTSENDTHREAD_COMPLETE, wxThreadEvent);

EventSendThread::~EventSendThread()
{
    callback->OnEventSendThreadDestruction();
}

wxThread::ExitCode EventSendThread::Entry()
{
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTSENDTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to create socket\n"));
        e->SetInt(id);
        wxQueueEvent(evtHandler, e);
        return nullptr;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(51265);                 
    serverAddress.sin_addr.s_addr = inet_addr(ip.mb_str());

    if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTSENDTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to connect to server\n"));
        e->SetInt(id);
        wxQueueEvent(evtHandler, e);
        closesocket(clientSocket);
        return nullptr;
    }

    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTSENDTHREAD_UPDATE);
        e->SetString(wxT("Connect successfully\n"));
        e->SetId(id);
        wxQueueEvent(evtHandler, e);
    }

    while (true)
    {
        if (TestDestroy())
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTSENDTHREAD_COMPLETE);
            e->SetInt(id);
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            return nullptr;
        }
        
        msg msg;
        {
            wxCriticalSectionLocker lock(mQcs);
            if (!msgQueue.empty()) 
            {
                msg = msgQueue.front();
                msgQueue.pop();
            }
            else Sleep(50);
        }

        char* msgData = new char[MSG_SIZE];
        memcpy(msgData, &msg, MSG_SIZE);

        if (send(clientSocket, (char *)msgData, MSG_SIZE, 0) == SOCKET_ERROR)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_EVENTSENDTHREAD_COMPLETE);
            e->SetString(wxT("Error: Failed to send event\n"));
            e->SetInt(id);
            wxQueueEvent(evtHandler, e);
            closesocket(clientSocket);
            delete[] msgData;
            return nullptr;
        }
    }

    closesocket(clientSocket);
    return nullptr;
}