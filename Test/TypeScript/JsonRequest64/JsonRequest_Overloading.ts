import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Overloading";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":5,"arguments":[123]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":"123"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[true]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":3,"sourceClientId":1,"targetClientId":2,"response":"true"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":2,"arguments":["abc"]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"response":"abc"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":3,"arguments":[{"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":6}]},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":6},"remoteClientId":1,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":2,"targetClientId":1},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":6},"methodId":7,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"response":"xyz"},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":8,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":6},"remoteClientId":1,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":8,"sourceClientId":2,"targetClientId":1},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":1,"targetClientId":2,"response":"xyz"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":4,"arguments":[123,true,"abc",{"$":"system::RpcObjectReference","clientId":2,"objectId":1,"typeId":6}]},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":10,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":1,"typeId":6},"remoteClientId":1,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":10,"sourceClientId":2,"targetClientId":1},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":11,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":1,"typeId":6},"methodId":7,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":11,"sourceClientId":2,"targetClientId":1,"response":"xyz"},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":12,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":1,"typeId":6},"remoteClientId":1,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":12,"sourceClientId":2,"targetClientId":1},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":1,"targetClientId":2,"response":"123,true,abc,xyz"},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":13,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":13,"sourceClientId":1,"targetClientId":2}
];
