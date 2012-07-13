/**
File:   xraydll.c
Notes:  xray hook dll - contains the hook procs
        This sample code provided by CodeSmith Software. Use as you wish.
**/

#define INCL_WIN
#define INCL_DOSMODULEMGR

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xray.h"
#include "xrayrc.h"

BOOL Hooked;            // flag set when hooks are enabled
HMODULE hmXrayDll;      // handle to the dll module
HAB habXray;            // anchor block of our process
HWND hwndXrayClient;    // handle to our client proc in xray.c
HATOMTBL hatomtblSystem;// handle to system atom table
GLOBALS Global;         // global data in the dll
                        // we sent a pointer to it in XrayInit()

// #define TESTHOOK     // define this to test hooks only within our process
                        // or comment out to use hooks system wide

#ifndef TESTHOOK
  HMQ hmqType = NULLHANDLE;     // use NULLHANDLE for system wide hooks
#else
  HMQ hmqType = HMQ_CURRENT;    // use HMQ_CURRENT for current thread only
#endif

                    // ConvertClass takes a predefined class on input (eg "#1")
                    // and converts it into English (eg. "Frame")
VOID ConvertClass(CHAR *class)
{
    INT i;
    static struct {
        CHAR *number;
        CHAR *desc;
    } cl[] = {
         "#1",  "Frame(#1)",
         "#2",  "Combobox(#2)",
         "#3",  "Button(#3)",
         "#4",  "Menu(#4)",
         "#5",  "Static(#5)",
         "#6",  "EntryField(#6)",
         "#7",  "Listbox(#7)",
         "#8",  "ScrollBar(#8)",
         "#9",  "TitleBar(#9)",
         "#10",  "MultilineEntry(#10)",
         "#16", "AppStat(#16)",
         "#17",  "KbdStat(#17)",
         "#18",  "Pecic(#18)",
         "#19",  "DBEkkpopup(#19)",
         "#32",  "SpinButton(#32)",
         "#37",  "Container(#37)",
         "#38",  "Slider(#38)",
         "#39",  "ValueSet(#39)",
         "#40",  "NoteBook(#40)",
         "#41",  "PenFirst(#41)",
         "#44",  "PenLast(#44)",
         "#64",  "MMPMFirst(#64)",
         "#65",  "CircularSlider(#65)",
         "#79",  "MMPMLast(#79)",
         "#32766", "Desktop(#32766)",
         0, 0
    };

    for(i = 0; cl[i].number; i++)
        if(strcmp(cl[i].number, class) == 0) {
            strcpy(class, cl[i].desc);
            break;
        }
}
                // converts any new line characters into a space
VOID ConvertNewlines(CHAR *buf)
{
    while(*buf) {
        if(*buf == '\n' || *buf == '\r')
            *buf = ' ';
        buf++;
    }
}

        // UpdateDisplay takes the input window hwnd, and extracts the info
        // from it (title, class, etc). We then post a msg to our client proc
        // to display this info, stored in a global string variable.
VOID UpdateDisplay(HWND hwnd, HAB habCurrent)
{
    CHAR title[24], class[32], p_class[32], menuId[24] = " ";
    HWND p_hwnd = WinQueryWindow(hwnd, QW_PARENT);
    HPS hps;
    RECTL rcl;
    PID pid, p_pid;
    TID tid, p_tid;

    if(habCurrent == habXray)       // if attempt to look at Xrays windows
        sprintf(Global.winInfo, " Right mouse button for menu.  Atoms used: wmu_UpdateDisplay=%X  wmu_FloatToTop=%X",Global.wmu_UpdateDisplay, Global.wmu_FloatToTop);
    else {
        if(WinQueryWindowText(hwnd, sizeof(title), title) == 0)
            strcpy(title,"(none)");
        else
            ConvertNewlines(title);
        WinQueryClassName(hwnd, sizeof(class), class);
        if(memcmp(class, "#4", 3) == 0)         // if a menu, get selected id
            sprintf(menuId, "ItemID:%X  ", WinSendMsg(hwnd, MM_QUERYSELITEMID, MPFROMSHORT(TRUE), 0));

        ConvertClass(class);
        WinQueryClassName(p_hwnd, sizeof(p_class), p_class);
        ConvertClass(p_class);
        WinQueryWindowProcess(hwnd, &pid, &tid);
        WinQueryWindowProcess(p_hwnd, &p_pid, &p_tid);
        sprintf(Global.winInfo, "TITLE:%s  CLASS:%s  ID:%X %sHwnd:%X  Style:%X  Pid,Tid:%X,%X  PARENT CLASS:%s  Hwnd:%X  Pid,Tid:%X,%X",
                    title, class, WinQueryWindowUShort(hwnd, QWS_ID), menuId, hwnd, WinQueryWindowULong(hwnd,QWL_STYLE),
                    pid, tid, p_class, p_hwnd, p_pid, p_tid);
    }
    WinInvalidateRect(hwndXrayClient, NULL, FALSE);      // refresh our window
}

            // Post message hook. If you want to process the message here,
            // return TRUE. If you want the message to be passed on, return FALSE.

