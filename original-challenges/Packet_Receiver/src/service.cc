/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2014 Cromulence LLC

Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "common.h"

int main(void)
{
    bool bDone = false;

    cgc_CDataStream oDataStream;
    cgc_CMessageHandler oMsgHandler;
    cgc_CMessageRenderer oMsgRenderer;

    oDataStream.cgc_Setup( STDIN );

    cgc_uint32_t packet_rx_count = 0;

    // Enter receive loop
    do
    {
        cgc_CMessagePacket *pPacket = cgc_CMessagePacket::cgc_ParseStream( &oDataStream );

        // Update packet counter
        packet_rx_count++;

        // Verify bytes received does not exceed maximum
        if ( oDataStream.cgc_BytesReceived() > MAX_BYTES_RECEIVED )
            bDone = true;

        if ( pPacket )
        {
            // Process a packet
            oMsgHandler.cgc_ReceivePacket( pPacket );

            if ( oMsgHandler.cgc_IsMsgAvailable() )
            {
                // Get the first message and process it
                cgc_CFullMessage *pNewMessage = oMsgHandler.cgc_PopFirstMessage();

                // Render message
                oMsgRenderer.cgc_RenderMessage( pNewMessage );

                // Free it
                delete pNewMessage;
            }
        }
    } while ( !bDone );


    return 0;
}
