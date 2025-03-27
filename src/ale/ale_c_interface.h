#ifndef __ALE_C_INTERFACE_H__
#define __ALE_C_INTERFACE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // For size_t

#ifdef __cplusplus
extern "C" {
#endif

// --- Opaque Pointer Types ---
// These hide the C++ implementation details from C.
typedef struct ALEInterface_c ALEInterface_c;
typedef ALEInterface_c* ALEInterface_handle;

typedef struct ALEState_c ALEState_c;
typedef ALEState_c* ALEState_handle;

typedef struct ScreenExporter_c ScreenExporter_c;
typedef ScreenExporter_c* ScreenExporter_handle;

// --- Basic Type Definitions (Assumptions - Verify with ALE's actual types) ---
typedef int Action;         // Assuming Action is an integer type
typedef float reward_t;     // Assuming reward_t is a float type
typedef int game_mode_t;    // Assuming game_mode_t is an integer type
typedef int difficulty_t; // Assuming difficulty_t is an integer type
typedef uint8_t byte_t;     // Assuming byte_t is uint8_t

// --- Constants and Enums (If needed, define C equivalents here) ---
// e.g., #define MAX_PLAYERS 2

// --- Lifecycle Functions ---
ALEInterface_handle ale_create();
ALEInterface_handle ale_create_with_display(bool display_screen); // Legacy
void ale_destroy(ALEInterface_handle ale);

// --- Settings Functions ---
// Returns the length of the string, or -1 on error/not found.
// Writes the string to output_buffer if not NULL and buffer_size is sufficient.
int ale_getString(ALEInterface_handle ale, const char* key, char* output_buffer, size_t buffer_size);
// Returns the value, or a default/error value (e.g., -1, 0) if the key is not found.
int ale_getInt(ALEInterface_handle ale, const char* key);
// Returns the value, or false if the key is not found.
bool ale_getBool(ALEInterface_handle ale, const char* key);
// Returns the value, or NaN/0.0f if the key is not found.
float ale_getFloat(ALEInterface_handle ale, const char* key);

// Returns 0 on success, -1 on error.
int ale_setString(ALEInterface_handle ale, const char* key, const char* value);
int ale_setInt(ALEInterface_handle ale, const char* key, int value);
int ale_setBool(ALEInterface_handle ale, const char* key, bool value);
int ale_setFloat(ALEInterface_handle ale, const char* key, float value);

// --- ROM Loading and Game Control ---
// rom_file_path can be NULL to reload the current ROM. Returns 0 on success, -1 on error.
int ale_loadROM(ALEInterface_handle ale, const char* rom_file_path);

// Returns the reward. Check for errors (e.g., negative reward if not possible).
reward_t ale_act(ALEInterface_handle ale, Action action);
// paddle_strength parameter version
reward_t ale_act_with_strength(ALEInterface_handle ale, Action action, float paddle_strength);

bool ale_game_over(ALEInterface_handle ale, bool with_truncation);
bool ale_game_truncated(ALEInterface_handle ale);
void ale_reset_game(ALEInterface_handle ale); // Returns 0 on success, -1 on error? (API doesn't specify return)

// --- Game Information ---
// Returns the number of available modes, or -1 on error.
// Fills the buffer with modes if not NULL and buffer_size is sufficient.
int ale_getAvailableModes(ALEInterface_handle ale, game_mode_t* mode_buffer, size_t buffer_size);
// Returns 0 on success, -1 on error.
int ale_setMode(ALEInterface_handle ale, game_mode_t mode);
game_mode_t ale_getMode(ALEInterface_handle ale);

// Returns the number of available difficulties, or -1 on error.
// Fills the buffer with difficulties if not NULL and buffer_size is sufficient.
int ale_getAvailableDifficulties(ALEInterface_handle ale, difficulty_t* difficulty_buffer, size_t buffer_size);
// Returns 0 on success, -1 on error.
int ale_setDifficulty(ALEInterface_handle ale, difficulty_t difficulty);
difficulty_t ale_getDifficulty(ALEInterface_handle ale);

// Returns the number of legal actions, or -1 on error.
// Fills the buffer with actions if not NULL and buffer_size is sufficient.
int ale_getLegalActionSet(ALEInterface_handle ale, Action* action_buffer, size_t buffer_size);

// Returns the number of minimal actions, or -1 on error.
// Fills the buffer with actions if not NULL and buffer_size is sufficient.
int ale_getMinimalActionSet(ALEInterface_handle ale, Action* action_buffer, size_t buffer_size);

int ale_getFrameNumber(ALEInterface_handle ale);
int ale_lives(ALEInterface_handle ale);
int ale_getEpisodeFrameNumber(ALEInterface_handle ale);

// --- Screen Access ---
// Returns screen width, or -1 on error.
int ale_getScreenWidth(ALEInterface_handle ale);
// Returns screen height, or -1 on error.
int ale_getScreenHeight(ALEInterface_handle ale);

// Returns the number of bytes written (width * height), or -1 on error or insufficient buffer.
int ale_getScreenGrayscale(ALEInterface_handle ale, unsigned char* output_buffer, size_t buffer_size);
// Returns the number of bytes written (width * height * 3), or -1 on error or insufficient buffer.
int ale_getScreenRGB(ALEInterface_handle ale, unsigned char* output_buffer, size_t buffer_size);

// --- Audio Access ---
// Returns the number of audio bytes, or -1 on error.
// Fills the buffer with audio data if not NULL and buffer_size is sufficient.
int ale_getAudio(ALEInterface_handle ale, uint8_t* output_buffer, size_t buffer_size);

// --- RAM Access ---
// Returns RAM size, or -1 on error.
int ale_getRAMSize(ALEInterface_handle ale);
// Returns the byte value, or -1 on error (if index out of bounds).
byte_t ale_getRAM(ALEInterface_handle ale, size_t memory_index);
// Returns 0 on success, -1 on error.
int ale_setRAM(ALEInterface_handle ale, size_t memory_index, byte_t value);

// --- State Cloning and Restoration ---
// Remember to call ale_destroyState on the returned handle.
ALEState_handle ale_cloneState(ALEInterface_handle ale, bool include_rng);
// Returns 0 on success, -1 on error.
int ale_restoreState(ALEInterface_handle ale, const ALEState_handle state);

// Equivalent to ale_cloneState(ale, true)
ALEState_handle ale_cloneSystemState(ALEInterface_handle ale);
// Equivalent to ale_restoreState(ale, state)
int ale_restoreSystemState(ALEInterface_handle ale, const ALEState_handle state);

// Destroys a state handle created by ale_cloneState or ale_cloneSystemState.
void ale_destroyState(ALEState_handle state);

// --- Screen Saving ---
// Returns 0 on success, -1 on error.
int ale_saveScreenPNG(ALEInterface_handle ale, const char* filename);

// Creates a ScreenExporter. Remember to call ale_destroyScreenExporter.
ScreenExporter_handle ale_createScreenExporter(ALEInterface_handle ale, const char* path);
// Destroys a ScreenExporter handle.
void ale_destroyScreenExporter(ScreenExporter_handle exporter);
// Add frame saving functionality for ScreenExporter if needed.
// int ale_screenExporter_saveFrame(ScreenExporter_handle exporter); // Example

// --- Static Utility Functions ---
// Returns 1 if supported and copies MD5 to buffer, 0 if not supported, -1 on error.
// output_md5_buffer should be at least 33 bytes (32 hex chars + null terminator).
int ale_isSupportedROM(const char* rom_file_path, char* output_md5_buffer, size_t buffer_size);

// Returns the length of the message, or -1 on error.
// Writes the message to output_buffer if not NULL and buffer_size is sufficient.
int ale_welcomeMessage(char* output_buffer, size_t buffer_size);

//void ale_disableBufferedIO();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __ALE_C_INTERFACE_H__
