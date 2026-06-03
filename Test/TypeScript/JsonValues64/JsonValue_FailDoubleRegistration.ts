import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_FailDoubleRegistration";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  {"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":0},
  1,
  "1st",
  {"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":0},
  "2nd",
  {"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":0},
  0,
  "1st",
  {"$":"system::RpcException","message":"vl::rpc_controller::RpcLifecycleBase::PtrToRef(Ptr<reflection::IDescriptable>)#Object registered to a different lifecycle."}
];
