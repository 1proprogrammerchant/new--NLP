// Ontological Entity State Model (OESM) - Minimal C Prototype
#include <stdio.h>
#include <string.h>

#define MAX_ATTR 4
#define MAX_REF 4
#define MAX_NAME 32

// Ontological States
typedef enum {
    UNDEFINED, DEFINED, REFERENCED, REINTERPRETED, CONTRADICTED, SPLIT, MERGED, ABSTRACTED, OBSERVER_RELATIVE, COLLAPSED
} OntState;

// Entity struct
typedef struct Entity {
    int identity; // persistent id
    char referents[MAX_REF][MAX_NAME]; // referential bindings (names)
    char attributes[MAX_ATTR][MAX_NAME];
    OntState state;
    int temporal_layer;
} Entity;

void print_entity(const Entity* e) {
    printf("Entity ID: %d\n", e->identity);
    printf("Ontological State: %d\n", e->state);
    printf("Temporal Layer: %d\n", e->temporal_layer);
    printf("Attributes: ");
    for(int i=0;i<MAX_ATTR;i++) if(strlen(e->attributes[i])) printf("%s ", e->attributes[i]);
    printf("\nReferents: ");
    for(int i=0;i<MAX_REF;i++) if(strlen(e->referents[i])) printf("%s ", e->referents[i]);
    printf("\n");
}

int main() {
    Entity man = {1, {"voice"}, {"male", "human", "entered", ""}, DEFINED, 0};
    print_entity(&man);
    // Transformation: "The man became a voice."
    man.state = REINTERPRETED;
    strcpy(man.attributes[2], "voice");
    man.temporal_layer = 1;
    print_entity(&man);
    // Transformation: "The voice denied being the man."
    man.state = SPLIT;
    man.temporal_layer = 2;
    print_entity(&man);
    return 0;
}
