// High-Performance Observer-Relative Interpretation Model (JavaScript)
// Plane 3: Semantic Interpretation Plane
// Author: 1proprogrammerchant
// Node.js 16+

class Observer {
  constructor(name, perspective) {
    this.name = name;
    this.perspective = perspective; // function(entity) => string/null
  }
  interpret(entity) {
    return this.perspective(entity);
  }
}

class Entity {
  constructor(id, name, state, layer) {
    this.id = id;
    this.name = name;
    this.state = state;
    this.layer = layer;
  }
}

// Example observers with different perspectives
const observers = [
  new Observer("A", e => (e.name === "the man" ? "the man" : null)),
  new Observer("B", e => (e.name === "the voice" ? "the voice" : null)),
  new Observer("C", e => (e.state === "Split" ? "fragmented entity" : null)),
  new Observer("D", e => null), // does not perceive
];

// Entities
const entities = [
  new Entity(1, "the man", "Defined", 0),
  new Entity(2, "the voice", "Defined", 1),
  new Entity(3, "the man (aspect A)", "Split", 2),
  new Entity(4, "the man (aspect B)", "Split", 2),
];

function interpretAll(observers, entities) {
  for (const obs of observers) {
    console.log(`Observer ${obs.name}:`);
    for (const ent of entities) {
      const view = obs.interpret(ent);
      if (view) {
        console.log(`  sees entity ${ent.id} as: ${view}`);
      } else {
        console.log(`  does not perceive entity ${ent.id}`);
      }
    }
  }
}

interpretAll(observers, entities);
