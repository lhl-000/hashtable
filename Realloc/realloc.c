#include "specific.h"
#include "../assoc.h"

/*insert the key and value to first table according to the hash number*/
assoc* assoc_init_fixed(int keysize, unsigned int fixedsize);
/*if the number of keys is half of the capacity of hash table,
then resize the old table and make the capacity of new table multiply 2*/
void resize(assoc** a, unsigned int capacity);
/*generate different hash numbers for the hash table*/
unsigned int hash(datatype key , int keysize, int sz);
/* judge weather the two keys are equal, if they are equal then update the value*/
bool equal(datatype key1, datatype key2, int keysize);
/* insert keys and values of old hash table to new hash table*/
void assoc_pass(assoc** a, assoc** new_a);
/*generate specially hash number for string type in the hash table*/
unsigned int hash_string (datatype key, int sz);
/*generate  hash numbers for other types in the  hash table*/
unsigned int hash_other (datatype key, int keysize, int sz);

assoc* assoc_init(int keysize)
{
    return assoc_init_fixed(keysize, FIXEDSIZE);
}

assoc* assoc_init_fixed(int keysize, unsigned int fixedsize)
{
    assoc* a;
    a = (assoc*)ncalloc(1, sizeof(assoc));
    a->keysize = keysize;
    a->capacity = fixedsize;
    a->arrsize = 0;
    a->keys=(datatype*)ncalloc(fixedsize,sizeof(datatype));
    a->data=(datatype*)ncalloc(fixedsize,sizeof(datatype));
    return a;
}

void assoc_insert(assoc** a, datatype key, datatype data)
{
    unsigned int i,h;
    if (a == NULL) {
        on_error("Not Initialized");
    }
    if ((*a)->arrsize * 2 > (*a)->capacity) {
        resize(a, 2*(*a)->capacity);
    }
    h = hash(key, (*a)->keysize, (*a)->capacity);
    for (i=h;(*a)->keys[i] != NULL; i=(i+1)%(*a)->capacity){
        if (equal((*a)->keys[i], key, (*a)->keysize)) {
            (*a)->data[i] = data;
            return;
        }
    }
    (*a)->keys[i] = key;
    (*a)->data[i] = data;
    (*a)->arrsize ++;
}

unsigned int hash(datatype key, int keysize, int sz)
{
    if (keysize == 0) {
        return hash_string(key, sz);
    }
    else {
        return hash_other(key, keysize, sz);
    }
}

/*FNV*/
unsigned int hash_other(datatype key, int keysize, int sz)
{
    int i;
    char* c;
    unsigned long hash;
    int p = 16777619;
    c = (char*)key;
    hash = 2166136261L;
    for(i=0;i<keysize;i++) {
            hash = (hash ^ (*c+i)) * p;
    }
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash%sz;
}

/*handbook*/
unsigned int hash_string(datatype key, int sz)
{
    int i,length;
    unsigned long hash;
    char str[STRMAX];
    strcpy(str,(char*)key);
    length = strlen(str);
    for (i=0, hash=0; i<length; i++) {
        hash = (26 * hash + (str[i] - 'a'));
    }
    return hash%sz;
}

void assoc_free(assoc* a)
{
    free(a->keys);
    free(a->data);
    free(a);
}


void resize(assoc** a, unsigned int capacity)
{
    assoc* new_a;
    new_a = assoc_init_fixed((*a)->keysize, capacity);
    assoc_pass(a, &new_a);
    assoc_free(*a);
    *a = new_a;
}

void assoc_pass(assoc** a, assoc** new_a)
{
    unsigned int i;
    unsigned int n;
    n = (*a)->capacity;
    for (i=0; i<n; i++) {
        if ((*a)->keys[i] != NULL) {
            assoc_insert(new_a, (*a)->keys[i], (*a)->data[i]);
        }
    }
}

bool equal(datatype key1, datatype key2, int keysize)
{
    if (keysize == 0) {
        if (strcmp((char*)key1, (char*)key2) == 0) {
            return true;
        }
    }
    else {
        if (memcmp(key1, key2, keysize) == 0) {
            return true;
        }
    }
    return false;
}

unsigned int assoc_count(assoc* a)
{
    if (a == NULL) {
        on_error("Not Initialized");
    }
    return a->arrsize;
}

datatype assoc_lookup(assoc* a, datatype key)
{
    int i,h;
    if (a == NULL) {
        on_error("Not Initialized");
    }
    h = hash(key, a->keysize, a->capacity);
    for (i=h; a->keys[i] != NULL; i=(i+1)%a->capacity) {
        if (equal(a->keys[i], key, a->keysize)) {
            return a->data[i];
        }
    }
    return NULL;
}
