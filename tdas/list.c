#include "list.h"
#include <stdio.h>
#include <stdlib.h>

struct Node {
  void *data;
  struct Node *next;
  struct Node *prev;
};

//typedef Node Node;

struct List {
  Node *head;
  Node *tail;
  Node *current;
  int size;
};

typedef List List;

List *list_create() {
  List *newList = (List *)malloc(sizeof(List));
  if (newList == NULL) {
    return NULL; // Fallo en la asignación de memoria
  }
  newList->head = NULL;
  newList->tail = NULL;
  newList->current = NULL;
  newList->size = 0;
  return newList;
}

void *list_first(List *L) {
  if (L == NULL || L->head == NULL) {
    return NULL; // Lista vacía o no inicializada
  }
  L->current = L->head;
  return L->current->data;
}

void *list_next(List *L) {
  if (L == NULL || L->current == NULL || L->current->next == NULL) {
    return NULL; // Lista vacía, no inicializada o no hay más elementos
  }
  L->current = L->current->next;
  return L->current->data;
}

void *list_prev(List *L) {
  if (L == NULL || L->current == NULL || L->current->prev == NULL) {
    return NULL;
  }
  L->current = L->current->prev;
  return L->current->data;
}

void list_pushFront(List *L, void *data) {
  if (L == NULL) return;

  Node *newNode = (Node *)malloc(sizeof(Node));
  if (newNode == NULL) return;

  newNode->data = data;
  newNode->next = L->head;
  newNode->prev = NULL;

  if (L->head != NULL) {
    L->head->prev = newNode;
  } else {
    L->tail = newNode;
  }

  L->head = newNode;
  L->size++;
}

void list_pushBack(List *L, void *data) {
  if (L == NULL) return;

  Node *newNode = (Node *)malloc(sizeof(Node));
  if (newNode == NULL) return;

  newNode->data = data;
  newNode->next = NULL;
  newNode->prev = L->tail;

  if (L->tail != NULL) {
    L->tail->next = newNode;
  } else {
    L->head = newNode;
  }

  L->tail = newNode;
  L->size++;
}

void list_pushCurrent(List *L, void *data) {
  if (L == NULL || L->current == NULL) return;

  Node *newNode = (Node *)malloc(sizeof(Node));
  if (newNode == NULL) return;

  newNode->data = data;
  newNode->next = L->current->next;
  newNode->prev = L->current;

  if (L->current->next != NULL) {
    L->current->next->prev = newNode;
  } else {
    L->tail = newNode;
  }

  L->current->next = newNode;
  L->size++;
}

void list_sortedInsert(List *L, void *data, int (*lower_than)(void *data1, void *data2)) {
  if (L == NULL) {
    return; // Lista no inicializada
  }

  // Caso especial: inserción al principio o en lista vacía
  if (L->head == NULL || lower_than(data, L->head->data)) {
    list_pushFront(L, data);
    return;
  }

  // Caso general: encontrar la posición correcta para insertar
  Node *current = L->head;
  while (current->next != NULL && !lower_than(data, current->next->data)) {
    current = current->next;
  }

  // Preparar para usar list_pushCurrent
  L->current = current;

  // Insertar el nodo en la posición actual
  list_pushCurrent(L, data);
}

void *list_popFront(List *L) {
  if (L == NULL || L->head == NULL) return NULL;

  Node *temp = L->head;
  void *data = temp->data;

  L->head = L->head->next;

  if (L->head != NULL) {
    L->head->prev = NULL;
  } else {
    L->tail = NULL;
  }

  free(temp);
  L->size--;
  return data;
}

void *list_popBack(List *L) {
  if (L == NULL || L->tail == NULL) return NULL;

  void *data = L->tail->data;
  Node *temp = L->tail;

  L->tail = L->tail->prev;

  if (L->tail != NULL) {
    L->tail->next = NULL;
  } else {
    L->head = NULL;
  }

  free(temp);
  L->size--;
  return data;
}


int list_size(List *L){
    return L->size;
}

void *list_popCurrent(List *L) {
  if (L == NULL || L->current == NULL) return NULL;

  Node *temp = L->current;
  void *data = temp->data;

  if (temp->prev != NULL) {
    temp->prev->next = temp->next;
  } else {
    L->head = temp->next; // Estaba al inicio
  }

  if (temp->next != NULL) {
    temp->next->prev = temp->prev;
  } else {
    L->tail = temp->prev; // Estaba al final
  }

  L->current = temp->next; // Avanzamos el puntero current (puedes ajustarlo según tu lógica)
  free(temp);
  L->size--;
  return data;
}


void list_clean(List *L) {
  if (L == NULL) {
    return; // Lista no inicializada
  }
  Node *current = L->head;
  Node *next;
  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
  L->head = NULL;
  L->tail = NULL;
  L->current = NULL;
  L->size = 0;
}
