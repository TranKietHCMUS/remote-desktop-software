#include <serversocketthread.h>


InputThread::InputThread(InputThreadCallback *callback, wxImage &capturedImage, wxCriticalSection &cIcs, wxSocketBase *socket)
    : callback(callback), capturedImage(capturedImage), cIcs(cIcs), socket(socket){}
InputThread::~InputThread()
{
    callback->OnInputThreadDestruction();
}

wxThread::ExitCode InputThread::Entry()
{
    char response;

    socket->SetFlags(wxSOCKET_WAITALL);

    socket->Read(&response, sizeof(char));

    wxPNGHandler *handler = new wxPNGHandler;
    wxImage::AddHandler(handler);

    wxMemoryOutputStream stream;
    
    {
        wxCriticalSectionLocker lock(cIcs);
        capturedImage.SaveFile(stream, wxBITMAP_TYPE_PNG);
    }

    wxUint32 imageSize = stream.GetSize();
    socket->Write(&imageSize, sizeof(wxUint32));
    socket->Write(stream.GetOutputStreamBuffer()->GetBufferStart(), imageSize);

    return nullptr;
}