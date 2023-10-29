#include <wx/wx.h>
#include <wx/dcbuffer.h>

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE | wxCLOSE_BOX | wxFULL_REPAINT_ON_RESIZE), capturedImage(1,1) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        // Set up a timer to continuously update the screen capture
        timer = new wxTimer(this, wxID_ANY);
        Bind(wxEVT_TIMER, &MyFrame::OnTimer, this, wxID_ANY);
        timer->Start(16); // Update screen capture every ~16 milliseconds (approximately 60 FPS)

        Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);
    }

    void OnPaint(wxPaintEvent& event) {
        wxAutoBufferedPaintDC dc(this);
        dc.DrawBitmap(wxBitmap(capturedImage), 0, 0);
    }

    void OnTimer(wxTimerEvent& event) {
        // Capture the full screen
        wxScreenDC screenDC;
        wxMemoryDC memDC;

        int screenWidth = screenDC.GetSize().GetWidth();
        int screenHeight = screenDC.GetSize().GetHeight();

        wxBitmap bitmap(screenWidth, screenHeight);
        memDC.SelectObject(bitmap);
        memDC.Blit(0, 0, screenWidth, screenHeight, &screenDC, 0, 0);
        memDC.SelectObject(wxNullBitmap);

        // Convert wxBitmap to wxImage
        capturedImage = bitmap.ConvertToImage();

        // Trigger a repaint to display the captured screen
        Refresh();
    }

    void OnClose(wxCloseEvent& event) {
        timer->Stop();
        Destroy();
    }

private:
    wxTimer* timer;
    wxImage capturedImage;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_PAINT(MyFrame::OnPaint)
wxEND_EVENT_TABLE()

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        // Create the main frame
        MyFrame *frame = new MyFrame("Screen Capture", wxPoint(100, 100), wxSize(800, 600));
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
