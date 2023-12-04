#include <eventthread.h>

EventThread::EventThread(EventThreadCallback *callback, std::queue<msg> &msgQueue, wxCriticalSection &mQcs)
    : callback(callback), msgQueue(msgQueue), mQcs(mQcs) {}
EventThread::~EventThread()
{
    callback->OnEventThreadDestruction();
}

wxThread::ExitCode EventThread::Entry()
{
    while(true)
    {
        msg msg;
        
        {
            wxCriticalSectionLocker lock(mQcs);
            if (msgQueue.empty()) continue;
            msg = msgQueue.front();
            msgQueue.pop();
        }

        if (msg.type)
        {
            if (!msg.flag) 
                keybd_event(msg.keyCode, 0, 0, 0);
            else 
                keybd_event(msg.keyCode, 0, 2, 0);
        }
        else
        {
            SetCursorPos(msg.x, msg.y);
            switch(msg.flag)
            {
                case 1:
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    break;
                case 2:
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    break;
                case 3:
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    break;
                case 4:
                    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
                    break;
                case 5:
                    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
                    break;
                case 6:
                    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
                    break;
                case 7:
                    mouse_event(MOUSEEVENTF_WHEEL, 0, 0, msg.data, 0);
            }
        }
    }
    return nullptr;
}