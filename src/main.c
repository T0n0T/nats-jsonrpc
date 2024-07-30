/**
 * @file main.c
 * @author T0n0T (823478402@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-07-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "mjson.h"
#include <stdio.h>

// A custom RPC handler. Many handlers can be registered.
static void foo(struct jsonrpc_request* r)
{
    double x;
    mjson_get_number(r->params, r->params_len, "$[1]", &x);
    jsonrpc_return_success(r, "{%Q:%g,%Q:%Q}", "x", x, "ud", r->userdata);
}

// Sender function receives a reply frame and must forward it to the peer.
static int sender(char* frame, int frame_len, void* privdata)
{
    printf("%.*s", frame_len, frame); // Print the JSON-RPC reply to stdout
    // printf("here is a break\n");
    return frame_len;
}

int main(void)
{
    jsonrpc_init(sender, NULL);

    // Call rpc.list
    char request1[] = "{\"id\": 1, \"method\": \"rpc.list\"}";
    jsonrpc_process(request1, strlen(request1), sender, NULL, NULL);

    // Call non-existent method
    char request2[] = "{\"id\": 1, \"method\": \"foo\"}";
    jsonrpc_process(request2, strlen(request2), sender, NULL, NULL);

    // Register our own function
    char request3[] = "{\"id\": 2, \"method\": \"foo\",\"params\":[0,1.23]}";
    jsonrpc_export("foo", foo);
    jsonrpc_process(request3, strlen(request3), sender, NULL, (void*)"hi!");

    // Here is a client response example
    char request4[] = "{\"jsonrpc\": \"2.0\", \"result\": 19, \"id\": 1}";
    jsonrpc_process(request4, strlen(request4), sender, NULL, (void*)"hi!");

    return 0;
}