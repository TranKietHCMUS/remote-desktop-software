#include <clientsocketthread.h>

InputThread::InputThread(InputThreadCallback *callback, wxImage &screenImage, wxCriticalSection &sIcs, wxSocketBase* socket)
    : callback(callback), screenImage(screenImage), sIcs(sIcs), socket(socket){}
InputThread::~InputThread()
{
    callback->OnInputThreadDestruction();
}

wxThread::ExitCode InputThread::Entry()
{
    const char response = '.';

    wxPNGHandler *handler = new wxPNGHandler;
    wxImage::AddHandler(handler);

    socket->SetFlags(wxSOCKET_WAITALL);

    wxUint32 imageSize;
    socket->Read(&imageSize, sizeof(wxUint32));
    std::vector<char> buf(imageSize);
    
    socket->Read(buf.data(), imageSize);
    
    wxMemoryInputStream stream(buf.data(), buf.size());
    
    {
        wxCriticalSectionLocker lock(sIcs);
        screenImage.LoadFile(stream, wxBITMAP_TYPE_PNG);
    }

    socket->Write(&response, sizeof(char));

    return nullptr;
}