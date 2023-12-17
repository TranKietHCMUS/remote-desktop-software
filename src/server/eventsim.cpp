#include <server/eventsim.h>

EventSimThread::~EventSimThread()
{
    callback->OnEventSimThreadDestruction();
}

wxThread::ExitCode EventSimThread::Entry()
{
    while(true)
    {
        if (TestDestroy())
        {
            return nullptr;
        }

        msg msg;
        {
            wxCriticalSectionLocker lock(mQcs);
            if (msgQueue.empty()) continue;
            msg = msgQueue.front();
            msgQueue.pop();
        }

        if (!msg.type)
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
                    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
                    break;
                case 4:
                    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
                    break;
                case 5:
                    mouse_event(MOUSEEVENTF_WHEEL, 0, 0, msg.data, 0);
                    break;
            }
        }
        else
        {
            if (!msg.flag) 
                keybd_event(msg.keyCode, 0, 0, 0);
            else 
                keybd_event(msg.keyCode, 0, 2, 0);
        }
    }

    return nullptr;
}