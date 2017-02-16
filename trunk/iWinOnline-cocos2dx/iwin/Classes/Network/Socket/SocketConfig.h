#ifndef SCOKETCONFIG_H
#define SCOKETCONFIG_H
#define SOCKET_MESSAGE_DATA_SIZE 64*1024 //Maximum length of data(in a message) from Application

#define SOCKET_BUFFER_SIZE 2*1024 //Length of data to send/receive in socket. The maximum of this value depend on each OS.
#define SOCKET_BUFFER_RECIVE_SIZE 512 //The length of main buffer to receive data
#define SOCKET_BUFFER_SEND_SIZE SOCKET_BUFFER_RECIVE_SIZE //The length of main buffer to send data
#define LENGHT_CONTENT 4

#if defined WINDOWS
#define USE_SOCKET_THREAD 1 // cannot enter game if set to 0, TODO: fix
#else
#define USE_SOCKET_THREAD 1
#endif

#endif
