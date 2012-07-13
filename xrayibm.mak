# File: xrayibm.mak
# Note: IBM C 3.0 make file

EXE=XRAY
DLL=XRAYDLL

CC=    icc -q -Ss -W1 -O
CCDLL= $(CC) -Ge- /Gm+
RC=rc -r

LIBSEXE= $(DLL).LIB

HEADERS=xrayrc.h

.all: $(DLL).dll $(EXE).exe

$(EXE).exe: xray.obj xray.res $(DLL).lib
        $(CC) xray.obj $(LIBSEXE) /b"/pm:pm /ex:1"
        rc -x xray.res $(EXE).exe

xray.obj: xray.c $(HEADERS)
    $(CC) -c $*.c

xray.res: xray.rc xrayrc.h xray.ico csb4.bmp
    $(RC) $*.rc   $*.RES

$(DLL).lib: $(DLL).dll xraydll.def
        implib $(DLL).lib $(DLL).dll


$(DLL).dll:  xraydll.obj
        $(CCDLL) xraydll.obj /Fe$(DLL).dll xraydll.def /b"/ex:1"

xraydll.obj: xraydll.c $(HEADERS)
    $(CCDLL) -c $*.c
