export type PredefinedSchema =
  | ["UInt8", string]
  | ["UInt16", string]
  | ["UInt32", string]
  | ["UInt64", string]
  | ["Int8", string]
  | ["Int16", string]
  | ["Int32", string]
  | ["Int64", string]
  | ["Single", string]
  | ["Double", string]
  | ["Boolean", string]
  | ["Char", string]
  | ["String", string]
  | ["DateTime", string]
  | ["Locale", string]
  ;

export type EnumSchema = never;

export type Schema =
  | null
  | PredefinedSchema
  | ListSchema
  | MapSchema
  | ObservableSchema
  | StructSchema_system_RpcObjectReference
  ;

export interface ListSchema<T extends Schema = Schema>
{
  "$": "list";
  values: T[];
}

export interface MapSchema<K extends Schema = Schema, V extends Schema = Schema>
{
  "$": "map";
  values: Array<[K, V]>;
}

export interface ObservableSchema<T extends Schema = Schema>
{
  "$": "observable-list";
  values: T[];
}

export interface StructSchema_system_RpcObjectReference
{
  "$": "system::RpcObjectReference";
  clientId: ["Int64", string];
  objectId: ["Int64", string];
  typeId: ["Int64", string];
}

