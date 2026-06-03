import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_PropDefaultList";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":["A"]},
  {"$":"list","values":["B","C","D"]},
  null,
  null,
  null,
  {"$":"list","values":["B","C","D"]}
];
