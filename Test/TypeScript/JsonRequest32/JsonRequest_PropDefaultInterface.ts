import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_PropDefaultInterface";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":{"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":5}},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":5},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":5},"methodId":6,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"response":"A"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":2,"arguments":[{"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":5}]},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":5},"remoteClientId":1,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":2,"targetClientId":1},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":1,"targetClientId":2,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":5},"methodId":6,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":1,"targetClientId":2,"response":"A"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":3,"arguments":[]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":9,"sourceClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"eventId":4,"arguments":[]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":9,"sourceClientId":2,"targetClientId":1,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":1,"targetClientId":2,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":10,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":10,"sourceClientId":1,"targetClientId":2,"response":{"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":5}},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":11,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":5},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":11,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":12,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":12,"sourceClientId":1,"targetClientId":2}
];
