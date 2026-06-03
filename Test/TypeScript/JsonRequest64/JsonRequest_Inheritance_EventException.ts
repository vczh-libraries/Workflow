import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Inheritance_EventException";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":4},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":4},"methodId":5,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":{"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":0}},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":4,"sourceClientId":2,"ref":{"clientId":1,"objectId":1,"typeId":0},"eventId":3,"arguments":[]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"response":[[1,{"message":"CrashedAtServer"}]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":0},"methodId":1,"arguments":[]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":6,"sourceClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":0},"eventId":7,"arguments":[]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":6,"sourceClientId":2,"targetClientId":1,"response":[[2,{"message":"CrashedAtClient"}]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":1,"targetClientId":2,"response":"2:CrashedAtClient;"},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":7,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":8,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":4},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":8,"sourceClientId":1,"targetClientId":2}
];
