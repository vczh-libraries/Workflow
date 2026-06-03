import type { KnownTypeSchema, UnknownTypeSchema } from "../DataSchema64/Serialization_PrimitiveTypes";

export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [
  5,
  6,
  10,
  12,
  1.5,
  1.75,
  2.75,
  2.875,
  "Hi",
  "Hi!",
  true,
  false,
  2,
  2,
  {"x":3,"y":7},
  {"x":13,"y":27}
];
