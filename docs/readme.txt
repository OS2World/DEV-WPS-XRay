INTRODUCTION

  Xray is an OS/2 programmers utility to display process and window information
  for the window under the mouse pointer. Source demonstrates system hooks.

INSTALLATION

  Files in this release:
                XRAY.EXE        - runtime
                XRAYDLL.DLL     - support file
                SOURCE.ZIP      - source files
                FILE_ID.DIZ     - id file for BBSs
                VENDOR.DOC      - vendor distribution license
                READ.ME         - this file

  Copy the first 2 files to any directory on a local drive. Insure this
  directory is in the LIBPATH statement in CONFIG.SYS or that the current
  directory is indicated (LIBPATH=.). Run Xray.exe.

USING XRAY

  Startup options:

                /F - float on top of all other windows
                /T - display xray window at top of screen (default is bottom)

  Press the right mouse button when over the Xray window for a popup menu of
  options.

  The Xray status line shows window info for the window that is under the mouse
  pointer. When a menu is active, use the up/down cursor keys to display the
  menu ids of each menu item. All numeric values are shown in hex, except for
  predefined classes which are prefixed with a '#' (for example,
  "CLASS: Container(#37)", the 37 is decimal). All info displayed after the word
  "PARENT" refer to the parent window of the window under the mouse pointer.

  When the mouse is over the Xray window, it displays the values of the two
  atoms that it uses for custom messages.

  Xray is freeware, and the source code can be used at your own risk. Primary
  reason for this product is to demonstrate system hooks. For more information,
  see the article "Hooking: TSRs invade PM" in EDM/2 issue Jan or Feb, 1997
  (www.iqpac.com/edm2), or email Michael Shillingford at CodeSmith Software:

    mshill@elkvalley.net                        (via Internet, AOL, BIX)
    TO:internet:mshill@elkvalley.net            (via Compuserve)
    EMS:INTERNET, MBX:mshill@elkvalley.net      (via MCI)
    mshill.elkvalley.net@inet#                  (via GEnie)
    TO:internet"mshill@elkvalley.net            (via Delphi)

ADDITIONAL CREDITS

  Thanks to the following for reviewing the EDM/2 article and source:
  Peter Fitzsimmons, Eric Slaats, Rick Walsh, Jim Read and Achim Hasenmueller.

DISCLAIMER

  This package is provided "as is", without any guarantees or warrantees
  whatsoever. CodeSmith and its employees are not liable or responsible for
  any loss or damage of any kind whatsoever, including, but not limited to,
  losses of a financial, physical, emotional, marital, social, or mental nature
  that may result from the use or the purported use of anything in this package,
  for any purpose whatsoever.
