/*
Mac移植版本的头文件
移植自Windows版本，替换Windows API为POSIX API

要求:
  clang++
  C++20
  POSIX API
  默认char为unsigned (-funsigned-char)
*/

#ifndef MAIN_MAC_H
#define MAIN_MAC_H

// 设置unsigned char为默认
#ifndef _CHAR_UNSIGNED
#define _CHAR_UNSIGNED
#endif

#include <bit>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <filesystem>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <algorithm>

#define DEBUG_INFO __FUNCTION__, __FILE__, __LINE__

constexpr char PROGRAM_NAME[] = "LuaJIT Decompiler v2 (Mac M系列)";
constexpr uint64_t DOUBLE_SIGN = 0x8000000000000000;
constexpr uint64_t DOUBLE_EXPONENT = 0x7FF0000000000000;
constexpr uint64_t DOUBLE_FRACTION = 0x000FFFFFFFFFFFFF;
constexpr uint64_t DOUBLE_SPECIAL = DOUBLE_EXPONENT;
constexpr uint64_t DOUBLE_NEGATIVE_ZERO = DOUBLE_SIGN;

// Mac兼容的类型定义
using HANDLE = int;
constexpr HANDLE INVALID_HANDLE_VALUE = -1;

// Mac兼容的函数声明
void print(const std::string& message);
void print_progress_bar(const double& progress = 0, const double& total = 100);
void erase_progress_bar();
void assert_mac(const bool& assertion, const std::string& message, const std::string& filePath, const std::string& function, const std::string& source, const uint32_t& line);
std::string byte_to_string(const uint8_t& byte);

// 重定义assert为Mac版本
#ifdef assert
#undef assert
#endif
#define assert(assertion, message, filePath, ...) \
    assert_mac(assertion, message, filePath, __VA_ARGS__)

// 前向声明
class Bytecode;
class Ast;
class Lua;

// 包含子模块头文件
#include "bytecode/bytecode.h"
#include "ast/ast.h"
#include "lua/lua.h"

#endif // MAIN_MAC_H
