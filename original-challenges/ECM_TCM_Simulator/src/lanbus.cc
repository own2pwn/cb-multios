/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
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
extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}

#include "lanbus.h"

cgc_CLANMessage::cgc_CLANMessage( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t *pMessageData, cgc_uint16_t messageLen )
	: m_srcID( srcID ), m_destID( destID ), m_messageLen( messageLen )
{
	m_pMessageData = new cgc_uint8_t[m_messageLen];

	cgc_memcpy( m_pMessageData, pMessageData, messageLen );
}

cgc_CLANMessage::~cgc_CLANMessage( )
{
	if ( m_pMessageData )
		delete [] m_pMessageData;

	m_pMessageData = NULL;
}

cgc_CLANBus::cgc_CLANBus( )
{

}

cgc_CLANBus::~cgc_CLANBus( )
{

}

bool cgc_CLANBus::cgc_SendMessage( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t *pMessageData, cgc_uint16_t messageLen )
{
	cgc_CLANMessage *pNewMessage = new cgc_CLANMessage( srcID, destID, pMessageData, messageLen );

	m_oSentList.cgc_AddLast( pNewMessage );

	return (true);
}

bool cgc_CLANBus::cgc_InjectSimulationMessage( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t *pMessageData, cgc_uint16_t messageLen )
{
	cgc_CLANMessage *pNewMessage = new cgc_CLANMessage( srcID, destID, pMessageData, messageLen );

	m_oReceiveList.cgc_AddLast( pNewMessage );

	return (true);
}

cgc_CLANMessage *cgc_CLANBus::cgc_RecvMessage( cgc_uint8_t *pDestIDList, cgc_uint8_t idCount, cgc_CLANMessage *pLastMessage )
{
	cgc_CLANMessage *pCur;
	bool bFound = false;

	// Check for message
	if ( pLastMessage == NULL )
		pCur = m_oReceiveList.cgc_GetFirst();
	else
		pCur = m_oReceiveList.cgc_GetNext( pLastMessage );

	for ( ; pCur; pCur = m_oReceiveList.cgc_GetNext( pCur ) )
	{
		for ( cgc_uint8_t i = 0; i < idCount; i++ )
		{
			if ( pCur->cgc_GetDestID() == pDestIDList[i] )
			{
				bFound = true;
				break;
			}
		}

		if ( bFound )
			break;
	}

	if ( bFound )
		return (pCur);
	else
		return (NULL);
}

void cgc_CLANBus::cgc_NextTick( void )
{
	// Clear out old messages that are already parsed
	m_oReceiveList.cgc_DeleteAll();

	// Move sent messages to messages to be received
	cgc_CLANMessage *pMessage;

	// Move them preserving order
	while ( (pMessage = m_oSentList.cgc_GetFirst()) )
	{
		pMessage->m_lanbusLink.cgc_Unlink();

		m_oReceiveList.cgc_AddLast( pMessage );
	}
}
