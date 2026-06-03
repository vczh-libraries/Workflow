import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_Collection_Nested_InByval_OutByref";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"list","values":[{"$":"list","values":[["Int32",1],["Int32",2],["Int32",3]]}]},
  {"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":-4},
  ["Int32",0],
  {"$":"system::RpcObjectReference","clientId":1,"objectId":2,"typeId":-4},
  ["Int32",5],
  ["Int32",4],
  ["Int32",0],
  {"$":"system::RpcObjectReference","clientId":1,"objectId":2,"typeId":-4},
  {"$":"system::RpcObjectReference","clientId":1,"objectId":3,"typeId":-2},
  true,
  ["Int32",1],
  true,
  ["Int32",2],
  true,
  ["Int32",3],
  true,
  ["Int32",4],
  true,
  ["Int32",5],
  false
];
