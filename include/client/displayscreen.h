#ifndef DISPLAYSCREEN_H
#define DISPLAYSCREEN_H

#include <wx/wx.h>

enum
{
    wxID_REFRESH_TIMER = wxID_HIGHEST
};

class DisplayScreenFrame : public wxFrame
{
    public:
        DisplayScreenFrame(const wxString &title, const wxPoint &pos, const wxSize &size, wxImage &screenImage, long style = wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE);
        virtual ~DisplayScreenFrame();
    private:
        wxImage &screenImage;
        wxTimer *refreshTimer;

        void OnRefreshTimer(wxTimerEvent &e);
        void OnPaint(wxPaintEvent &e);
        void OnClose(wxCloseEvent &e);
};

#endif