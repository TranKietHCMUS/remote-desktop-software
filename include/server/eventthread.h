#ifndef EVENTTHREAH_H
#define EVENTTHREAD_H

#include <wx/wx.h>
#include <queue>
#include <../message.h>

class EventThreadCallback
{
public:
    virtual void OnEventThreadDestruction() = 0;
};

class EventThread : public wxThread
{
    public:
        EventThread(EventThreadCallback *callback, std::queue<msg> &msgQueue, wxCriticalSection &mQcs);
        virtual ~EventThread();

    protected:
        virtual ExitCode Entry();

    private:
        EventThreadCallback *callback;
        std::queue<msg> &msgQueue;
        wxCriticalSection &mQcs;
};

#endif