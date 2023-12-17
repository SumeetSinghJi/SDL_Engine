#include <iostream>
#include <WinSock2.h>
#include <string>

/* TO DO
* Upgrade to works with linux also
* ChatGPT - how can I make below code more secure?
* Wrap in class - try myself first, then on failure ask ChatGPT
* have a local index.html file that can be accessible and viewed by the client via web server
* have an error.html page for error's to display to user if index.html (Home page) is not found
* have another leaderboard.html web page that the user can browse to from index.htm
* Basic multiplayer game
*/

int main()
{

    std::cout << "Attempting to create a web server" << std::endl;

    SOCKET wsocket;
    SOCKET new_wsocket;
    WSADATA wsaData;
    struct sockaddr_in server;
    int server_len;
    int BUFFER_SIZE = 30720;
    std::string clientIPAddress = "127.0.0.1";
    int clientPortNumber = 8080;

    // Initialise Web server
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Could not initialise Winsocket" << std::endl;
    }

    // Create a web socket
    // AF_INET - IPV4
    // SOCK_STREAM - TCP
    // IPPROTO_TCP - TCP
    wsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (wsocket == INVALID_SOCKET)
    {
        std::cout << "Error: Could not create a web socket" << std::endl;
    }

    // bind socket to address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(clientIPAddress.c_str());
    server.sin_port = htons(clientPortNumber);
    server_len = sizeof(server);

    if (bind(wsocket, (SOCKADDR *)&server, server_len) != 0)
    {
        std::cout << "Error: Could not bind address: " << clientIPAddress << ":" << clientPortNumber << " to web socket" << std::endl;
    }

    // listen for address
    if (listen(wsocket, 20) != 0)
    {
        std::cout << "Error: Could not listen to anything on address: " << clientIPAddress << ":" << clientPortNumber << std::endl;
    }
    else
    {
        std::cout << "Successfully listening address: " << clientIPAddress << ":" << clientPortNumber << std::endl;
    }

    // bytes being recieved
    int bytes = 0;
    while (true)
    {
        // accept client request
        new_wsocket = accept(wsocket, (SOCKADDR *)&server, &server_len);
        if (new_wsocket == INVALID_SOCKET)
        {
            std::cout << "Error: Unable to accept client request: \n"
                      << std::endl;
        }

        // read request
        char buff[30720] = {0};
        bytes = recv(new_wsocket, buff, BUFFER_SIZE, 0);
        if (bytes < 0)
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
            bytesSent = send(new_wsocket, responseBuffer + totalBytesSent, serverMessage.size() - totalBytesSent, 0);

            if (bytesSent <= 0)
            {
                std::cout << "Error: Couldn't send response" << std::endl;
                break; // Break out of the loop on send error
            }

            totalBytesSent += bytesSent;
        }

        if (totalBytesSent == serverMessage.size())
        {
            std::cout << "Response sent successfully" << std::endl;
        }
        else
        {
            std::cout << "Error: Couldn't send complete response" << std::endl;
        }
        closesocket(new_wsocket);
    }

    closesocket(wsocket);
    WSACleanup();

    return 0;
}