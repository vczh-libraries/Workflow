import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_CollectionOblist_InByval_OutByref";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[{"$":"oblist","values":[["Int32",1],["Int32",2],["Int32",3]]}]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":{"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":-5}},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"methodId":-7,"arguments":[["Int32",5]]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":5,"sourceClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"eventId":-1,"arguments":[["Int32",4],["Int32",0],["Int32",1]]},
  {"rpcMethod":"IObjectEventOps_InvokeEvent","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"response":["Int32",4]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":6,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"methodId":-5,"arguments":[["Int32",0]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":6,"sourceClientId":1,"targetClientId":2,"response":["Int32",1]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"methodId":-5,"arguments":[["Int32",1]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":1,"targetClientId":2,"response":["Int32",2]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"methodId":-5,"arguments":[["Int32",2]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":1,"targetClientId":2,"response":["Int32",3]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"methodId":-5,"arguments":[["Int32",3]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":1,"targetClientId":2,"response":["Int32",4]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":10,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"methodId":-5,"arguments":[["Int32",4]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":10,"sourceClientId":1,"targetClientId":2,"response":["Int32",5]},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":11,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-5},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":11,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":12,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":12,"sourceClientId":1,"targetClientId":2}
];
