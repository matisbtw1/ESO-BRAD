#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    if ((float)map->size / map->capacity > 0.7) {
        enlarge(map);
    }
    size_t pos = hash(key,map->capacity);
    Pair * par = createPair(key,value);

    for (size_t k = 0 ; k < map->capacity ; k++)
    {
        if (map->buckets[pos] == NULL || map->buckets[pos]->key == NULL) {
            map->buckets[pos] = par;
            map->size++;
            map->current = pos;
            return;}
        if (is_equal(map->buckets[pos]->key, key) ) return;
        
        pos = (pos + 1) % map->capacity;
    }
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)

    Pair** aux = map->buckets;
    size_t old_capacity = map->capacity;

    map->capacity *= 2;
    map->buckets = malloc(sizeof(Pair*)*map->capacity);
    if (map->buckets == NULL) exit(EXIT_FAILURE);
    for (size_t k = 0 ; k < map->capacity ; k++){
        map->buckets[k] = NULL;
    }
    map->size = 0;
    map->current = -1;
    for (size_t k = 0 ; k < old_capacity ; k++)
    {
        if (aux[k] != NULL && aux[k]->key != NULL) {
            insertMap(map, aux[k]->key, aux[k]->value);
            
        }
    }
    free(aux);




}


HashMap * createMap(long capacity) {
    HashMap * mapa = malloc(sizeof(HashMap));
    if (mapa == NULL) exit(EXIT_FAILURE);

    mapa->buckets = malloc(sizeof(Pair*)*capacity);
    if (mapa->buckets == NULL) exit(EXIT_FAILURE);

    for (size_t k = 0 ; k < capacity ; k++){
        mapa->buckets[k] = NULL;
    }
    mapa->size = 0;
    mapa->capacity = capacity;
    mapa->current = -1;
    return mapa;

    return NULL;
}

void eraseMap(HashMap * map,  char * key) {    
    size_t pos = hash(key,map->capacity);
    for (size_t k = 0 ; k < map->capacity ; k++)
    {
        if (map->buckets[pos] == NULL) return;
        
        if (is_equal(map->buckets[pos]->key, key)) {
            map->buckets[pos]->key = NULL;
            map->size--;
            return;
        }
        
        pos = (pos + 1) % map->capacity;
    }

}

Pair * searchMap(HashMap * map,  char * key) {   

    size_t pos = hash(key,map->capacity);
    for (size_t k = 0 ; k < map->capacity ; k++)
    {
        if (map->buckets[pos] == NULL) return NULL;
        if (is_equal(map->buckets[pos]->key, key)) {
            map->current = pos;
            return map->buckets[pos];
        }
        
        pos = (pos + 1) % map->capacity;
    }

    return NULL;
}

Pair * firstMap(HashMap * map) {
    for (size_t k = 0 ; k < map->capacity ; k++)
    {
        if (map->buckets[k] != NULL && map->buckets[k]->key != NULL)  {
            map->current = k;
            return map->buckets[k];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    for (size_t k = map->current + 1 ; k < map->capacity ; k++)
    {
        if (map->buckets[k] != NULL && map->buckets[k]->key != NULL) {
            map->current = k;
            return map->buckets[k];
        }
    }
    return NULL;
    
}
void* map_get(HashMap *map, char *key) {
    Pair *pair = searchMap(map, key);  // Buscar el par en el mapa
    if (pair != NULL) {
        return pair->value;  // Si existe, devolver el valor asociado
    }
    return NULL;  // Si no se encuentra, devolver NULL
}