VIA_ENABLE = yes
VIAL_ENABLE = yes
OLED_ENABLE = yes
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
WPM_ENABLE = yes
TAP_DANCE_ENABLE = yes
COMBO_ENABLE = yes
GRAVE_ESC_ENABLE = no
MAGIC_ENABLE = no
BOOTMAGIC_ENABLE = no

COMMAND_ENABLE = no
CONSOLE_ENABLE = no

QMK_SETTINGS = yes
LTO_ENABLE = yes

KEY_OVERRIDE_ENABLE = yes
OS_DETECTION_ENABLE = yes

# Suppress warning about unknown pragmas (e.g., #pragma region/endregion) being
# treated as errors by toolchains that enable -Werror. This keeps IDE folding
# markers harmless without changing source files.
CFLAGS += -Wno-unknown-pragmas
CCFLAGS += -Wno-unknown-pragmas
