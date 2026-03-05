#define CHAOS_LEXER_IMPLEMENTATION
#include <iostream>
#include <string>
#include <string_view>

#import "./chaos_parser.h"

void print_ast(Chaos_AST *node, int indent = 0) {
  if (!node)
    return;
  std::string pad(indent, ' ');

  switch (node->kind) {
  case AST_INT:
    std::cout << pad << "Int: " << node->literal << "\n";
    break;
  case AST_FLOAT:
    std::cout << pad << "Float: " << node->literal << "\n";
    break;
  case AST_IDENT:
    std::cout << pad << "Ident: " << node->ident << "\n";
    break;
  case AST_BINARY:
    std::cout << pad << "Binary Op: " << node->binary.op << "\n";
    print_ast(node->binary.l, indent + 2);
    print_ast(node->binary.r, indent + 2);
    break;
  case AST_UNARY:
    std::cout << pad << "Unary Op: " << node->unary.op << "\n";
    print_ast(node->unary.expr, indent + 2);
    break;
  case AST_IF:
    std::cout << pad << "If Node\n";
    std::cout << pad << "  Cond:\n";
    print_ast(node->if_stmt.cond, indent + 4);
    std::cout << pad << "  Then:\n";
    print_ast(node->if_stmt.then_br, indent + 4);
    if (node->if_stmt.else_br) {
      std::cout << pad << "  Else:\n";
      print_ast(node->if_stmt.else_br, indent + 4);
    }
    break;
  case AST_WHILE:
    std::cout << pad << "While Node\n";
    std::cout << pad << "  Cond:\n";
    print_ast(node->while_stmt.cond, indent + 4);
    std::cout << pad << "  Body:\n";
    print_ast(node->while_stmt.then, indent + 4);
    break;
  case AST_VAR_DECL:
    std::cout << pad << "VarDecl: " << node->var_decl.name << " : "
              << node->var_decl.type << "\n";
    print_ast(node->var_decl.init, indent + 2);
    break;

  case AST_ASIGN:
    std::cout << pad << "Assign\n";
    print_ast(node->assign.target, indent + 2);
    print_ast(node->assign.value, indent + 2);
    break;
  case AST_RETURN:
    std::cout << pad << "Return\n";
    print_ast(node->unary.expr, indent + 2);
    break;
  case AST_FUNCTION:
    std::cout << pad << "Function: " << node->function.name << "\n";
    std::cout << pad << "  Params:\n";
    for (auto &[name, type] : node->function.params) {
      std::cout << pad << "    " << name << ": " << type << "\n";
    }
    std::cout << pad << "  Return Type: " << node->function.return_type << "\n";
    std::cout << pad << "  Body:\n";
    print_ast(node->function.body, indent + 4);
    break;
  case AST_PROGRAM:
  case AST_BLOCK:
    std::cout << pad << (node->kind == AST_PROGRAM ? "Program" : "Block")
              << "\n";
    for (Chaos_AST *stmt : node->block.statements) {
      print_ast(stmt, indent + 2);
    }
    break;
  default:
    std::cout << pad << "Other AST Node\n";
    break;
  }
}

int main() {
  std::string source = R"(
fn add(x: int, y: int): int {
  var z: int = x+y;
  return z;
}

var x: int = 5;
x = x + 1;

if (x > 10){
  var b: int = 3;
}
  )";

  Chaos_Lexer lexer = {0};
  chaos_lexer_run(&lexer, source);

  Chaos_Parser parser(&lexer.tokens);
  Chaos_AST *program = parse_program(&parser);

  print_ast(program);

  return 0;
}
