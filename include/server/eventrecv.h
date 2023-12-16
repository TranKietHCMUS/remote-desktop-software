#ifndef EVENTRECV_H
#define EVENTRECV_H

#include <../message.h>
#include <wx/wx.h>
#include <winsock2.h>
#include <queue>

class EventRecvCallback
{
public:
    virtual void OnEventRecvThreadDestruction() = 0;
};

wxDECLARE_EVENT(wxEVT_EVENTRECVTHREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_EVENTRECVTHREAD_COMPLETE, wxThreadEvent);

class EventRecvThread : public wxThread
{
public:
    EventRecvThread(EventRecvCallback *callback, wxEvtHandler *evtHandler, std::queue<msg> &msgQueue, wxCriticalSection &mQcs)
                    : callback(callback), evtHandler(evtHandler), msgQueue(msgQueue), mQcs(mQcs){}
    virtual ~EventRecvThread();

protected:
    ExitCode Entry() override;

private:
    EventRecvCallback *callback;
    wxEvtHandler *evtHandler;
    std::queue<msg> &msgQueue;
    wxCriticalSection &mQcs;
};

#endif