/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <errno.h>
#include <stdint.h>
#include "bst.h"
#include "malloc.h"
#include "memset.h"


cgc_bst_node_t *cgc_bst_create_node(int key) {

	cgc_bst_node_t *np = cgc_malloc(sizeof(cgc_bst_node_t));
	if (NULL == np) {
		_terminate(ERRNO_ALLOC);
	}
	np->key = key;
	np->data = NULL;
	np->parent = NULL_NODE;
	np->left = NULL_NODE;
	np->right = NULL_NODE;
	return np;
}

void cgc_bst_delete_node(cgc_bst_node_t **node) {
	cgc_memset(*node, '\0', sizeof(cgc_bst_node_t));
	cgc_free(*node);
}

void cgc_bst_init(cgc_bst_t *bst) {
	bst->root = NULL_NODE;
	bst->count = 0;
	bst->iter_node = NULL_NODE;
	bst->iter_asc = TRUE;
}

cgc_bst_node_t *cgc_bst_search(cgc_bst_t *bst, int key) {
	cgc_bst_node_t *node = bst->root;
	while ((node != NULL_NODE) && (node->key != key)) {
		if (key < node->key) {
			node = node->left;
		} else {
			node = node->right;
		}
	}

	return node;
}

cgc_bst_node_t *cgc_bst_max_node_subtree(cgc_bst_node_t *node) {
	while ((NULL_NODE != node) && (NULL_NODE != node->right)) node = node->right;

	return node;
}

int cgc_bst_max_subtree(cgc_bst_node_t *node) {
	return cgc_bst_max_node_subtree(node)->key;
}

cgc_bst_node_t *cgc_bst_max_node(cgc_bst_t *bst) {
	cgc_bst_node_t *node = bst->root;
	return cgc_bst_max_node_subtree(node);
}

int cgc_bst_max(cgc_bst_t *bst) {
	cgc_bst_node_t *node = bst->root;
	return cgc_bst_max_subtree(node);
}

cgc_bst_node_t *cgc_bst_min_node_subtree(cgc_bst_node_t *node) {
	while ((NULL_NODE != node) && (NULL_NODE != node->left)) node = node->left;

	return node;
}

int cgc_bst_min_subtree(cgc_bst_node_t *node) {
	return cgc_bst_min_node_subtree(node)->key;
}

cgc_bst_node_t *cgc_bst_min_node(cgc_bst_t *bst) {
	return cgc_bst_min_node_subtree(bst->root);
}

int cgc_bst_min(cgc_bst_t *bst) {
	return cgc_bst_min_subtree(bst->root);
}

cgc_bst_node_t *cgc_bst_predecessor(cgc_bst_node_t *node) {
	if (NULL_NODE != node->left) {
		return cgc_bst_max_node_subtree(node->left);
	}
	cgc_bst_node_t *n_child = node;
	cgc_bst_node_t *n_parent = node->parent;
	while ((n_parent != NULL_NODE) && (n_child == n_parent->left)) {
		n_child = n_parent;
		n_parent = n_parent->parent;
	}

	return n_parent;
}

cgc_bst_node_t *cgc_bst_successor(cgc_bst_node_t *node) {
	if (NULL_NODE != node->right) {
		return cgc_bst_min_node_subtree(node->right);
	}
	cgc_bst_node_t *n_child = node;
	cgc_bst_node_t *n_parent = node->parent;
	while ((n_parent != NULL_NODE) && (n_child == n_parent->right)) {
		n_child = n_parent;
		n_parent = n_parent->parent;
	}

	return n_parent;
}

cgc_bst_node_t *cgc_bst_iter_start(cgc_bst_t *bst, unsigned char ascending) {
	bst->iter_asc = ascending;
	cgc_bst_node_t *n;
	if (TRUE == bst->iter_asc) {
		bst->iter_node = cgc_bst_min_node(bst);
	} else {
		bst->iter_node = cgc_bst_max_node(bst);
	}
	return bst->iter_node;
}

cgc_bst_node_t *cgc_bst_iter_next(cgc_bst_t *bst) {
	if (TRUE == bst->iter_asc) {
		bst->iter_node = cgc_bst_successor(bst->iter_node);
	} else {
		bst->iter_node = cgc_bst_predecessor(bst->iter_node);
	}
	return bst->iter_node;
}

int cgc_bst_iter_end(cgc_bst_t *bst) {
	return NULL_NODE == bst->iter_node;
}

void cgc_bst_insert_node(cgc_bst_t *bst, cgc_bst_node_t *node) {
	cgc_bst_node_t *n_parent = bst->root;
	cgc_bst_node_t *n_child = bst->root;

	while (n_child != NULL_NODE) {
		n_parent = n_child;
		if (node->key < n_child->key) {
			n_child = n_child->left;
		} else {
			n_child = n_child->right;
		}
	}

	node->parent = n_parent;
	if (NULL_NODE == n_parent) {
		bst->root = node;
	} else if (node->key < n_parent->key) {
		n_parent->left = node;
	} else {
		n_parent->right = node;
	}

	bst->count++;
}

void cgc_bst_insert(cgc_bst_t *bst, int key) {
	cgc_bst_node_t *n = cgc_bst_create_node(key);
	cgc_bst_insert_node(bst, n);
}

cgc_bst_node_t *cgc_bst_remove_node(cgc_bst_t *bst, cgc_bst_node_t *node) {
	cgc_bst_node_t *n_splice;
	cgc_bst_node_t *n_child;
	if ((NULL_NODE == node->left) || (NULL_NODE == node->right)) {
		n_splice = node;
	} else {
		n_splice = cgc_bst_successor(node);
	}

	if (NULL_NODE != n_splice->left) {
		n_child = n_splice->left;
	} else {
		n_child = n_splice->right;
	}

	if (NULL_NODE != n_child) {
		n_child->parent = n_splice->parent;
	}

	if (NULL_NODE == n_splice->parent) {
		bst->root = n_child;
	} else if (n_splice == n_splice->parent->left) {
		n_splice->parent->left = n_child;
	} else {
		n_splice->parent->right = n_child;
	}

	if (n_splice != node) {
		node->key = n_splice->key;
	}


	bst->count--;

	return n_splice;
}

cgc_bst_node_t *cgc_bst_remove(cgc_bst_t *bst, int key) {
	cgc_bst_node_t *node = cgc_bst_search(bst, key);
	if (NULL_NODE == node) return node;

	return cgc_bst_remove_node(bst, node);
}

int cgc_bst_is_empty(cgc_bst_t *bst) {
	return NULL_NODE == bst->root;
}

