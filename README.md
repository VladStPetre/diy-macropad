# DIY Macropad --- QMK + Vial Setup Guide

This guide documents the Linux setup for a DIY macropad based on an
**Arduino Pro Micro / ATmega32U4**, with:

-   6 mechanical switches
-   1 clickable rotary encoder
-   2 × 4 switch matrix
-   QMK firmware
-   Vial-QMK firmware
-   Vial Web (`vial.rocks`) for live remapping

The project source is kept outside the QMK repositories:

``` text
~/projects/diy-macropad/keyboard/handy
```

Both QMK and Vial-QMK use symbolic links to that directory, so there is
only one real copy of the keyboard definition.

------------------------------------------------------------------------

## 1. Hardware / pin mapping

The example configuration used by this project is:

  Function           Pro Micro label QMK / ATmega32U4 pin
  ---------------- ----------------- ----------------------
  Row 0                            4 `D4`
  Row 1                            5 `C6`
  Column 0                         6 `D7`
  Column 1                         7 `E6`
  Column 2                         8 `B4`
  Column 3                        14 `B3`
  Encoder A                        9 `B5`
  Encoder B                       10 `B6`
  Encoder common                 GND GND

The six main switches occupy:

``` text
          C0       C1       C2       C3
R0        SW1      SW2      SW3      Encoder Push
R1        SW4      SW5      SW6      unused
```

With `COL2ROW`, each matrix switch is wired as:

``` text
COLUMN -> SWITCH -> DIODE -> ROW
```

The diode cathode / striped end faces the row.

The encoder rotation pins are independent of the matrix:

``` text
Encoder A      -> Pro Micro 9  / B5
Encoder Common -> GND
Encoder B      -> Pro Micro 10 / B6
```

The encoder pushbutton is wired like an ordinary matrix switch at
`R0,C3`.

------------------------------------------------------------------------

# 2. Install QMK on Linux

On Ubuntu/Debian, the QMK installer can be used:

``` bash
curl -fsSL https://install.qmk.fm | sh
qmk setup
```

The normal QMK checkout will typically be:

``` text
~/qmk_firmware
```

Verify:

``` bash
qmk --version
```

For an ATmega32U4/Pro Micro, make sure the AVR toolchain is available:

``` bash
sudo apt update
sudo apt install gcc-avr avr-libc avrdude
```

Verify:

``` bash
avr-gcc --version
```

------------------------------------------------------------------------

# 3. Install Vial-QMK

Clone Vial-QMK separately:

``` bash
cd ~
git clone --recurse-submodules https://github.com/vial-kb/vial-qmk.git
```

The result should be:

``` text
~/vial-qmk
```

Do not replace the normal QMK repository with Vial-QMK. Keeping both
repositories makes it easy to test the keyboard first with upstream QMK
and then build the Vial firmware.

------------------------------------------------------------------------

# 4. Project directory

Keep the actual keyboard files in your project:

``` text
~/projects/diy-macropad/keyboard/handy
```

A useful structure is:

``` text
handy/
├── config.h
├── keyboard.json
├── rules.mk
└── keymaps/
    ├── default/
    │   └── keymap.c
    └── vial/
        ├── config.h
        ├── keymap.c
        ├── rules.mk
        └── vial.json
```

This directory remains the source of truth and can be committed to Git
normally.

------------------------------------------------------------------------

# 5. Link the project into QMK

Instead of copying the keyboard directory, create a symbolic link:

``` bash
ln -s ~/projects/diy-macropad/keyboard/handy \
      ~/qmk_firmware/keyboards/handy
```

Check it:

``` bash
ls -l ~/qmk_firmware/keyboards/handy
```

It should point to:

``` text
~/projects/diy-macropad/keyboard/handy
```

QMK can now address the keyboard simply as:

``` text
handy
```

------------------------------------------------------------------------

# 6. Link the same project into Vial-QMK

Create another symbolic link:

``` bash
ln -s ~/projects/diy-macropad/keyboard/handy \
      ~/vial-qmk/keyboards/handy
```

The resulting arrangement is:

``` text
                    REAL SOURCE
                        |
                        v
~/projects/diy-macropad/keyboard/handy
              /                     \
             /                       \
            v                         v
~/qmk_firmware/keyboards/handy   ~/vial-qmk/keyboards/handy
        symlink                         symlink
```

Changes made in the project directory are therefore immediately visible
to both firmware trees.

------------------------------------------------------------------------

# 7. Basic QMK keyboard configuration

## `keyboard.json`

Create:

``` text
~/projects/diy-macropad/keyboard/handy/keyboard.json
```

Example:

