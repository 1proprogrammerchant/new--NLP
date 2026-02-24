# High-Performance Semantic Mutation Generator (Python)
# Plane 3: Semantic Interpretation Plane
# Author: OPA-∞ Architecture
import json
from typing import List, Dict, Any

class SemanticMutationProposal:
    def __init__(self, entity_id: int, from_form: str, to_form: str, mutation_type: str, layer: int):
        self.entity_id = entity_id
        self.from_form = from_form
        self.to_form = to_form
        self.mutation_type = mutation_type
        self.layer = layer
    def to_dict(self):
        return {
            "entity_id": self.entity_id,
            "from": self.from_form,
            "to": self.to_form,
            "type": self.mutation_type,
            "layer": self.layer
        }
    def __repr__(self):
        return json.dumps(self.to_dict())

class MutationEngine:
    def __init__(self):
        self.history: List[SemanticMutationProposal] = []
    def propose(self, entity_id: int, from_form: str, to_forms: List[str], mutation_type: str, layer: int) -> List[SemanticMutationProposal]:
        proposals = [SemanticMutationProposal(entity_id, from_form, to, mutation_type, layer) for to in to_forms]
        self.history.extend(proposals)
        return proposals
    def get_history(self) -> List[Dict[str, Any]]:
        return [p.to_dict() for p in self.history]
    def print_history(self):
        for p in self.history:
            print(p)

def main():
    engine = MutationEngine()
    # Example: "the man" can become "the voice" or "the silence"
    proposals = engine.propose(1, "the man", ["the voice", "the silence"], "mutation", 1)
    for p in proposals:
        print(f"Propose: {p.from_form} → {p.to_form} [{p.mutation_type}] (layer {p.layer})")
    # Simulate recursive mutation
    proposals2 = engine.propose(2, "the voice", ["the echo"], "mutation", 2)
    for p in proposals2:
        print(f"Propose: {p.from_form} → {p.to_form} [{p.mutation_type}] (layer {p.layer})")
    print("\nMutation History:")
    engine.print_history()

if __name__ == "__main__":
    main()
