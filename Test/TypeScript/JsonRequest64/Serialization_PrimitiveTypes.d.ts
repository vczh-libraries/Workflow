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
  "$": "list" | "oblist";
  values: UnknownTypeSchema[];
}

export interface UnknownType_Map
{
  "$": "map";
  values: [UnknownTypeSchema, UnknownTypeSchema][];
}

export type UnknownTypeSchema =
  | UnknownType_PrimitiveSchema
  | UnknownType_EnumSchema
  | UnknownType_List
  | UnknownType_Map
  | UnknownType_test_Point
  | UnknownType_system_RpcObjectReference
  | UnknownType_system_RpcException
  ;

export interface UnknownType_test_Point extends test_Point
{
  "$": "test::Point";
}

export interface UnknownType_system_RpcObjectReference extends system_RpcObjectReference
{
  "$": "system::RpcObjectReference";
}

export interface UnknownType_system_RpcException extends system_RpcException
{
  "$": "system::RpcException";
}

// below are all known types

export enum RpcPrimitiveTest_Season
{
  Spring = 0,
  Summer = 1,
  Autumn = 2,
  Winter = 3,
}

export interface test_Point
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

export interface system_RpcException
{
  message: string;
}

// All enum_type_full_name is omitted because in known type enums are just numbers
export type KnownTypeSchema =
  | number
  | true
  | false
  | string
  | KnownTypeSchema[]
  | [KnownTypeSchema, KnownTypeSchema][]
  | test_Point
  | system_RpcObjectReference
  | system_RpcException
  ;
