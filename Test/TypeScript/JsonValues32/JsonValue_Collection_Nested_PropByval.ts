import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_Collection_Nested_PropByval";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[{"$":"list","values":[["Int32",1],["Int32",2],["Int32",3]]}]},
  null,
  {"$":"list","values":[{"$":"list","values":[["Int32",1],["Int32",2],["Int32",3],["Int32",4]]}]}
];
