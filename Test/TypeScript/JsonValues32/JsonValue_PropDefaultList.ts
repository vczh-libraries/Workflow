import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_PropDefaultList";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":["A"]},
  {"$":"list","values":["B","C","D"]},
  null,
  null,
  null,
  {"$":"list","values":["B","C","D"]}
];
