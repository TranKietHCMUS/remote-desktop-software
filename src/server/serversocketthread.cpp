#include <serversocketthread.h>

InputThread::InputThread(InputThreadCallback *callback)
    : callback(callback) {}
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
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return nullptr;
    }

    // Tạo socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return nullptr;
    }

    // Gắn socket với cổng
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Lắng nghe trên tất cả các địa chỉ IP
    serverAddress.sin_port = htons(8080);       // Cổng lắng nghe

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return nullptr;
    }

    while (1)
    {
        // Lắng nghe kết nối từ client
        listen(serverSocket, SOMAXCONN);

        printf("Waiting for client connected\n");

        // Chấp nhận kết nối từ client
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Failed to accept client connection." << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return nullptr;
        }

        printf("Client connected.\n");

        wxPNGHandler *handler = new wxPNGHandler;
        wxImage::AddHandler(handler);

        while (1)
        {
            char buffer[1024];
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead <= 0)
            {
                std::cerr << "Failed to receive data." << std::endl;
                break;
            }

            wxScreenDC screenDC;
            wxMemoryDC memDC;

            int screenWidth = screenDC.GetSize().GetWidth();
            int screenHeight = screenDC.GetSize().GetHeight();

            wxBitmap bitmap(screenWidth, screenHeight);
            memDC.SelectObject(bitmap);
            memDC.Blit(0, 0, screenWidth, screenHeight, &screenDC, 0, 0);

            wxImage image = bitmap.ConvertToImage();
            wxMemoryOutputStream stream;
            image.SaveFile(stream, wxBITMAP_TYPE_PNG);

            // Lấy con trỏ đến dữ liệu trong buffer
            const wxUint8 *data = reinterpret_cast<const wxUint8 *>(stream.GetOutputStreamBuffer()->GetBufferStart());
            size_t dataSize = stream.GetSize();

            // Gửi kích thước dữ liệu của ảnh đến máy client
            size_t dataSizeNetworkOrder = htonl(dataSize);
            if (send(clientSocket, reinterpret_cast<const char *>(&dataSizeNetworkOrder), sizeof(dataSizeNetworkOrder), 0) == SOCKET_ERROR)
            {
                // Gửi kích thước dữ liệu thất bại
                closesocket(clientSocket);
                WSACleanup();
                return nullptr;
            }

            // Gửi dữ liệu ảnh đến máy client
            if (send(clientSocket, reinterpret_cast<const char *>(data), dataSize, 0) == SOCKET_ERROR)
            {
                // Gửi dữ liệu ảnh thất bại
                closesocket(clientSocket);
                WSACleanup();
                return nullptr;
            }
            printf("Image sent successfully.\n");
        }
        // closesocket(clientSocket);
    }

    // Đóng kết nối và giải phóng tài nguyên
    // closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return nullptr;
}