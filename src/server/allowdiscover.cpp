#include <server/allowdiscover.h>

wxDEFINE_EVENT(wxEVT_ALLOWDISCOVERTHREAD_COMPLETE, wxThreadEvent);

AllowDiscoverThread::~AllowDiscoverThread()
{
    callback->OnAllowDiscoverThreadDestruction();
}

wxThread::ExitCode AllowDiscoverThread::Entry()
{
    SOCKET broadcastSocket;

    broadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (broadcastSocket == INVALID_SOCKET)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_ALLOWDISCOVERTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to create socket\n"));
        wxQueueEvent(evtHandler, e);
        return nullptr;
    }

    char broadcast = '1';

    if(setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST,&broadcast, sizeof(broadcast)) < 0)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_ALLOWDISCOVERTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to set option for socket\n"));;
        wxQueueEvent(evtHandler, e);
        closesocket(broadcastSocket);
        return nullptr;
    }

    sockaddr_in recvAddr;  
    sockaddr_in sendAddr;

    int len = sizeof(sockaddr_in);

    char sendChar = '.';
    char recvChar;

    recvAddr.sin_family = AF_INET;        
    recvAddr.sin_port = htons(16165);   
    recvAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(broadcastSocket, (sockaddr*)&recvAddr, len) < 0)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_ALLOWDISCOVERTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to bind socket\n"));;
        wxQueueEvent(evtHandler, e);
        closesocket(broadcastSocket);
        return nullptr;
    }

    while (true)
    {
        if (recvfrom(broadcastSocket, &recvChar, 1, 0, (sockaddr *)&sendAddr, &len) <= 0)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_ALLOWDISCOVERTHREAD_COMPLETE);
            e->SetString(wxT("Error: Failed to receive packet\n"));;
            wxQueueEvent(evtHandler, e);
            closesocket(broadcastSocket);
            return nullptr;
        }

        if (sendto(broadcastSocket, &sendChar, 1, 0, (sockaddr *)&sendAddr, len) == SOCKET_ERROR)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_ALLOWDISCOVERTHREAD_COMPLETE);
            e->SetString(wxT("Error: Failed to send packet\n"));;
            wxQueueEvent(evtHandler, e);
            closesocket(broadcastSocket);
            return nullptr;
        }
    }

    closesocket(broadcastSocket);
    return nullptr;
}