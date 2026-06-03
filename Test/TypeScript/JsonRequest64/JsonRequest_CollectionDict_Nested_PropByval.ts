import type { KnownTypeSchema, UnknownTypeSchema } from "./Serialization_CollectionDict_Nested_PropByval";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":2,"arguments":[{"$":"list","values":[{"$":"map","values":[[["Int64",1],"A"],[["Int64",2],"B"],[["Int64",3],"C"]]}]}]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":3,"sourceClientId":1,"targetClientId":2,"response":{"value":{"$":"list","values":[{"$":"map","values":[[["Int64",1],"A"],[["Int64",2],"B"],[["Int64",3],"C"],[["Int64",4],"D"]]}]},"slot":0}},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"slot":0},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":5,"sourceClientId":1,"targetClientId":2}
];
