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
#include "graph.h"
#include "malloc.h"
#include "io.h"

extern cgc_pNode Nodes;
extern cgc_pEdge Edges;
extern cgc_uint32_t NumNodes;
extern cgc_uint32_t NumEdges;

// add a node to a list
cgc_pNode cgc_AddNode(cgc_pNode Element) {

	if (!Element) {
		return(NULL);
	}

	if (!Nodes) {
		Nodes = Element;
		Element->Next = NULL;
		NumNodes++;
		return(Element);
	}
	
	Element->Next = Nodes;
	Nodes = Element;
	NumNodes++;

	return(Element);

}

// add an edge to a list
cgc_pEdge cgc_AddEdge(cgc_pEdge Element) {

	if (!Element) {
		return(NULL);
	}
	
	if (!Edges) {
		Edges = Element;
		NumEdges++;
		return(Element);
	}

	Element->Next = Edges;
	Edges = Element;
	NumEdges++;

	return(Element);

}

// remove a node from a list by pointer
cgc_pNode cgc_RemoveNode(cgc_pNode Element) {
	cgc_pNode n;
	cgc_pNode prev;

	if (!Nodes || !Element) {
		return(NULL);
	}

	if (Element == Nodes) {
		Nodes = Element->Next;
		Element->Next = NULL;
		return(Nodes);
	}

	prev = Nodes;
	n = Nodes->Next;
	while (n) {
		if (n == Element) {
			prev->Next = n->Next;
			Element->Next = NULL;
			return(Nodes);
		}
		n = n->Next;
	}

	return(NULL);
	
}

// remove an edge from a list by pointer
cgc_pEdge cgc_RemoveEdge(cgc_pEdge Element) {
	cgc_pEdge e;
	cgc_pEdge prev;

	if (!Edges || !Element) {
		return(NULL);
	}

	if (Element == Edges) {
		Edges = Element->Next;
		Element->Next = NULL;
		return(Edges);
	}

	prev = Edges;
	e = Edges->Next;
	while (e) {
		if (e == Element) {
			prev->Next = e->Next;
			Element->Next = NULL;
			return(Edges);
		}
		e = e->Next;
	}

	return(NULL);
	
}

// find a node in a list by its Name
cgc_pNode cgc_FindNode(cgc_uint32_t TargetName) {
	cgc_pNode Curr = Nodes;

	if (!Nodes) {
		return(NULL);
	}

	while (Curr) {
		if (Curr->Name == TargetName) {
			return(Curr);
		}
		Curr = Curr->Next;
	}

	return(NULL);

}

// find an edge in a list by its starting and ending node
cgc_pEdge cgc_FindEdge(cgc_pNode NodeA, cgc_pNode NodeZ) {
	cgc_pEdge Curr = Edges;

	if (!Edges) {
		return(NULL);
	}

	while (Curr) {
		if ((Curr->NodeA == NodeA) && (Curr->NodeZ == NodeZ)) {
			return(Curr);
		}
		if ((Curr->NodeA == NodeZ) && (Curr->NodeZ == NodeA)) {
			return(Curr);
		}
		Curr = Curr->Next;
	}

	return(NULL);

}

// Destroy a nodes list
cgc_uint8_t cgc_DestroyNodes(void) {
	cgc_pNode Curr = Nodes;
	cgc_pNode Next;

	if (!Nodes) {
		return(0);
	}

	while (Curr) {
		Next = Curr->Next;
		cgc_free(Curr);
		Curr = Next;
	}

	Nodes = NULL;

	return(1);
}

// Destroy a edges list
cgc_uint8_t cgc_DestroyEdges(void) {
	cgc_pEdge Curr = Edges;
	cgc_pEdge Next;

	if (!Edges) {
		return(0);
	}

	while (Curr) {
		Next = Curr->Next;
		cgc_free(Curr);
		Curr = Next;
	}

	Edges = NULL;

	return(1);
}
