export declare const RpcTypeId_IValueEnumerable: -1;
export declare const RpcTypeId_IValueEnumerator: -2;
export declare const RpcTypeId_IValueArray: -3;
export declare const RpcTypeId_IValueList: -4;
export declare const RpcTypeId_IValueObservableList: -5;
export declare const RpcTypeId_IValueDictionary: -6;
export declare const RpcTypeId_IValueReadonlyList: -7;

export declare const RpcMethodId_IValueEnumerable_CreateEnumerator: -1;
export declare const RpcMethodId_IValueEnumerator_Next: -2;
export declare const RpcMethodId_IValueEnumerator_GetCurrent: -3;
export declare const RpcMethodId_IValueReadonlyList_GetCount: -4;
export declare const RpcMethodId_IValueReadonlyList_Get: -5;
export declare const RpcMethodId_IValueList_Set: -6;
export declare const RpcMethodId_IValueList_Add: -7;
export declare const RpcMethodId_IValueList_Insert: -8;
export declare const RpcMethodId_IValueList_RemoveAt: -9;
export declare const RpcMethodId_IValueList_Clear: -10;
export declare const RpcMethodId_IValueReadonlyList_Contains: -11;
export declare const RpcMethodId_IValueReadonlyList_IndexOf: -12;
export declare const RpcMethodId_IValueReadonlyDictionary_GetCount: -13;
export declare const RpcMethodId_IValueReadonlyDictionary_Get: -14;
export declare const RpcMethodId_IValueDictionary_Set: -15;
export declare const RpcMethodId_IValueDictionary_Remove: -16;
export declare const RpcMethodId_IValueDictionary_Clear: -17;
export declare const RpcMethodId_IValueReadonlyDictionary_ContainsKey: -18;
export declare const RpcMethodId_IValueReadonlyDictionary_GetKeys: -19;
export declare const RpcMethodId_IValueReadonlyDictionary_GetValues: -20;
export declare const RpcMethodId_IValueArray_Resize: -21;

export declare const RpcEventId_IValueObservableList_ItemChanged: -1;

export interface system_RpcObjectReference
{
  clientId: number;
  objectId: number;
  typeId: number;
}

export interface system_RpcException
{
  message: string;
}

export interface system_RpcByvalReturnValue<T>
{
  value: T;
  slot: number;
}

export type RpcEventExceptionMap =
  | null
  | [number, system_RpcException][]
  ;

export interface MessageBase {
  rpcRequestId: number;
  sourceClientId: number;
}

export interface BroadcastRequest extends MessageBase {
}

export interface DirectRequest extends MessageBase {
  targetClientId: number;
}

export interface IObjectOps_InvokeMethod_Request<T>
{
  rpcMethod: "IObjectOps_InvokeMethod";
  rpcRequestId: number;
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  methodId: number;
  arguments: T[];
}

export interface IObjectOps_InvokeMethod_Response<T>
{
  rpcMethod: "IObjectOps_InvokeMethod";
  rpcRequestId: number;
  sourceClientId: number;
  targetClientId: number;
  response: T | system_RpcByvalReturnValue<T>;
}

export interface IObjectOps_EndInvokeMethod_Request
{
  rpcMethod: "IObjectOps_EndInvokeMethod";
  rpcRequestId: number;
  sourceClientId: number;
  targetClientId: number;
  slot: number;
}

export interface IObjectOps_EndInvokeMethod_Response
{
  rpcMethod: "IObjectOps_EndInvokeMethod";
  rpcRequestId: number;
  sourceClientId: number;
  targetClientId: number;
}

export interface IObjectOps_ObjectHold_Request
{
  rpcMethod: "IObjectOps_ObjectHold";
  rpcRequestId: number;
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  remoteClientId: number;
  hold: boolean;
}

export interface IObjectOps_ObjectHold_Response
{
  rpcMethod: "IObjectOps_ObjectHold";
  rpcRequestId: number;
  sourceClientId: number;
  targetClientId: number;
}

export interface IObjectEventOps_InvokeEvent_Request<T>
{
  rpcMethod: "IObjectEventOps_InvokeEvent";
  rpcRequestId: number;
  sourceClientId: number;
  ref: system_RpcObjectReference;
  eventId: number;
  arguments: T[];
}

export interface Broadcast_Response
{
  rpcMethod: "IObjectEventOps_InvokeEvent";
  rpcRequestId: number;
  sourceClientId: number;
  targetClientId: number;
  response: RpcEventExceptionMap;
}

export type Request<T> =
  | IObjectOps_InvokeMethod_Request<T>
  | IObjectOps_EndInvokeMethod_Request
  | IObjectOps_ObjectHold_Request
  | IObjectEventOps_InvokeEvent_Request<T>
  ;

export type Response<T> =
  | IObjectOps_InvokeMethod_Response<T>
  | IObjectOps_EndInvokeMethod_Response
  | IObjectOps_ObjectHold_Response
  | Broadcast_Response
  ;
