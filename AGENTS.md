# QMK Userspace Vial Development Instructions

## Project Overview
This is a QMK userspace repository for building Vial-compatible firmware for the Lily58 keyboard using the Helios MCU conversion. The project is configured as an external QMK userspace that builds against [vial-kb/vial-qmk](https://github.com/vial-kb/vial-qmk) rather than mainline QMK. The base repo is checked out at `../qmk_firmware` relative to this userspace.
The documentation for qmk can be found at https://docs.qmk.fm/.

## Architecture & Key Components

### Userspace Structure
- **`qmk.json`**: Defines build targets
- **`keyboards/lily58/`**: Keyboard-specific overrides and keymaps
- **`keyboards/lily58/rev1/keymaps/vial/`**: Primary keymap directory containing:
  - `keymap.c`: Layer definitions, OLED functions, OS detection
  - `config.h`: Vial settings, timing configurations, feature flags
  - `rules.mk`: Build feature enables/disables
  - `vial.json`: UI layout definition for Vial app
  - `layout.vil`: Vial layout json file (exported from Vial app)

### Build System Integration
- **GitHub Actions**: Uses `vial-kb/vial-qmk` fork instead of mainline QMK
- **Makefile**: Delegates to QMK firmware root with userspace path
- **Local builds**: Requires `qmk config user.overlay_dir` setup

## Development Workflows

### Essential Build Commands
```bash
# Build all userspace targets
qmk userspace-compile
```

### VS Code Tasks
Use the predefined tasks instead of manual commands:
- **"QMK: Build All Userspace Targets"**: Primary build command
- **"QMK: Clean Build"**: Clean QMK build artifacts
- **"QMK: Doctor (Environment Check)"**: Run QMK environment checks
- **"QMK: Userspace Doctor"**: Validate userspace configuration
- **"QMK: Syntax Check (Compile Only)"**: Compile to check for syntax errors and warnings

## Vial-Specific Configuration

### Keymap Structure
- **Layer activation**: Uses `MO()` for momentary layers, `LT()` for layer-tap

### Key Configuration Patterns
```c
// Timing settings in config.h
#define TAPPING_TERM 200
#define QUICK_TAP_TERM 200  // For preventing accidental holds
#define FLOW_TAP_TERM 100   // Fast tap sequences

// Layer definitions use standard QMK macros
[_LAYER] = LAYOUT(
    KC_key, LT(_LAYER, KC_key), // Layer-tap combinations
    _______, XXXXXXX,           // Transparent and blocked keys
)
```

## Hardware-Specific Details

### Helios MCU Conversion
- **Target**: `CONVERT_TO=helios` required for all builds

### OLED Implementation

## Common Editing Patterns

### Adding New Layers
1. Update `enum layer_number` in `keymap.c`
2. Increase `DYNAMIC_KEYMAP_LAYER_COUNT` in `config.h`
3. Add layer definition with `LAYOUT()` macro
4. Update OLED display function if needed

### Modifying Key Behavior
- **Hold/tap timing**: Adjust `TAPPING_TERM` and `QUICK_TAP_TERM`
- **Preventing accidental activation**: Use `TAPPING_FORCE_HOLD`
- **OS-specific behavior**: Handled in `process_detected_host_os_kb()`

### OLED Customization
- **Status display**: Modify `render_status()` function
- **Custom graphics**: Update logo rendering or add new elements
- **Layer indicators**: Extend switch statement in status renderer

## Integration Points

### OS Detection
- **macOS**: Automatically swaps Cmd/Ctrl keys
- **Windows/Linux**: Uses standard layout
- **Implementation**: `process_detected_host_os_kb()` in keymap.c

### Vial App Compatibility
- **Layout sync**: `vial.json` must match physical `LAYOUT()` definition
- **Matrix mapping**: 10 rows × 6 cols for split configuration
- **Visual layout**: Coordinates in `vial.json` define key positions

## Debugging & Troubleshooting

### Build Issues
- **Missing Helios**: Ensure `CONVERT_TO=helios` in all build commands
- **Userspace not found**: Verify `qmk config user.overlay_dir` points to repository
- **Vial features**: Check that vial-qmk is being used, not mainline QMK

Always test builds locally before pushing to ensure GitHub Actions will succeed.

## Adding custom key actions & Vial customKeycodes

When you need a key that performs non-standard behavior, follow these steps and best-practices so the code stays portable and visible to the Vial app.

1) Define a custom keycode in your keymap

 - For ordinary QMK-only custom keycodes you can use the SAFE_RANGE pattern:

 ```c
 enum custom_keycodes {
   MY_CUSTOM = SAFE_RANGE,
 };
 ```

 - For Vial integration (so the key appears in the Vial GUI and can be assigned from the app), map your enum into Vial's custom keycode range using QK_KB_n. Example (first Vial slot):

 ```c
 enum custom_keycodes {
   CHAT_MODE = QK_KB_0, // maps to vial.json entry #0
 };
 ```

2) Implement behavior in process_record_user

 - `process_record_user(uint16_t keycode, keyrecord_t *record)` is the common hook for custom behavior. Return `false` if you handled the key completely and don't want QMK to send a keycode; return `true` to let QMK process the key normally.

Tips and pitfalls:
 - LT(key) (layer-tap) emits the tap keycode on release. If you react to Enter only on key press, LT taps may be missed. Handling Enter on release is the simplest fix.

3) Make the custom key show up in Vial (`vial.json`)

 - Add a `customKeycodes` array at the top-level of `keyboards/.../keymaps/<map>/vial.json` with entries that correspond to the QK_KB_x mapping.

 - The first `customKeycodes` entry maps to `QK_KB_0`, the second to `QK_KB_1`, and so on. Ensure your C enum uses the same index (e.g., `CHAT_MODE = QK_KB_0`).