``` json
{
    "manufacturer": "DIY",
    "keyboard_name": "Handy Macropad",
    "maintainer": "you",

    "development_board": "promicro",

    "usb": {
        "vid": "0xFEED",
        "pid": "0x6060",
        "device_version": "1.0.0"
    },

    "diode_direction": "COL2ROW",

    "matrix_pins": {
        "rows": ["D4", "C6"],
        "cols": ["D7", "E6", "B4", "B3"]
    },

    "layouts": {
        "LAYOUT": {
            "layout": [
                {"matrix": [0, 0], "x": 0, "y": 0},
                {"matrix": [0, 1], "x": 1, "y": 0},
                {"matrix": [0, 2], "x": 2, "y": 0},
                {"matrix": [0, 3], "x": 3, "y": 0},

                {"matrix": [1, 0], "x": 0, "y": 1},
                {"matrix": [1, 1], "x": 1, "y": 1},
                {"matrix": [1, 2], "x": 2, "y": 1}
            ]
        }
    }
}
```

------------------------------------------------------------------------

## Keyboard-level `config.h`

Create:

``` text
~/projects/diy-macropad/keyboard/handy/config.h
```

``` c
#pragma once

#define ENCODER_A_PINS { B5 }
#define ENCODER_B_PINS { B6 }

#define ENCODER_RESOLUTION 4
```

If the encoder behaves strangely, `ENCODER_RESOLUTION` may need to be
adjusted for the particular encoder. Values such as `2` or `1` can be
tested.

------------------------------------------------------------------------

## Basic QMK `rules.mk`

Create:

``` text
~/projects/diy-macropad/keyboard/handy/rules.mk
```

A minimal starting point is:

``` makefile
ENCODER_ENABLE = yes
ENCODER_MAP_ENABLE = yes
```

------------------------------------------------------------------------

## Default QMK keymap

Create:

``` text
keymaps/default/keymap.c
```

For example:

``` c
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_1, KC_2, KC_3, KC_MUTE,
        KC_4, KC_5, KC_6
    )
};

#if defined(ENCODER_MAP_ENABLE)

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {
        ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
    }
};

#endif
```

------------------------------------------------------------------------

# 8. Compile normal QMK firmware

From the QMK repository:

``` bash
cd ~/qmk_firmware
qmk compile -kb handy -km default
```

If compilation succeeds, flash with:

``` bash
qmk flash -kb handy -km default
```

When QMK waits for the bootloader, reset the Pro Micro.

A typical Pro Micro can be reset by briefly connecting:

``` text
RST -> GND
```

The Caterina bootloader should appear and QMK/avrdude should flash the
firmware.

Testing the keyboard with ordinary QMK first is useful. Verify that all
six switches, the encoder pushbutton, and encoder rotation work before
introducing Vial.

------------------------------------------------------------------------

# 9. Configure Vial

Create:

``` text
keymaps/vial/
```

under the same keyboard project.

The Vial keymap is separate from `keymaps/default`, allowing the
ordinary QMK firmware and Vial firmware to coexist.

------------------------------------------------------------------------

## Generate a Vial keyboard UID

From the Vial-QMK repository:

``` bash
cd ~/vial-qmk
python3 util/vial_generate_keyboard_uid.py
```

It produces something similar to:

``` c
#define VIAL_KEYBOARD_UID {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF}
```

Use the value generated for your keyboard. Do not copy the example UID.

------------------------------------------------------------------------

## `keymaps/vial/config.h`

Example:

``` c
#pragma once

#define VIAL_KEYBOARD_UID { YOUR_GENERATED_UID_HERE }

#define VIAL_UNLOCK_COMBO_ROWS { 0, 1 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 2 }
```

The example unlock combination corresponds to:

``` text
R0,C0 = SW1
R1,C2 = SW6
```

If `VIAL_INSECURE = yes` is used, Vial security/unlocking is disabled,
which can be convenient for a personal macropad.

------------------------------------------------------------------------

# 10. Vial layout including the encoder

Create:

``` text
keymaps/vial/vial.json
```

The clickable encoder button remains a normal matrix position (`0,3`).

Encoder rotation itself must also be represented in the Vial layout so
Vial can show editable CCW/CW controls.

Example:

``` json
{
  "name": "Handy Macropad",
  "vendorId": "0xFEED",
  "productId": "0x6060",
  "lighting": "none",

  "matrix": {
    "rows": 2,
    "cols": 4
  },

  "layouts": {
    "keymap": [
      [
        { "x": 1 },
        "0,0\n\n\n\n\n\n\n\n\ne",
        "0,1\n\n\n\n\n\n\n\n\ne"
      ],

      [
        "0,0",
        "0,1",
        "0,2",
        "0,3"
      ],

      [
        "1,0",
        "1,1",
        "1,2"
      ]
    ]
  }
}
```

The special entries ending in `e` represent encoder 0:

