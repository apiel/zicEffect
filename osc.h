#ifndef _OSC_H_
#define _OSC_H_

#include <lo/lo.h>

#include "def.h"
#include "midi.h"

lo_server_thread serverThread;

void error(int num, const char* msg, const char* path)
{
    APP_PRINT("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}

int midiHandler(const char* path, const char* types, lo_arg** argv, int argc, lo_message data, void* user_data)
{
    std::vector<unsigned char> msg;
    for (int i = 0; i < argc; i++) {
        msg.push_back(argv[i]->c);
    }
    midiControllerCallback(0, &msg);

    return 0;
}

int exitHandler(const char* path, const char* types, lo_arg** argv, int argc, lo_message data, void* user_data)
{
    lo_server_thread_free(serverThread);
    exit(0);

    return 0;
}

void oscServer(char* port)
{
    serverThread = lo_server_thread_new(port, error);

    printf("OSC server listening on port %s\n", port);

    lo_server_thread_add_method(serverThread, "/midi", NULL, midiHandler, NULL);
    lo_server_thread_add_method(serverThread, "/exit", "", exitHandler, NULL);

    lo_server_thread_start(serverThread);
}

#endif
