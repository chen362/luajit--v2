# Makefile for LuaJIT Decompiler v2 - Mac M系列芯片版本
# 移植自Windows版本

CXX = clang++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -funsigned-char -I.
TARGET = luajit-decompiler-v2-mac
SRCDIR = .
OBJDIR = obj
OUTDIR = output

# 源文件
SOURCES = main_mac.cpp \
          ast/ast.cpp \
          bytecode/bytecode.cpp \
          bytecode/prototype.cpp \
          lua/lua.cpp

# 对象文件
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# 创建目录
$(shell mkdir -p $(OBJDIR)/ast $(OBJDIR)/bytecode $(OBJDIR)/lua $(OUTDIR))

# 默认目标
all: $(TARGET)

# 链接
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)
	@echo "编译完成! 可执行文件: $(TARGET)"

# 编译规则
$(OBJDIR)/%.o: %.cpp
	@echo "编译: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "清理完成"

# 安装
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	@echo "安装完成"

# 测试
test: $(TARGET)
	@echo "测试反编译器..."
	./$(TARGET) --help

# 调试版本
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

.PHONY: all clean install test debug
