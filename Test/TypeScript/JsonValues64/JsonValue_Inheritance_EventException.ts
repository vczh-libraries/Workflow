import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Inheritance_EventException";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":0},
  {"$":"map","values":[[["Int64",1],{"$":"system::RpcException","message":"CrashedAtServer"}]]},
  {"$":"map","values":[[["Int64",2],{"$":"system::RpcException","message":"CrashedAtClient"}]]},
  "2:CrashedAtClient;"
];
