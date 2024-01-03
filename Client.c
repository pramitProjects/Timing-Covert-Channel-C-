#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 2592

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("WSAStartup failed");
        return EXIT_FAILURE;
    }

    // Creating a socket
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Setting up server address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connecting to the server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        perror("Connection failed");
        closesocket(client_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    double threshold = 0.5;
    char data[21] = "";
    char fake_data[21] = "";

    // Receiving 20 bits of data from the server
    for (int i = 0; i < 20; ++i) {
        time_t start_time = time(NULL);
        recv(client_socket, fake_data + i, 1, 0);
        time_t end_time = time(NULL);
        double delay = difftime(end_time, start_time);

        // Comparing with the threshold
        if (delay < threshold) {
            data[i] = '0';
        } else {
            data[i] = '1';
        }
    }

    // Displaying the actual data known from the delays
    printf("Actual data: %s\n", data);

    // Displaying the wrong data that was sent by the server to misguide the network administrator
    printf("Received data: %s\n", fake_data);

    // Close the connection
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
