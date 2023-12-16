#ifndef EVENTSEND_H
#define EVENTSEND_H

#include <../message.h>
#include <wx/wx.h>
#include <winsock2.h>
#include <queue>

class EventSendCallback
{
public:
    virtual void OnEventSendThreadDestruction() = 0;
};

wxDECLARE_EVENT(wxEVT_EVENTSENDTHREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_EVENTSENDTHREAD_COMPLETE, wxThreadEvent);

class EventSendThread : public wxThread
{
public:
    EventSendThread(EventSendCallback *callback, wxEvtHandler *evtHandler, int id, wxString &ip, std::queue<msg> &msgQueue, wxCriticalSection &mQcs)
                    : callback(callback), evtHandler(evtHandler), id(id), ip(ip), msgQueue(msgQueue), mQcs(mQcs){}
    virtual ~EventSendThread();

protected:
    ExitCode Entry() override;

private:
    EventSendCallback *callback;
    wxEvtHandler *evtHandler;
    int id;
    wxString &ip;
    std::queue<msg> &msgQueue;
    wxCriticalSection &mQcs;
};

#endif