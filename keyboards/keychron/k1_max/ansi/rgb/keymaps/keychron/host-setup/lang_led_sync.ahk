; ============================================================================
;  Language ↔ Scroll Lock LED sync (Windows side, AutoHotkey v2)
; ============================================================================
;
;  Mirror of the Linux setup `Options=grp_led:scroll` in kxkbrc.
;
;  Background:
;    Keychron K1 Max custom firmware listens to the Scroll Lock LED HID-bit
;    to switch between Colemak-DH (LED off, English) and QWERTY passthrough
;    (LED on, Russian/йцукен). On Linux this is done by libxkbcommon via the
;    `grp_led:scroll` XKB option. Windows has no equivalent, so this script
;    bridges the gap.
;
;  What it does:
;    * Polls the focused window's keyboard layout 5 times per second.
;    * If layout is Russian (LANGID 0x0419) and Scroll Lock LED is OFF → turn ON.
;    * If layout is not Russian and Scroll Lock LED is ON → turn OFF.
;
;  Requirements:
;    AutoHotkey v2.0+ — https://www.autohotkey.com/
;
;  Install & run:
;    1. Install AutoHotkey v2 (full installer, not v1.x).
;    2. Double-click this file to run.
;    3. (Optional) For auto-start on login:
;         Win+R → shell:startup → place a shortcut to this file inside.
;
;  Verifying it works:
;    Press Win+Space or whatever you've set for layout switch. Scroll Lock LED
;    on the keyboard should turn on/off in sync. The Colemak ↔ QWERTY switch
;    happens automatically inside the K1 Max firmware in response.
;
;  Notes / caveats:
;    * Scroll Lock toggle state is physically flipped — apps that listen to
;      ScrollLock (e.g. Excel) will see this. In practice 99% of users never
;      use ScrollLock for anything, so this is fine.
;    * If you have multiple non-Russian layouts (e.g. us + de) and want the
;      LED to track a different language, change `RUSSIAN_LANGID` below.
;
; ============================================================================

#Requires AutoHotkey v2.0
#SingleInstance Force

; LANGID for the language that should turn the LED ON.
; 0x0419 = ru-RU (russian). See:
;   https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/
RUSSIAN_LANGID := 0x0419

; Poll interval in milliseconds. 200ms is responsive enough and cheap.
POLL_INTERVAL := 200

; Optional: tray icon tooltip
A_IconTip := "Language ↔ ScrollLock LED sync"

SetTimer(SyncLayoutLed, POLL_INTERVAL)

SyncLayoutLed() {
    global RUSSIAN_LANGID
    try {
        hwnd := WinGetID("A")
        if (!hwnd)
            return

        threadId := DllCall("GetWindowThreadProcessId", "Ptr", hwnd, "UInt*", &dummy := 0)
        layout   := DllCall("GetKeyboardLayout", "UInt", threadId, "UPtr")
        if (!layout)
            return

        langId      := layout & 0xFFFF
        isRussian   := (langId = RUSSIAN_LANGID)
        scrollState := GetKeyState("ScrollLock", "T")

        if (isRussian && !scrollState) {
            SetScrollLockState(true)
        } else if (!isRussian && scrollState) {
            SetScrollLockState(false)
        }
    } catch {
        ; Window transitions can briefly return invalid HWNDs; just skip
        ; this tick and the next one will pick up the new state.
    }
}
