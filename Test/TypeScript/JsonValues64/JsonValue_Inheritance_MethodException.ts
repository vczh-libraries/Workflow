import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Inheritance_MethodException";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":0},
  {"$":"system::RpcException","message":"DoNotSetOneValue"},
  {"$":"system::RpcException","message":"DoNotSetTwoValue"}
];
