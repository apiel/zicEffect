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
    // debug("midiHandler (%s) (%s): %d %d %d\n", path, types, argv[0]->c, argv[1]->c, argv[2]->c);

    std::vector<unsigned char> msg;
    msg.push_back(argv[0]->c);
    msg.push_back(argv[1]->c);
    msg.push_back(argv[2]->c);

    midiControllerCallback(0, &msg);

    return 0;
}

void oscServerStart(char* port)
{
    serverThread = lo_server_thread_new(port, error);

    printf("OSC server listening on port %s\n", port);

    lo_server_thread_add_method(serverThread, "/midi", "mmm", midiHandler, NULL);

    lo_server_thread_start(serverThread);
}

void oscServerEnd()
{
    if (serverThread) {
        lo_server_thread_free(serverThread);
    }
}

#endif
