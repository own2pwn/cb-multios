/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "tr.h"

cgc_tr_t cgc__tr_rot_left(cgc_tr_t root)
{
    cgc_tr_node_t *nroot;
    nroot = root->left;
    root->left = nroot->right;
    nroot->right = root;
    return nroot;
}

cgc_tr_t cgc__tr_rot_right(cgc_tr_t root)
{
    cgc_tr_node_t *nroot;
    nroot = root->right;
    root->right = nroot->left;
    nroot->left = root;
    return nroot;
}

cgc_tr_t cgc_tr_insert(cgc_tr_t root, char *key, cgc_T value)
{
    if (root == NULL)
    {
        root = (cgc_tr_node_t *) cgc_malloc(sizeof(cgc_tr_node_t));
        root->key = cgc_strdup(key);
        cgc_random(&(root->prio), sizeof(root->prio), NULL);
        root->value = value;
        root->left = root->right = NULL;
    }
    else if (cgc_strcmp(key, root->key) < 0)
    {
        root->left = cgc_tr_insert(root->left, key, value);
        if (root->left->prio < root->prio)
            root = cgc__tr_rot_left(root);
    }
    else if (cgc_strcmp(key, root->key) > 0)
    {
        root->right = cgc_tr_insert(root->right, key, value);
        if (root->right->prio < root->prio)
            root = cgc__tr_rot_right(root);
    }
    else
    {
        destroy_value_fn(root->value);
        root->value = value;
    }

    return root;
}

void cgc__tr_destroy_node(cgc_tr_node_t *node)
{
    //destroy_value_fn(node->value);
    cgc_free(node->key);
    cgc_free(node);
}

cgc_tr_t cgc_tr_delete(cgc_tr_t root, char *key)
{
    cgc_tr_node_t *tmp;
    if (root == NULL)
        return NULL;
    if (cgc_strcmp(key, root->key) < 0)
        root->left = cgc_tr_delete(root->left, key);
    else if (cgc_strcmp(key, root->key) > 0)
        root->right = cgc_tr_delete(root->right, key);
    else
    {
        if (root->left && root->right)
        {
            if (root->left->prio < root->right->prio)
            {
                root = cgc__tr_rot_left(root);
                root->right = cgc_tr_delete(root->right, key);
            }
            else
            {
                root = cgc__tr_rot_right(root);
                root->left = cgc_tr_delete(root->left, key);
            }
        }
        else
        {
            tmp = root;
            if (root->left)
                root = root->left;
            else if (root->right)
                root = root->right;
            else
                root = NULL;
            cgc__tr_destroy_node(tmp);
        }
    }
    return root;
}

cgc_T cgc_tr_find(cgc_tr_t root, char *key)
{
    if (root == NULL)
        return NULL;
    if (cgc_strcmp(key, root->key) < 0)
        return cgc_tr_find(root->left, key);
    else if (cgc_strcmp(key, root->key) > 0)
        return cgc_tr_find(root->right, key);
    else
        return root->value;
}

void cgc_tr_destroy(cgc_tr_t root)
{
    if (root != NULL)
    {
        cgc_tr_destroy(root->left);
        cgc__tr_destroy_node(root);
        cgc_tr_destroy(root->right);
    }
}

void cgc_tr_apply(cgc_tr_t root, cgc_tr_apply_fn apply, void *arg)
{
    if (root != NULL)
    {
        cgc_tr_apply(root->left, apply, arg);
        apply(root, arg);
        cgc_tr_apply(root->right, apply, arg);
    }
}

void cgc_tr_print(cgc_tr_t root)
{
    if (root != NULL)
    {
        cgc_tr_print(root->left);
        cgc_printf("&s (&d)" NL, root->key, (int) root->value);
        cgc_tr_print(root->right);
    }
}
