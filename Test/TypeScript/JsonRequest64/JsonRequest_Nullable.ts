import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Nullable";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":["abc",null]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":"[abc][null]"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[null,{"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":2}]},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":2},"remoteClientId":1,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":4,"sourceClientId":2,"targetClientId":1},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":2},"methodId":3,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"response":"def"},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":2},"remoteClientId":1,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":2,"targetClientId":1},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":3,"sourceClientId":1,"targetClientId":2,"response":"[null][def]"},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":7,"sourceClientId":1,"targetClientId":2}
];
