import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_CollectionDict_Nested_InByval_OutByref";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[{"$":"map","values":[[["Int32",1],"A"],[["Int32",2],"B"],[["Int32",3],"C"]]}]},
  {"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":-4},
  ["Int32",0],
  {"$":"system::RpcObjectReference","clientId":1,"objectId":2,"typeId":-6},
  ["Int32",5],
  "E",
  null,
  ["Int32",0],
  {"$":"system::RpcObjectReference","clientId":1,"objectId":2,"typeId":-6},
  ["Int32",5],
  ["Int32",1],
  "A",
  ["Int32",2],
  "B",
  ["Int32",3],
  "C",
  ["Int32",4],
  "D",
  ["Int32",5],
  "E"
];
