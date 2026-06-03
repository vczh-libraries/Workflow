import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_CollectionDict_Nested_Default";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[{"$":"map","values":[[["Int32",1],"A"],[["Int32",2],"B"],[["Int32",3],"C"]]}]},
  {"$":"list","values":[{"$":"map","values":[[["Int32",1],"A"],[["Int32",2],"B"],[["Int32",3],"C"],[["Int32",4],"D"]]}]}
];
