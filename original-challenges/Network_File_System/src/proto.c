/*

Author: Joe Rogers <joe@cromulence.com>

Copyright (c) 2015 Cromulence LLC

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
#include <libcgc.h>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"
#include "fs.h"
#include "proto.h"

cgc_uint32_t cgc_ReadBytes(char *buf, cgc_uint32_t len) {
	cgc_uint32_t TotalBytes = 0;
	cgc_size_t rx_bytes;

	while (TotalBytes < len) {
		if (receive(STDIN, buf+TotalBytes, len-TotalBytes, &rx_bytes) != 0) {
			_terminate(0);
		}
		if (rx_bytes == 0) {
			_terminate(0);
		}
		TotalBytes += rx_bytes;
	}

	return(TotalBytes);
}

cgc_uint32_t cgc_SendBytes(char *buf, cgc_uint32_t len) {
	cgc_uint32_t TotalBytes = 0;
	cgc_size_t tx_bytes;

	while (TotalBytes < len) {
		if (transmit(STDOUT, buf+TotalBytes, len-TotalBytes, &tx_bytes) != 0) {
			_terminate(0);
		}
		if (tx_bytes != len-TotalBytes) {
			_terminate(0);
		}
		TotalBytes += tx_bytes;
	}

	return(TotalBytes);
}

#ifdef PATCHED_1
void cgc_CallocAndRead(char **buf, cgc_uint32_t len) {
#else 
void cgc_CallocAndRead(char **buf, cgc_uint8_t len) {
#endif
	// extra byte to be sure strings are null terminated
	len++;
	if ((*buf = cgc_calloc(len)) == NULL) {
		_terminate(0);
	}
#ifdef DEBUG
	char debug[50];
	cgc_sprintf(debug, "cgc_calloc($d)=$08x\n", len, *buf);
	transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
	len--;
	cgc_ReadBytes(*buf, len);
}

cgc_pRequest cgc_ReceiveRequest(void) {
	char buf[10];
	cgc_pRequestHeader pReqHdr = (cgc_pRequestHeader)buf;
	cgc_pRequest pReq;

	// Allocate a struct to hold the request
	if ((pReq = cgc_calloc(sizeof(cgc_Request))) == NULL) {
		_terminate(0);
	}
#ifdef DEBUG
	char debug[50];
	cgc_sprintf(debug, "cgc_calloc($d)=$08x\n", sizeof(cgc_Request), pReq);
	transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif

	// Receive until we have cgc_RequestHeader length bytes
	cgc_ReadBytes(buf, REQUEST_HEADER_LEN);

	// See what type of packet we have and
	// receive the bytes for the specified type header
	pReq->Type = pReqHdr->Type;
	switch (pReqHdr->Type) {
		case (CFS_LOGIN): 
			cgc_ReadBytes(buf+REQUEST_HEADER_LEN, sizeof(cgc_LoginHeader));
			cgc_pLoginHeader pLoginHdr = (cgc_pLoginHeader)(buf+REQUEST_HEADER_LEN);

			// allocate space to hold the username and read it in
			cgc_CallocAndRead(&(pReq->Username), pLoginHdr->UsernameLen);
			
			// allocate space to hold the password and read it in
			cgc_CallocAndRead(&(pReq->Password), pLoginHdr->PasswordLen);
			
			break;

		case (CFS_DIR):
			// nothing more to parse
			break;

		case (CFS_READ):
			cgc_ReadBytes(buf+REQUEST_HEADER_LEN, sizeof(cgc_ReadHeader));
			cgc_pReadHeader pReadHdr = (cgc_pReadHeader)(buf+REQUEST_HEADER_LEN);

			// alocate space to hold the filename and read it in
			cgc_CallocAndRead(&(pReq->Filename), pReadHdr->FilenameLen);

			// Get the read Offset and Length values
			pReq->Offset = pReadHdr->Offset;
			pReq->ReadWriteLength = pReadHdr->Length;

			break;

		case (CFS_WRITE):
		case (CFS_WRITE_APPEND):
			cgc_ReadBytes(buf+REQUEST_HEADER_LEN, sizeof(cgc_WriteHeader));
			cgc_pWriteHeader pWriteHdr = (cgc_pWriteHeader)(buf+REQUEST_HEADER_LEN);

			// alocate space to hold the filename and read it in
			cgc_CallocAndRead(&(pReq->Filename), pWriteHdr->FilenameLen);

			// Get the Length value
			pReq->ReadWriteLength = pWriteHdr->Length;

			// read in the bytes to be written
			pReq->DataLen = pWriteHdr->Length;
			cgc_CallocAndRead(&(pReq->Data), pWriteHdr->Length);

			break;

		case (CFS_DEL):
			cgc_ReadBytes(buf+REQUEST_HEADER_LEN, sizeof(cgc_DelHeader));
			cgc_pDelHeader pDelHdr = (cgc_pDelHeader)(buf+REQUEST_HEADER_LEN);

			// alocate space to hold the filename and read it in
			cgc_CallocAndRead(&(pReq->Filename), pDelHdr->FilenameLen);

			break;

		case (CFS_RENAME):
			cgc_ReadBytes(buf+REQUEST_HEADER_LEN, sizeof(cgc_RenameHeader));
			cgc_pRenameHeader pRenameHdr = (cgc_pRenameHeader)(buf+REQUEST_HEADER_LEN);

			// alocate space to hold the original filename and read it in
			cgc_CallocAndRead(&(pReq->Filename), pRenameHdr->OldFilenameLen);
			
			// alocate space to hold the new filename and read it in
			cgc_CallocAndRead(&(pReq->Filename2), pRenameHdr->NewFilenameLen);
			
			break;
		default:
			// invalid type
#ifdef DEBUG
			cgc_sprintf(debug, "cgc_free($08x)\n", pReq);
			transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
			cgc_free(pReq);
			pReq = NULL;
	}

	// Return the populated cgc_Request
	return(pReq);

}

cgc_uint8_t cgc_HandleLogin(cgc_pRequest pReq, cgc_pResponse pResp) {
	if (!pReq || !pResp) {
		return(0);
	}

	if (!cgc_CheckPasswd(pReq->Username, pReq->Password)) {
		// login failed
		pResp->Code = RESP_LOGIN_FAILED;
		return(0);
	}
	if (!cgc_Login(pReq->Username)) {
		// system error logging in
		pResp->Code = RESP_SYSTEM_FAILURE;
		return(0);
	}

	// login success
	pResp->Code = RESP_SUCCESS;

	return(1);
}

cgc_uint8_t cgc_HandleDir(cgc_pRequest pReq, cgc_pResponse pResp) {
	char *Buf;

	if (!pReq || !pResp) {
		return(0);
	}

	// get the listing
	if (!cgc_ListFiles(&Buf)) {
		pResp->Code = RESP_SYSTEM_FAILURE;
		return(0);
	}

	// Send back the listing
	pResp->Code = RESP_SUCCESS;
	pResp->DataLen = cgc_strlen(Buf);
	pResp->Data = Buf;

	return(1);

}

cgc_uint8_t cgc_HandleRead(cgc_pRequest pReq, cgc_pResponse pResp) {
	cgc_FILE *in;
	cgc_uint32_t i;
	cgc_uint32_t rx_bytes;
	char buf[128];

	if (!pReq || !pResp) {
		return(0);
	}

	// open the requested file
	if (pReq->Filename == NULL) {
		pResp->Code = RESP_INVALID_FILE;
		return(0);
	}
	if ((in = cgc_fopen(pReq->Filename, "r")) == NULL) {
		pResp->Code = RESP_INVALID_FILE;
		return(0);
	}
	
	// seek to the requested offset
	for (i = 0; i < pReq->Offset; ) {
		if ((pReq->Offset-i) > 128) {
			if ((rx_bytes = cgc_fread(buf, 128, 1, in)) == 0) {
				pResp->Code = RESP_SYSTEM_FAILURE;
				cgc_fclose(in);
				return(0);
			}
		} else {
			if ((rx_bytes = cgc_fread(buf, pReq->Offset-i, 1, in)) == 0) {
				pResp->Code = RESP_SYSTEM_FAILURE;
				cgc_fclose(in);
				return(0);
			}
		}
		i += rx_bytes;
	}
	
	// allocate space to hold the response
	if ((pResp->Data = cgc_calloc(pReq->ReadWriteLength)) == NULL) {
		pResp->Code = RESP_SYSTEM_FAILURE;
		cgc_fclose(in);
		return(0);
	}
#ifdef DEBUG
	char debug[50];
	cgc_sprintf(debug, "cgc_calloc($d)=$08x\n", pReq->ReadWriteLength, pResp);
	transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
	
	// read the requested number of bytes
	if ((rx_bytes = cgc_fread(pResp->Data, pReq->ReadWriteLength, 1, in)) == 0) {
		pResp->Code = RESP_SYSTEM_FAILURE;
		cgc_fclose(in);
		return(0);
	}
	pResp->DataLen = rx_bytes;

	// request complete
	cgc_fclose(in);
	pResp->Code = RESP_SUCCESS;
	return(1);

}

cgc_uint8_t cgc_HandleWrite(cgc_pRequest pReq, cgc_pResponse pResp) {
	cgc_FILE *out;
	cgc_uint32_t i;

	if (!pReq || !pResp) {
		return(0);
	}

	// open the requested file
	if (pReq->Filename == NULL) {
		pResp->Code = RESP_INVALID_FILE;
		return(0);
	}
	if ((out = cgc_fopen(pReq->Filename, "w")) == NULL) {
		pResp->Code = RESP_INVALID_FILE;
		return(0);
	}
	
	// write the requested number of bytes
	if (cgc_fwrite(pReq->Data, pReq->DataLen, 1, out) == 0) {
		pResp->Code = RESP_SYSTEM_FAILURE;
		cgc_fclose(out);
		return(0);
	}

	// request complete
	cgc_fclose(out);
	pResp->Code = RESP_SUCCESS;
	return(1);

}

cgc_uint8_t cgc_HandleWriteAppend(cgc_pRequest pReq, cgc_pResponse pResp) {
	char *CurrContents;
	cgc_uint32_t CurrFileLen;
	cgc_FILE *in, *out;

	if (!pReq || !pResp) {
		return(0);
	}

	// read in the file's current contents
	if (pReq->Filename == NULL) {
		pResp->Code = RESP_INVALID_FILE;
		return(0);
	}
	if ((in = cgc_fopen(pReq->Filename, "r")) == NULL) {
		pResp->Code = RESP_INVALID_FILE;
		return(0);
	}
	CurrFileLen = in->cgc_Inode->FileSize;
	if ((CurrContents = cgc_calloc(CurrFileLen)) == NULL) {
		pResp->Code = RESP_SYSTEM_FAILURE;
		cgc_fclose(in);
		return(0);
	}
#ifdef DEBUG
	char debug[50];
	cgc_sprintf(debug, "cgc_calloc($d)=$08x\n", CurrFileLen, CurrContents);
	transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
	if (cgc_fread(CurrContents, CurrFileLen, 1, in) != CurrFileLen) {
		pResp->Code = RESP_SYSTEM_FAILURE;
		cgc_free(CurrContents);
		cgc_fclose(in);
		return(0);
	}
	cgc_fclose(in);

	// write the current contents back to the file
	if ((out = cgc_fopen(pReq->Filename, "w")) == NULL) {
		pResp->Code = RESP_SYSTEM_FAILURE;
		cgc_free(CurrContents);
		return(0);
	}
	if (cgc_fwrite(CurrContents, CurrFileLen, 1, out) != CurrFileLen) {
		pResp->Code = RESP_SYSTEM_FAILURE;
		cgc_free(CurrContents);
		cgc_fclose(out);
		return(0);
	}
	cgc_free(CurrContents);
	
	// write the new data to the end of the file 
	if (cgc_fwrite(pReq->Data, pReq->DataLen, 1, out) == 0) {
		pResp->Code = RESP_SYSTEM_FAILURE;
		cgc_fclose(out);
		return(0);
	}

	// request complete
	cgc_fclose(out);
	pResp->Code = RESP_SUCCESS;
	return(1);
}

cgc_uint8_t cgc_HandleDel(cgc_pRequest pReq, cgc_pResponse pResp) {
	if (!pReq || !pResp) {
		return(0);
	}

	// attempt to delete the requested file
	if (!cgc_DeleteFile(pReq->Filename)) {
		pResp->Code = RESP_DELETE_FAILED;
		return(0);
	}
	
	// request complete
	pResp->Code = RESP_SUCCESS;
	return(1);
}

cgc_uint8_t cgc_HandleRename(cgc_pRequest pReq, cgc_pResponse pResp) {
	if (!pReq || !pResp) {
		return(0);
	}

	// attempt to rename the requested file
	if (!cgc_RenameFile(pReq->Filename, pReq->Filename2)) {
		pResp->Code = RESP_RENAME_FAILED;
		return(0);
	}
	
	// request complete
	pResp->Code = RESP_SUCCESS;
	return(1);
}

cgc_pResponse cgc_HandleRequest(cgc_pRequest pReq) {
	cgc_pResponse pResp;

	if (!pReq) {
		return(NULL);
	}

	// Allocate a response
	if ((pResp = cgc_calloc(sizeof(cgc_Response))) == NULL) {
		_terminate(0);
	}
#ifdef DEBUG
	char debug[50];
	cgc_sprintf(debug, "cgc_calloc($d)=$08x\n", sizeof(cgc_Response), pResp);
	transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
	pResp->Type = pReq->Type;

	// See what type of request we have
	switch (pReq->Type) {
		case (CFS_LOGIN): 
			cgc_HandleLogin(pReq, pResp);
			break;

		case (CFS_DIR):
			cgc_HandleDir(pReq, pResp);
			break;

		case (CFS_READ):
			cgc_HandleRead(pReq, pResp);
			break;

		case (CFS_WRITE):
			cgc_HandleWrite(pReq, pResp);
			break;

		case (CFS_WRITE_APPEND):
			cgc_HandleWriteAppend(pReq, pResp);
			break;

		case (CFS_DEL):
			cgc_HandleDel(pReq, pResp);
			break;

		case (CFS_RENAME):
			cgc_HandleRename(pReq, pResp);
			break;

		default:
#ifdef DEBUG
			cgc_sprintf(debug, "cgc_free($08x)\n", pResp);
			transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
			cgc_free(pResp);
			pResp = NULL;
			break;
	}

	// Return the reponse
	return(pResp);

}

cgc_uint8_t cgc_SendResponse(cgc_pResponse pResp) {

	if (!pResp) {
		return(0);
	}

	// Send the cgc_Response header 
	if (cgc_SendBytes((char *)pResp, RESPONSE_HEADER_LEN) != RESPONSE_HEADER_LEN) {
		return(0);
	}

	// Return if there's no extra data to send
	if (pResp->DataLen == 0) {
		return(1);
	}

	// Send the extra data
	if (cgc_SendBytes(pResp->Data, pResp->DataLen) != pResp->DataLen) {
		return(0);
	}

	return(1);
}

cgc_uint8_t cgc_FreeRequest(cgc_pRequest pReq) {

	if (!pReq) {
		return(0);
	}

#ifdef DEBUG
	char debug[50];
#endif

	// check and cgc_free the Username
	if (pReq->Username) {
#ifdef DEBUG
		cgc_sprintf(debug, "cgc_free($08x)\n", pReq->Username);
		transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
		cgc_free(pReq->Username);
		pReq->Username = NULL;
	}

	// check and cgc_free the Password
	if (pReq->Password) {
#ifdef DEBUG
		cgc_sprintf(debug, "cgc_free($08x)\n", pReq->Password);
		transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
		cgc_free(pReq->Password);
		pReq->Password = NULL;
	}

	// check and cgc_free the Filename
	if (pReq->Filename) {
#ifdef DEBUG
		cgc_sprintf(debug, "cgc_free($08x)\n", pReq->Filename);
		transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
		cgc_free(pReq->Filename);
		pReq->Filename = NULL;
	}

	// check and cgc_free the Filename2
	if (pReq->Filename2) {
#ifdef DEBUG
		cgc_sprintf(debug, "cgc_free($08x)\n", pReq->Filename2);
		transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
		cgc_free(pReq->Filename2);
		pReq->Filename2 = NULL;
	}

	// check and cgc_free the Data
	if (pReq->Data) {
#ifdef DEBUG
		cgc_sprintf(debug, "cgc_free($08x)\n", pReq->Data);
		transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
		cgc_free(pReq->Data);
		pReq->Data = NULL;
	}

#ifdef DEBUG
	cgc_sprintf(debug, "cgc_free($08x)\n", pReq);
	transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
	cgc_free(pReq);
	pReq = NULL;

	return(1);
}

cgc_uint8_t cgc_FreeResponse(cgc_pResponse pResp) {

	if (!pResp) {
		return(0);
	}

#ifdef DEBUG
	char debug[50];
#endif 

	if (pResp->Data) {
#ifdef DEBUG
		char debug[50];
		cgc_sprintf(debug, "cgc_free($08x)\n", pResp->Data);
		transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
		cgc_free(pResp->Data);
		pResp->Data = NULL;
	}
#ifdef DEBUG
	cgc_sprintf(debug, "cgc_free($08x)\n", pResp);
	transmit(STDERR, debug, cgc_strlen(debug), 0);
#endif
	cgc_free(pResp);
	pResp = NULL;

	return(1);
}
