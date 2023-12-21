#include <iostream>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <string>

/* TO DO
 * index.html
 * error.html
 * leaderboard.html
 * robots.txt
 * input validation - prevent SQL injection, buffer overflow, XSS attacks
 * limit buffer sizes
 * secure socket operations
 * Enable https using (SSL/TLS) create a cert
 * Content Security Policy (CSP)
 */

class SUMEETS_WEBSERVER
{
private:
#ifdef _WIN32
    SOCKET serverSocket;
    SOCKET newServerSocket;
    WSADATA wsaData;
#else
    int serverSocket;
    int newServerSocket;
#endif

    // Global private variables
    struct sockaddr_in server;
    int server_len;
    int BUFFER_SIZE = 30720;
    std::string clientIPAddress = "";
    int clientPortNumber = 0;
    int bytesReceived = 0;

public:
    SUMEETS_WEBSERVER(const std::string &ipAddress, int portNumber) : BUFFER_SIZE(30720), clientIPAddress(ipAddress), clientPortNumber(portNumber) {}

    void setIPAddress(const std::string &ipAddress)
    {
        this->clientIPAddress = ipAddress;
    }

    void setPortNumber(int portNumber)
    {
        this->clientPortNumber = portNumber;
    }

    bool initialise_web_server()
    {
        std::cout << "Attempting to create a web server" << std::endl;

// Windows - Initialise Web server
#ifdef _WIN32
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cout << "Could not initialise Winsocket" << std::endl;
        }
#endif

        // Create a web socket
        // AF_INET - IPV4 (Domain)
        // SOCK_STREAM - Asynchronous, Full-duplex (Type)
        // IPPROTO_TCP - TCP (Protocol)
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET)
        {
            std::cout << "Error: Could not create a web socket" << std::endl;
        }

        // bind socket to address
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(clientIPAddress.c_str());
        server.sin_port = htons(clientPortNumber);
        server_len = sizeof(server);

        if (bind(serverSocket, (SOCKADDR *)&server, server_len) != 0)
        {
            std::cout << "Error: Could not bind address: " << clientIPAddress << ":" << clientPortNumber << " to web socket" << std::endl;
        }

        // listen for address
        if (listen(serverSocket, 20) != 0)
        {
            std::cout << "Error: Could not listen to anything on address: " << clientIPAddress << ":" << clientPortNumber << std::endl;
        }
        return true;
    }

    bool run_web_server()
    {
        while (true)
        {
// accept client request
#ifdef _WIN32
            newServerSocket = accept(serverSocket, (SOCKADDR *)&server, &server_len);
#else
            newServerSocket = accept(serverSocket, (struct sockaddr *)&server, &server_len);
#endif

            if (newServerSocket == INVALID_SOCKET)
            {
                std::cout << "Error: Unable to accept client request: \n"
                          << std::endl;
            }

            // read request
            char buff[30720] = {0};
            bytesReceived = recv(newServerSocket, buff, BUFFER_SIZE, 0);
            if (bytesReceived < 0)
            {
                std::cout << "Error: Could not read client request" << std::endl;
            }

            // Response to Client browser
            std::string serverMessage = "HTTP/1.1 200 OK\n"
                                        "Content-Type: text/html\n"
                                        "Content-Length: ";
            std::string response = "<html><h1>Hello world</h1></html>";
            serverMessage.append(std::to_string(response.size()));
            serverMessage.append("\n\n");
            serverMessage.append(response);

            // Tracking if the size of the response sent to client when they view web server from web browser, matches total response size
            int bytesSent = 0;
            int totalBytesSent = 0;
            const char *responseBuffer = serverMessage.c_str(); // Pointer to the beginning of the response

            while (totalBytesSent < serverMessage.size())
            {
                bytesSent = send(newServerSocket, responseBuffer + totalBytesSent, serverMessage.size() - totalBytesSent, 0);

                if (bytesSent <= 0)
                {
                    std::cout << "Error: Couldn't send response" << std::endl;
                    break; // Break out of the loop on send error
                }

                totalBytesSent += bytesSent;
            }

            if (totalBytesSent != serverMessage.size())
            {
                std::cout << "Error: Full response from server not sent to client. Byte size mismatch" << std::endl;
            }
            closesocket(newServerSocket);
        }

        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
    };
};

int main()
{
    SUMEETS_WEBSERVER server("127.0.0.1", 8080);
    if (server.initialise_web_server())
    {
        server.run_web_server();
    }
    return 0;
}
