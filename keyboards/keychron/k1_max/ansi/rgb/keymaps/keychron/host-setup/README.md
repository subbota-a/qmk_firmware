# Host-side setup

This firmware listens to the **Scroll Lock LED HID-bit** to decide whether the
COLEMAK overlay should engage or not:

| Scroll Lock LED | Firmware behaviour                                        |
|-----------------|-----------------------------------------------------------|
| OFF             | English mode → Colemak-DH active on the letter positions  |
| ON              | Russian (or other non-Latin) mode → pure QWERTY passthrough so the OS gets standard `KeyA…KeyZ` scancodes |

That's only half the story — the OS must actually toggle the Scroll Lock LED
when the keyboard layout changes. This directory contains the host-side
configuration that makes it happen.

There's no host-side software running for this on Linux (it's a kernel-level
XKB option). On Windows there is no built-in equivalent, so a tiny background
script bridges the gap.

---

## Linux (Plasma 6 / Wayland)

Add a single line to `~/.config/kxkbrc`, into the `[Layout]` section:

```ini
[Layout]
LayoutList=us,ru
Options=grp_led:scroll      # ← this one
…rest of your existing settings…
```

`grp_led:scroll` is a standard XKB option (documented in
`/usr/share/X11/xkb/rules/evdev.lst`). libxkbcommon binds it to the Scroll
Lock LED:

- Group 0 (first layout, e.g. `us`) → LED off
- Group 1 (second layout, e.g. `ru`) → LED on

Apply the change with a logout / login. Verify:

```bash
setxkbmap -query | grep options
# expected: options:    grp_led:scroll

# while switching languages with Win+Space, watch the LED state:
watch -n 0.3 'cat /sys/class/leds/input*::scrolllock/brightness'
```

### Notes & caveats

- This is XKB-level, **applies to all keyboards** — both the K1 Max and the
  laptop's built-in keyboard will have their Scroll Lock LEDs driven.
- On Plasma 6.6.5 + Fedora 44 the option `grp_led:scroll` triggered a one-off
  crash of `xdg-desktop-portal-kde` during the first relogin. It did not
  reproduce on subsequent logins. If you hit it once and want a different
  bit, `grp_led:caps` or `grp_led:num` work the same way (just patch the
  firmware's `led_update_user` accordingly).

---

## Windows

Use the `lang_led_sync.ahk` script in this directory.

1. Install **AutoHotkey v2** (not v1.x) from <https://www.autohotkey.com/>.
2. Double-click `lang_led_sync.ahk`. A green H icon appears in the system
   tray; the script is now running.
3. For auto-start on login:
   - `Win+R` → `shell:startup` → paste a shortcut to `lang_led_sync.ahk`
     into that folder.

The script polls the active window's keyboard layout 5×/second and toggles
the Scroll Lock LED accordingly. The firmware does the rest.

### Notes & caveats

- The script physically flips the Scroll Lock toggle state. Apps that read
  `VK_SCROLL` (most notably Excel for scroll-vs-move-selection behaviour)
  will see it change. In practice almost nobody uses Scroll Lock for
  anything, so this is fine.
- LANGID `0x0419` is hard-coded as the "language that turns the LED on".
  Adjust at the top of the script if you want a different language (or
  multiple non-Latin languages).
- AHK v1 syntax is not compatible — the script uses v2.

---

## How to verify firmware reaction

After OS-side setup is done and the firmware is flashed, do the following:

1. Slide the side switch to **Mac** (or **Cable+Mac** if you use Bluetooth).
2. Make sure you're in **English** mode (Scroll Lock LED off).
3. Type — keys should produce Colemak-DH letters (`s` is on `KeyD`, `r` on
   `KeyS`, etc.).
4. Switch to **Russian** (Win+Space on Linux, or your usual hotkey on
   Windows). Scroll Lock LED should turn on.
5. Type — keys now produce standard йцукен (`KeyC` → `с`, etc.).
6. Hold `Ctrl` and press a letter — Colemak overlay disengages, so
   `Ctrl+C` triggers on the physical `KeyC` position regardless of which
   language is active.

If any of these don't work, the most likely culprits are:

- LED not toggling at all → host setup not applied (XKB option missing or
  AHK script not running)
- Layout switches but Colemak doesn't change → check the firmware's
  `led_update_user` is being called (add `xprintf` for debugging)
- Mac/Win switch not gating Colemak → check `colemak_allowed` in
  `keymap.c::on_mac_windows_switch`
