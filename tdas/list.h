#ifndef LIST_H
#define LIST_H

typedef struct Node {
    void *data;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct List {
    Node *head;
    Node *tail;
    Node *current;
    int size;
} List;


// Esta función crea una lista vacía y devuelve un puntero a la lista.
List *create_list();

void imprime_lista(List *L); // Esta función imprime la lista en formato [a b c d e]

// Esta función devuelve un puntero al primer elemento de la lista.
void *first(List *L);

// Esta función mueve el puntero de la lista al siguiente elemento y devuelve un
// puntero a dicho elemento.
void *next(List *L);

// Esta función inserta un nuevo elemento al inicio de la lista.
void pushFront(List *L, void *dato);

// Esta función inserta un nuevo elemento al final de la lista.
void pushBack(List *L, void *dato);

// Esta función inserta un nuevo elemento a continuación del actual de la lista.
void pushCurrent(List *L, void *dato);

// Esta función elimina el primer elemento de la lista.
void *popFront(List *L);

// Esta función elimina el último elemento de la lista.
void *popBack(List *L);

// Esta función elimina el elemento actual de la lista.
void *popCurrent(List *L);

// Esta función elimina todos los elementos de la lista.
void list_clean(List *L);

// Función para insertar ordenado de acuerdo a la función lower_than
void list_sortedInsert(List *L, void *data,
                       int (*lower_than)(void *data1, void *data2));

int list_size(List* L);

#endif