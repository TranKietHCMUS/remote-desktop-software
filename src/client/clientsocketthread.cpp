#include <clientsocketthread.h>

InputThread::InputThread(InputThreadCallback *callback, bool &stop, wxImage &screenImage, wxCriticalSection &sIcs)
    : callback(callback), stop(stop), screenImage(screenImage), sIcs(sIcs) {}
InputThread::~InputThread()
{
    callback->OnInputThreadDestruction();
}

wxThread::ExitCode InputThread::Entry()
{
    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        wxMessageBox("Failed to initialize Winsock.", "Error", wxOK | wxICON_ERROR);
        // Close();
        return nullptr;
    }

    // Tạo socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        wxMessageBox("Failed to create socket.", "Error", wxOK | wxICON_ERROR);
        WSACleanup();
        // Close();
        return nullptr;
    }

    // Kết nối đến server
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);                   // Cổng server
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Địa chỉ IP server

    if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        wxMessageBox("Failed to connect to the server.", "Error", wxOK | wxICON_ERROR);
        closesocket(clientSocket);
        WSACleanup();
        // Close();
        return nullptr;
    }
    wxPNGHandler *handler = new wxPNGHandler;
    wxImage::AddHandler(handler);

    while (!stop)
    {
        // Chuỗi cần gửi
        const char *message = ".";

        // Gửi chuỗi
        if (send(clientSocket, message, strlen(message), 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send string." << std::endl;
            break;
        }
        // Nhận kích thước dữ liệu ảnh
        size_t dataSizeNetworkOrder;
        if (recv(clientSocket, reinterpret_cast<char *>(&dataSizeNetworkOrder), sizeof(dataSizeNetworkOrder), 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to receive image data size." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
        }

        size_t dataSize = ntohl(dataSizeNetworkOrder);

        // Nhận dữ liệu ảnh
        std::vector<char> imageData(dataSize);
        size_t totalReceived = 0;
        while (totalReceived < dataSize)
        {
            int received = recv(clientSocket, imageData.data() + totalReceived, dataSize - totalReceived, 0);
            if (received == SOCKET_ERROR)
            {
                std::cerr << "Failed to receive image data." << std::endl;
                closesocket(clientSocket);
                WSACleanup();
                return nullptr;
            }
            totalReceived += received;
        }
        printf("%d\n", totalReceived);
        wxMemoryInputStream stream(imageData.data(), dataSize);
        {
            wxCriticalSectionLocker lock(sIcs);
            screenImage.LoadFile(stream, wxBITMAP_TYPE_PNG);
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return nullptr;
}