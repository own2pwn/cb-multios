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
#ifndef __LANBUS_H__
#define __LANBUS_H__

#include <cutil_list.h>

class cgc_CLANMessage
{
public:
	cgc_CLANMessage( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t *pMessageData, cgc_uint16_t messageLen );
	~cgc_CLANMessage( );

	cgc_uint8_t cgc_GetSourceID( void ) const { return m_srcID; };
	cgc_uint8_t cgc_GetDestID( void ) const { return m_destID; };
	cgc_uint16_t cgc_GetMessageLen( void ) const { return m_messageLen; };
	cgc_uint8_t *cgc_GetMessageData( void ) { return m_pMessageData; };

private:
	cgc_uint8_t *m_pMessageData;
	cgc_uint16_t m_messageLen;
	cgc_uint8_t m_destID;
	cgc_uint8_t m_srcID;

public:
	CUtil::DLL_LINK( cgc_CLANMessage ) m_lanbusLink;
};

class cgc_CLANBus
{
public:
	cgc_CLANBus( );
	~cgc_CLANBus( );

	bool cgc_SendMessage( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t *pMessageData, cgc_uint16_t messageLen );
	bool cgc_InjectSimulationMessage( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t *pMessageData, cgc_uint16_t messageLen );

	cgc_CLANMessage *cgc_RecvMessage( cgc_uint8_t *pDestList, cgc_uint8_t destListCount, cgc_CLANMessage *pLastMessage = NULL );

	void cgc_NextTick( void );

private:
	// Messages waiting to be sent (next tick)
	CUtil::DLL_LIST( cgc_CLANMessage, m_lanbusLink ) m_oSentList;

	// Messages read (current tick)
	CUtil::DLL_LIST( cgc_CLANMessage, m_lanbusLink ) m_oReceiveList;
};

#endif // __ECM_H__
