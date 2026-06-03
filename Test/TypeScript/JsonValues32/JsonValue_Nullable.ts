import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema32/Serialization_Nullable";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  "abc",
  null,
  "[abc][null]",
  null,
  {"$":"system::RpcObjectReference","clientId":2,"objectId":0,"typeId":2},
  "def",
  "[null][def]"
];
