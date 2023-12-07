#ifndef DISPLAYSCREEN_H
#define DISPLAYSCREEN_H

#include <wx/wx.h>
#include <../message.h>
#include <queue>

enum
{
    wxID_REFRESH_TIMER = wxID_HIGHEST
};

class DisplayScreenFrame : public wxFrame
{
    public:
        DisplayScreenFrame(const wxString &title, const wxPoint &pos, const wxSize &size, wxBitmap &bitmap, wxCriticalSection &bcs, std::queue<msg> &msgQueue, wxCriticalSection &mQcs, long style = wxICONIZE | wxCAPTION | wxFRAME_NO_TASKBAR | wxFULL_REPAINT_ON_RESIZE);
        virtual ~DisplayScreenFrame();

        void OnClose();
    private:
        wxBitmap &bitmap;
        wxCriticalSection &bcs;
        wxTimer *refreshTimer;

        std::queue<msg> &msgQueue;
        wxCriticalSection &mQcs;

        void OnRefreshTimer(wxTimerEvent &e);
        void OnPaint(wxPaintEvent &e);

        void OnMotion(wxMouseEvent& e);
        void OnLeftDown(wxMouseEvent& e);
        void OnLeftUp(wxMouseEvent& e);
        void OnLeftDClick(wxMouseEvent& e);
        void OnRightDown(wxMouseEvent& e);
        void OnRightUp(wxMouseEvent& e);
        void OnRightDClick(wxMouseEvent& e);
        void OnMouseWheel(wxMouseEvent& e);
        void OnKeyDown(wxKeyEvent& e);
        void OnKeyUp(wxKeyEvent& e);
        
};

#endif