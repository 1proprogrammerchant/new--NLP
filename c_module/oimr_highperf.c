#include <unistd.h>
#include <sys/types.h>
int ftruncate(int fd, off_t length);
// Lock-Free, Cache-Aligned Identity Registry (OIMR) - High-Performance C Implementation
// Plane 0: Physical Identity Plane
// Role: Deterministic, persistent, lock-free identity storage
// Author: 1proprogrammerchant

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdatomic.h>
#include <errno.h>
#define _XOPEN_SOURCE 700
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#endif

#define CACHE_LINE_SIZE 64
#define MAX_IDENTITIES 1024
#define IDENTITY_KEY_SIZE 32
#define REGISTRY_FILE "identity_registry.dat"

// Fixed-size, cache-aligned identity block
struct __attribute__((aligned(CACHE_LINE_SIZE))) IdentityBlock {
    atomic_int in_use; // 4 bytes, atomic for lock-free
    int id;            // 4 bytes
    char identity_key[IDENTITY_KEY_SIZE]; // 32 bytes
    uint8_t reserved[CACHE_LINE_SIZE - 4 - 4 - IDENTITY_KEY_SIZE]; // pad to 64 bytes
};

_Static_assert(sizeof(struct IdentityBlock) == CACHE_LINE_SIZE, "IdentityBlock must be 64 bytes");

struct IdentityRegistry {
    struct IdentityBlock blocks[MAX_IDENTITIES];
};

struct IdentityRegistry *registry = NULL;

// Memory-map the registry file for persistence
int map_registry(const char *filename) {
#ifdef _WIN32
    HANDLE hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return -1;
    DWORD size = sizeof(struct IdentityRegistry);
    SetFilePointer(hFile, size, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);
    HANDLE hMap = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, size, NULL);
    if (!hMap) return -1;
    registry = (struct IdentityRegistry *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, size);
    CloseHandle(hMap); CloseHandle(hFile);
    return registry ? 0 : -1;
#else
    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd < 0) return -1;
    size_t size = sizeof(struct IdentityRegistry);
    if (ftruncate(fd, size) != 0) { close(fd); return -1; }
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (ptr == MAP_FAILED) return -1;
    registry = (struct IdentityRegistry *)ptr;
    return 0;
#endif
}

// Lock-free allocation of a new identity block
int allocate_identity(const char *key) {
    for (int i = 0; i < MAX_IDENTITIES; ++i) {
        int expected = 0;
        if (atomic_compare_exchange_strong(&registry->blocks[i].in_use, &expected, 1)) {
            registry->blocks[i].id = i + 1;
            strncpy(registry->blocks[i].identity_key, key, IDENTITY_KEY_SIZE - 1);
            registry->blocks[i].identity_key[IDENTITY_KEY_SIZE - 1] = '\0';
            return registry->blocks[i].id;
        }
    }
    return -1; // full
}

// Lock-free retrieval by id
struct IdentityBlock *get_identity(int id) {
    if (id < 1 || id > MAX_IDENTITIES) return NULL;
    struct IdentityBlock *blk = &registry->blocks[id - 1];
    if (atomic_load(&blk->in_use)) return blk;
    return NULL;
}

// Print all identities
void print_identities() {
    printf("Identity Registry:\n");
    for (int i = 0; i < MAX_IDENTITIES; ++i) {
        if (atomic_load(&registry->blocks[i].in_use)) {
            printf("ID: %d | Key: %s\n", registry->blocks[i].id, registry->blocks[i].identity_key);
        }
    }
}

// Leak check: ensure all allocated blocks are accounted for
void leak_check() {
    int count = 0;
    for (int i = 0; i < MAX_IDENTITIES; ++i) {
        if (atomic_load(&registry->blocks[i].in_use)) ++count;
    }
    printf("[LeakCheck] Allocated blocks: %d\n", count);
}

int main() {
    if (map_registry(REGISTRY_FILE) != 0) {
        fprintf(stderr, "Failed to map registry file: %s\n", strerror(errno));
        return 1;
    }
    int id1 = allocate_identity("the man");
    int id2 = allocate_identity("the voice");
    int id3 = allocate_identity("the silence");
    print_identities();
    struct IdentityBlock *ib = get_identity(id1);
    if (ib) printf("Retrieved: %d = %s\n", ib->id, ib->identity_key);
    leak_check();
    // Unmap and cleanup handled by OS on exit
    return 0;
}
