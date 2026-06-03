import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_CollectionOblist_Nested_InByval_OutByval";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"oblist","values":[{"$":"oblist","values":[["Int32",1],["Int32",2],["Int32",3]]}]},
  {"$":"oblist","values":[{"$":"oblist","values":[["Int32",1],["Int32",2],["Int32",3],["Int32",4]]}]}
];
