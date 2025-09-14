// Change the product ID to avoid the Linux-specific VIA bug (if a keyboard
// with the supported VID:PID but a non-VIA firmware is connected, the app
// becomes very slow to respond).
#undef PRODUCT_ID
#define PRODUCT_ID      0x8080

// Measure the matrix scan rate and report it in the debug console.
#define DEBUG_MATRIX_SCAN_RATE

// Use the minimum possible I/O delay for the matrix to speed up scanning.
#define MATRIX_IO_DELAY 1

// Select the hold action for a dual-use key immediately when another key is
// pressed (this is different from PERMISSIVE_HOLD, which selects the hold
// action when another key is pressed and then released).  The per-key option
// is required, because using the HOLD_ON_OTHER_KEY_PRESS behavior globally is
// not appropriate for some cases like OSL() or OSM().
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY

// Increase the backlight PWM frequency by decreasing the resolution.  The
// default value (0xFFFFU) together with the 16 MHz clock results in ≈244 Hz
// PWM frequency, which is very noticeable.  Another limitation of the ID80 PCB
// (that might be specific to some particular batch) is that it emits a slight
// whining sound when the PWM output frequency is about 8000 Hz; however, using
// 4000 Hz looks good enough, but does not cause any annoying sounds.
#define BACKLIGHT_RESOLUTION (F_CPU / 4000)

// Enable lighting layers
#define RGBLIGHT_LAYERS
