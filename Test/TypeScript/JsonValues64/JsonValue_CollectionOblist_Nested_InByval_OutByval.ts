import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_CollectionOblist_Nested_InByval_OutByval";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"oblist","values":[{"$":"oblist","values":[["Int64",1],["Int64",2],["Int64",3]]}]},
  {"$":"oblist","values":[{"$":"oblist","values":[["Int64",1],["Int64",2],["Int64",3],["Int64",4]]}]}
];
