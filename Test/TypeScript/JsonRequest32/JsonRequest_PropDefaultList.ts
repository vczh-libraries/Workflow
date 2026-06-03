import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_PropDefaultList";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":{"value":{"$":"list","values":["A"]},"slot":0}},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"slot":0},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":3,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":2,"arguments":[{"$":"list","values":["B","C","D"]}]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":3,"arguments":[]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":6,"sourceClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"eventId":4,"arguments":[]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":6,"sourceClientId":2,"targetClientId":1,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":1,"targetClientId":2,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":1,"targetClientId":2,"response":{"value":{"$":"list","values":["B","C","D"]},"slot":1}},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":8,"sourceClientId":2,"targetClientId":1,"slot":1},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":8,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":9,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":9,"sourceClientId":1,"targetClientId":2}
];
