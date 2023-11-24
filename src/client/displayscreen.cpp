#include <displayscreen.h>

DisplayScreenFrame::DisplayScreenFrame(const wxString &title, const wxPoint &pos, const wxSize &size, wxImage &screenImage, wxCriticalSection &sIcs, long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style), screenImage(screenImage), sIcs(sIcs)
{ 
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    refreshTimer = new wxTimer(this, wxID_REFRESH_TIMER);
    Bind(wxEVT_TIMER, &DisplayScreenFrame::OnRefreshTimer, this, wxID_REFRESH_TIMER);
    refreshTimer->Start(16);

    Bind(wxEVT_PAINT, &DisplayScreenFrame::OnPaint, this);

    Bind(wxEVT_CLOSE_WINDOW, &DisplayScreenFrame::OnClose, this);
}

DisplayScreenFrame::~DisplayScreenFrame(){}

void DisplayScreenFrame::OnRefreshTimer(wxTimerEvent &e)
{
    Refresh();
}

void DisplayScreenFrame::OnPaint(wxPaintEvent &e)
{
    wxPaintDC dc(this);
    wxCriticalSectionLocker lock(sIcs);
    dc.DrawBitmap(wxBitmap(screenImage), 0, 0);
}

void DisplayScreenFrame::OnClose(wxCloseEvent &e)
{
    refreshTimer->Stop();
    Destroy();
}