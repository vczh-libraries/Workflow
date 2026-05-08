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

export interface IListOps_EnumCreate_Request
{
  rpcMethod: "IListOps_EnumCreate";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
}

export interface IListOps_EnumCreate_Response
{
  rpcMethod: "IListOps_EnumCreate";
  sourceClientId: number;
  targetClientId: number;
  response: system_RpcObjectReference;
}

export interface IListOps_EnumNext_Request
{
  rpcMethod: "IListOps_EnumNext";
  sourceClientId: number;
  targetClientId: number;
  enumerator: system_RpcObjectReference;
}

export interface IListOps_EnumNext_Response
{
  rpcMethod: "IListOps_EnumNext";
  sourceClientId: number;
  targetClientId: number;
  response: boolean;
}

export interface IListOps_EnumGetCurrent_Request
{
  rpcMethod: "IListOps_EnumGetCurrent";
  sourceClientId: number;
  targetClientId: number;
  enumerator: system_RpcObjectReference;
}

export interface IListOps_EnumGetCurrent_Response<T>
{
  rpcMethod: "IListOps_EnumGetCurrent";
  sourceClientId: number;
  targetClientId: number;
  response: T;
}

export interface IListOps_ListGetCount_Request
{
  rpcMethod: "IListOps_ListGetCount";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
}

export interface IListOps_ListGetCount_Response
{
  rpcMethod: "IListOps_ListGetCount";
  sourceClientId: number;
  targetClientId: number;
  response: number;
}

export interface IListOps_ListGet_Request
{
  rpcMethod: "IListOps_ListGet";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  index: number;
}

export interface IListOps_ListGet_Response<T>
{
  rpcMethod: "IListOps_ListGet";
  sourceClientId: number;
  targetClientId: number;
  response: T;
}

export interface IListOps_ListSet_Request<T>
{
  rpcMethod: "IListOps_ListSet";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  index: number;
  value: T;
}

export interface IListOps_ListSet_Response
{
  rpcMethod: "IListOps_ListSet";
  sourceClientId: number;
  targetClientId: number;
}

export interface IListOps_ListAdd_Request<T>
{
  rpcMethod: "IListOps_ListAdd";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  value: T;
}

export interface IListOps_ListAdd_Response
{
  rpcMethod: "IListOps_ListAdd";
  sourceClientId: number;
  targetClientId: number;
  response: number;
}

export interface IListOps_ListInsert_Request<T>
{
  rpcMethod: "IListOps_ListInsert";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  index: number;
  value: T;
}

export interface IListOps_ListInsert_Response
{
  rpcMethod: "IListOps_ListInsert";
  sourceClientId: number;
  targetClientId: number;
  response: number;
}

export interface IListOps_ListRemoveAt_Request
{
  rpcMethod: "IListOps_ListRemoveAt";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  index: number;
}

export interface IListOps_ListRemoveAt_Response
{
  rpcMethod: "IListOps_ListRemoveAt";
  sourceClientId: number;
  targetClientId: number;
  response: boolean;
}

export interface IListOps_ListClear_Request
{
  rpcMethod: "IListOps_ListClear";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
}

export interface IListOps_ListClear_Response
{
  rpcMethod: "IListOps_ListClear";
  sourceClientId: number;
  targetClientId: number;
}

export interface IListOps_ListContains_Request<T>
{
  rpcMethod: "IListOps_ListContains";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  value: T;
}

export interface IListOps_ListContains_Response
{
  rpcMethod: "IListOps_ListContains";
  sourceClientId: number;
  targetClientId: number;
  response: boolean;
}

export interface IListOps_ListIndexOf_Request<T>
{
  rpcMethod: "IListOps_ListIndexOf";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  value: T;
}

export interface IListOps_ListIndexOf_Response
{
  rpcMethod: "IListOps_ListIndexOf";
  sourceClientId: number;
  targetClientId: number;
  response: number;
}

export interface IListOps_DictGetCount_Request
{
  rpcMethod: "IListOps_DictGetCount";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
}

export interface IListOps_DictGetCount_Response
{
  rpcMethod: "IListOps_DictGetCount";
  sourceClientId: number;
  targetClientId: number;
  response: number;
}

export interface IListOps_DictGet_Request<T>
{
  rpcMethod: "IListOps_DictGet";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  key: T;
}

export interface IListOps_DictGet_Response<T>
{
  rpcMethod: "IListOps_DictGet";
  sourceClientId: number;
  targetClientId: number;
  response: T;
}

export interface IListOps_DictSet_Request<T>
{
  rpcMethod: "IListOps_DictSet";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  key: T;
  value: T;
}

export interface IListOps_DictSet_Response
{
  rpcMethod: "IListOps_DictSet";
  sourceClientId: number;
  targetClientId: number;
}

export interface IListOps_DictRemove_Request<T>
{
  rpcMethod: "IListOps_DictRemove";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  key: T;
}

export interface IListOps_DictRemove_Response
{
  rpcMethod: "IListOps_DictRemove";
  sourceClientId: number;
  targetClientId: number;
  response: boolean;
}

export interface IListOps_DictClear_Request
{
  rpcMethod: "IListOps_DictClear";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
}

export interface IListOps_DictClear_Response
{
  rpcMethod: "IListOps_DictClear";
  sourceClientId: number;
  targetClientId: number;
}

export interface IListOps_DictContainsKey_Request<T>
{
  rpcMethod: "IListOps_DictContainsKey";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  key: T;
}

export interface IListOps_DictContainsKey_Response
{
  rpcMethod: "IListOps_DictContainsKey";
  sourceClientId: number;
  targetClientId: number;
  response: boolean;
}

