import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_Inheritance";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":2},
  "",
  null,
  "One",
  {"$":"system::RpcObjectReference","clientId":1,"objectId":2,"typeId":8},
  "",
  null,
  "Two",
  {"$":"system::RpcObjectReference","clientId":1,"objectId":3,"typeId":0},
  "",
  "",
  null,
  "Derived"
];
