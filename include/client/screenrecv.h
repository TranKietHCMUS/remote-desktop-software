#ifndef SCREENRECV_H
#define SCREENRECV_H

#include <wx/wx.h>
#include <winsock2.h>

class ScreenRecvCallback
{
    public:
    virtual void OnScreenRecvThreadDestruction() = 0;
};

wxDECLARE_EVENT(wxEVT_SCREENRECVTHREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_SCREENRECVTHREAD_COMPLETE, wxThreadEvent);

class ScreenRecvThread : public wxThread
{
    public:
    ScreenRecvThread(ScreenRecvCallback *callback, wxEvtHandler *evtHandler, int id, wxString ip, wxBitmap &bitmap, wxCriticalSection &bcs)
                    : callback(callback), evtHandler(evtHandler), id(id), ip(ip), bitmap(bitmap), bcs(bcs){}
    virtual ~ScreenRecvThread();

    protected:
    ExitCode Entry() override;

    private:
    ScreenRecvCallback *callback;
    wxEvtHandler *evtHandler;
    int id;
    wxString ip;
    wxBitmap &bitmap;
    wxCriticalSection &bcs;
};

#endif