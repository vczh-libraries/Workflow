import type { KnownTypeSchema, UnknownTypeSchema } from "./Serialization_Oblist_EventException";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":{"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":-5}},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"methodId":-7,"arguments":[["Int32",0]]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":5,"sourceClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"eventId":-1,"arguments":[["Int32",0],["Int32",0],["Int32",1]]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"response":{"$":"system::RpcException","message":"0,0,1"}},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":6,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":7,"sourceClientId":1,"targetClientId":2}
];
