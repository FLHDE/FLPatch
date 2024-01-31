# VC6 makefile

BIN_DIR = bin
INCLUDE_DIR = include
OBJ_DIR = obj
RC_DIR = rc
SRC_DIR = src
DEF_DIR = def

RC_FILE = $(RC_DIR)\main.rc
CPP_FILES = $(SRC_DIR)\*.cpp
COMMON_DEF_FILE = $(DEF_DIR)\Common.def
DACOM_DEF_FILE = $(DEF_DIR)\Dacom.def

RES_FILE = $(OBJ_DIR)\main.RES
OBJ_FILES = $(OBJ_DIR)\*.obj
COMMON_LIB_FILE = $(OBJ_DIR)\Common.lib
DACOM_LIB_FILE = $(OBJ_DIR)\Dacom.lib
LIB_FILES = $(COMMON_LIB_FILE) $(DACOM_LIB_FILE)

OUTPUT_FILE = $(BIN_DIR)\FLPatch.dll

CXX_FLAGS = /c /GX /O2 /nologo /W3 /WX /LD /MD
LD_FLAGS = /DLL /FILEALIGN:512 /NOLOGO /RELEASE
LIB_FLAGS = /NOLOGO /MACHINE:IX86

$(OUTPUT_FILE): $(RES_FILE) $(LIB_FILES) $(OBJ_FILES) $(BIN_DIR)
    link $(OBJ_FILES) $(LIB_FILES) $(RES_FILE) $(LD_FLAGS) /OUT:$(OUTPUT_FILE)

{$(SRC_DIR)}.cpp{$(OBJ_DIR)}.obj::
    $(CPP) $(CXX_FLAGS) $< -I$(INCLUDE_DIR) /Fo./$(OBJ_DIR)/

$(RES_FILE): $(RC_FILE) $(OBJ_DIR)
    rc /fo $(RES_FILE) $(RC_FILE)

$(COMMON_LIB_FILE): $(COMMON_DEF_FILE)
    lib $(LIB_FLAGS) /def:$(COMMON_DEF_FILE) /name:COMMON /out:$(COMMON_LIB_FILE)

$(DACOM_LIB_FILE): $(DACOM_DEF_FILE)
    lib $(LIB_FLAGS) /def:$(DACOM_DEF_FILE) /name:DACOM /out:$(DACOM_LIB_FILE)

$(OBJ_DIR):
    if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
    if not exist $(BIN_DIR) mkdir $(BIN_DIR)

# Dependencies
$(SRC_DIR)\main.cpp: $(INCLUDE_DIR)\utils.h $(INCLUDE_DIR)\Common.h $(INCLUDE_DIR)\internal.h $(INCLUDE_DIR)\Dacom.h $(INCLUDE_DIR)\debug.h
$(SRC_DIR)\internal.cpp: $(INCLUDE_DIR)\internal.h
$(SRC_DIR)\utils.cpp: $(INCLUDE_DIR)\utils.h
$(SRC_DIR)\debug.cpp: $(INCLUDE_DIR)\debug.h $(INCLUDE_DIR)\utils.h

clean:
	del $(BIN_DIR)\*.dll $(OBJ_DIR)\*.obj $(OBJ_DIR)\*.RES $(OBJ_DIR)\*.lib