BOOL EXPENTRY XrayHookInput(HAB hab, PQMSG pQmsg, USHORT fs)
{
    static HWND hwndLast;
    static BOOL first = 1;

    switch(pQmsg->msg) {

        case WM_MOUSEMOVE:
                        // When an app is first loaded, it always receives a
                        // WM_MOUSEMOVE, even if the mouse doesn't move. This
                        // allows setting a custom mouse pointer.
                        // Here we just display a startup msg.
                    if(first) {
                        first = 0;
                        sprintf(Global.winInfo, "  XúRAY  Version:%s    Press right mouse button over this window for menu", VERSION);
                        WinInvalidateRect(hwndXrayClient, NULL, FALSE);      // refresh our window
                        break;
                    }
                        // if mouse over a new window, display its info
                    if(pQmsg->hwnd != hwndLast) {
                        hwndLast = pQmsg->hwnd;
                        UpdateDisplay(pQmsg->hwnd, hab);
                    }
                    break;                  // pass this msg on

        default:                    // if this is our custom msg, then mp1 = window handle; mp2 = hab
                                    // to double check this is our msg, match client hwnds and verify params
                    if(pQmsg->msg == Global.wmu_UpdateDisplay && pQmsg->hwnd == hwndXrayClient && WinIsWindow((HAB)pQmsg->mp2, (HWND)pQmsg->mp1) == TRUE) {
                        UpdateDisplay((HWND)pQmsg->mp1, (HAB)pQmsg->mp2);
                        return TRUE;        // msg processed, return TRUE
                    }
                    break;
    }
    return FALSE;                           // msg not processed if FALSE
}

        // Send Message hook. Don't do any processing in here, especially calling
        // WinSendMsg(). Simply post a msg to yourself, for processing either
        // in your client proc, or in the input hook.

VOID EXPENTRY XrayHookSendMsg(HAB habAnchor, PSMHSTRUCT psmh, BOOL fInterTask)
{
    switch(psmh->msg) {
        case WM_MENUSELECT:
                        // When a menu selection is changed (via keyboard), we
                        // want to display the menu id of the new selection.
                        // We simply pass the menu hwnd (mp2) for processing
                        // in the input hook proc
                    WinPostMsg(hwndXrayClient, Global.wmu_UpdateDisplay, psmh->mp2, (MPARAM)habAnchor);
                    return;
        case WM_WINDOWPOSCHANGED:
                        // When any window changes position , we notify our client
                        // proc to move itself to the top of all windows
                    if(Global.floatOnTop)
                        WinPostMsg(hwndXrayClient, Global.wmu_FloatToTop, 0, 0);
                    return;
    }
    return;
}

        // next 3 routines are called from the exe, so we need to export them

GLOBALS * EXPENTRY EXPORT XraySetGlobals(void)
{
    return &Global;   // allow client access to the dlls global data
}

BOOL EXPENTRY EXPORT XrayInit(HWND hwnd)       // enable hooks
{
    hwndXrayClient = hwnd;      // save client hwnd so we can post msgs to it
    if(!Hooked) {
                                // create unique message ids (can't use WM_USER in hooks)
        hatomtblSystem = WinQuerySystemAtomTable();
        srand((UINT)hwnd);
        sprintf(Global.szUpdateDisplay,"XrayUpdateDisplay%d",rand());
        sprintf(Global.szFloatToTop,"XrayFloatToTop%d",rand());
        Global.wmu_UpdateDisplay = WinAddAtom(hatomtblSystem, Global.szUpdateDisplay);
        Global.wmu_FloatToTop = WinAddAtom(hatomtblSystem, Global.szFloatToTop);
        if(DosQueryModuleHandle("xraydll", &hmXrayDll))
            return FALSE;
        habXray = WinQueryAnchorBlock(hwnd);
        WinSetHook(habXray, hmqType, HK_INPUT, (PFN)XrayHookInput, hmXrayDll);
        WinSetHook(habXray, hmqType, HK_SENDMSG, (PFN)XrayHookSendMsg, hmXrayDll);
        Hooked = 1;
    }
    return TRUE;
}

BOOL EXPENTRY EXPORT XrayKill()        // disable hooks
{
    if(Hooked) {
        WinReleaseHook(habXray, hmqType, HK_INPUT, (PFN)XrayHookInput, hmXrayDll);
        WinReleaseHook(habXray, hmqType, HK_SENDMSG, (PFN)XrayHookSendMsg, hmXrayDll);
        WinDeleteAtom(hatomtblSystem, Global.wmu_UpdateDisplay);
        WinDeleteAtom(hatomtblSystem, Global.wmu_FloatToTop);
        Hooked = 0;
    }
    return TRUE;
}
