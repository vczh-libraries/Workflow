import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_Collection_InByval_OutByval";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[["Int32",1],["Int32",2],["Int32",3]]},
  {"$":"list","values":[["Int32",1],["Int32",2],["Int32",3],["Int32",4]]}
];
