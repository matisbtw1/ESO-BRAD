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
    int height; // NUEVO: Campo para la altura del nodo (AVL)
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

// ==================================================================
// NUEVAS FUNCIONES AUXILIARES PARA EL ÁRBOL AVL
// ==================================================================

// Devuelve la altura de un nodo (0 si es NULL)
static int height(TreeNode * n) {
    if (n == NULL) return 0;
    return n->height;
}

// Devuelve el máximo entre dos enteros
static int max(int a, int b) {
    return (a > b) ? a : b;
}

// Actualiza la altura de un nodo basándose en la altura de sus hijos
static void update_height(TreeNode * n) {
    if (n != NULL) {
        n->height = 1 + max(height(n->left), height(n->right));
    }
}

// Calcula el factor de equilibrio de un nodo
static int get_balance(TreeNode * n) {
    if (n == NULL) return 0;
    return height(n->left) - height(n->right);
}

// Rotación simple a la derecha (LL)
static TreeNode* right_rotate(TreeMap *tree, TreeNode *y) {
    TreeNode *x = y->left;
    TreeNode *T2 = x->right;

    // Realizar rotación
    x->right = y;
    y->left = T2;
    
    // Actualizar padres
    x->parent = y->parent;
    y->parent = x;
    if (T2 != NULL) T2->parent = y;
    
    // Actualizar la raíz del árbol si es necesario
    if (x->parent == NULL) {
        tree->root = x;
    } else if (x->parent->left == y) {
        x->parent->left = x;
    } else {
        x->parent->right = x;
    }

    // Actualizar alturas
    update_height(y);
    update_height(x);

    return x; // Nueva raíz del subárbol
}

// Rotación simple a la izquierda (RR)
static TreeNode* left_rotate(TreeMap *tree, TreeNode *x) {
    TreeNode *y = x->right;
    TreeNode *T2 = y->left;

    // Realizar rotación
    y->left = x;
    x->right = T2;

    // Actualizar padres
    y->parent = x->parent;
    x->parent = y;
    if (T2 != NULL) T2->parent = x;

    // Actualizar la raíz del árbol si es necesario
    if (y->parent == NULL) {
        tree->root = y;
    } else if (y->parent->left == x) {
        y->parent->left = y;
    } else {
        y->parent->right = y;
    }
    
    // Actualizar alturas
    update_height(x);
    update_height(y);

    return y; // Nueva raíz del subárbol
}

// Función para rebalancear el árbol desde un nodo hacia la raíz
void rebalance(TreeMap *tree, TreeNode *node) {
    TreeNode *current = node;
    while (current != NULL) {
        update_height(current);
        int balance = get_balance(current);

        // Caso Izquierda-Izquierda (LL)
        if (balance > 1 && get_balance(current->left) >= 0) {
            current = right_rotate(tree, current);
        }
        // Caso Derecha-Derecha (RR)
        else if (balance < -1 && get_balance(current->right) <= 0) {
            current = left_rotate(tree, current);
        }
        // Caso Izquierda-Derecha (LR)
        else if (balance > 1 && get_balance(current->left) < 0) {
            current->left = left_rotate(tree, current->left);
            current = right_rotate(tree, current);
        }
        // Caso Derecha-Izquierda (RL)
        else if (balance < -1 && get_balance(current->right) > 0) {
            current->right = right_rotate(tree, current->right);
            current = left_rotate(tree, current);
        }
        
        // Moverse hacia la raíz
        if (current != NULL) {
           current = current->parent;
        }
    }
}
// ==================================================================
// FIN DE LAS NUEVAS FUNCIONES
// ==================================================================

TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    new->height = 1; // MODIFICADO: Inicializar altura en 1
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
        if (is_equal(tree, key, current->pair->key)) return;
        parent = current;
        if (tree->lower_than(key, current->pair->key))
            current = current->left;
        else
            current = current->right;
    }

    TreeNode * nuevo = createTreeNode(key, value);
    nuevo->parent = parent;
    if (parent == NULL)
        tree->root = nuevo;
    else if (tree->lower_than(key, parent->pair->key))
        parent->left = nuevo;
    else
        parent->right = nuevo;

    tree->current = nuevo;

    // MODIFICADO: Llamar a rebalance para mantener el árbol equilibrado
    rebalance(tree, nuevo->parent);
}

TreeNode * minimum(TreeNode * x)
{
    if (x == NULL) return NULL;
    while (x->left != NULL) x = x->left;
    return x;
}


void removeNode(TreeMap * tree, TreeNode* node) 
{
    if (node == NULL) return;

    TreeNode *parent_of_removed = node->parent;
    TreeNode *child = NULL;

    // Caso 1: El nodo no tiene hijo derecho
    if (node->right == NULL) {
        child = node->left;
        if (node->parent == NULL) tree->root = child;
        else if (node == node->parent->left) node->parent->left = child;
        else node->parent->right = child;

        if (child != NULL) child->parent = node->parent;
    } 
    // Caso 2: El nodo no tiene hijo izquierdo
    else if (node->left == NULL) {
        child = node->right;
        if (node->parent == NULL) tree->root = child;
        else if (node == node->parent->left) node->parent->left = child;
        else node->parent->right = child;
        
        if (child != NULL) child->parent = node->parent;
    }
    // Caso 3: El nodo tiene dos hijos
    else {
        TreeNode* successor = minimum(node->right);
        parent_of_removed = successor->parent;
        
        // El sucesor no es el hijo directo del nodo a eliminar
        if (successor->parent != node) {
            // Se reemplaza al sucesor con su hijo derecho
            if (successor->right != NULL) successor->right->parent = successor->parent;
            successor->parent->left = successor->right;
            
            // Se conecta el sucesor con el hijo derecho del nodo original
            successor->right = node->right;
            node->right->parent = successor;
        } else {
            // Si el sucesor es el hijo directo, el punto de rebalanceo es el sucesor mismo
            parent_of_removed = successor;
        }
        
        // Se reemplaza el nodo a eliminar con el sucesor
        if (node->parent == NULL) tree->root = successor;
        else if (node == node->parent->left) node->parent->left = successor;
        else node->parent->right = successor;
        
        successor->parent = node->parent;
        successor->left = node->left;
        node->left->parent = successor;
    }

    free(node->pair);
    free(node);

    // MODIFICADO: Llamar a rebalance desde el padre del nodo eliminado
    rebalance(tree, parent_of_removed);
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
            current = current->left;
        else
            current = current->right;
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
    TreeNode * current = minimum(tree->root);
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
    
    tree->current = parent;
    if (parent != NULL) return parent->pair;

    return NULL;
}