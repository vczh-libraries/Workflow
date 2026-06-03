import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Collection_Nested_Default";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[{"$":"list","values":[["Int64",1],["Int64",2],["Int64",3]]}]},
  {"$":"list","values":[{"$":"list","values":[["Int64",1],["Int64",2],["Int64",3],["Int64",4]]}]}
];
