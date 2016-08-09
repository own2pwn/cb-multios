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
#ifndef __MESSAGE_HANDLER_H__
#define __MESSAGE_HANDLER_H__

class cgc_CFullMessage;

class cgc_CMessageFragmentList
{
public:
    cgc_CMessageFragmentList( void )
    {
        m_fragmentCount = 0;
    }

    ~cgc_CMessageFragmentList( )
    {
        m_fragmentList.cgc_DeleteAll();
    }

    bool cgc_AddFragment( cgc_CMessagePacket *pPacket, cgc_CFullMessage *&pNewFullMessage );

private:
    class cgc_CFragmentData : public cgc_CDoubleListElement
    {
    public:
        cgc_CFragmentData( cgc_uint16_t sequenceNumber ) : cgc_CDoubleListElement( ), m_sequenceNumber( sequenceNumber )
        {
            m_fragmentTotalCount = 0;

            for ( cgc_uint16_t i = 0; i < MAX_MESSAGE_FRAGMENTS; i++ )
                m_pMessageFragments[i] = NULL;
        }

        ~cgc_CFragmentData( )
        {
            for ( cgc_uint16_t i = 0; i < MAX_MESSAGE_FRAGMENTS; i++ )
            {
                if ( m_pMessageFragments[i] )
                    delete m_pMessageFragments[i];
            }
        }

        cgc_uint16_t cgc_GetSequenceNumber( void ) const { return m_sequenceNumber; };

        bool cgc_SetTotalFragmentCount( cgc_uint8_t fragmentTotalCount )
        {
            if ( fragmentTotalCount > MAX_MESSAGE_FRAGMENTS )
                return (false);

            m_fragmentTotalCount = fragmentTotalCount;

            return (true);
        }

        bool cgc_AddFragmentData( cgc_CMessagePacket *pPacket, cgc_uint8_t fragmentIndex )
        {
            if ( fragmentIndex >= MAX_MESSAGE_FRAGMENTS )
                return (false);

            if ( m_pMessageFragments[fragmentIndex] != NULL )
                return (false);     // Don't add in fragments we already have??

            m_pMessageFragments[fragmentIndex] = pPacket;

            return (true);
        }

        bool cgc_HasAllFragments( void ) const
        {
            cgc_uint32_t fragmentCount = 0;

            if ( m_fragmentTotalCount == 0 )
                return (false);

            for ( cgc_uint32_t i = 0; i < m_fragmentTotalCount; i++ )
            {
                if ( m_pMessageFragments[i] )
                    fragmentCount++;
            }

            return (fragmentCount == m_fragmentTotalCount);
        }

        cgc_uint32_t cgc_GetAssembledMessageSize( void ) const;
        cgc_CFullMessage *cgc_AssembleFragments( void ) const;

    private:
        cgc_CMessagePacket *m_pMessageFragments[MAX_MESSAGE_FRAGMENTS];
        cgc_uint8_t m_fragmentTotalCount;
        cgc_uint16_t m_sequenceNumber;
    };

    cgc_CFragmentData *cgc_GetFragmentForSequenceNumber( cgc_uint16_t sequenceNumber );

private:
    cgc_uint32_t m_fragmentCount;
    cgc_CDoubleList m_fragmentList;
};

class cgc_CFullMessage : public cgc_CDoubleListElement
{
public:
    cgc_CFullMessage( cgc_uint8_t *pMessageData, cgc_uint32_t messageLen, bool bCopyData = false );
    ~cgc_CFullMessage( );

    cgc_uint8_t *cgc_GetData( void ) const { return m_pMessageData; };
    cgc_uint32_t cgc_GetLength( void ) const { return m_messageLen; };

private:
    cgc_uint8_t *m_pMessageData;
    cgc_uint32_t m_messageLen;
};

// Receives message packets and assembles them into parsed messages
// to send to the message renderer
class cgc_CMessageHandler
{
public:
    cgc_CMessageHandler();
    ~cgc_CMessageHandler();

    void cgc_ReceivePacket( cgc_CMessagePacket *pPacket );

    bool cgc_IsMsgAvailable( void );

    cgc_CFullMessage *cgc_PopFirstMessage( void );

private:
    cgc_CMessageFragmentList m_messageFragmentList;
    cgc_CDoubleList m_fullMessageList;
};

#endif // __MESSAGE_HANDLER_H__
