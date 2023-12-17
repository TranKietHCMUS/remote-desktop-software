#include <client/displayscreen.h>

DisplayWindow::DisplayWindow(wxWindow *parent, std::queue<msg> &msgQueue, wxCriticalSection &mQcs)
                            : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS), bitmap(SCREEN_WIDTH, SCREEN_HEIGHT), msgQueue(msgQueue), mQcs(mQcs)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(wxColor(255, 255, 255));

    Bind(wxEVT_PAINT, &DisplayWindow::OnPaint, this);
    Bind(wxEVT_MOTION, &DisplayWindow::OnMotion, this);
    Bind(wxEVT_LEFT_DOWN, &DisplayWindow::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &DisplayWindow::OnLeftUp, this);
    Bind(wxEVT_LEFT_DCLICK, &DisplayWindow::OnLeftDown, this);
    Bind(wxEVT_RIGHT_DOWN, &DisplayWindow::OnRightDown, this);
    Bind(wxEVT_RIGHT_UP, &DisplayWindow::OnRightUp, this);
    Bind(wxEVT_RIGHT_DCLICK, &DisplayWindow::OnRightDown, this);
    Bind(wxEVT_MOUSEWHEEL, &DisplayWindow::OnMouseWheel, this);
    Bind(wxEVT_KEY_DOWN, &DisplayWindow::OnKeyDown, this);
    Bind(wxEVT_KEY_UP, &DisplayWindow::OnKeyUp, this);
}

void DisplayWindow::SetBitmap(wxBitmap &bitmap)
{
    this->bitmap = bitmap;

    Refresh();
}

void DisplayWindow::OnPaint(wxPaintEvent &e)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    gc->DrawBitmap(bitmap, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    delete gc;
}

void DisplayWindow::OnMotion(wxMouseEvent &e)
{
    wxPoint pos = e.GetPosition();
    msg msg(0, 0, 0, pos.x, pos.y);

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnLeftDown(wxMouseEvent &e)
{
    wxPoint pos = e.GetPosition();
    msg msg(0, 1, 0, pos.x, pos.y);

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnLeftUp(wxMouseEvent &e)
{
    wxPoint pos = e.GetPosition();
    msg msg(0, 2, 0, pos.x, pos.y);

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnRightDown(wxMouseEvent &e)
{
    wxPoint pos = e.GetPosition();
    msg msg(0, 3, 0, pos.x, pos.y);

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnRightUp(wxMouseEvent &e)
{
    wxPoint pos = e.GetPosition();
    msg msg(0, 4, 0, pos.x, pos.y);

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnMouseWheel(wxMouseEvent &e)
{
    wxPoint pos = e.GetPosition();
    msg msg(0, 5, e.GetWheelRotation(), pos.x, pos.y);;

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnKeyDown(wxKeyEvent& e)
{
    msg msg(1, 0, 0, 0, 0, e.GetRawKeyCode());

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnKeyUp(wxKeyEvent& e)
{
    msg msg(1, 1, 0, 0, 0, e.GetRawKeyCode());

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}