/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <cstdio.h>
#include "interface.h"

cgc_Interface::cgc_Interface(cgc_Engine &engine) : d_engine(engine)
{
}

cgc_Interface::~cgc_Interface()
{
}

bool cgc_Interface::cgc_process(unsigned char *data, unsigned int len)
{
    unsigned int msg;

    if (len < 4)
        return true;

    msg = *(unsigned int *)data;
    data += 4;
    len -= 4;

    switch (msg)
    {
    case MSG_QUIT:
        cgc_send_response(RESP_OK);
        return false;
    case MSG_UPDATE:
        if (d_engine.cgc_update_rules(data, len))
            return cgc_send_response(RESP_OK);
        else
            return cgc_send_response(RESP_ERROR);
    case MSG_CHECK:
        if (d_engine.cgc_process(data, len))
            // return ERROR if malware
            return cgc_send_response(RESP_ERROR);
        else
            return cgc_send_response(RESP_OK);
    case MSG_WHITELIST:
        d_engine.cgc_whitelist(data, len);
        return cgc_send_response(RESP_OK);
    default:
        return cgc_send_response(RESP_ERROR);
    }
}

bool cgc_Interface::cgc_send_response(unsigned int resp)
{
    return cgc_fwrite(&resp, 4, stdout) == 4;
}
