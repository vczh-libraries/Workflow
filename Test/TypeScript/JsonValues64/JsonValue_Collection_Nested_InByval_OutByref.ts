import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Collection_Nested_InByval_OutByref";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[{"$":"list","values":[["Int64",1],["Int64",2],["Int64",3]]}]},
  {"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":-4},
  ["Int64",0],
  {"$":"system::RpcObjectReference","clientId":1,"objectId":2,"typeId":-4},
  ["Int64",5],
  ["Int64",4],
  ["Int64",0],
  {"$":"system::RpcObjectReference","clientId":1,"objectId":2,"typeId":-4},
  {"$":"system::RpcObjectReference","clientId":1,"objectId":3,"typeId":-2},
  true,
  ["Int64",1],
  true,
  ["Int64",2],
  true,
  ["Int64",3],
  true,
  ["Int64",4],
  true,
  ["Int64",5],
  false
];
