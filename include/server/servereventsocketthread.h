#ifndef SERVEREVENTSOCKETTHREAD_H
#define SERVEREVENTSOCKETTHREAD_H

#include <wx/wx.h>
#include <winsock2.h>
#include <../message.h>
#include <queue>

class EventSocketThreadCallback
{
    public:
        virtual void OnEventSocketThreadDestruction() = 0;
};

class EventSocketThread : public wxThread
{
    public:
        EventSocketThread(EventSocketThreadCallback *callback, std::queue<msg> &msgQueue, wxCriticalSection &mQcs);
        virtual ~EventSocketThread();

    protected:
        virtual ExitCode Entry();

    private:
        EventSocketThreadCallback *callback;
        std::queue<msg> &msgQueue;
        wxCriticalSection &mQcs;
};

#endif