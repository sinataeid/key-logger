# Windows Backdoor with Keylogger and Remote Control

This repository contains code for a simple backdoor program written in C for Windows operating systems. The backdoor allows for remote control of a target system, including executing commands, changing directories, starting a keylogger, and creating a persistent registry entry for auto-startup.

## Contents

- `backdoor.c`: Client-side code for the backdoor. It establishes a connection to a specified server and listens for commands from the server.
- `keylogger.h`: Header file containing the implementation of a basic keylogger. The keylogger captures keypresses and writes them to a file.
- `server.c`: Server-side code. It listens for incoming connections from clients, accepts commands from the server operator, and sends the commands to the client.

## How to Use

### Compiling the Code

To compile the code, you need a C compiler such as GCC. You can compile the client-side (`backdoor.c`) and server-side (`server.c`) programs separately.

For `backdoor.c`:

```bash
gcc backdoor.c -o backdoor -lws2_32
```
For `server.c`:

```bash
gcc server.c -o server
```
### Running the Server
Before running the server, ensure that you have a static IP address where the server will listen for connections. Update the IP address in server.c accordingly.

#### Run the server executable:

```bash
./server
```
The server will start listening for incoming connections.

### Running the Client (Backdoor)
After compiling backdoor.c, transfer the executable to the target system. Execute the backdoor on the target system:

```bash
backdoor.exe
```
The backdoor will attempt to establish a connection to the server. Once connected, it will wait for commands from the server.

### Sending Commands
On the server machine, run the compiled server executable. You can now interact with the client (target system) by typing commands into the server terminal. Supported commands include:

- cd [directory]: Change directory on the target system.
- persist: Create a persistent registry entry for auto-startup.
- keylog_start: Start the keylogger on the target system.

Any other command will be executed on the target system, and the output will be displayed on the server.

### Exiting
To exit the backdoor program on the client, type q and press Enter. This will close the connection and exit the program.

## Disclaimer
This software is intended for educational and testing purposes only. Misuse of this software may violate laws and regulations. No responsibilities are taken for any damage caused by the misuse of this software.

## Note
Ensure that you have appropriate permissions before running the server and backdoor programs. Additionally, use this software responsibly and only on systems you own or have explicit permission to access.
