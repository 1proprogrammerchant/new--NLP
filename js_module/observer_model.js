// JavaScript Observer-Relative Interpretation Model (minimal prototype)
// Role: Model observer-dependent entity states
const observers = [
  { name: "A", sees: "the man" },
  { name: "B", sees: "the voice" },
  { name: "C", sees: null }
];

const entity = "the man";

observers.forEach(obs => {
  if (obs.sees === entity) {
    console.log(`Observer ${obs.name} sees entity as itself.`);
  } else if (obs.sees) {
    console.log(`Observer ${obs.name} sees entity as: ${obs.sees}`);
  } else {
    console.log(`Observer ${obs.name} does not perceive the entity.`);
  }
});
