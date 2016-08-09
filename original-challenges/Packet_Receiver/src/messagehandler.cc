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

cgc_CMessageFragmentList::cgc_CFragmentData *cgc_CMessageFragmentList::cgc_GetFragmentForSequenceNumber( cgc_uint16_t sequenceNumber )
{
    cgc_CFragmentData *pCur;

    for ( pCur = (cgc_CFragmentData *)m_fragmentList.cgc_GetFirst(); pCur; pCur = (cgc_CFragmentData *)pCur->cgc_GetNext() )
    {
        if ( pCur->cgc_GetSequenceNumber() == sequenceNumber )
            return (pCur);
    }

    return (NULL);
}

cgc_uint32_t cgc_CMessageFragmentList::cgc_CFragmentData::cgc_GetAssembledMessageSize( void ) const
{
    // Assemble size
    cgc_uint32_t messageFullSize = 0;
    for ( cgc_uint32_t i = 0; i < m_fragmentTotalCount; i++ )
    {
        if ( m_pMessageFragments[i] )
            messageFullSize += m_pMessageFragments[i]->cgc_GetDataLength( );
    }

    return (messageFullSize);
}

cgc_CFullMessage *cgc_CMessageFragmentList::cgc_CFragmentData::cgc_AssembleFragments( void ) const
{
    // Allocate buffer for full message
    cgc_uint8_t *pFullMessageData = new cgc_uint8_t[cgc_GetAssembledMessageSize()];

    cgc_uint32_t fullMessagePos = 0;

    // BUG:: Message full size was calculated using only up to m_fragmentCount where as this will count fragments that were ABOVE the last fragment number
#ifdef PATCHED
    for ( cgc_uint32_t i = 0; i < m_fragmentTotalCount; i++ )
#else
    for ( cgc_uint32_t i = 0; i < MAX_MESSAGE_FRAGMENTS; i++ )
#endif
    {
        if ( m_pMessageFragments[i] )
        {
            // Copy data
            cgc_memcpy( pFullMessageData+fullMessagePos, (void *)m_pMessageFragments[i]->cgc_GetData(), m_pMessageFragments[i]->cgc_GetDataLength() );

            // Update full message position
            fullMessagePos += m_pMessageFragments[i]->cgc_GetDataLength();
        }
    }

    // In this case don't pass true to third argument of constructor (so it will not copy full data)
    cgc_CFullMessage *pNewFullMessage = new cgc_CFullMessage( pFullMessageData, fullMessagePos );

    return (pNewFullMessage);
}

bool cgc_CMessageFragmentList::cgc_AddFragment( cgc_CMessagePacket *pPacket, cgc_CFullMessage *&pNewFullMessage )
{
    // This will be set if all the fragments are assembled
    pNewFullMessage = NULL;

    // Check for NULL -- false means a new message has not been assembled yet
    if ( pPacket == NULL )
        return (false);

    // Add message into circular list
    if ( m_fragmentCount >= MESSAGE_FRAGMENT_LIST_MAX )
        return (false);

    if ( !pPacket->cgc_IsFragmented() )
        return (false);

    // Get sequence number
    cgc_uint16_t currentSeqNumber = pPacket->cgc_GetSequenceNumber();

    cgc_CFragmentData *pOtherFragments = cgc_GetFragmentForSequenceNumber( currentSeqNumber );
    if ( pOtherFragments == NULL )
    {
        // No other fragments -- lets add a new one at the tail of the list
        cgc_CFragmentData *pNewFragmentData = new cgc_CFragmentData( currentSeqNumber );

        if ( pPacket->cgc_IsLastFragment() )
        {
            // Last fragment -- set the total count
            pNewFragmentData->cgc_SetTotalFragmentCount( pPacket->cgc_GetFragmentNumber()+1 );
        }

        pNewFragmentData->cgc_AddFragmentData( pPacket, pPacket->cgc_GetFragmentNumber() );

        // Check if all fragments are available (this is a special case where only 1 packet is received that is the last fragment...)
        if ( pNewFragmentData->cgc_HasAllFragments() )
        {
            // Assemble message
            pNewFullMessage = pNewFragmentData->cgc_AssembleFragments( );

            // Free memory
            delete pNewFragmentData;
        }
        else
        {
            // More fragments needed -- add it to the fragment list
            m_fragmentList.cgc_InsertTail( pNewFragmentData );

            // Update the list tracking the number of sequence numbers we are tracking in fragments
            m_fragmentCount++;
        }
    }
    else
    {
        // Other fragments present -- add onto the list (add possibly assemble)

        if ( pPacket->cgc_IsLastFragment() )
        {
            // Last fragment -- set total count
            pOtherFragments->cgc_SetTotalFragmentCount( pPacket->cgc_GetFragmentNumber()+1 );
        }

        pOtherFragments->cgc_AddFragmentData( pPacket, pPacket->cgc_GetFragmentNumber() );

        if ( pOtherFragments->cgc_HasAllFragments() )
        {
            // Build a new assembled message, unlink from fragment list, and delete the fragment data
            pNewFullMessage = pOtherFragments->cgc_AssembleFragments( );

            // Remove from fragment list
            m_fragmentList.cgc_Unlink( pOtherFragments );

            // Free memory
            delete pOtherFragments;

            // Keep track of fragment count in list (to limit this list size)
            m_fragmentCount--;
        }
    }

    return (true);
}

cgc_CFullMessage::cgc_CFullMessage( cgc_uint8_t *pMessageData, cgc_uint32_t messageLength, bool bDataCopy )
{
    if ( bDataCopy )
    {
        m_pMessageData = new cgc_uint8_t[messageLength];

        cgc_memcpy( m_pMessageData, pMessageData, messageLength );
    }
    else
        m_pMessageData = pMessageData;

    m_messageLen = messageLength;
}

cgc_CFullMessage::~cgc_CFullMessage( )
{
    if ( m_pMessageData )
        delete m_pMessageData;
}

cgc_CMessageHandler::cgc_CMessageHandler( )
{

}

cgc_CMessageHandler::~cgc_CMessageHandler( )
{

}

void cgc_CMessageHandler::cgc_ReceivePacket( cgc_CMessagePacket *pPacket )
{
    if ( pPacket == NULL )
        return;

    if ( pPacket->cgc_IsFragmented() )
    {
        cgc_CFullMessage *pFullMessage = NULL;

        if ( !m_messageFragmentList.cgc_AddFragment( pPacket, pFullMessage ) )
        {
            // Fragment reassembly failed -- drop message
            delete pPacket;

            return;
        }

        if ( pFullMessage )
        {
            // Add full message into list for processing
            m_fullMessageList.cgc_InsertTail( pFullMessage );
        }
    }
    else
    {
        // Generate a full message
        cgc_CFullMessage *pNewFullMessage = new cgc_CFullMessage( (cgc_uint8_t *)pPacket->cgc_GetData(), pPacket->cgc_GetDataLength(), true );

        // Delete packet
        delete pPacket;

        m_fullMessageList.cgc_InsertTail( pNewFullMessage );
    }
}

bool cgc_CMessageHandler::cgc_IsMsgAvailable( void )
{
    return (m_fullMessageList.cgc_GetFirst() != NULL);
}

cgc_CFullMessage *cgc_CMessageHandler::cgc_PopFirstMessage( void )
{
    if ( !cgc_IsMsgAvailable() )
        return (NULL);

    cgc_CFullMessage *pFirstMsg = (cgc_CFullMessage *)m_fullMessageList.cgc_GetFirst();

    m_fullMessageList.cgc_Unlink( pFirstMsg );

    return (pFirstMsg);
}
