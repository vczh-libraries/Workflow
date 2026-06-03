import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_PrimitiveTypes";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":5,"arguments":[5]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":6},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":8,"arguments":[10]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":3,"sourceClientId":1,"targetClientId":2,"response":12},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":4,"arguments":[1.5]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"response":1.75},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":2,"arguments":[2.75]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":1,"targetClientId":2,"response":2.875},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":6,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":6,"arguments":["Hi"]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":6,"sourceClientId":1,"targetClientId":2,"response":"Hi!"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[true]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":1,"targetClientId":2,"response":false},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":3,"arguments":[2]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":1,"targetClientId":2,"response":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":7,"arguments":[{"x":3,"y":7}]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":1,"targetClientId":2,"response":{"x":13,"y":27}},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":10,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":10,"sourceClientId":1,"targetClientId":2}
];
