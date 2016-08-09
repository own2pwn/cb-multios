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
#include "stdint.h"
#include "stdio.h"
#include "malloc.h"
#include "io.h"
#include "ll.h"
#include "graph.h"
#include "string.h"

extern cgc_uint32_t NumNodes;
extern cgc_uint32_t NumEdges;
const unsigned char *rand_page = (const unsigned char *)0x4347C000;

cgc_uint32_t cgc_ReadBytes(unsigned char *Buf, cgc_uint32_t TargetLen) {
	cgc_size_t rxbytes;
	cgc_uint32_t TotalLen = 0;

	while ((TotalLen < TargetLen) && (receive(STDIN, Buf+TotalLen, TargetLen-TotalLen, &rxbytes) == 0)) {
		if (rxbytes == 0) {
			return(0);
		}
		TotalLen += rxbytes;
	}
	if (TotalLen != TargetLen) {
		return(0);
	}

	return(TotalLen);
}

cgc_uint32_t cgc_ReadNull(cgc_uint32_t TargetLen) {
	cgc_size_t rxbytes;
	cgc_uint32_t TotalLen = 0;
	unsigned char *Buf;

	if (TargetLen == 0) {
		return(0);
	}

	if ((Buf = (unsigned char *)cgc_calloc(TargetLen)) == NULL) {
		_terminate(0);
	}

	while ((TotalLen < TargetLen) && (receive(STDIN, Buf+TotalLen, TargetLen-TotalLen, &rxbytes) == 0)) {
		if (rxbytes == 0) {
			cgc_free(Buf);
			return(0);
		}
		TotalLen += rxbytes;
	}
	if (TotalLen != TargetLen) {
		cgc_free(Buf);
		return(0);
	}

	cgc_free(Buf);
	return(TotalLen);
}

cgc_uint32_t cgc_SendBytes(unsigned char *Buf, cgc_uint32_t Len) {
	cgc_size_t txbytes;
	cgc_uint32_t TotalLen = 0;

	while (((transmit(STDOUT, Buf+TotalLen, Len-TotalLen, &txbytes)) == 0) && TotalLen < Len) {
		if (txbytes == 0) {
			return(0);
		}
		TotalLen += txbytes;
	}
	if (TotalLen != Len) {
		return(0);
	}

	return(TotalLen);
}

cgc_uint8_t cgc_SendResponse(cgc_uint8_t Code, cgc_uint8_t NumElements, cgc_uint32_t *Elements) {
	cgc_Response r;

	r.Code = Code;
	r.NumElements = NumElements;
	if (cgc_SendBytes((unsigned char *)&r, sizeof(cgc_Response)) != sizeof(cgc_Response)) {
		return(0);
	}
	
	// send the elements
	if (NumElements > 0) {
		if (cgc_SendBytes((unsigned char *)Elements, NumElements*sizeof(cgc_uint32_t))) {
			return(0);
		}
	}

	return(1);
}

cgc_uint8_t cgc_SendErrorResponse(cgc_uint8_t Code) {
	cgc_Response r;

	return(cgc_SendResponse(Code, 0, NULL));
}


