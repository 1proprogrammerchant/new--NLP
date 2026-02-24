// Ontological Entity State Model (OESM) - Minimal C++ Prototype

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

enum class OntState {
    Undefined, Defined, Referenced, Reinterpreted, Contradicted, Split, Merged, Abstracted, ObserverRelative, Collapsed
};

enum class RefIntegrityStatus {
    Valid, IdentityChanged, IdentitySplit, IdentityMerged, Invalidated, Unresolved, ObserverRelative
};

struct ReferenceObject {
    int sourceEntityId;
    int targetEntityId;
    OntState targetStateAtCreation;
    int creationLayer;
    int lastValidatedLayer;
    RefIntegrityStatus integrityStatus;
    std::set<int> candidateTargets; // for split

    ReferenceObject(int src, int tgt, OntState tgtState, int layer)
        : sourceEntityId(src), targetEntityId(tgt), targetStateAtCreation(tgtState), creationLayer(layer),
          lastValidatedLayer(layer), integrityStatus(RefIntegrityStatus::Valid) {}

    void print() const {
        std::cout << "Reference: " << sourceEntityId << " -> " << targetEntityId
                  << " | Status: " << static_cast<int>(integrityStatus);
        if (!candidateTargets.empty()) {
            std::cout << " | Candidates: ";
            for (int c : candidateTargets) std::cout << c << " ";
        }
        std::cout << std::endl;
    }
};

struct Entity {
    int identity; // persistent id
    std::vector<std::string> referents;
    std::vector<std::string> attributes;
    OntState state;
    int temporal_layer;

    std::vector<int> incomingReferences;
    std::vector<int> outgoingReferences;

    void print() const {
        std::cout << "Entity ID: " << identity << "\nOntological State: " << static_cast<int>(state)
                  << "\nTemporal Layer: " << temporal_layer << "\nAttributes: ";
        for(const auto& a : attributes) std::cout << a << " ";
        std::cout << "\nReferents: ";
        for(const auto& r : referents) std::cout << r << " ";
        std::cout << "\n";
    }
};

// Reference Registry
std::vector<ReferenceObject> referenceRegistry;
std::map<int, Entity> entityRegistry;

// Reference Validator
void validateReferences(int changedEntityId, OntState newState, int newLayer, const std::vector<int>& splitIds = {}) {
    for (auto& ref : referenceRegistry) {
        if (ref.targetEntityId == changedEntityId) {
            if (newState == OntState::Split) {
                ref.integrityStatus = RefIntegrityStatus::Unresolved;
                ref.candidateTargets.clear();
                for (int id : splitIds) ref.candidateTargets.insert(id);
            } else if (newState == OntState::Merged) {
                ref.integrityStatus = RefIntegrityStatus::IdentityMerged;
            } else if (newState == OntState::ObserverRelative) {
                ref.integrityStatus = RefIntegrityStatus::ObserverRelative;
            } else if (newState == OntState::Collapsed) {
                ref.integrityStatus = RefIntegrityStatus::Invalidated;
            } else if (newState != ref.targetStateAtCreation) {
                ref.integrityStatus = RefIntegrityStatus::IdentityChanged;
            } else {
                ref.integrityStatus = RefIntegrityStatus::Valid;
            }
            ref.lastValidatedLayer = newLayer;
        }
    }
}

int main() {
    // Layer 0: E1 = "the man"
    Entity E1{1, {}, {"male", "human"}, OntState::Defined, 0};
    entityRegistry[E1.identity] = E1;
    std::cout << "Layer 0:\n"; entityRegistry[1].print();

    // Layer 1: E2 = "the voice", reference(E2 -> E1)
    Entity E2{2, {}, {"voice"}, OntState::Defined, 1};
    entityRegistry[E2.identity] = E2;
    ReferenceObject refE2toE1(2, 1, OntState::Defined, 1);
    referenceRegistry.push_back(refE2toE1);
    entityRegistry[2].outgoingReferences.push_back(0); // index in registry
    entityRegistry[1].incomingReferences.push_back(0);
    std::cout << "Layer 1:\n"; entityRegistry[2].print(); referenceRegistry[0].print();

    // Layer 2: E1 splits into E1a and E1b
    Entity E1a{3, {}, {"male", "human", "aspectA"}, OntState::Split, 2};
    Entity E1b{4, {}, {"male", "human", "aspectB"}, OntState::Split, 2};
    entityRegistry[E1a.identity] = E1a;
    entityRegistry[E1b.identity] = E1b;
    entityRegistry[1].state = OntState::Split;
    entityRegistry[1].temporal_layer = 2;
    validateReferences(1, OntState::Split, 2, {3,4});
    std::cout << "Layer 2 (split):\n"; entityRegistry[1].print(); referenceRegistry[0].print();

    // Layer 3: E2 denies being E1
    // (No merge, but reference remains unresolved)
    std::cout << "Layer 3 (denial):\n"; entityRegistry[2].print(); referenceRegistry[0].print();

    // Output summary
    std::cout << "\nSummary:\n";
    std::cout << "reference(E2 -> E1) = ";
    switch(referenceRegistry[0].integrityStatus) {
        case RefIntegrityStatus::Unresolved:
            std::cout << "Unresolved\n";
            std::cout << "candidate targets = {E1a, E1b}\n";
            std::cout << "referential integrity = unstable\n";
            std::cout << "identity persistence = indeterminate\n";
            break;
        case RefIntegrityStatus::Valid:
            std::cout << "Valid\n"; break;
        default:
            std::cout << "Other\n"; break;
    }
    return 0;
}
