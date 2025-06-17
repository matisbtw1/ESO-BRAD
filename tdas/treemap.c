#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) 
{
    TreeMap * nuevo = (TreeMap *)malloc(sizeof(TreeMap));
    if (nuevo == NULL) return NULL;

    nuevo->root = NULL;
    nuevo->current = NULL;
    nuevo->lower_than = lower_than;

    return nuevo;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) 
{
    if (tree == NULL) return;

    TreeNode *parent = NULL;
    TreeNode *current = tree->root;

    while (current != NULL)
    {
        if (is_equal(tree, key, current->pair->key))
        {
            return;
        }
        
        parent = current;

        if (tree->lower_than(key, current->pair->key))
        {
            current = current->left;
        } else{
            current = current->right;
        }
    }

    TreeNode * nuevo = createTreeNode(key, value);
    nuevo->parent = parent;
    if (parent == NULL)
    {
        tree->root = nuevo;
    } else if (tree->lower_than(key, parent->pair->key))
    {
        parent->left = nuevo;
    } else
    {
        parent->right = nuevo;
    }

    tree->current = nuevo;


}

TreeNode * minimum(TreeNode * x)
{
    
if (x == NULL) return NULL;

while (x->left != NULL)
{
    x = x->left;
}

return x;
}


void removeNode(TreeMap * tree, TreeNode* node) 
{
    if (node == NULL) return;

    if (node->left == NULL && node->right == NULL)
    {
        if (node->parent == NULL)
        {
            tree->root = NULL;
        } else {
            if (node->parent->left == node){
                node->parent->left = NULL;
            } else {
                node->parent->right = NULL;
            }
        }
    }
    else if (node->left == NULL || node->right == NULL)
    {
        TreeNode *hijo;
        if (node->left != NULL)
        {
            hijo = node->left;
        } else {
            hijo = node->right;
        }

        if (node->parent == NULL)
        {
            tree->root = hijo;
        } else {
            if (node->parent->left == node)
            {
                node->parent->left = hijo;
            } else {
                node->parent->right = hijo;
            }
        }
        hijo->parent = node->parent;
    }
    else
    {
        TreeNode * siguiente = minimum(node->right);
        node->pair = siguiente->pair;
        removeNode(tree, siguiente);
        return;
    }

    free(node->pair);
    free(node);

}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}

Pair * searchTreeMap(TreeMap * tree, void* key) 
{
    if (tree == NULL || tree->root == NULL) return NULL;

    TreeNode * current = tree->root;

    while (current != NULL)
    {
        if (is_equal(tree, key, current->pair->key))
        {
            tree->current = current;
            return current->pair;
        }

        if (tree->lower_than(key, current->pair->key))
        {
            current = current->left;
        } else
        {
            current = current->right;
        }
    }

    return NULL;
}


Pair * upperBound(TreeMap * tree, void* key) 
{
    if (tree == NULL || tree->root == NULL) return NULL;

    TreeNode * current = tree->root;
    TreeNode * candidato = NULL;

    while (current != NULL)
    {
        if (is_equal(tree, key, current->pair->key))
        {
            tree->current = current;
            return current->pair;
        }

        if (!tree->lower_than(current->pair->key, key))
        {
            candidato = current;
            current = current->left;
        } else
        {
            current = current->right;
        }
    }

    if (candidato != NULL)
    {
        tree->current = candidato;
        return candidato->pair;
    }

    return NULL;
}

Pair * firstTreeMap(TreeMap * tree) 
{
    if (tree == NULL || tree->root == NULL) return NULL;

    TreeNode * current = tree->root;

    while (current->left != NULL)
    {
        current = current->left;
    }

    tree->current = current;

    return current->pair;
}

Pair * nextTreeMap(TreeMap * tree) 
{
    if (tree == NULL || tree->current == NULL) return NULL;
    TreeNode * current = tree->current;

    if (current->right != NULL)
    {
        TreeNode *siguiente = minimum(current->right);
        tree->current = siguiente;
        return siguiente->pair;
    }

    TreeNode *parent = current->parent;
    while (parent != NULL && current == parent->right)
    {
        current = parent;
        parent = parent->parent;
    }

    if (parent != NULL)
    {
        tree->current = parent;
        return parent->pair;
    }

    return NULL;
}