export interface IListOps_DictGetKeys_Request
{
  rpcMethod: "IListOps_DictGetKeys";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
}

export interface IListOps_DictGetKeys_Response
{
  rpcMethod: "IListOps_DictGetKeys";
  sourceClientId: number;
  targetClientId: number;
  response: system_RpcObjectReference;
}

export interface IListOps_DictGetValues_Request
{
  rpcMethod: "IListOps_DictGetValues";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
}

export interface IListOps_DictGetValues_Response
{
  rpcMethod: "IListOps_DictGetValues";
  sourceClientId: number;
  targetClientId: number;
  response: system_RpcObjectReference;
}

export interface IObjectOps_InvokeMethod_Request<T>
{
  rpcMethod: "IObjectOps_InvokeMethod";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  methodId: number;
  arguments: T[];
}

export interface IObjectOps_InvokeMethod_Response<T>
{
  rpcMethod: "IObjectOps_InvokeMethod";
  sourceClientId: number;
  targetClientId: number;
  response: T | system_RpcByvalReturnValue<T>;
}

export interface IObjectOps_EndInvokeMethod_Request
{
  rpcMethod: "IObjectOps_EndInvokeMethod";
  sourceClientId: number;
  targetClientId: number;
  slot: number;
}

export interface IObjectOps_EndInvokeMethod_Response
{
  rpcMethod: "IObjectOps_EndInvokeMethod";
  sourceClientId: number;
  targetClientId: number;
}

export interface IObjectOps_ObjectHold_Request
{
  rpcMethod: "IObjectOps_ObjectHold";
  sourceClientId: number;
  targetClientId: number;
  ref: system_RpcObjectReference;
  remoteClientId: number;
  hold: boolean;
}

export interface IObjectOps_ObjectHold_Response
{
  rpcMethod: "IObjectOps_ObjectHold";
  sourceClientId: number;
  targetClientId: number;
}

export interface IObjectOps_RegisterService_Request
{
  rpcMethod: "IObjectOps_RegisterService";
  sourceClientId: number;
  targetClientId: number;
  typeId: number;
  service: system_RpcObjectReference;
}

export interface IObjectOps_RegisterService_Response
{
  rpcMethod: "IObjectOps_RegisterService";
  sourceClientId: number;
  targetClientId: number;
}

export interface IListEventOps_OnItemChanged_Request
{
  rpcMethod: "IListEventOps_OnItemChanged";
  sourceClientId: number;
  ref: system_RpcObjectReference;
  index: number;
  oldCount: number;
  newCount: number;
}

export interface IListEventOps_OnItemChanged_Response
{
  rpcMethod: "IListEventOps_OnItemChanged";
  sourceClientId: number;
  targetClientId: number;
}

export interface IObjectEventOps_InvokeEvent_Request<T>
{
  rpcMethod: "IObjectEventOps_InvokeEvent";
  sourceClientId: number;
  ref: system_RpcObjectReference;
  eventId: number;
  arguments: T[];
}

export interface IObjectEventOps_InvokeEvent_Response
{
  rpcMethod: "IObjectEventOps_InvokeEvent";
  sourceClientId: number;
  targetClientId: number;
  response: RpcEventExceptionMap;
}

export type Request<T> =
  | IListOps_EnumCreate_Request
  | IListOps_EnumNext_Request
  | IListOps_EnumGetCurrent_Request
  | IListOps_ListGetCount_Request
  | IListOps_ListGet_Request
  | IListOps_ListSet_Request<T>
  | IListOps_ListAdd_Request<T>
  | IListOps_ListInsert_Request<T>
  | IListOps_ListRemoveAt_Request
  | IListOps_ListClear_Request
  | IListOps_ListContains_Request<T>
  | IListOps_ListIndexOf_Request<T>
  | IListOps_DictGetCount_Request
  | IListOps_DictGet_Request<T>
  | IListOps_DictSet_Request<T>
  | IListOps_DictRemove_Request<T>
  | IListOps_DictClear_Request
  | IListOps_DictContainsKey_Request<T>
  | IListOps_DictGetKeys_Request
  | IListOps_DictGetValues_Request
  | IObjectOps_InvokeMethod_Request<T>
  | IObjectOps_EndInvokeMethod_Request
  | IObjectOps_ObjectHold_Request
  | IObjectOps_RegisterService_Request
  | IListEventOps_OnItemChanged_Request
  | IObjectEventOps_InvokeEvent_Request<T>
  ;

export type Response<T> =
  | IListOps_EnumCreate_Response
  | IListOps_EnumNext_Response
  | IListOps_EnumGetCurrent_Response<T>
  | IListOps_ListGetCount_Response
  | IListOps_ListGet_Response<T>
  | IListOps_ListSet_Response
  | IListOps_ListAdd_Response
  | IListOps_ListInsert_Response
  | IListOps_ListRemoveAt_Response
  | IListOps_ListClear_Response
  | IListOps_ListContains_Response
  | IListOps_ListIndexOf_Response
  | IListOps_DictGetCount_Response
  | IListOps_DictGet_Response<T>
  | IListOps_DictSet_Response
  | IListOps_DictRemove_Response
  | IListOps_DictClear_Response
  | IListOps_DictContainsKey_Response
  | IListOps_DictGetKeys_Response
  | IListOps_DictGetValues_Response
  | IObjectOps_InvokeMethod_Response<T>
  | IObjectOps_EndInvokeMethod_Response
  | IObjectOps_ObjectHold_Response
  | IObjectOps_RegisterService_Response
  | IListEventOps_OnItemChanged_Response
  | IObjectEventOps_InvokeEvent_Response
  ;
