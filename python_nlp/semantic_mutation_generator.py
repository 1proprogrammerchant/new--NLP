# Python Semantic Mutation Generator (minimal prototype)
# Role: Propose semantic transformations for ontology engine

def propose_transformations(entity):
    # Example: "the man" can become "the voice"
    return [
        {"from": entity, "to": "the voice", "type": "mutation"},
        {"from": entity, "to": "the silence", "type": "mutation"}
    ]

if __name__ == "__main__":
    entity = "the man"
    proposals = propose_transformations(entity)
    for p in proposals:
        print(f"Propose: {p['from']} → {p['to']} [{p['type']}]")