``` text
encoder 0 / direction 0 -> CCW
encoder 0 / direction 1 -> CW
```

After flashing, Vial should display two encoder direction controls in
addition to the encoder's ordinary pushbutton.

------------------------------------------------------------------------

# 11. Vial keymap

Create:

``` text
keymaps/vial/keymap.c
```

For a single layer:

``` c
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_1, KC_2, KC_3, KC_MUTE,
        KC_4, KC_5, KC_6
    )
};

#if defined(ENCODER_MAP_ENABLE)

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {
        ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
    }
};

#endif
```

If using four Vial layers, define four keyboard layers **and four
corresponding encoder-map layers**.

For example:

``` c
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [1] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [2] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [3] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) }
};
```

------------------------------------------------------------------------

# 12. Vial `rules.mk`

The ATmega32U4 has limited flash space, so feature selection matters.

A useful configuration for this macropad is:

``` makefile
VIA_ENABLE = yes
VIAL_ENABLE = yes
VIAL_INSECURE = yes

ENCODER_ENABLE = yes
ENCODER_MAP_ENABLE = yes

EXTRAKEY_ENABLE = yes

LTO_ENABLE = yes

CONSOLE_ENABLE = no
COMMAND_ENABLE = no
MOUSEKEY_ENABLE = no

NKRO_ENABLE = no
MAGIC_ENABLE = no
BOOTMAGIC_ENABLE = no
SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no
```

### Media keys

`EXTRAKEY_ENABLE = yes` is important if you want keys such as:

``` text
Volume Up
Volume Down
Mute
Play/Pause
Next Track
Previous Track
```

Without Extra Keys enabled, Vial may still allow these keycodes to be
selected, but the firmware will not produce the required consumer HID
events.

### Mouse wheel / scrolling

Vial also allows assignment of mouse-wheel actions such as:

``` text
MS_WHLU
MS_WHLD
```

These require:

``` makefile
MOUSEKEY_ENABLE = yes
```

However, Mouse Keys adds a significant amount of firmware on an
ATmega32U4.

In this project, enabling it increased firmware size beyond the Pro
Micro/Caterina flash limit. Therefore leave:

``` makefile
MOUSEKEY_ENABLE = no
```

unless enough other functionality can be removed.

Note that Vial may still display and allow assignment of mouse-wheel
keycodes even when `MOUSEKEY_ENABLE` is disabled. They simply will not
perform any action in that firmware build.

------------------------------------------------------------------------

# 13. Additional AVR size optimization

If more flash space is required, unused QMK features can be disabled.

For example:

``` makefile
COMBO_ENABLE = no
TAP_DANCE_ENABLE = no
KEY_OVERRIDE_ENABLE = no
CAPS_WORD_ENABLE = no
AUTO_SHIFT_ENABLE = no

AUDIO_ENABLE = no
MUSIC_ENABLE = no
RGBLIGHT_ENABLE = no
RGB_MATRIX_ENABLE = no
OLED_ENABLE = no
```

For a simple macropad using ordinary combinations such as:

``` text
Win + 1
Ctrl + Alt + K
Ctrl + C
Ctrl + V
```

you can also consider adding to `config.h`:

``` c
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE

#define NO_ACTION_ONESHOT
#define NO_ACTION_TAPPING

#define LAYER_STATE_8BIT
```

Do **not** disable layer support if you use Vial layers.

The Pro Micro's ATmega32U4 has 32 KiB of physical flash, but the
Caterina bootloader occupies part of it. A typical QMK build therefore
reports a usable application limit around:

``` text
28672 bytes
```

------------------------------------------------------------------------

# 14. Compile Vial firmware

Always compile the Vial keymap from the **Vial-QMK repository**, not the
upstream QMK repository:

``` bash
cd ~/vial-qmk
make handy:vial
```

For troubleshooting or after changing build configuration, do a clean
build:

``` bash
make clean
make handy:vial
```

A successful build will report that the firmware fits within the
ATmega32U4 flash limit.

------------------------------------------------------------------------

# 15. Flash Vial firmware

Run:

``` bash
cd ~/vial-qmk
make handy:vial:flash
```

When prompted, reset the Pro Micro by briefly shorting:

``` text
RST -> GND
```

After flashing, unplug and reconnect the macropad.

------------------------------------------------------------------------

# 16. Configure the keyboard with Vial Web

Open:

``` text
https://vial.rocks
```

Use a Chromium-based browser with WebHID support.

Click **Start Vial**, select:

``` text
DIY Handy Macropad
```

and grant access.

Vial can then change keys, modifier combinations, layers, macros, media
controls, and encoder mappings without recompiling the firmware.

Changes are stored on the keyboard.

------------------------------------------------------------------------

