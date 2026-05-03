export type UnknownType_PrimitiveSchema =
  | ["UInt8", number]
  | ["UInt16", number]
  | ["UInt32", number]
  | ["UInt64", number]
  | ["Int8", number]
  | ["Int16", number]
  | ["Int32", number]
  | ["Int64", number]
  | ["Single", number]
  | ["Double", number]
  | ["Char", string]
  | ["DateTime", string]
  | ["Locale", string]
  | null
  | true
  | false
  | string
  ;

export type TypeList_Enum = never;

export type UnknownType_EnumSchema = [TypeList_Enum, number];

export interface UnknownType_List
{
  "$": "list" | "map" | "oblist";
  values: UnknownTypeSchema[];
}

export type UnknownTypeSchema =
  | UnknownType_PrimitiveSchema
  | UnknownType_EnumSchema
  | UnknownType_List
  | UnknownType_system_RpcObjectReference
  ;

export interface UnknownType_system_RpcObjectReference extends system_RpcObjectReference
{
  "$": "system::RpcObjectReference";
}

// below are all known types

export interface system_RpcObjectReference
{
  clientId: number;
  objectId: number;
  typeId: number;
}

