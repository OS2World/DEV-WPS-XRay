# File: xraybor.mak
# Note: borland 2.0 make file

EXE=XRAY
DLL=XRAYDLL
BORLIB=E:\C\BCOS2\LIB\  # need this comment for last '\'
LIB=$(BORLIB)

CC=    bcc -c -I$(INCLUDE) -Ot
LINK=   tlink -Toe -aa -c -B:0x10000 -Oc -L$(LIB) $(BORLIB)c02.obj
LINKDLL=tlink -Tod -aa -c -Oc -L$(LIB) $(BORLIB)c02d.obj
RC=brcc -r -i $(INCLUDE)

LIBSEXE= $(DLL).LIB + c2 + os2
LIBSDLL= c2mt + os2

HEADERS=xrayrc.h

.c.obj:
     $(CC) $*.c

$(EXE).exe : xray.obj xray.res $(DLL).LIB
        $(LINK) xray, $@, NUL, $(LIBSEXE);
        rc -x xray.res $(EXE).exe

xray.obj : xray.c $(HEADERS)

xray.res: xray.rc xrayrc.h xray.ico csb4.bmp
    $(RC) $*.rc


$(DLL).lib: $(DLL).dll xraydll.def
        implib $(DLL).lib $(DLL).dll

$(DLL).dll:  xraydll.obj xraydll.def
        $(LINKDLL) xraydll.obj, $(DLL).dll, NUL, $(LIBSDLL), xraydll.def;

xraydll.obj:    xraydll.c $(HEADERS)
