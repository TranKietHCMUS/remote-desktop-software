#ifndef SCREENSEND_H
#define SCREENSEND_H

#include <wx/wx.h>
#include <winsock2.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 2764800

class ScreenSendCallback
{
    public:
    virtual void OnScreenSendThreadDestruction() = 0;
};

wxDECLARE_EVENT(wxEVT_SCREENSENDTHREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_SCREENSENDTHREAD_COMPLETE, wxThreadEvent);

class ScreenSendThread : public wxThread
{
    public:
    ScreenSendThread(ScreenSendCallback *callback, wxEvtHandler *evtHandler, wxImage &image, wxCriticalSection &ics)
                    : callback(callback), evtHandler(evtHandler), image(image), ics(ics){}
    virtual ~ScreenSendThread();

    protected:
    ExitCode Entry() override;

    private:
    ScreenSendCallback *callback;
    wxEvtHandler *evtHandler;
    wxImage &image;
    wxCriticalSection &ics;
};

#endif