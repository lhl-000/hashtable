#include "specific.h"
#include "../assoc.h"

/*if the number of keys is half of the capacity of hash table,
then resize the old table and make the capacity of new table multiply 2*/
void resize(assoc** a, unsigned int capacity);
/*build a suitable initial function which can change capacity in hash tables*/
assoc* assoc_init_fixed(int keysize, int fixedsize);
/*insert the key and value to first table according to the hash number*/
bool insert_one(assoc** a, datatype key, datatype data, int hash);
/*insert the key and value to first table according to the  hash number*/
bool insert_two(assoc** a, datatype key, datatype data, int hash);
/*use the greed algorithm to insert the key and value to first table
 and kick the old key and value to second hash table*/
void cuckoo_insert_one(assoc** a, datatype key, datatype data, int hash);
/*use the greed algorithm to insert the key and value to second table
 and kick the old key and value to first hash table*/
void cuckoo_insert_two(assoc** a, datatype key, datatype data, int hash);
/*if the function cuckoo_insert_one and the function cuckoo_insert_two
call each other too many times, then insert key and value sequently*/
void insert_sequence(assoc** a, int strat, datatype key, datatype data);
/*generate different hash numbers for first hash table*/
unsigned int hash1(datatype key , int keysize, int sz);
/*generate specially hash number for string type in the first hash table*/
unsigned int hash_string1 (datatype key, int sz);
/*generate  hash numbers for other types in the first hash table*/
unsigned int hash_other1 (datatype key, int keysize, int sz);
/*generate different hash numbers for second hash table*/
unsigned int hash_other2 (datatype key, int keysize, int sz);
/*generate specially hash number for string type in the second hash table*/
unsigned int hash2(datatype key , int keysize, int sz);
/*generate  hash numbers for other types in the second hash table*/
unsigned int hash_string2 (datatype key, int sz);
/* judge weather the two keys are equal, if they are equal then update the value*/
bool equal(datatype key1, datatype key2, int keysize);
/* insert keys and values of old hash table to new hash table*/
void assoc_pass(assoc** a, assoc** new_a);


assoc* assoc_init(int keysize)
{

    return assoc_init_fixed(keysize, FIXEDSIZE);
}

assoc* assoc_init_fixed(int keysize, int fixedsize)
{
    assoc* a;
    a = (assoc*)ncalloc(1, sizeof(assoc));
    a->keysize = keysize;
    a->capacity = fixedsize;
    a->arrsize = 0;
    a->keys1=(datatype*)ncalloc(fixedsize,sizeof(datatype));
    a->data1=(datatype*)ncalloc(fixedsize,sizeof(datatype));
    a->keys2=(datatype*)ncalloc(fixedsize,sizeof(datatype));
    a->data2=(datatype*)ncalloc(fixedsize,sizeof(datatype));
    return a;
}

void assoc_insert(assoc** a, datatype key, datatype data)
{
    int i, j;
    if (a == NULL) {
        on_error("Not Initialized");
    }
    if (2*(*a)->arrsize > (*a)->capacity) {
        resize(a, 2*(*a)->capacity);
    }
    i = hash1(key, (*a)->keysize, (*a)->capacity);
    if (insert_one(a, key, data, i) == false) {
        j = hash2(key, (*a)->keysize, (*a)->capacity);
        if ((insert_two(a, key, data, j)) == false) {
            if (rand()%2 == 0) {
                cuckoo_insert_one(a, key, data, i);
            }
            else {
                cuckoo_insert_two(a, key, data, j);
            }
        }
    }
    return;
}

bool insert_one(assoc** a, datatype key, datatype data, int hash)
{
    int i;
    i = hash;
    if ((*a)->keys1[i] == NULL) {
       (*a)->keys1[i] = key;
       (*a)->data1[i] = data;
       (*a)->arrsize ++;
       return true;
    }
    if (equal((*a)->keys1[i], key, (*a)->keysize)) {
       (*a)->data1[i] = data;
       return true;
    }
    return false;
}

bool insert_two(assoc** a, datatype key, datatype data, int hash)
{
    int i;
    i = hash;
    if ((*a)->keys2[i] == NULL) {
        (*a)->keys2[i] = key;
        (*a)->data2[i] = data;
        (*a)->arrsize ++;
        return true;
    }
    if (equal((*a)->keys2[i], key, (*a)->keysize)) {
       (*a)->data2[i] = data;
       return true;
    }
       return false;
}

