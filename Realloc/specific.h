typedef void* datatype;

typedef enum bool {false, true} bool;
#define STRMAX 50
#define FIXEDSIZE 19
#define HASHTYPE "Realloc"

struct assoc {
   /* Underlying array */
   datatype* keys;
   datatype* data;
   unsigned int arrsize;
   unsigned int capacity;
   unsigned int keysize;
};

typedef struct assoc assoc;
