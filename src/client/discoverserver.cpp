#include <client/discoverserver.h>

wxDEFINE_EVENT(wxEVT_DISCOVERSERVERTHREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_DISCOVERSERVERTHREAD_COMPLETE, wxThreadEvent);

DiscoverServerThread::~DiscoverServerThread()
{
    callback->OnDiscoverServerThreadDestruction();
}

wxThread::ExitCode DiscoverServerThread::Entry()
{
    SOCKET broadcastSocket;

    broadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (broadcastSocket == INVALID_SOCKET)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_DISCOVERSERVERTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to create socket\n"));
        wxQueueEvent(evtHandler, e);
        return nullptr;
    }

    char broadcast = '1';

    if(setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_DISCOVERSERVERTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to set broadcast option for socket\n"));
        wxQueueEvent(evtHandler, e);
        closesocket(broadcastSocket);
        return nullptr;
    }
    int timeOut = 5000;
    if (setsockopt(broadcastSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeOut, sizeof(timeOut)) < 0)
    {
        wxThreadEvent *e = new wxThreadEvent(wxEVT_DISCOVERSERVERTHREAD_COMPLETE);
        e->SetString(wxT("Error: Failed to set time out for socket\n"));;
        wxQueueEvent(evtHandler, e);
        closesocket(broadcastSocket);
        return nullptr;
    }

    sockaddr_in recvAddr;  
    sockaddr_in sendAddr;

    int len = sizeof(sockaddr_in);

    char sendChar = '.';
    char recvChar;

    wxIPV4address addr;
    addr.Hostname(wxGetHostName());
    wxString localIPAddr = addr.IPAddress();
    int index = localIPAddr.Find(wxUniChar('.'), true);
    localIPAddr.Remove(index + 1);
    localIPAddr.Append(wxT("255"));

    recvAddr.sin_family = AF_INET;        
    recvAddr.sin_port = htons(16165);   
    recvAddr.sin_addr.s_addr = inet_addr(localIPAddr.mb_str());

    if (sendto(broadcastSocket, &sendChar, 1, 0, (sockaddr *)&recvAddr, len) == SOCKET_ERROR)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_DISCOVERSERVERTHREAD_UPDATE);
            e->SetString(wxT("Error: Failed to send packet\n"));
            wxQueueEvent(evtHandler, e);
            closesocket(broadcastSocket);
            return nullptr;
        }

    while (true)
    { 
        if (recvfrom(broadcastSocket, &recvChar, 1, 0, (sockaddr *)&sendAddr, &len) > 0)
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_DISCOVERSERVERTHREAD_UPDATE);
            in_addr clientAddr = sendAddr.sin_addr;
            char* ipAddr = inet_ntoa(clientAddr);
            wxString ip(ipAddr, wxConvUTF8);
            e->SetString(ip);
            wxQueueEvent(evtHandler, e);
        }
        else
        {
            wxThreadEvent *e = new wxThreadEvent(wxEVT_DISCOVERSERVERTHREAD_COMPLETE);
            e->SetString(wxT("Error: Failed to receive packet\n"));
            wxQueueEvent(evtHandler, e);
            closesocket(broadcastSocket);
            return nullptr;
        }
    }

    closesocket(broadcastSocket);
    return nullptr;
}