// Read a command from the CRS
cgc_uint8_t cgc_ReadCmd(void) {
	cgc_Command Cmd;
	cgc_uint32_t *NewNodeNames;
	cgc_pEdgeArray NewEdges;
	cgc_uint32_t i;
	cgc_pNode NewNode;
	cgc_pEdge NewEdge;
	cgc_pEdge ExistingEdge;
	cgc_uint8_t NodeCount;
	cgc_uint32_t *SPT;

	// read in the basic command header
	if (cgc_ReadBytes((unsigned char *)&Cmd, sizeof(cgc_Command)) != sizeof(cgc_Command)) {
		return(0);
	}

	if (Cmd.Action == CMD_SEND_NODES) {
		// read in the indicated number of Nodes
#ifdef PATCHED_1
		if ((Cmd.NumElements + NumNodes) > MAX_NODES) {
#else
		if (Cmd.NumElements > MAX_NODES) {
#endif
			cgc_ReadNull(sizeof(cgc_uint32_t)*Cmd.NumElements);
			cgc_SendErrorResponse(RESP_ERROR_TOO_MANY_NODES);
			return(0);
		}
		if ((NewNodeNames = (cgc_uint32_t *)cgc_calloc(sizeof(cgc_uint32_t)*Cmd.NumElements)) == NULL) {
			cgc_DestroyNodes();
			cgc_DestroyEdges();
			_terminate(1);
		}
		if (cgc_ReadBytes((unsigned char *)NewNodeNames, sizeof(cgc_uint32_t)*Cmd.NumElements) != sizeof(cgc_uint32_t)*Cmd.NumElements) {
			cgc_free(NewNodeNames);
			return(0);
		}

		// make sure none of the new node names exist already
		for (i = 0; i < Cmd.NumElements; i++) {
			if (cgc_FindNode(NewNodeNames[i]) != NULL) {
				cgc_free(NewNodeNames);
				cgc_SendErrorResponse(RESP_ERROR_DUPLICATE_NODE);
				return(0);
			}
		}

		// Create the new nodes 
		for (i = 0; i < Cmd.NumElements; i++) {
			// create a new node
			if ((NewNode = (cgc_pNode)cgc_calloc(sizeof(cgc_Node))) == NULL) {
				cgc_free(NewNodeNames);
				cgc_DestroyNodes();
				cgc_DestroyEdges();
				_terminate(1);
			}
			NewNode->Name = NewNodeNames[i];
			NewNode->Distance = SIZE_MAX;
			// Add it to the graph
			if (!cgc_AddNode(NewNode)) {
				cgc_free(NewNodeNames);
				cgc_DestroyNodes();
				cgc_DestroyEdges();
				_terminate(1);
			}
		}

		// done creating new nodes
		cgc_free(NewNodeNames);

	} else if (Cmd.Action == CMD_SEND_EDGES) {
		// read in the indicated number of Edges
		if ((Cmd.NumElements + NumEdges) > MAX_EDGES) {
			cgc_ReadNull(sizeof(cgc_EdgeArray)*Cmd.NumElements);
			cgc_SendErrorResponse(RESP_ERROR_TOO_MANY_EDGES);
			return(0);
		}
		if ((NewEdges = (cgc_pEdgeArray)cgc_calloc(sizeof(cgc_EdgeArray)*Cmd.NumElements)) == NULL) {
			cgc_DestroyNodes();
			cgc_DestroyEdges();
			_terminate(1);
		}
		if (cgc_ReadBytes((unsigned char *)NewEdges, sizeof(cgc_EdgeArray)*Cmd.NumElements) != sizeof(cgc_EdgeArray)*Cmd.NumElements) {
			cgc_free(NewEdges);
			return(0);
		}
		
		// Create the new edges 
		for (i = 0; i < Cmd.NumElements; i++) {
			// create a new cgc_Edge
			if ((NewEdge = (cgc_pEdge)cgc_calloc(sizeof(cgc_Edge))) == NULL) {
				cgc_free(NewEdges);
				cgc_DestroyNodes();
				cgc_DestroyEdges();
				_terminate(1);
			}
			// make sure the starting and ending nodes exist
			if ((NewEdge->NodeA = cgc_FindNode(NewEdges[i].NodeA)) == NULL) {
				cgc_SendErrorResponse(RESP_ERROR_INVALID_NODE);
				cgc_free(NewEdge);
				cgc_free(NewEdges);
				cgc_DestroyNodes();
				cgc_DestroyEdges();
				_terminate(1);
			}
			if ((NewEdge->NodeZ = cgc_FindNode(NewEdges[i].NodeZ)) == NULL) {
				cgc_SendErrorResponse(RESP_ERROR_INVALID_NODE);
				cgc_free(NewEdge);
				cgc_free(NewEdges);
				cgc_DestroyNodes();
				cgc_DestroyEdges();
				_terminate(1);
			}
			// offset the weight by a fixed magic_page-based value
			NewEdge->Weight = NewEdges[i].Weight + rand_page[NumNodes];

			// see if one already exists
			if ((ExistingEdge = cgc_FindEdge(NewEdge->NodeA, NewEdge->NodeZ)) != NULL) {
				if (ExistingEdge->Weight > NewEdge->Weight) {
					ExistingEdge->Weight = NewEdge->Weight;
				}

				// keep the existing edge
				cgc_free(NewEdge);
				continue;
			}

			// Add it to the graph
			if (!cgc_AddEdge(NewEdge)) {
				cgc_free(NewEdge);
				cgc_free(NewEdges);
				cgc_DestroyNodes();
				cgc_DestroyEdges();
				_terminate(1);
			}
		}

	} else if (Cmd.Action == CMD_RUN_SPT) {
		if ((SPT = cgc_FindSpt(Cmd.StartingNode, Cmd.EndingNode, &NodeCount)) == NULL) {
			// unable to find SPT
			cgc_SendErrorResponse(RESP_ERROR_SPT_FAIL);
			return(0);
		}
		cgc_SendResponse(RESP_NODE_SET, NodeCount, SPT);
		cgc_free(SPT);

	} else {
		cgc_SendErrorResponse(RESP_ERROR_INVALID_CMD);
		return(0);
	}

	return(1);
}
