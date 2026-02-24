// Ontological Identity Memory Registry (OIMR) - Minimal C Prototype
// Role: Deterministic entity identity storage and retrieval
#include <stdio.h>
#include <string.h>
#define MAX_ENTITIES 32
#define IDENTITY_KEY_SIZE 32

typedef struct {
    int id;
    char identity_key[IDENTITY_KEY_SIZE]; // immutable
    int in_use;
} IdentityBlock;

IdentityBlock registry[MAX_ENTITIES];

// Allocate a new identity block
int allocate_identity(const char* key) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (!registry[i].in_use) {
            registry[i].id = i + 1;
            strncpy(registry[i].identity_key, key, IDENTITY_KEY_SIZE-1);
            registry[i].identity_key[IDENTITY_KEY_SIZE-1] = '\0';
            registry[i].in_use = 1;
            return registry[i].id;
        }
    }
    return -1; // full
}

// Retrieve identity block by id
IdentityBlock* get_identity(int id) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (registry[i].in_use && registry[i].id == id) return &registry[i];
    }
    return NULL;
}

// Print all identities
void print_identities() {
    printf("Identity Registry:\n");
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (registry[i].in_use) {
            printf("ID: %d | Key: %s\n", registry[i].id, registry[i].identity_key);
        }
    }
}

int main() {
    int id1 = allocate_identity("the man");
    int id2 = allocate_identity("the voice");
    print_identities();
    IdentityBlock* ib = get_identity(id1);
    if (ib) printf("Retrieved: %d = %s\n", ib->id, ib->identity_key);
    return 0;
}
