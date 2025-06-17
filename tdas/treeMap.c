#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
    int height;
} TreeNode;

typedef struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
} TreeMap;

int is_equal(TreeMap* tree, void* key1, void* key2){
    return tree->lower_than(key1,key2)==0 && tree->lower_than(key2,key1)==0;
}

TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    new->height = 1;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap * nuevo = (TreeMap *)malloc(sizeof(TreeMap));
    if (nuevo == NULL) return NULL;
    nuevo->root = NULL;
    nuevo->current = NULL;
    nuevo->lower_than = lower_than;
    return nuevo;
}

int height(TreeNode *node) {
    return (node == NULL) ? 0 : node->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int getBalance(TreeNode *node) {
    return (node == NULL) ? 0 : height(node->left) - height(node->right);
}

TreeNode* rotateRight(TreeNode* y) {
    TreeNode* x = y->left;
    TreeNode* T2 = x->right;

    x->right = y;
    y->left = T2;
    if (T2) T2->parent = y;
    x->parent = y->parent;
    y->parent = x;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

TreeNode* rotateLeft(TreeNode* x) {
    TreeNode* y = x->right;
    TreeNode* T2 = y->left;

    y->left = x;
    x->right = T2;
    if (T2) T2->parent = x;
    y->parent = x->parent;
    x->parent = y;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

TreeNode* rebalance(TreeNode* node, TreeMap* tree) {
    if (node == NULL) return NULL;

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && getBalance(node->left) >= 0)
        return rotateRight(node);

    // Left Right Case
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Right Case
    if (balance < -1 && getBalance(node->right) <= 0)
        return rotateLeft(node);

    // Right Left Case
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

TreeNode* insertAVL(TreeMap* tree, TreeNode* node, void* key, void* value, TreeNode* parent) {
    if (node == NULL) {
        TreeNode* nuevo = createTreeNode(key, value);
        nuevo->parent = parent;
        tree->current = nuevo;
        return nuevo;
    }

    if (is_equal(tree, key, node->pair->key)) return node;

    if (tree->lower_than(key, node->pair->key)) {
        node->left = insertAVL(tree, node->left, key, value, node);
    } else {
        node->right = insertAVL(tree, node->right, key, value, node);
    }

    return rebalance(node, tree);
}

void insertTreeMap(TreeMap * tree, void* key, void * value) {
    if (tree == NULL) return;
    tree->root = insertAVL(tree, tree->root, key, value, NULL);
}

TreeNode * minimum(TreeNode * x) {
    if (x == NULL) return NULL;
    while (x->left != NULL) x = x->left;
    return x;
}

TreeNode* removeAVL(TreeMap* tree, TreeNode* node, void* key) {
    if (node == NULL) return NULL;

    if (tree->lower_than(key, node->pair->key)) {
        node->left = removeAVL(tree, node->left, key);
        if (node->left) node->left->parent = node;
    } else if (tree->lower_than(node->pair->key, key)) {
        node->right = removeAVL(tree, node->right, key);
        if (node->right) node->right->parent = node;
    } else {
        if (node->left == NULL || node->right == NULL) {
            TreeNode* temp = node->left ? node->left : node->right;

            if (temp == NULL) {
                free(node->pair);
                free(node);
                return NULL;
            } else {
                temp->parent = node->parent;
                free(node->pair);
                free(node);
                return temp;
            }
        } else {
            TreeNode* temp = minimum(node->right);
            node->pair = temp->pair;
            node->right = removeAVL(tree, node->right, temp->pair->key);
            if (node->right) node->right->parent = node;
        }
    }

    node->height = 1 + max(height(node->left), height(node->right));
    return rebalance(node, tree);
}

void eraseTreeMap(TreeMap * tree, void* key) {
    if (tree == NULL || tree->root == NULL) return;
    if (searchTreeMap(tree, key) == NULL) return;
    tree->root = removeAVL(tree, tree->root, key);
}

Pair * searchTreeMap(TreeMap * tree, void* key) {
    if (tree == NULL || tree->root == NULL) return NULL;

    TreeNode * current = tree->root;
    while (current != NULL) {
        if (is_equal(tree, key, current->pair->key)) {
            tree->current = current;
            return current->pair;
        }
        if (tree->lower_than(key, current->pair->key))
            current = current->left;
        else
            current = current->right;
    }
    return NULL;
}

Pair * upperBound(TreeMap * tree, void* key) {
    if (tree == NULL || tree->root == NULL) return NULL;

    TreeNode * current = tree->root;
    TreeNode * candidato = NULL;

    while (current != NULL) {
        if (is_equal(tree, key, current->pair->key)) {
            tree->current = current;
            return current->pair;
        }
        if (!tree->lower_than(current->pair->key, key)) {
            candidato = current;
            current = current->left;
        } else {
            current = current->right;
        }
    }

    if (candidato != NULL) {
        tree->current = candidato;
        return candidato->pair;
    }

    return NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode * current = tree->root;
    while (current->left != NULL)
        current = current->left;
    tree->current = current;
    return current->pair;
}

Pair * nextTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->current == NULL) return NULL;
    TreeNode * current = tree->current;

    if (current->right != NULL) {
        TreeNode * siguiente = minimum(current->right);
        tree->current = siguiente;
        return siguiente->pair;
    }

    TreeNode * parent = current->parent;
    while (parent != NULL && current == parent->right) {
        current = parent;
        parent = parent->parent;
    }

    if (parent != NULL) {
        tree->current = parent;
        return parent->pair;
    }

    return NULL;
}
