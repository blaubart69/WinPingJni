###################################################################################################
##
##               
##
##
##  FILE:        makefile
##
##  PURPOSE:     make binaries (cl)
##
##
##
###################################################################################################

MINOSVER = 0x0400
OSDEFINE = /DWIN /D_WINNT /D_WIN32_WINNT=$(MINOSVER)

##OUT      = out\windows\$(Platform)
OUT      = out\windows\AMD64
CC       = cl.exe /nologo /W4 $(OSDEFINE) /GF /Ox /O2 /Ob2 /MD /LD
##CC     = cl.exe /nologo /W4 $(OSDEFINE) /Zi /GS- /MD /LD
LK       = cl.exe /nologo /MD /LD
##LK     = cl.exe /nologo /MD /Zi /LD
LOPT     = /NODEFAULTLIB /SUBSYSTEM:CONSOLE /RELEASE
LIBS     = kernel32.lib msvcrt.lib libucrt.lib libvcruntime.lib vcruntime.lib
##msvcrt.lib
##/NODEFAULTLIB

OBJS = $(OUT)\WinPingJni.obj

all: $(OUT)\WinPingJni.dll

clean:
##    -del $(OUT)\iserver.exe
##    -del $(OUT)\iserver.map
##    -del $(OBJS)
##    -del $(OUT)\*.bin
##    -del $(OUT)\*.pdb
##    -del *.pdb

$(OUT)\WinPingJni.dll: $(OUT) $(OBJS) makefile
    $(LK) /Fe$@ /Fm$(OUT)\WinPingJni.map $(OBJS) $(LIBS) /link $(LOPT)

$(OUT):
    if not exist $(OUT)\. mkdir $(OUT)

$(OUT)\WinPingJni.obj: WinPingJni.c makefile at_spindi_WinPing.h
    $(CC) /c /Fo$@ WinPingJni.c

## -=EOF=-
