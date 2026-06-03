import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_CollectionDict_InByval_OutByref";
import type { Request, Response } from "../Rpc";

export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":1,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"methodId":1,"arguments":[{"$":"map","values":[[["Int32",1],"A"],[["Int32",2],"B"],[["Int32",3],"C"]]}]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":2,"sourceClientId":1,"targetClientId":2,"response":{"$":"system::RpcObjectReference","clientId":1,"objectId":1,"typeId":-6}},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-6},"remoteClientId":2,"hold":true},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":3,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-6},"methodId":-15,"arguments":[["Int32",5],"E"]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":4,"sourceClientId":1,"targetClientId":2,"response":null},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-6},"methodId":-13,"arguments":[]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":5,"sourceClientId":1,"targetClientId":2,"response":["Int32",5]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":6,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-6},"methodId":-14,"arguments":[["Int32",1]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":6,"sourceClientId":1,"targetClientId":2,"response":"A"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-6},"methodId":-14,"arguments":[["Int32",2]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":7,"sourceClientId":1,"targetClientId":2,"response":"B"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-6},"methodId":-14,"arguments":[["Int32",3]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":8,"sourceClientId":1,"targetClientId":2,"response":"C"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-6},"methodId":-14,"arguments":[["Int32",4]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":9,"sourceClientId":1,"targetClientId":2,"response":"D"},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":10,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-6},"methodId":-14,"arguments":[["Int32",5]]},
  {"rpcMethod":"IObjectOps_InvokeMethod","rpcRequestId":10,"sourceClientId":1,"targetClientId":2,"response":"E"},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":11,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":1,"typeId":-6},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":11,"sourceClientId":1,"targetClientId":2},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":12,"sourceClientId":2,"targetClientId":1,"ref":{"clientId":1,"objectId":0,"typeId":0},"remoteClientId":2,"hold":false},
  {"rpcMethod":"IObjectOps_ObjectHold","rpcRequestId":12,"sourceClientId":1,"targetClientId":2}
];
