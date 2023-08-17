#ifndef _OSC_H_
#define _OSC_H_

#include <lo/lo.h>
#include "def.h"

lo_server_thread serverThread;

void error(int num, const char *msg, const char *path)
{
    APP_PRINT("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}

void oscServerStart(char * port)
{
    serverThread = lo_server_thread_new(port, error);

    printf("OSC server listening on port %s\n", port);

    // lo_server server = lo_server_thread_get_server(serverThread);

    // lo_server_thread_add_method(serverThread, "/seq", "iii", seq_handler, NULL);
    // lo_server_thread_add_method(serverThread, "/step", "iiiiiii", step_handler, NULL);
    // lo_server_thread_add_method(serverThread, "/msg", NULL, msg_handler, NULL);
    // lo_server_thread_add_method(serverThread, "/msg_get", "i", msg_get_handler, server);
    // lo_server_thread_add_method(serverThread, "/msg_arg", NULL, msg_arg_handler, NULL);
    // lo_server_thread_add_method(serverThread, "/sub", NULL, sub_handler, NULL);
    // lo_server_thread_add_method(serverThread, "/quit", "", quit_handler, NULL);

    lo_server_thread_start(serverThread);
}

void oscServerEnd()
{
    lo_server_thread_free(serverThread);
}

#endif
