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

static int jsonrpc_call(const char* func_name, const char* params_fmt, ...)
{
    va_list ap;
    va_start(ap, params_fmt);
    mjson_rpc_call("127.0.0.1", 1234, func_name, params_fmt, ap);
    va_end(ap);
    return 0;
}