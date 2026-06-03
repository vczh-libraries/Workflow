import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Collection_Nested_Default";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[{"$":"list","values":[{"$":"list","values":[["Int64",1],["Int64",2],["Int64",3]]}]}]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":{"value":{"$":"list","values":[{"$":"list","values":[["Int64",1],["Int64",2],["Int64",3],["Int64",4]]}]},"slot":0}},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"slot":0},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":3,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":4,"sourceClientId":1,"targetClientId":2}
];
