/**
File:   xray.c
Notes:  main interface - uses xrayhook dll
        This sample code provided by CodeSmith Software. Use as you wish.
**/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSPROCESS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "xray.h"
#include "xrayrc.h"


HWND hwndFrame;             // xrays frame
BOOL OnTop = 0;             // if set, then display xray window at top of screen
ULONG cxDesktop,            // width of desktop
      cyDesktop,            // height of desktop
      cyXray;               // height of our xray window
GLOBALS *Global;        // pointer to global data in the dll

MRESULT EXPENTRY AboutDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CHAR version[30];

    switch (msg) {

        case WM_COMMAND:
                    switch (COMMANDMSG(&msg)->cmd) {
                                case DID_OK:
                                case DID_CANCEL:    WinDismissDlg (hwnd, TRUE);
                                                    return 0;
                    }
                    break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY ClientWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CHAR buf[128];
    HPS hps;
    RECTL rcl;
    static HWND hwndMenu;
    static ULONG yOffset = 0;
    POINTL ptlMouse;

    switch (msg) {

        case WM_CREATE:
                    hwndMenu = WinLoadMenu(HWND_OBJECT, 0, IDM_POPUP);
                    if(OnTop)
                        WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_MOVE, TRUE), MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
                    if(Global->floatOnTop)
                        WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_FLOAT, TRUE), MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
                    return 0;

        case WM_CONTEXTMENU:
                    if(hwndMenu != NULLHANDLE) {
                        WinQueryPointerPos(HWND_DESKTOP, &ptlMouse);
                        WinPopupMenu(HWND_DESKTOP, hwnd, hwndMenu, ptlMouse.x, ptlMouse.y, 0,
                                     PU_HCONSTRAIN | PU_VCONSTRAIN | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_KEYBOARD);
                        return 0;
                    }
                    break;

        case WM_COMMAND:
                    switch(COMMANDMSG(&msg)->cmd) {
                        case IDM_CLOSE:
                                    WinSendMsg(hwnd, WM_CLOSE, 0L, 0L);
                                    return 0;
                        case IDM_MOVE:
                                    if(WinSendMsg(hwndMenu, MM_QUERYITEMATTR, MPFROM2SHORT(IDM_MOVE, TRUE), MPFROMSHORT(MIA_CHECKED)) != 0) {
                                        yOffset = 0;
                                        WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_MOVE, TRUE), MPFROM2SHORT(MIA_CHECKED, 0));
                                    } else {
                                        yOffset = cyDesktop - cyXray;
                                        WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_MOVE, TRUE), MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
                                    }
                                    WinSetWindowPos(hwndFrame, 0, 0, yOffset, cxDesktop, cyXray, SWP_MOVE | SWP_SIZE);
                                    return 0;
                        case IDM_FLOAT:
                                    if(WinSendMsg(hwndMenu, MM_QUERYITEMATTR, MPFROM2SHORT(IDM_FLOAT, TRUE), MPFROMSHORT(MIA_CHECKED)) != 0) {
                                        Global->floatOnTop = 0;
                                        WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_FLOAT, TRUE), MPFROM2SHORT(MIA_CHECKED, 0));
                                    } else {
                                        Global->floatOnTop = 1;
                                        WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_FLOAT, TRUE), MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
                                    }
                                    return 0;
                        case IDM_ABOUT:
                                    WinDlgBox(HWND_DESKTOP, hwnd, AboutDlgProc, NULLHANDLE, DLG_ABOUT, NULL);
                                    return 0;
                    }
                    break;

        case WM_PAINT:
                    hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
                    WinQueryWindowRect (hwnd, &rcl);
                    WinDrawText (hps, -1, Global->winInfo, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
                                DT_LEFT | DT_VCENTER | DT_ERASERECT);

                    WinEndPaint(hps);
                    return 0;
        default:                    // if this is our custom float-to-top msg
                    if(msg == Global->wmu_FloatToTop) {
                         WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER);
                         WinInvalidateRect(hwnd, NULL, FALSE);       // refresh window
                         return 0;
                    }
                    break;

    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

INT ProcessArgs(int argc, char **argv)
{
    INT i, gotBadArg = 0;
    CHAR *s;

    for(i=1; i < argc; i++)  {                          // process each argument
        if(argv[i][0] == '/' || argv[i][0] == '-') {    // an option ?
    	    for(s = argv[i]; *s; s++) {
	    	    switch(toupper(*s)) {
                    case 'F':   Global->floatOnTop = 1;
                                break;
                    case 'T':   OnTop = 1;
                                break;
                    case '/':
                    case '-':   break;          // start of next arg

                    default:    gotBadArg = 1;
                }
	        }
        }
    }
    return gotBadArg;
}

VOID APIENTRY ExeTrap()
{
    XrayKill();
    DosExitList(EXLST_EXIT, (PFNEXITLIST)ExeTrap);
}

INT main (INT argc, CHAR **argv) {
    HMQ  hmq ;
    HWND hwndClient;
    QMSG qmsg ;
    HAB habXray;
    ULONG frameFlags = FCF_BORDER | FCF_TASKLIST;
    static CHAR *clientClass = "Xray";
    FONTMETRICS fm;
    ULONG cyBorder;
    HPS hps;

    habXray = WinInitialize(0);
    hmq = WinCreateMsgQueue(habXray, 0);

    DosExitList(EXLST_ADD, (PFNEXITLIST)ExeTrap);   // trap exceptions to ensure hooks released
    Global = XraySetGlobals();                        // initialize global data pointer
    ProcessArgs(argc, argv);
    WinRegisterClass (habXray, clientClass, ClientWndProc, 0L, 0);
    hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE, &frameFlags, clientClass,
                                    "X-Ray", 0L, 0, ID_ICON, &hwndClient);
                        // now set up frame size, based on a small font
    cxDesktop = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
    cyDesktop = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
    cyBorder = WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);
    WinSetPresParam(hwndClient, PP_FONTNAMESIZE, 7, (PVOID)"8.Helv");
    hps = WinGetPS(hwndClient);
    GpiQueryFontMetrics(hps, sizeof(fm), &fm);
    WinReleasePS(hps);
    cyXray = fm.lMaxBaselineExt + cyBorder * 2;
    WinSetWindowPos(hwndFrame, 0, 0, (OnTop) ? cyDesktop - cyXray : 0,
                                  cxDesktop, cyXray, SWP_MOVE | SWP_SIZE);

    XrayInit(hwndClient);                       // enable hooks
    while (WinGetMsg(habXray, &qmsg, NULLHANDLE, 0, 0))
         WinDispatchMsg(habXray, &qmsg);
    XrayKill();                                 // disable hooks
    WinDestroyWindow (hwndFrame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(habXray);
    return 0;
}
