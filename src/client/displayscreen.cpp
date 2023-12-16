#include <client/displayscreen.h>

DisplayWindow::DisplayWindow(wxWindow *parent, std::queue<msg> &msgQueue, wxCriticalSection &mQcs)
                            : wxScrolled<wxWindow>(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE | wxVSCROLL | wxHSCROLL | wxWANTS_CHARS), bitmap(1920, 1080), msgQueue(msgQueue), mQcs(mQcs)
{
    drawPointX = 0;
    drawPointY = 0;
    
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(wxColor(255, 255, 255));
    SetScrollRate(5, 5);
    SetVirtualSize(bitmap.GetWidth(), bitmap.GetHeight());

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
    
    SetVirtualSize(bitmap.GetWidth(), bitmap.GetHeight());

    Refresh();
}

void DisplayWindow::OnPaint(wxPaintEvent &e)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    DoPrepareDC(dc);

    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    wxSize size = GetClientSize();
    drawPointX = size.GetWidth() > bitmap.GetWidth() ? (size.GetWidth() - bitmap.GetWidth()) / 2 : 0;
    drawPointY = size.GetHeight() > bitmap.GetHeight() ? (size.GetHeight() - bitmap.GetHeight()) / 2 : 0;

    gc->DrawBitmap(bitmap, drawPointX, drawPointY, bitmap.GetWidth(), bitmap.GetHeight());
    delete gc;
}

void DisplayWindow::OnMotion(wxMouseEvent &e)
{
    wxPoint scrollPos = e.GetPosition();
    wxPoint pos = CalcUnscrolledPosition(scrollPos);
    msg msg(0, 0, 0, pos.x - drawPointX, pos.y - drawPointY);
    if (msg.x < 0 || msg.y < 0) return;
    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnLeftDown(wxMouseEvent &e)
{
    wxPoint scrollPos = e.GetPosition();
    wxPoint pos = CalcUnscrolledPosition(scrollPos);
    msg msg(0, 1, 0, pos.x - drawPointX, pos.y - drawPointY);
    if (msg.x < 0 || msg.y < 0) return;
    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnLeftUp(wxMouseEvent &e)
{
    wxPoint scrollPos = e.GetPosition();
    wxPoint pos = CalcUnscrolledPosition(scrollPos);;
    msg msg(0, 2, 0, pos.x - drawPointX, pos.y - drawPointY);
    if (msg.x < 0 || msg.y < 0) return;
    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnRightDown(wxMouseEvent &e)
{
    wxPoint scrollPos = e.GetPosition();
    wxPoint pos = CalcUnscrolledPosition(scrollPos);
    msg msg(0, 3, 0, pos.x - drawPointX, pos.y - drawPointY);
    if (msg.x < 0 || msg.y < 0) return;
    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnRightUp(wxMouseEvent &e)
{
    wxPoint scrollPos = e.GetPosition();
    wxPoint pos = CalcUnscrolledPosition(scrollPos);
    msg msg(0, 4, 0, pos.x - drawPointX, pos.y - drawPointY);
    if (msg.x < 0 || msg.y < 0) return;
    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    e.Skip();
}

void DisplayWindow::OnMouseWheel(wxMouseEvent &e)
{
    wxPoint scrollPos = e.GetPosition();
    wxPoint pos = CalcUnscrolledPosition(scrollPos);
    msg msg(0, 5, e.GetWheelRotation(), pos.x - drawPointX, pos.y - drawPointY);
    if (msg.x < 0 || msg.y < 0) return;
    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }
}

void DisplayWindow::OnKeyDown(wxKeyEvent& e)
{
    msg msg(1, 0, 0, 0, 0, e.GetRawKeyCode());

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    if (msg.keyCode != VK_LEFT && msg.keyCode != VK_UP && msg.keyCode != VK_RIGHT && msg.keyCode != VK_DOWN) e.Skip();
}

void DisplayWindow::OnKeyUp(wxKeyEvent& e)
{
    msg msg(1, 1, 0, 0, 0, e.GetRawKeyCode());

    {
        wxCriticalSectionLocker lock(mQcs);
        msgQueue.push(msg);
    }

    if (msg.keyCode != VK_LEFT && msg.keyCode != VK_UP && msg.keyCode != VK_RIGHT && msg.keyCode != VK_DOWN) e.Skip();
}