import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_Collection_InByref_OutByref";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[{"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":-4}]},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":-4},"remoteClientId":1,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":2,"targetClientId":1},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":-4},"methodId":-7,"arguments":[["Int64",4]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"response":["Int64",3]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":{"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":-4}},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":-4},"methodId":-5,"arguments":[["Int64",0]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"response":["Int64",1]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":6,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":-4},"methodId":-5,"arguments":[["Int64",1]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":6,"sourceClientId":2,"targetClientId":1,"response":["Int64",2]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":-4},"methodId":-5,"arguments":[["Int64",2]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"response":["Int64",3]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":-4},"methodId":-5,"arguments":[["Int64",3]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":2,"targetClientId":1,"response":["Int64",4]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":1,"targetClientId":2,"ref":{"clientId":2,"objectId":0,"typeId":-4},"methodId":-5,"arguments":[["Int64",4]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":2,"targetClientId":1,"response":["Int64",5]},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":10,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":10,"sourceClientId":1,"targetClientId":2}
];
