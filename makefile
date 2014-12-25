BUILDPLATFORM = ${shell uname}


ifeq ($(BUILDPLATFORM), Linux)
	BIN  = mokoi-package-tool

	COMPILER_FLAGSPP = -std=c++0x -DLINUX -g
	COMPILER_LIBS =

else
	BIN  = mokoi-package-tool.exe

	COMPILER_FLAGSPP = -std=c++0x -D__WINDOWS__ -g -m32
	COMPILER_LIBS = -lWs2_32 -mconsole -lmingw32 -m32 -s

endif





CPP  = g++
CC   = gcc
RM = rm -f



COMPILER_FLAGS += $(CFLAGS)
COMPILER_FLAGSPP += $(CPPFLAGS)
COMPILER_LIBS += $(LDFLAGS)

OBJ = objects/main.o objects/elix_endian.o objects/elix_file.o  objects/mokoi_patch.o objects/mokoi_resource.o objects/mokoi_game.o objects/mokoi_package.o

.PHONY: all  clean clean-custom

all:  $(BIN)


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	@$(CPP) $(OBJ) -o $(BIN) $(COMPILER_LIBS)

objects/%.o : src/%.cpp
	@$(CPP) $(COMPILER_FLAGSPP) -o $@ -c $<

