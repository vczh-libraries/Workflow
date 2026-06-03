import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_CollectionDict_Nested_PropByval";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[{"$":"map","values":[[["Int64",1],"A"],[["Int64",2],"B"],[["Int64",3],"C"]]}]},
  null,
  {"$":"list","values":[{"$":"map","values":[[["Int64",1],"A"],[["Int64",2],"B"],[["Int64",3],"C"],[["Int64",4],"D"]]}]}
];
