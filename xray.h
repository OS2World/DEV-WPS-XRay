/**
File:   xray.h
Notes:  for xray.c and xraydll.c
**/

#ifdef __BORLANDC__
  #define EXPORT _export
#else
  #define EXPORT _Export
#endif
                    // global data - to be shared between dll and exe
typedef struct {
    CHAR winInfo[300];          // window info (title, class, id, etc)
    BOOL floatOnTop;            // float on top of desktop
    ATOM wmu_UpdateDisplay;     // msg atom to update our window info
    CHAR szUpdateDisplay[40];   // unique string identifier for system atom
    ATOM wmu_FloatToTop;        // float our window to top of others
    CHAR szFloatToTop[40];      // unique string identifier for system atom
} GLOBALS;

BOOL EXPENTRY XrayInit(HWND);
BOOL EXPENTRY XrayKill(void);
GLOBALS *EXPENTRY XraySetGlobals(void);
