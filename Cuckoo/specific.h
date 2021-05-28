typedef void* datatype;

typedef enum bool {false, true} bool;
typedef enum table {one, two} table;
#define STRMAX 50
#define FIXEDSIZE 19
#define HASHTYPE "Cuckoo"

struct assoc {
   /* Underlying array */
   datatype* keys1;
   datatype* data1;
   datatype* keys2;
   datatype* data2;
   unsigned int arrsize;
   unsigned int capacity;
   unsigned int keysize;
};

typedef struct assoc assoc;
