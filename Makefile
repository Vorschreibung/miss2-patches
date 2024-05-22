-include config.mk

INPUT       = miss2.dat
OUTPUT      = miss2.exe
LDS         = miss2.lds

LDFLAGS     = -Wl,--file-alignment=0x1000 -Wl,--subsystem=windows -Wl,--disable-nxcompat -Wl,--disable-reloc-section -Wl,--enable-stdcall-fixup -static
ASFLAGS     = -Iinc
NFLAGS      = -Iinc -f elf
CFLAGS      = -Iinc -O2 -march=pentium4 -Wall -masm=intel
CXXFLAGS    = -Iinc -O2 -march=pentium4 -Wall -masm=intel

LIBS        = -lgdi32 -lshlwapi

OBJS        = \
				sym.o \
				rsrc.o \
				src/winmain.o \

CC          = i686-w64-mingw32-gcc
CXX         = i686-w64-mingw32-g++
AS          = i686-w64-mingw32-as
STRIP      ?= i686-w64-mingw32-strip
WINDRES    ?= i686-w64-mingw32-windres
PETOOL     ?= petool
NASM       ?= nasm

IAT         = 12 0x1863A4 792

all: $(OUTPUT)

%.o: %.asm
	$(NASM) $(NFLAGS) -o $@ $<

%.o: %.rc
	$(WINDRES) $(WFLAGS) $< $@

src/winmain.o: \
	inc/app.h \
	src/compile.c \
	src/globals.h \
	src/utils.c \
	sym.c

rsrc.o: $(INPUT)
	$(PETOOL) re2obj $< $@

$(OUTPUT): $(OBJS)
	$(CXX) $(LDFLAGS) -T $(LDS) -o "$@" $^ $(LIBS)
	$(PETOOL) setdd "$@" $(IAT) || ($(RM) "$@" && exit 1)
	$(PETOOL) setsc "$@" .p_text 0x60000020 || ($(RM) "$@" && exit 1)
	$(PETOOL) patch "$@" || ($(RM) "$@" && exit 1)
	$(STRIP) -R .patch "$@" || ($(RM) "$@" && exit 1)
	$(PETOOL) dump "$(INPUT)"
	$(PETOOL) dump "$@"

clean:
	$(RM) $(OUTPUT) $(OBJS)
