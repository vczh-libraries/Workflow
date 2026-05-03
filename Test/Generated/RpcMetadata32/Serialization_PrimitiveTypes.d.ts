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

export type TypeList_Enum =
  | "RpcPrimitiveTest::Season"
  ;

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
  | UnknownType_RpcPrimitiveTest_Point
  | UnknownType_system_RpcObjectReference
  ;

export interface UnknownType_RpcPrimitiveTest_Point extends RpcPrimitiveTest_Point
{
  "$": "RpcPrimitiveTest::Point";
}

export interface UnknownType_system_RpcObjectReference extends system_RpcObjectReference
{
  "$": "system::RpcObjectReference";
}

// below are all known types

export enum RpcPrimitiveTest_Season
{
  Spring = 0,
  Summer = 1,
  Autumn = 2,
  Winter = 3,
}

export interface RpcPrimitiveTest_Point
{
  x: number;
  y: number;
}

export interface system_RpcObjectReference
{
  clientId: number;
  objectId: number;
  typeId: number;
}

