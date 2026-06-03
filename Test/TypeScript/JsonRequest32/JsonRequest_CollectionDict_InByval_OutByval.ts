import type { KnownTypeSchema, UnknownTypeSchema } from "./Serialization_CollectionDict_InByval_OutByval";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[{"$":"map","values":[[["Int32",1],"A"],[["Int32",2],"B"],[["Int32",3],"C"]]}]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":{"value":{"$":"map","values":[[["Int32",1],"A"],[["Int32",2],"B"],[["Int32",3],"C"],[["Int32",4],"D"]]},"slot":0}},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"slot":0},
  {"rpcMethod":"IObjectOps_EndInvokeMethod","rpcRequestId":3,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":4,"sourceClientId":1,"targetClientId":2}
];
