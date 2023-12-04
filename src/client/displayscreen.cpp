#include <displayscreen.h>

DisplayScreenFrame::DisplayScreenFrame(const wxString &title, const wxPoint &pos, const wxSize &size, wxImage &screenImage, wxCriticalSection &sIcs, std::queue<msg> &msgQueue, wxCriticalSection &mQcs, long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style), screenImage(screenImage), sIcs(sIcs), msgQueue(msgQueue), mQcs(mQcs)
{ 
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    refreshTimer = new wxTimer(this, wxID_REFRESH_TIMER);
    Bind(wxEVT_TIMER, &DisplayScreenFrame::OnRefreshTimer, this, wxID_REFRESH_TIMER);
    refreshTimer->Start(16);

    Bind(wxEVT_MOTION, &DisplayScreenFrame::OnMotion, this);
    Bind(wxEVT_LEFT_DOWN, &DisplayScreenFrame::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &DisplayScreenFrame::OnLeftUp, this);
    Bind(wxEVT_LEFT_DCLICK, &DisplayScreenFrame::OnLeftDClick, this);
    Bind(wxEVT_RIGHT_DOWN, &DisplayScreenFrame::OnRightDown, this);
    Bind(wxEVT_RIGHT_UP, &DisplayScreenFrame::OnRightUp, this);
    Bind(wxEVT_RIGHT_DCLICK, &DisplayScreenFrame::OnRightDClick, this);
    Bind(wxEVT_MOUSEWHEEL, &DisplayScreenFrame::OnMouseWheel, this);
    Bind(wxEVT_KEY_DOWN, &DisplayScreenFrame::OnKeyDown, this);
    Bind(wxEVT_KEY_UP, &DisplayScreenFrame::OnKeyUp, this);

    Bind(wxEVT_PAINT, &DisplayScreenFrame::OnPaint, this);

    Bind(wxEVT_CLOSE_WINDOW, &DisplayScreenFrame::OnClose, this);
}

DisplayScreenFrame::~DisplayScreenFrame(){}

void DisplayScreenFrame::OnRefreshTimer(wxTimerEvent &e)
{
    Refresh();
}

void DisplayScreenFrame::OnMotion(wxMouseEvent& e)
{
    wxPoint p = e.GetPosition();
    msg msg;
    msg.type = 0;
    msg.flag = 0;
    msg.x = p.x;
    msg.y = p.y;
    msg.data = 0; 
    msg.keyCode = 0;

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayScreenFrame::OnLeftDown(wxMouseEvent& e)
{
    wxPoint p = e.GetPosition();
    msg msg;
    msg.type = 0;
    msg.flag = 1;
    msg.x = p.x;
    msg.y = p.y;
    msg.data = 0; 
    msg.keyCode = 0;

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }
    
    e.Skip();
}

void DisplayScreenFrame::OnLeftUp(wxMouseEvent& e)
{
    wxPoint p = e.GetPosition();
    msg msg;
    msg.type = 0;
    msg.flag = 2;
    msg.x = p.x;
    msg.y = p.y; 
    msg.data = 0;
    msg.keyCode = 0;

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }
    
    e.Skip();
}

void DisplayScreenFrame::OnLeftDClick(wxMouseEvent& e)
{
    wxPoint p = e.GetPosition();
    msg msg;
    msg.type = 0;
    msg.flag = 3;
    msg.x = p.x;
    msg.y = p.y; 
    msg.data = 0;
    msg.keyCode = 0;

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }
    
    e.Skip();
}

void DisplayScreenFrame::OnRightDown(wxMouseEvent& e)
{
    wxPoint p = e.GetPosition();
    msg msg;
    msg.type = 0;
    msg.flag = 4;
    msg.x = p.x;
    msg.y = p.y; 
    msg.data = 0;
    msg.keyCode = 0;

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }
    
    e.Skip();
}

void DisplayScreenFrame::OnRightUp(wxMouseEvent& e)
{
    wxPoint p = e.GetPosition();
    msg msg;
    msg.type = 0;
    msg.flag = 5;
    msg.x = p.x;
    msg.y = p.y; 
    msg.data = 0;
    msg.keyCode = 0;

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }
    
    e.Skip();
}

void DisplayScreenFrame::OnRightDClick(wxMouseEvent& e)
{
    wxPoint p = e.GetPosition();
    msg msg;
    msg.type = 0;
    msg.flag = 6;
    msg.x = p.x;
    msg.y = p.y; 
    msg.data = 0;
    msg.keyCode = 0;

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }
    
    e.Skip();
}

void DisplayScreenFrame::OnMouseWheel(wxMouseEvent& e)
{
    wxPoint p = e.GetPosition();
    msg msg;
    msg.type = 0;
    msg.flag = 7;
    msg.x = p.x;
    msg.y = p.y;
    msg.data = e.GetWheelRotation(); 
    msg.keyCode = 0;

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }
    
    e.Skip();
}

void DisplayScreenFrame::OnKeyDown(wxKeyEvent& e)
{
    msg msg;
    msg.type = 1;
    msg.flag = 0;
    msg.x = 0;
    msg.y = 0;
    msg.keyCode = e.GetRawKeyCode();

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayScreenFrame::OnKeyUp(wxKeyEvent& e)
{
    msg msg;
    msg.type = 1;
    msg.flag = 1;
    msg.x = 0;
    msg.y = 0;
    msg.keyCode = e.GetRawKeyCode();
    
    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
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