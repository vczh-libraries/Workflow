import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_Event";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":3,"sourceClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"eventId":2,"arguments":["A"]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":3,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"response":null},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":5,"sourceClientId":2,"ref":{"clientId":1,"objectId":0,"typeId":0},"eventId":2,"arguments":["B"]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":5,"sourceClientId":1,"targetClientId":2,"response":null},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":1,"targetClientId":2}
];
