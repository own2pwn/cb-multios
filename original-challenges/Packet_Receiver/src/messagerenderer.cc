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

bool cgc_CMessageRenderer::cgc_RenderHeader( void )
{
    cgc_printf( "<HTML>\n" );
    cgc_printf( "<HEAD>Messages</HEAD>\n" );
    cgc_printf( "<BODY>\n" );

    return (true);
}

bool cgc_CMessageRenderer::cgc_RenderFooter( void )
{
    cgc_printf( "</BODY>\n" );
    cgc_printf( "</HTML>\n" );

    return (true);
}

cgc_uint32_t cgc_add_render_buffer( cgc_uint8_t *pBuffer, cgc_uint32_t bufferPos, const char *pszString, cgc_uint32_t maxBufferPos )
{
#ifdef PATCHED
    cgc_uint32_t lenToAdd = cgc_strlen( pszString );

    if ( (bufferPos + lenToAdd) > maxBufferPos )
        return 0;
#endif

    cgc_memcpy( (cgc_uint8_t*)pBuffer+bufferPos, (cgc_uint8_t*)pszString, cgc_strlen( pszString ) );

    return cgc_strlen( pszString );
}

bool cgc_CMessageRenderer::cgc_RenderMessage( cgc_CFullMessage *pMessage )
{
    // BUG:: It is possible to overrun the renderBuffer by writing out a bunch of mark ups (these cost only 1-byte in the message itself but equal to 3 bytes in the render buffer)
    cgc_uint8_t renderBuffer[MAX_MESSAGE_FRAGMENTS*256];
    cgc_uint32_t maxBufferPos = MAX_MESSAGE_FRAGMENTS*256;

    // Get from address
    if ( pMessage->cgc_GetLength() < 1 )
        return (false);

    cgc_uint8_t fromAddress = *((cgc_uint8_t*)(pMessage->cgc_GetData()+0));

    if ( pMessage->cgc_GetLength() < 2 )
        return (false);

    cgc_uint8_t toAddress = *((cgc_uint8_t*)(pMessage->cgc_GetData()+1));

    // Now render message!
    cgc_uint32_t messagePos = 2;
    cgc_uint8_t *pMessageData = pMessage->cgc_GetData();

    // Render position
    cgc_uint32_t renderPos = 0;
    cgc_uint32_t renderLen = 0;

    bool bMarkupItalics = false;
    bool bMarkupUnderline = false;
    bool bMarkupBold = false;

    // Render message header
    cgc_RenderHeader();

    // Render message data
    while ( messagePos < pMessage->cgc_GetLength() )
    {
        // Read markup -- if it exists
        cgc_uint8_t nextchar = pMessageData[messagePos++];

        if ( nextchar & 0x80 )
        {
            if ( nextchar & 0x40 )
            {
                bool bNewItalics = false;
                bool bNewBold = false;
                bool bNewUnderline = false;

                // Mark something up
                switch( nextchar & 0xF )
                {
                case 0: // No markup
                    bNewItalics = false;
                    bNewBold = false;
                    bNewUnderline = false;
                    break;

                case 1: // Italics
                    bNewItalics = true;
                    break;

                case 2: // Bold
                    bNewBold = true;
                    break;

                case 3: // Underline
                    bNewUnderline = true;
                    break;

                case 4: // Italics + Bold
                    bNewItalics = true;
                    bNewBold = true;
                    break;

                case 5: // Italics + Underline
                    bNewItalics = true;
                    bNewUnderline = true;
                    break;

                case 6: // Bold + Underline
                    bNewBold = true;
                    bNewUnderline = true;
                    break;

                case 7: // Italics + Bold + Underline
                    bNewItalics = true;
                    bNewBold = true;
                    bNewUnderline = true;
                    break;

                default:
                    // No markup
                    bNewItalics = bMarkupItalics;
                    bNewBold = bMarkupBold;
                    bNewUnderline = bMarkupUnderline;
                    break;
                }

                if ( bMarkupItalics && !bNewItalics )
                {
                    // Stop italics
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "</i>", maxBufferPos );
                    if ( renderLen == 0 )
                        break;
                    renderPos += renderLen;
                }

                if ( !bMarkupItalics && bNewItalics )
                {
                    // Start italics
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "<i>", maxBufferPos );
                    if ( renderLen == 0 )
                        break;
                    renderPos += renderLen;
                }

                if ( bMarkupBold && !bNewBold )
                {
                    // Stop bold
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "</b>", maxBufferPos );
                    if ( renderLen == 0 )
                        break;
                    renderPos += renderLen;
                }

                if ( !bMarkupBold && bNewBold )
                {
                    // Start bold
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "<b>", maxBufferPos );
                    if ( renderLen == 0 )
                        break;
                    renderPos += renderLen;
                }

                if ( bMarkupUnderline && !bNewUnderline )
                {
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "</u>", maxBufferPos );
                    if ( renderLen == 0 )
                        break;
                    renderPos += renderLen;
                }

                if ( !bMarkupUnderline && bNewUnderline )
                {
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "<u>", maxBufferPos );
                    if ( renderLen == 0 )
                        break;
                    renderPos += renderLen;
                }

                // Update state machine
                bMarkupItalics = bNewItalics;
                bMarkupBold = bNewBold;
                bMarkupUnderline = bNewUnderline;
            }
            else
            {
                // Reset render length for overrun checking
                renderLen = 0;

                // Exit condition
                bool bExit = false;

                // Emit a break, tabstop, space etc.
                switch( nextchar & 0xF )
                {
                case 0: // BR
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "<BR>", maxBufferPos );
                    // Check for render overrun
                    if ( renderLen == 0 )
                        bExit = true;
                    else
                        renderPos += renderLen;

                    break;

                case 1: // TABSTOP
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "&nbsp&nbsp&nbsp&nbsp", maxBufferPos );
                    // Check for render overrun
                    if ( renderLen == 0 )
                        bExit = true;
                    else
                        renderPos += renderLen;

                    break;

                case 2: // Paragraph
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "<p>", maxBufferPos );
                    // Check for render overrun
                    if ( renderLen == 0 )
                        bExit = true;
                    else
                        renderPos += renderLen;

                    break;

                case 3: // End paragraph
                    renderLen = cgc_add_render_buffer( renderBuffer, renderPos, "\n", maxBufferPos );
                    // Check for render overrun
                    if ( renderLen == 0 )
                        bExit = true;
                    else
                        renderPos += renderLen;

                    break;

                default:
                    // No breaks
                    break;
                }

                // Check for exit condition
                if ( bExit )
                    break;
            }
        }
        else
        {
            // Check overflow
            if ( renderPos >= maxBufferPos )
                break;

            if ( nextchar >= 32 )
            {
                // Emit printable range
                renderBuffer[renderPos++] = nextchar;
            }
            else
                renderBuffer[renderPos++] = '_';
        }

        if ( renderPos >= maxBufferPos )
            break;
    }

    // Output message header information
    cgc_printf( "<b>Message @d:</b><BR>\n", m_messageRenderCount );

    // Update message render counter
    m_messageRenderCount++;

    // Print who the message is from
    cgc_printf( "<b>From:</b> @d<BR>\n", fromAddress );

    // Print who the message is to
    cgc_printf( "<b>To:</b> @d<BR>\n", toAddress );

    // Print out the message data
    renderBuffer[renderPos] = '\0';

    // Lastly print out the message data
    cgc_printf( "<b>Message Data:</b><BR>\n@s\n", renderBuffer );

    // Now render message footer
    cgc_RenderFooter();

    return (true);
}
