#import "./chaos_parser.h"
#include <cassert>
#include <memory>
#include <string>
#include <variant>
#include <vector>

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

struct Chaos_Type;

struct Array_Data {
  std::shared_ptr<Chaos_Type> element;
  size_t size;
};

struct Function_Data {
  std::vector<std::shared_ptr<Chaos_Type>> params;
  std::shared_ptr<Chaos_Type> ret;
};

struct Struct_Field {
  std::string name;
  std::shared_ptr<Chaos_Type> type;
};

struct Struct_Data {
  std::string name;
  std::vector<Struct_Field> fields;
};

struct Void_Data {};

using Type_Data =
    std::variant<Void_Data, Chaos_Primitive_Type, Array_Data, Function_Data,
                 std::shared_ptr<Chaos_Type>, Struct_Data>;

typedef struct Chaos_Type {
  enum {
    TYPE_PRIMITIVE,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_POINTER,
    TYPE_STRUCT,
    TYPE_VOID
  } kind;

  Type_Data data;

  static std::shared_ptr<Chaos_Type> make_void() {
    auto t = std::make_shared<Chaos_Type>();
    t->kind = TYPE_VOID;
    t->data = Void_Data{};
    return t;
  }

  static std::shared_ptr<Chaos_Type> make_primitive(Chaos_Primitive_Type p) {
    auto t = std::make_shared<Chaos_Type>();
    t->kind = TYPE_PRIMITIVE;
    t->data = p;
    return t;
  }

  static std::shared_ptr<Chaos_Type>
  make_array(std::shared_ptr<Chaos_Type> elem, size_t sz) {
    auto t = std::make_shared<Chaos_Type>();
    t->kind = TYPE_ARRAY;
    t->data = Array_Data{elem, sz};
    return t;
  }

  static std::shared_ptr<Chaos_Type>
  make_pointer(std::shared_ptr<Chaos_Type> pointee) {
    auto t = std::make_shared<Chaos_Type>();
    t->kind = TYPE_POINTER;
    t->data = pointee;
    return t;
  }

  static std::shared_ptr<Chaos_Type>
  make_function(std::vector<std::shared_ptr<Chaos_Type>> params,
                std::shared_ptr<Chaos_Type> ret) {
    auto t = std::make_shared<Chaos_Type>();
    t->kind = TYPE_FUNCTION;
    t->data = Function_Data{std::move(params), ret};
    return t;
  }

  Chaos_Primitive_Type &primitive() {
    return std::get<Chaos_Primitive_Type>(data);
  }
  const Chaos_Primitive_Type &primitive() const {
    return std::get<Chaos_Primitive_Type>(data);
  }

  Array_Data &array() { return std::get<Array_Data>(data); }
  const Array_Data &array() const { return std::get<Array_Data>(data); }

  Function_Data &function() { return std::get<Function_Data>(data); }
  const Function_Data &function() const {
    return std::get<Function_Data>(data);
  }

  std::shared_ptr<Chaos_Type> &pointer() {
    return std::get<std::shared_ptr<Chaos_Type>>(data);
  }
  const std::shared_ptr<Chaos_Type> &pointer() const {
    return std::get<std::shared_ptr<Chaos_Type>>(data);
  }

  Struct_Data &structure() { return std::get<Struct_Data>(data); }
  const Struct_Data &structure() const { return std::get<Struct_Data>(data); }

  bool is_primitive() const { return kind == TYPE_PRIMITIVE; }
  bool is_array() const { return kind == TYPE_ARRAY; }

  bool is_numeric() const {
    if (!is_primitive())
      return false;
    switch (primitive()) {
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
    if (!is_primitive())
      return false;
    switch (primitive()) {
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
    return is_primitive() &&
           (primitive() == PRIM_F32 || primitive() == PRIM_F64);
  }

  template <typename Visitor> auto visit(Visitor &&v) {
    return std::visit(std::forward<Visitor>(v), data);
  }

  template <typename Visitor> auto visit(Visitor &&v) const {
    return std::visit(std::forward<Visitor>(v), data);
  }

  size_t size_bytes() const { assert(0 && "NOT IMPLEMENTED"); }
} Chaos_Type;

typedef enum {
  SYM_VARIABLE,
  SYM_CONSTANT,
  SYM_FUNCTION,
  SYM_PARAMETER,
} Chaos_Symbol_Kind;

typedef struct Chaos_Symbol {
  Chaos_Symbol_Kind kind;
  std::string_view name;
  Chaos_Type type;

  // for Functions
  std::vector<Chaos_Type> param_types;
  Chaos_Type return_type;

  int scope_depth;
  Chaos_Symbol(Chaos_Symbol_Kind k, std::string_view n, Chaos_Type t)
      : kind(k), name(n), type(t), return_type(*Chaos_Type::make_void()),
        scope_depth(0) {}
} Chaos_Symbol;
