import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Collection_Default";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[["Int64",1],["Int64",2],["Int64",3]]},
  {"$":"list","values":[["Int64",1],["Int64",2],["Int64",3],["Int64",4]]}
];
