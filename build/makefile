#define macros
EXECUTABLE_NAME = Decom.exe
DIR_SRC = ..\src
DIR_INCLUDE = ..\include
DIR_BIN = ..\bin
DIR_BIN_X64 = $(DIR_BIN)\x64
DIR_INTERMEDIATE = .\intermediate
DIR_INTERMEDIATE_X64 = $(DIR_INTERMEDIATE)\x64

{$(DIR_SRC)}.cpp{$(DIR_INTERMEDIATE_X64)}.obj ::
	@echo Compiling...
	cl /c /Zi /GS- /GL /W3 /Gy /Gm- /MD /O2 /EHsc /W3 /O2 /GL /favor:INTEL64 /Fo$(DIR_INTERMEDIATE_X64)\ /I$(DIR_INCLUDE) $<

$(EXECUTABLE_NAME) : $(DIR_INTERMEDIATE_X64)\*.obj
	@echo Linking $(EXECUTABLE_NAME)...
	link /LTCG /OPT:ICF /OPT:REF /DEBUG /out:$(DIR_BIN_X64)\$(EXECUTABLE_NAME) $(DIR_INTERMEDIATE_X64)\*.obj

test: $(EXECUTABLE_NAME)

create_dirs:
	@if not exist $(DIR_BIN_X64) mkdir $(DIR_BIN_X64)
	@if not exist $(DIR_INTERMEDIATE_X64) mkdir $(DIR_INTERMEDIATE_X64)

clean:
	@if exist $(DIR_BIN) rmdir /S /Q $(DIR_BIN)
	@if exist $(DIR_INTERMEDIATE) rmdir /S /Q $(DIR_INTERMEDIATE)

all: clean create_dirs test
