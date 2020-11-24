CC=gcc-9
CXX=g++-9
CFLAGS=-O3

CSRC = readmtx.c mmio.c coo2csc.c
CPPSRC = main.cpp

BUILD_PATH = build
CPPOBJ := $(CPPSRC:%.cpp=%.o)
COBJ := $(CSRC:%.c=%.o)
OBJ := $(COBJ) $(CPPOBJ)
BUILTOBJ := $(addprefix $(BUILD_PATH)/,$(OBJ))

all: main

main: $(BUILTOBJ)
	$(CXX) $(CPPFLAGS) -o $@ $(BUILTOBJ) $(LDLIBS)

$(BUILD_PATH)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_PATH)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f main
