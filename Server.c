#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 2592

void compute_bandwidth(int bits, double time_taken) {
    double bandwidth = bits / time_taken;
    printf("Maximum bandwidth: %.2f bits/s\n", bandwidth);
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("WSAStartup failed");
        return EXIT_FAILURE;
    }

    // Creating a socket
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Binding the socket to a specific address and port
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        perror("Bind failed");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Listening for incoming connections
    if (listen(server_socket, 1) == SOCKET_ERROR) {
        perror("Listen failed");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Accepting a connection
    SOCKET client_socket;
    struct sockaddr_in client_address;
    int client_address_len = sizeof(client_address);

    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
    if (client_socket == INVALID_SOCKET) {
        perror("Accept failed");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Connection from: %s\n", inet_ntoa(client_address.sin_addr));

    // Sending 20 bits of data over the connection
    const char* data = "10101001110110011010";
    time_t start_time = time(NULL);
    
    for (int i = 0; i < 20; ++i) {
        if (data[i] == '0') {
            double delay = (rand() % 300 + 100) / 1000.0; // Generating delays from 0.1 to 0.4 seconds for bit '0'
            Sleep((DWORD)(delay * 1000));
        } else {
            double delay = (rand() % 400 + 600) / 1000.0; // Generating delays from 0.6 to 1 second for bit '1'
            Sleep((DWORD)(delay * 1000));
        }
        send(client_socket, "1", 1, 0); // Sending bit '1' irrespective of the bit read from the actual data
    }

    // Measuring the duration of transmission
    time_t end_time = time(NULL);
    double time_taken = difftime(end_time, start_time);

    // Displaying the actual data that was conveyed and also the bandwidth of the channel
    printf("Sent Data: %s\n", data);
    compute_bandwidth(20, time_taken);

    // Close the connection
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
