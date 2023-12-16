#ifndef EVENT_H
#define EVENTTHREAD_H

#include <../message.h>
#include <wx/wx.h>
#include <queue>

class EventSimCallback
{
public:
    virtual void OnEventSimThreadDestruction() = 0;
};

class EventSimThread : public wxThread
{
    public:
        EventSimThread(EventSimCallback *callback, std::queue<msg> &msgQueue, wxCriticalSection &mQcs)
                        : callback(callback), msgQueue(msgQueue), mQcs(mQcs){}
        virtual ~EventSimThread();

    protected:
        ExitCode Entry() override;

    private:
        EventSimCallback *callback;
        std::queue<msg> &msgQueue;
        wxCriticalSection &mQcs;
};

#endif