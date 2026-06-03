import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_CollectionDict_Nested_InByval_OutByref";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[{"$":"map","values":[[["Int64",1],"A"],[["Int64",2],"B"],[["Int64",3],"C"]]}]},
  {"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":-4},
  ["Int64",0],
  {"$":"system::RpcObjectReference","clientId":1,"objectId":2,"typeId":-6},
  ["Int64",5],
  "E",
  null,
  ["Int64",0],
  {"$":"system::RpcObjectReference","clientId":1,"objectId":2,"typeId":-6},
  ["Int64",5],
  ["Int64",1],
  "A",
  ["Int64",2],
  "B",
  ["Int64",3],
  "C",
  ["Int64",4],
  "D",
  ["Int64",5],
  "E"
];
