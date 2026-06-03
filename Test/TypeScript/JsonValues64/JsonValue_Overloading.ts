import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Overloading";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  123,
  "123",
  true,
  "true",
  "abc",
  "abc",
  {"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":6},
  "xyz",
  "xyz",
  123,
  true,
  "abc",
  {"$":"system::RpcObjectReference","clientId":2,"objectId":1,"typeId":6},
  "xyz",
  "123,true,abc,xyz"
];
