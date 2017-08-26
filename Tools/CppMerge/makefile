.PHONY: all clean pre-build
.DEFAULT_GOAL := all

CPP_COMPILE_OPTIONS=-I ../../Import -I ../../Release
include $(VCPROOT)/vl/makefile-cpp

pre-build:
	if ! [ -d ./Bin ]; then mkdir ./Bin; fi
	if ! [ -d ./Obj ]; then mkdir ./Obj; fi
	if ! [ -d ./Coverage ]; then mkdir ./Coverage; fi

clean:
	if [ -d ./Bin ]; then rm -r ./Bin; fi
	if [ -d ./Obj ]; then rm -r ./Obj; fi
	if [ -d ./Coverage ]; then rm -r ./Coverage; fi

all:pre-build ./Bin/CppMerge

./Bin/CppMerge:./Obj/Vlpp.o ./Obj/VlppWorkflow.o ./Obj/VlppWorkflowCompiler.o ./Obj/Main.o
	$(CPP_LINK)

./Obj/Vlpp.o: ../../Import/Vlpp.cpp ../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/VlppWorkflow.o: ../../Release/VlppWorkflow.cpp ../../Release/VlppWorkflow.h ../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/VlppWorkflowCompiler.o: ../../Release/VlppWorkflowCompiler.cpp ../../Release/VlppWorkflowCompiler.h ../../Release/VlppWorkflow.h ../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/Main.o: CppMerge/Main.cpp ../../Release/VlppWorkflowCompiler.h ../../Release/VlppWorkflow.h ../../Import/Vlpp.h
	$(CPP_COMPILE)
