#ifndef TREEMAP_H
#define TREEMAP_H

typedef struct Pair {
    void* key;
    void* value;
} Pair;

typedef struct TreeMap TreeMap;

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2));
void insertTreeMap(TreeMap * tree, void* key, void * value);
void eraseTreeMap(TreeMap * tree, void* key);
Pair * searchTreeMap(TreeMap * tree, void* key);
Pair * upperBound(TreeMap * tree, void* key);
Pair * firstTreeMap(TreeMap * tree);
Pair * nextTreeMap(TreeMap * tree);

#endif
