/**
 * @file server.c
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

// A custom RPC handler. Many handlers can be registered.
static void foo(struct jsonrpc_request* r)
{
    double x;
    mjson_get_number(r->params, r->params_len, "$[1]", &x);
    jsonrpc_return_success(r, "{%Q:%g,%Q:%Q}", "x", x, "ud", "test");
}

// Sender function receives a reply frame and must forward it to the peer.
static int onJsonrpcBufHandle(const char* frame, int frame_len, void* privdata)
{
    strbuf_t buf = (strbuf_t*)privdata;
    strview_t strbuf = {
        .data = frame,
        .size = frame_len,
    };
    strbuf_append_strview(&buf, strbuf);
    return frame_len;
}

static void onMsgServerHandle(natsConnection* nc, natsSubscription* sub, natsMsg* msg, void* closure)
{
    printf("Received msg: %s - %.*s\n",
        natsMsg_GetSubject(msg),
        natsMsg_GetDataLength(msg),
        natsMsg_GetData(msg));

    // Sends a reply
    if (natsMsg_GetReply(msg) != NULL) {
        const char* recv_data = natsMsg_GetData(msg);
        // find the jsonrpc method and call it in process
        strbuf_t* buf = strbuf_create(0, NULL);
        jsonrpc_process(recv_data, strlen(recv_data), onJsonrpcBufHandle, buf, NULL);
        strbuf_destroy(&buf);
        natsConnection_Publish(nc, natsMsg_GetReply(msg), frame, frame_len);
    }

    // Need to destroy the message!
    natsMsg_Destroy(msg);

    // Notify the main thread that we are done.
    // *(bool*)(closure) = true;
}

int main(void)
{
    // put the code related to Mod-initialization here

    // put the code related to Pre-initialization here

    // jsonrpc server initialize
    jsonrpc_init(NULL, NULL);

    // register function(s)
    jsonrpc_export("foo", foo);

    // prepare for transfer middleware(nats)
    natsConnection* conn = NULL;
    natsSubscription* sub = NULL;
    natsStatus s;
    volatile bool done = false;

    s = natsConnection_ConnectTo(&conn, NATS_DEFAULT_URL);
    if (s != NATS_OK) {
        printf("Could not connect to nats\n");
        return -1;
    }
    s = natsConnection_Subscribe(&sub, conn, "test.jsonrpc", onMsgServerHandle, (void*)&done);
    if (s == NATS_OK) {
        while (!done) {
            nats_Sleep(100);
        }
    }

    natsConnection_Destroy(conn);

    if (s != NATS_OK) {
        nats_PrintLastErrorStack(stderr);
        exit(2);
    }
    return 0;
}