void cuckoo_insert_one(assoc** a, datatype key, datatype data, int hash)
{
    int i,j;
    datatype old_key, old_data;
    i = hash;
    old_key =  (*a)->keys1[i];
    old_data = (*a)->data1[i];
    (*a)->keys1[i] = key;
    (*a)->data1[i] = data;
    /*printf("#%d table1  location: %d capacity %d\n",cnt,i,(*a)->capacity);*/
     j = hash2(old_key, (*a)->keysize, (*a)->capacity);
     if (insert_two(a, old_key, old_data, j) == false) {
         cuckoo_insert_two(a, old_key, old_data, j);
     }
    return;
}

void cuckoo_insert_two(assoc** a, datatype key, datatype data, int hash)
{
    int i, j;
    static int times = 0;
    datatype old_key, old_data;
    i = hash;
    old_key =  (*a)->keys2[i];
    old_data = (*a)->data2[i];
    (*a)->keys2[i] = key;
    (*a)->data2[i] = data;
    times++;
    if (times > 4) {
        insert_sequence(a, i,old_key,old_data);
        times = 0;
        return;
    }
    /*printf("#%d table2  location: %d capacity %d\n",cnt,i,(*a)->capacity);*/
    j = hash1(key, (*a)->keysize, (*a)->capacity);
    if (insert_one(a, old_key, old_data, j) == false) {
        cuckoo_insert_one(a, old_key, old_data, j);
    }
    times = 0;
    return;
}

void insert_sequence(assoc **a, int start, datatype key, datatype data)
{
    int i;
    for (i=start; (*a)->keys2[i] != NULL; i=(i+1)%(*a)->capacity) {
        if (equal((*a)->keys2[i], key, (*a)->keysize)) {
            (*a)->data2[i] = data;
            return;
        }
    }
    (*a)->keys2[i] = key;
    (*a)->data2[i] = data;
    (*a)->arrsize ++;
}

unsigned int hash1(datatype key, int keysize, int sz)
{
    if (keysize == 0) {
        return hash_string1(key, sz);
    }
    else {
        return hash_other1(key, keysize, sz);
    }
}

unsigned int hash2(datatype key, int keysize, int sz)
{
    if (keysize == 0) {
        return hash_string2(key, sz);
    }
    else {
        return hash_other2(key, keysize, sz);
    }
}

/*handbook*/
unsigned int hash_string1(datatype key, int sz)
{
    int i,length;
    unsigned long hash;
    char str[STRMAX];
    strcpy(str, (char*)key);
    length = strlen(str);
    hash = 5381;
    for (i=0; i<length; i++) {
        hash = (26 * hash + (str[i] - 'a'));
    }
    return hash%sz;
}

/*FNV*/
unsigned int hash_string2(datatype key, int sz)
{
    int i,length;
    unsigned long hash;
    char str[STRMAX];
    int p = 16777619;
    strcpy(str, (char*)key);
    length = strlen(str);
    hash = 2166136261L;
    for(i=0; i<length; i++) {
       hash = (hash ^ str[i]) * p;
    }
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash%sz;
}

/*handbook*/
unsigned int hash_other1(datatype key, int keysize, int sz)
{
    int i;
    char* c;
    unsigned long hash;
    hash = 5381;
    c = (char*)key;
    for (i=0; i<keysize; i++) {
        hash = 26 * hash + (*c + i);
    }
    return hash%sz;
}

/*FNV*/
unsigned int hash_other2(datatype key, int keysize, int sz)
{
    int i;
    char* c;
    unsigned long hash;
    int p = 16777619;
    c = (char*)key;
    hash = 2166136261L;
    for(i=0; i<keysize;i++) {
        hash = (hash ^ (*c+i)) * p;
    }
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash%sz;
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
        if ((*a)->keys1[i] != NULL) {
            assoc_insert(new_a, (*a)->keys1[i], (*a)->data1[i]);
        }
    }
    for (i=0; i<n; i++) {
        if ((*a)->keys2[i] != NULL) {
             assoc_insert(new_a, (*a)->keys2[i], (*a)->data2[i]);
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
    int i;
    if (a == NULL) {
        on_error("Not Initialized");
    }
    for (i=hash1(key, a->keysize, a->capacity);
    a->keys1[i] != NULL; i=(i+1)%a->capacity) {
        if (equal(a->keys1[i], key, a->keysize)) {
            return a->data1[i];
        }
    }
    for (i=hash2(key, a->keysize, a->capacity);
    a->keys2[i] != NULL; i=(i+1)%a->capacity) {
        if (equal(a->keys2[i], key, a->keysize)) {
            return a->data2[i];
        }
    }
    return NULL;
}

void assoc_free(assoc* a)
{
    free(a->keys1);
    free(a->data1);
    free(a->keys2);
    free(a->data2);
    free(a);
}
