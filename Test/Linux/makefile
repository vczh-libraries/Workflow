# This file is generated from "./makefile.debug-clang.makegen" by Vczh Makefile Generator

# Config
.PHONY : all clean

# Targets
bin_TARGET = ././Bin/
obj_TARGET = ././Obj/

# Folders
HOST_DIR = ./../UnitTest/UnitTest/
HOST_cpp = $(HOST_DIR)Main.cpp
UNITTEST_DIR = ./../Source/
UNITTEST_cpp = $(wildcard $(UNITTEST_DIR)*.cpp)
VLPP_DIR = ./../../Source/../Import/
VLPP_cpp = $(wildcard $(VLPP_DIR)*.cpp)
VLPP_h = $(wildcard $(VLPP_DIR)*.h)
WORKFLOW_ANALYZER_DIR = ./../../Source/./Analyzer/
WORKFLOW_ANALYZER_cpp = $(wildcard $(WORKFLOW_ANALYZER_DIR)*.cpp)
WORKFLOW_ANALYZER_h = $(wildcard $(WORKFLOW_ANALYZER_DIR)*.h)
WORKFLOW_BASIC_DIR = ./../../Source/./
WORKFLOW_BASIC_h = $(wildcard $(WORKFLOW_BASIC_DIR)*.h)
WORKFLOW_EXPRESSION_DIR = ./../../Source/./Expression/
WORKFLOW_EXPRESSION_cpp = $(wildcard $(WORKFLOW_EXPRESSION_DIR)*.cpp)
WORKFLOW_EXPRESSION_h = $(wildcard $(WORKFLOW_EXPRESSION_DIR)*.h)
WORKFLOW_RUNTIME_DIR = ./../../Source/./Runtime/
WORKFLOW_RUNTIME_cpp = $(wildcard $(WORKFLOW_RUNTIME_DIR)*.cpp)
WORKFLOW_RUNTIME_h = $(wildcard $(WORKFLOW_RUNTIME_DIR)*.h)

# Output Categories
HOST_o = $(patsubst $(HOST_DIR)%.cpp, $(obj_TARGET)%.o, $(HOST_cpp))
UNITTEST_o = $(patsubst $(UNITTEST_DIR)%.cpp, $(obj_TARGET)%.o, $(UNITTEST_cpp))
VLPP_o = $(patsubst $(VLPP_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_cpp))
WORKFLOW_ANALYZER_o = $(patsubst $(WORKFLOW_ANALYZER_DIR)%.cpp, $(obj_TARGET)%.o, $(WORKFLOW_ANALYZER_cpp))
WORKFLOW_EXPRESSION_o = $(patsubst $(WORKFLOW_EXPRESSION_DIR)%.cpp, $(obj_TARGET)%.o, $(WORKFLOW_EXPRESSION_cpp))
WORKFLOW_RUNTIME_o = $(patsubst $(WORKFLOW_RUNTIME_DIR)%.cpp, $(obj_TARGET)%.o, $(WORKFLOW_RUNTIME_cpp))

# All
ALL_o = $(VLPP_o) $(WORKFLOW_ANALYZER_o) $(WORKFLOW_EXPRESSION_o) $(WORKFLOW_RUNTIME_o) $(HOST_o) $(UNITTEST_o)
all : $(ALL_o)
	clang++ -std=c++14 -pthread -g -o $(bin_TARGET)UnitTest  $(ALL_o)

# Dependencies
# $(WORKFLOW_BASIC_h) : $(VLPP_h)
# $(WORKFLOW_EXPRESSION_h) : $(WORKFLOW_BASIC_h)
# $(WORKFLOW_RUNTIME_h) : $(WORKFLOW_BASIC_h)
# $(WORKFLOW_ANALYZER_h) : $(WORKFLOW_EXPRESSION_h) $(WORKFLOW_RUNTIME_h)
# $(WORKFLOW_ANALYZER_cpp) : $(WORKFLOW_ANALYZER_h)
# $(WORKFLOW_EXPRESSION_cpp) : $(WORKFLOW_EXPRESSION_h)
# $(WORKFLOW_RUNTIME_cpp) : $(WORKFLOW_RUNTIME_h)
# : $(VLPP_h)
# : $(VLPP_h)
# $(UNITTEST_cpp) :
# :

# Rules
$(VLPP_o) : $(obj_TARGET)%.o : $(VLPP_DIR)%.cpp
	clang++ -std=c++14 -g -o $@ -c $<
$(WORKFLOW_ANALYZER_o) : $(obj_TARGET)%.o : $(WORKFLOW_ANALYZER_DIR)%.cpp $(VLPP_h) $(WORKFLOW_ANALYZER_h) $(WORKFLOW_BASIC_h) $(WORKFLOW_EXPRESSION_h) $(WORKFLOW_RUNTIME_h)
	clang++ -std=c++14 -g -o $@ -c $<
$(WORKFLOW_EXPRESSION_o) : $(obj_TARGET)%.o : $(WORKFLOW_EXPRESSION_DIR)%.cpp $(VLPP_h) $(WORKFLOW_BASIC_h) $(WORKFLOW_EXPRESSION_h)
	clang++ -std=c++14 -g -o $@ -c $<
$(WORKFLOW_RUNTIME_o) : $(obj_TARGET)%.o : $(WORKFLOW_RUNTIME_DIR)%.cpp $(VLPP_h) $(WORKFLOW_BASIC_h) $(WORKFLOW_RUNTIME_h)
	clang++ -std=c++14 -g -o $@ -c $<
$(HOST_o) : $(obj_TARGET)%.o : $(HOST_DIR)%.cpp
	clang++ -std=c++14 -g -o $@ -c $<
$(UNITTEST_o) : $(obj_TARGET)%.o : $(UNITTEST_DIR)%.cpp
	clang++ -std=c++14 -g -o $@ -c $<

# Clean
clean:
	rm $(bin_TARGET)* -rf
	rm $(obj_TARGET)* -rf
