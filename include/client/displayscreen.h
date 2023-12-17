#ifndef DISPLAYSCREEN_H
#define DISPLAYSCREEN_H

#include <message.h>
#include <wx/wx.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <queue>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

class DisplayWindow : public wxWindow
{
    public:
    DisplayWindow(wxWindow *parent, std::queue<msg> &msgQueue, wxCriticalSection &mQcs);
    virtual ~DisplayWindow(){}

    void SetBitmap(wxBitmap &bitmap);
    
    private:
    wxBitmap bitmap;
    std::queue<msg> &msgQueue;
    wxCriticalSection &mQcs;

    void OnPaint(wxPaintEvent &e);
    void OnMotion(wxMouseEvent& e);
    void OnLeftDown(wxMouseEvent& e);
    void OnLeftUp(wxMouseEvent& e);
    void OnRightDown(wxMouseEvent& e);
    void OnRightUp(wxMouseEvent& e);
    void OnMouseWheel(wxMouseEvent& e);
    void OnKeyDown(wxKeyEvent& e);
    void OnKeyUp(wxKeyEvent& e);
};

#endif