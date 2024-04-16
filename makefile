# VC6 makefile

BIN_DIR = bin
INCLUDE_DIR = include
OBJ_DIR = obj
RC_DIR = rc
SRC_DIR = src
DEF_DIR = def

RC_FILE = $(RC_DIR)\main.rc
COMMON_DEF_FILE = $(DEF_DIR)\Common.def
DACOM_DEF_FILE = $(DEF_DIR)\Dacom.def

RES_FILE = $(OBJ_DIR)\main.RES
OBJ_FILES = $(OBJ_DIR)\debug.obj $(OBJ_DIR)\internal.obj $(OBJ_DIR)\main.obj $(OBJ_DIR)\utils.obj
COMMON_LIB_FILE = $(OBJ_DIR)\Common.lib
DACOM_LIB_FILE = $(OBJ_DIR)\Dacom.lib
LIB_FILES = $(COMMON_LIB_FILE) $(DACOM_LIB_FILE)

OUTPUT_FILE = $(BIN_DIR)\FLPatch.dll

CPP_FLAGS = /c /GX /O2 /nologo /W3 /WX /LD /MD
LD_FLAGS = /DLL /FILEALIGN:512 /NOLOGO /RELEASE
LIB_FLAGS = /NOLOGO /MACHINE:IX86

$(OUTPUT_FILE): $(OBJ_FILES) $(RES_FILE) $(LIB_FILES) $(BIN_DIR)
    link $(OBJ_FILES) $(LIB_FILES) $(RES_FILE) $(LD_FLAGS) /OUT:$(OUTPUT_FILE)

{$(SRC_DIR)}.cpp{$(OBJ_DIR)}.obj::
    $(CPP) $(CPP_FLAGS) $< -I$(INCLUDE_DIR) /Fo./$(OBJ_DIR)/

$(OBJ_FILES): makefile

$(OBJ_DIR)\main.obj:$(SRC_DIR)\main.cpp $(INCLUDE_DIR)\utils.h $(INCLUDE_DIR)\internal.h $(INCLUDE_DIR)\Common.h $(INCLUDE_DIR)\Dacom.h $(INCLUDE_DIR)\debug.h
$(OBJ_DIR)\internal.obj: $(SRC_DIR)\internal.cpp $(INCLUDE_DIR)\internal.h $(INCLUDE_DIR)\Dacom.h
$(OBJ_DIR)\utils.obj: $(SRC_DIR)\utils.cpp $(INCLUDE_DIR)\utils.h
$(OBJ_DIR)\debug.obj: $(SRC_DIR)\debug.cpp $(INCLUDE_DIR)\debug.h $(INCLUDE_DIR)\utils.h $(INCLUDE_DIR)\Dacom.h

$(RES_FILE): $(RC_FILE) $(OBJ_DIR) makefile
    rc /fo $(RES_FILE) $(RC_FILE)

$(COMMON_LIB_FILE): $(COMMON_DEF_FILE) makefile
    lib $(LIB_FLAGS) /def:$(COMMON_DEF_FILE) /name:COMMON /out:$(COMMON_LIB_FILE)

$(DACOM_LIB_FILE): $(DACOM_DEF_FILE) makefile
    lib $(LIB_FLAGS) /def:$(DACOM_DEF_FILE) /name:DACOM /out:$(DACOM_LIB_FILE)

$(OBJ_DIR):
    if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
    if not exist $(BIN_DIR) mkdir $(BIN_DIR)

clean:
    del $(BIN_DIR)\*.dll $(OBJ_DIR)\*.obj $(OBJ_DIR)\*.RES $(OBJ_DIR)\*.lib
