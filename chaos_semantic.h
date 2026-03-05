#import "./chaos_parser.h"
#include <cassert>

typedef enum {
  PRIM_I8,
  PRIM_I16,
  PRIM_I32,
  PRIM_I64,
  PRIM_U8,
  PRIM_U16,
  PRIM_U32,
  PRIM_U64,
  PRIM_F32,
  PRIM_F64,
  PRIM_BOOL,
  PRIM_VOID,
  PRIM_UNKNOWN
} Chaos_Primitive_Type;

typedef struct Chaos_Type {
  enum {
    TYPE_PRIMITIVE,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_POINTER,
    TYPE_STRUCT
  } kind;

  union {
    Chaos_Primitive_Type primitive;
    struct {
      Chaos_Type *element;
      size_t size;
    } array;
    struct {
      std::vector<Chaos_Type *> params;
      Chaos_Type *ret;
    } function;
    struct {
      Chaos_Type *pointee;
    } pointer;
    struct {
      std::string_view name;
      std::vector<std::pair<std::string, Chaos_Type *>> fields;
    } structure;
  } data;

  bool is_numeric() const {
    if (kind != TYPE_PRIMITIVE)
      return false;
    switch (data.primitive) {
    case PRIM_I8:
    case PRIM_I16:
    case PRIM_I32:
    case PRIM_I64:
    case PRIM_U8:
    case PRIM_U16:
    case PRIM_U32:
    case PRIM_U64:
    case PRIM_F32:
    case PRIM_F64:
      return true;
    default:
      return false;
    }
  }

  bool is_integer() const {
    if (kind != TYPE_PRIMITIVE)
      return false;
    switch (data.primitive) {
    case PRIM_I8:
    case PRIM_I16:
    case PRIM_I32:
    case PRIM_I64:
    case PRIM_U8:
    case PRIM_U16:
    case PRIM_U32:
    case PRIM_U64:
      return true;
    default:
      return false;
    }
  }

  bool is_float() const {
    return kind == TYPE_PRIMITIVE &&
           (data.primitive == PRIM_F32 || data.primitive == PRIM_F64);
  }

  size_t size_bytes() const { assert(0 && "NOT IMPLEMENTED"); }
} Chaos_Type;