# 17. Linux udev permissions

On Linux, Vial needs access to the keyboard's `/dev/hidraw*` interfaces.

Install the Vial udev rule according to the current Vial Linux
instructions. After installing or changing a rule, reload udev and
physically reconnect the macropad.

Useful diagnostics:

``` bash
lsusb
```

The example device should appear as:

``` text
feed:6060 DIY Handy Macropad
```

Inspect HID devices:

``` bash
ls -l /dev/hidraw*
```

And:

``` bash
for d in /dev/hidraw*; do
    echo "=== $d ==="
    udevadm info --query=all --name="$d" |
        grep -Ei 'vial|vendor|product|serial'
done
```

A Vial device should expose a serial marker similar to:

``` text
vial:f64c2b3c
```

If the browser can display the device chooser and the Handy Macropad can
be selected, WebHID permission itself is generally working.

------------------------------------------------------------------------

# 18. Ubuntu browser tip: prefer native `.deb` packages

Ubuntu's Chromium package is commonly distributed as a **Snap**. Snap
confinement can add another permissions layer around USB/HID devices.

For Vial Web, prefer a natively installed `.deb` Chromium-based browser
when possible.

For example, if using Brave, install the official Debian/Ubuntu
package/repository version rather than the Snap version.

Check how a browser is installed:

``` bash
which chromium
snap list chromium 2>/dev/null
flatpak list | grep -i chromium
```

For Brave:

``` bash
which brave-browser
snap list brave 2>/dev/null
flatpak list | grep -i brave
```

A native `.deb` browser avoids Snap/Flatpak sandbox-specific USB/HID
permission issues and makes Linux udev troubleshooting more
straightforward.

If Vial Web reaches **Connecting...** after the browser has already let
you select the device, the browser permission step has succeeded;
investigate firmware/Vial configuration rather than repeatedly changing
browser HID permissions.

------------------------------------------------------------------------

# 19. Encoder troubleshooting

A useful way to separate hardware/QMK problems from Vial problems is to
temporarily disable encoder mapping:

``` makefile
ENCODER_MAP_ENABLE = no
```

and use a direct QMK callback:

``` c
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        tap_code(KC_A);
    } else {
        tap_code(KC_B);
    }

    return false;
}
```

After flashing, rotate the encoder in a text editor.

Expected:

``` text
CW  -> aaaaa
CCW -> bbbbb
```

If this works, the following are proven good:

-   encoder hardware
-   A/B wiring
-   B5/B6 pin configuration
-   encoder resolution
-   QMK encoder scanning

The remaining issue is then in the encoder-map/Vial configuration.

Restore:

``` makefile
ENCODER_MAP_ENABLE = yes
```

and remove the direct callback after testing.

------------------------------------------------------------------------

# 20. EEPROM and Vial mappings

Vial/VIA stores dynamic key and encoder mappings in EEPROM. Reflashing
firmware does not always mean that existing dynamic mappings disappear.

If defaults and the Vial configuration appear out of sync, reset the
dynamic configuration/EEPROM and reconnect the keyboard.

A temporary QMK key can also be assigned:

``` c
EE_CLR
```

Press it once after flashing, then unplug/replug the keyboard.

Remove the temporary `EE_CLR` assignment afterward.

------------------------------------------------------------------------

# 21. Typical development workflow

For ordinary QMK development:

``` bash
cd ~/qmk_firmware
qmk compile -kb handy -km default
qmk flash -kb handy -km default
```

For Vial development:

``` bash
cd ~/vial-qmk
make clean
make handy:vial
make handy:vial:flash
```

Once a working Vial firmware is flashed, normal remapping no longer
requires recompiling:

``` text
Change key / macro / encoder action
              |
              v
          vial.rocks
              |
              v
      stored on macropad
```

Recompile and reflash only when changing firmware-level functionality
such as:

-   matrix pins
-   encoder pins
-   enabled QMK features
-   Vial layout definition
-   number of layers
-   firmware code

------------------------------------------------------------------------

# 22. Useful project layout

Final recommended repository layout:

``` text
~/projects/diy-macropad/
└── keyboard/
    └── handy/
        ├── keyboard.json
        ├── config.h
        ├── rules.mk
        │
        └── keymaps/
            ├── default/
            │   └── keymap.c
            │
            └── vial/
                ├── config.h
                ├── keymap.c
                ├── rules.mk
                └── vial.json
```

With:

``` text
~/qmk_firmware/keyboards/handy
    -> ~/projects/diy-macropad/keyboard/handy

~/vial-qmk/keyboards/handy
    -> ~/projects/diy-macropad/keyboard/handy
```

This keeps the macropad repository clean and independent from both large
firmware repositories while still allowing QMK and Vial-QMK to build it
directly.
