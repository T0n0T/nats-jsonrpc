/**
 * @file client.c
 * @author T0n0T (823478402@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-07-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "mjson.h"
#include "nats.h"
#include "strbuf.h"

static int jsonrpc_call(uint32_t client_id, const char* method_name, const char* params_fmt, ...)
{
    va_list ap;
    va_start(ap, params_fmt);
    mjson_aprintf("{%Q: %u, %Q: %u, %Q: %}")
        // mjson_printf(r->fn, r->fn_data, "{\"jsonrpc\": \"2.0\", \"method\": \"%S\", \"params\": %s, \"id\": %u}",
        //     method_name, params_fmt, client_id);
        char* params
        = strbuf_vprintf(params_fmt, ap);
    va_end(ap);
    return 0;
}

int main(int argc, char** argv)
{
    // put the code related to Mod-initialization here

    // put the code related to Pre-initialization here

    // jsonrpc client initialize
    jsonrpc_init(NULL, NULL);

    // nats client initialize
    natsConnection* conn = NULL;
    natsMsg* reply = NULL;
    natsStatus s;

    // Creates a connection to the default NATS URL
    s = natsConnection_ConnectTo(&conn, NATS_DEFAULT_URL);
    if (s == NATS_OK) {
        // Sends a request on "help" and expects a reply.
        // Will wait only for a given number of milliseconds,
        // say for 5 seconds in this example.
        s = natsConnection_RequestString(&reply, conn, "test.jsonrpc",
            "{\"id\": 2, \"method\": \"foo\",\"params\":[0,1.23]}", 5000);
    }
    if (s == NATS_OK) {
        // If we are here, we should have received the reply
        printf("Received reply: %.*s\n",
            natsMsg_GetDataLength(reply),
            natsMsg_GetData(reply));

        // Need to destroy the message!
        natsMsg_Destroy(reply);
    }

    // Anything that is created need to be destroyed
    natsConnection_Destroy(conn);

    // If there was an error, print a stack trace and exit
    if (s != NATS_OK) {
        nats_PrintLastErrorStack(stderr);
        exit(2);
    }

    return 0;
}
