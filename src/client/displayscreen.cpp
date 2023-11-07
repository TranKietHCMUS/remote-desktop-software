#include <displayscreen.h>

wxBEGIN_EVENT_TABLE(DisplayScreenFrame, wxFrame)
    EVT_PAINT(DisplayScreenFrame::OnPaint)
wxEND_EVENT_TABLE()

DisplayScreenFrame::DisplayScreenFrame(const wxString &title, const wxPoint &pos, const wxSize &size, wxImage &screenImage, long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style), screenImage(screenImage)
{ 
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    refreshTimer = new wxTimer(this, wxID_REFRESH_TIMER);
    Bind(wxEVT_TIMER, &DisplayScreenFrame::OnRefreshTimer, this, wxID_REFRESH_TIMER);
    refreshTimer->Start(16);

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
    dc.DrawBitmap(wxBitmap(screenImage), 0, 0);
}

void DisplayScreenFrame::OnClose(wxCloseEvent &e)
{
    refreshTimer->Stop();
    Destroy();
}