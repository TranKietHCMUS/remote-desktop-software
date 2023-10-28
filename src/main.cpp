#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/statbmp.h>

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size) {
        wxButton* captureButton = new wxButton(this, wxID_ANY, "Capture and Update Image", wxPoint(10, 10));
        Bind(wxEVT_BUTTON, &MyFrame::OnCaptureButtonClick, this, captureButton->GetId());

        // Initial image
        wxImage::AddHandler(new wxPNGHandler);
        originalImage = wxImage("download.png", wxBITMAP_TYPE_PNG); // Replace "image.png" with your image file path
        
        // Create a wxStaticBitmap control to display the image
        staticBitmap = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxPoint(10, 50), wxSize(300, 200));
        UpdateImage(originalImage); // Display the initial image
    }

    void OnCaptureButtonClick(wxCommandEvent& event) {
        // Modify the image (for example, rotate it)
        wxImage modifiedImage = originalImage.Rotate90(clockwise); // Rotate the image 90 degrees clockwise
        clockwise = !clockwise; // Toggle the rotation direction for the next click
        
        UpdateImage(modifiedImage); // Update the displayed image with the modified image
    }

    void UpdateImage(const wxImage& image) {
        // Create a bitmap from the image
        wxBitmap bitmap(image);
        
        // Set the bitmap to the wxStaticBitmap control to update the displayed image
        staticBitmap->SetBitmap(bitmap);
    }

private:
    wxImage originalImage;
    bool clockwise = true; // Rotation direction flag
    wxStaticBitmap* staticBitmap;
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame("Image Display and Modification Example", wxPoint(10, 10), wxSize(800, 600));
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);