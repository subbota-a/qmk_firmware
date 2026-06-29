# K1 Max — build & flash (keychron keymap)

Personal workflow for building this keymap in Docker and flashing it over DFU.
No local QMK toolchain needed — everything compiles inside the `qmk_cli`
container.

- **Keyboard:** `keychron/k1_max/ansi/rgb`
- **Keymap:** `keychron`
- **Make target:** `keychron/k1_max/ansi/rgb:keychron`

## 1. Build (Docker)

```sh
./util/docker_build.sh keychron/k1_max/ansi/rgb:keychron
```

`util/docker_build.sh` runs `make` inside the container with
`SKIP_FLASHING_SUPPORT=1` (no privileged `/dev` mount — we only compile here)
and `QMK_USERSPACE=` (disables the "copy firmware to userspace folder" step,
which otherwise fails trying to write to the unmounted `/qmk_userspace`).

On success the firmware lands in the repo root:

```
keychron_k1_max_ansi_rgb_keychron.bin
```

## 2. Enter the bootloader (DFU)

Per the board readme (`keyboards/keychron/k1_max/readme.md`):

1. Set the side mode switch to **"Cable"** (wired — 2.4G/BT will not flash).
2. Hold **Esc** (or press the reset button under the spacebar)…
3. …while plugging in the USB cable. Release.

The board enumerates as USB device `0483:df11` (STM32 DFU). Verify:

```sh
sudo dfu-util -l        # look for 0483:df11, alt 0, "@Internal Flash"
```

`sudo` is required — without it dfu-util reports `LIBUSB_ERROR_ACCESS`.

## 3. Flash

```sh
sudo dfu-util -d 0483:df11 -a 0 -s 0x08000000:leave -D keychron_k1_max_ansi_rgb_keychron.bin
```

- `-d 0483:df11` — target the STM32 DFU device (VID:PID).
- `-a 0` — alt-setting 0 = Internal Flash.
- `-s 0x08000000:leave` — STM32 flash base address; `:leave` exits DFU and
  boots the new firmware afterwards.
- `-D <file>` — download (write) this `.bin` to the board.

This is the same command as `restore.sh`, just pointed at our build instead of
the stock backup (`k1_max_stock_backup.bin`). To roll back to factory firmware,
run `restore.sh`.
