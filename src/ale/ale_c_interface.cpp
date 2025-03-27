#include "ale_c_interface.h"
#include "ale_interface.hpp" // Include the original C++ header

#include <vector>
#include <string>
#include <iostream> // For error messages
#include <exception> // For std::exception
#include <cstring>   // For strncpy, memcpy
#include <limits>   // For numeric_limits
#include <new>      // For std::nothrow

// Helper macro for error handling
#define ALE_TRY try {
#define ALE_CATCH(error_return_val) \
    } catch (const std::exception& e) { \
        std::cerr << "ALE C Interface Error: " << e.what() << std::endl; \
        return error_return_val; \
    } catch (...) { \
        std::cerr << "ALE C Interface Error: Unknown exception caught." << std::endl; \
        return error_return_val; \
    }

// Define opaque struct types (pointers to the C++ classes)
struct ALEInterface_c : public ale::ALEInterface {};
struct ALEState_c : public ale::ALEState {
    ALEState_c(const ale::ALEState& other): ale::ALEState(other) {}
};
struct ScreenExporter_c : public ale::ScreenExporter {};


extern "C" {

// --- Lifecycle Functions ---

ALEInterface_handle ale_create() {
    ALE_TRY
        return static_cast<ALEInterface_handle>(new(std::nothrow) ALEInterface_c());
    ALE_CATCH(nullptr)
}

ALEInterface_handle ale_create_with_display(bool display_screen) {
     ALE_TRY
        // Note: The modern ALEInterface constructor doesn't take bool display_screen.
        // We might need to use setBool("display_screen", ...) before loadROM instead.
        // This implementation calls the default constructor for now.
        // Or, if the old constructor still exists conditionally, use it.
        // For simplicity, assuming modern ALE:
        auto* ale = new(std::nothrow) ALEInterface_c();
        if (ale) {
            ale->setBool("display_screen", display_screen);
        }
        return static_cast<ALEInterface_handle>(ale);
     ALE_CATCH(nullptr)
}


void ale_destroy(ALEInterface_handle ale) {
    if (ale) {
        delete static_cast<ALEInterface_c*>(ale);
    }
}

// --- Settings Functions ---

int ale_getString(ALEInterface_handle ale, const char* key, char* output_buffer, size_t buffer_size) {
    if (!ale || !key) return -1;
    ALE_TRY
        const std::string& value = static_cast<ALEInterface_c*>(ale)->getStringInplace(key); // Use Inplace for efficiency
        if (output_buffer && buffer_size > 0) {
            size_t len_to_copy = value.length();
            if (len_to_copy >= buffer_size) {
                len_to_copy = buffer_size - 1; // Leave space for null terminator
            }
            std::memcpy(output_buffer, value.c_str(), len_to_copy);
            output_buffer[len_to_copy] = '\0';
        }
        return static_cast<int>(value.length()); // Return original length
    ALE_CATCH(-1)
}

int ale_getInt(ALEInterface_handle ale, const char* key) {
    if (!ale || !key) return std::numeric_limits<int>::min(); // Or some other error indicator
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->getInt(key);
    ALE_CATCH(std::numeric_limits<int>::min())
}

bool ale_getBool(ALEInterface_handle ale, const char* key) {
    if (!ale || !key) return false; // Default to false on error
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->getBool(key);
    ALE_CATCH(false)
}

float ale_getFloat(ALEInterface_handle ale, const char* key) {
    if (!ale || !key) return std::numeric_limits<float>::quiet_NaN(); // Or 0.0f
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->getFloat(key);
    ALE_CATCH(std::numeric_limits<float>::quiet_NaN())
}

int ale_setString(ALEInterface_handle ale, const char* key, const char* value) {
    if (!ale || !key || !value) return -1;
    ALE_TRY
        static_cast<ALEInterface_c*>(ale)->setString(key, value);
        return 0;
    ALE_CATCH(-1)
}

int ale_setInt(ALEInterface_handle ale, const char* key, int value) {
    if (!ale || !key) return -1;
    ALE_TRY
        static_cast<ALEInterface_c*>(ale)->setInt(key, value);
        return 0;
    ALE_CATCH(-1)
}

int ale_setBool(ALEInterface_handle ale, const char* key, bool value) {
     if (!ale || !key) return -1;
    ALE_TRY
        static_cast<ALEInterface_c*>(ale)->setBool(key, value);
        return 0;
    ALE_CATCH(-1)
}

int ale_setFloat(ALEInterface_handle ale, const char* key, float value) {
     if (!ale || !key) return -1;
    ALE_TRY
        static_cast<ALEInterface_c*>(ale)->setFloat(key, value);
        return 0;
    ALE_CATCH(-1)
}

// --- ROM Loading and Game Control ---

int ale_loadROM(ALEInterface_handle ale, const char* rom_file_path) {
    if (!ale) return -1;
    ALE_TRY
        if (rom_file_path) {
            static_cast<ALEInterface_c*>(ale)->loadROM(rom_file_path);
        } else {
            static_cast<ALEInterface_c*>(ale)->loadROM(); // Call overload with default (empty path)
        }
        return 0;
    ALE_CATCH(-1)
}

reward_t ale_act(ALEInterface_handle ale, Action action) {
    if (!ale) return std::numeric_limits<reward_t>::quiet_NaN(); // Error value
    ALE_TRY
        // Assuming Action maps directly or requires a cast
        return static_cast<ALEInterface_c*>(ale)->act(static_cast<ale::Action>(action));
    ALE_CATCH(std::numeric_limits<reward_t>::quiet_NaN())
}

reward_t ale_act_with_strength(ALEInterface_handle ale, Action action, float paddle_strength) {
    if (!ale) return std::numeric_limits<reward_t>::quiet_NaN(); // Error value
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->act(static_cast<ale::Action>(action), paddle_strength);
    ALE_CATCH(std::numeric_limits<reward_t>::quiet_NaN())
}


bool ale_game_over(ALEInterface_handle ale, bool with_truncation) {
    if (!ale) return true; // Default to game over on error? Or false? Let's say true.
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->game_over(with_truncation);
    ALE_CATCH(true)
}

bool ale_game_truncated(ALEInterface_handle ale) {
    if (!ale) return false; // Default to not truncated on error.
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->game_truncated();
    ALE_CATCH(false)
}


void ale_reset_game(ALEInterface_handle ale) {
    if (!ale) return;
    ALE_TRY
        static_cast<ALEInterface_c*>(ale)->reset_game();
    ALE_CATCH() // No return value for catch
}

// --- Game Information ---

int ale_getAvailableModes(ALEInterface_handle ale, game_mode_t* mode_buffer, size_t buffer_size) {
    if (!ale) return -1;
    ALE_TRY
        const ale::ModeVect& modes = static_cast<ALEInterface_c*>(ale)->getAvailableModes();
        size_t num_modes = modes.size();
        if (mode_buffer && buffer_size >= num_modes) {
            for (size_t i = 0; i < num_modes; ++i) {
                // Assuming game_mode_t maps directly or requires a cast
                mode_buffer[i] = static_cast<game_mode_t>(modes[i]);
            }
        }
        return static_cast<int>(num_modes);
    ALE_CATCH(-1)
}

int ale_setMode(ALEInterface_handle ale, game_mode_t mode) {
    if (!ale) return -1;
    ALE_TRY
        // Assuming game_mode_t maps directly or requires a cast
        static_cast<ALEInterface_c*>(ale)->setMode(static_cast<ale::game_mode_t>(mode));
        return 0;
    ALE_CATCH(-1)
}

game_mode_t ale_getMode(ALEInterface_handle ale) {
    if (!ale) return -1; // Or some other error indicator
    ALE_TRY
        return static_cast<game_mode_t>(static_cast<ALEInterface_c*>(ale)->getMode());
     ALE_CATCH(-1)
}


int ale_getAvailableDifficulties(ALEInterface_handle ale, difficulty_t* difficulty_buffer, size_t buffer_size) {
    if (!ale) return -1;
     ALE_TRY
        const ale::DifficultyVect& difficulties = static_cast<ALEInterface_c*>(ale)->getAvailableDifficulties();
        size_t num_difficulties = difficulties.size();
        if (difficulty_buffer && buffer_size >= num_difficulties) {
            for (size_t i = 0; i < num_difficulties; ++i) {
                 // Assuming difficulty_t maps directly or requires a cast
                difficulty_buffer[i] = static_cast<difficulty_t>(difficulties[i]);
            }
        }
        return static_cast<int>(num_difficulties);
     ALE_CATCH(-1)
}

int ale_setDifficulty(ALEInterface_handle ale, difficulty_t difficulty) {
    if (!ale) return -1;
     ALE_TRY
         // Assuming difficulty_t maps directly or requires a cast
        static_cast<ALEInterface_c*>(ale)->setDifficulty(static_cast<ale::difficulty_t>(difficulty));
        return 0;
     ALE_CATCH(-1)
}

difficulty_t ale_getDifficulty(ALEInterface_handle ale) {
    if (!ale) return -1; // Or some other error indicator
     ALE_TRY
        return static_cast<difficulty_t>(static_cast<ALEInterface_c*>(ale)->getDifficulty());
     ALE_CATCH(-1)
}


int ale_getLegalActionSet(ALEInterface_handle ale, Action* action_buffer, size_t buffer_size) {
    if (!ale) return -1;
    ALE_TRY
        const ale::ActionVect& actions = static_cast<ALEInterface_c*>(ale)->getLegalActionSet();
        size_t num_actions = actions.size();
        if (action_buffer && buffer_size >= num_actions) {
            for (size_t i = 0; i < num_actions; ++i) {
                // Assuming Action maps directly or requires a cast
                action_buffer[i] = static_cast<Action>(actions[i]);
            }
        }
        return static_cast<int>(num_actions);
    ALE_CATCH(-1)
}

int ale_getMinimalActionSet(ALEInterface_handle ale, Action* action_buffer, size_t buffer_size) {
     if (!ale) return -1;
    ALE_TRY
        const ale::ActionVect& actions = static_cast<ALEInterface_c*>(ale)->getMinimalActionSet();
        size_t num_actions = actions.size();
        if (action_buffer && buffer_size >= num_actions) {
            for (size_t i = 0; i < num_actions; ++i) {
                 // Assuming Action maps directly or requires a cast
                action_buffer[i] = static_cast<Action>(actions[i]);
            }
        }
        return static_cast<int>(num_actions);
    ALE_CATCH(-1)
}


int ale_getFrameNumber(ALEInterface_handle ale) {
    if (!ale) return -1;
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->getFrameNumber();
    ALE_CATCH(-1)
}

int ale_lives(ALEInterface_handle ale) {
    if (!ale) return -1;
     ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->lives();
    ALE_CATCH(-1)
}

int ale_getEpisodeFrameNumber(ALEInterface_handle ale) {
    if (!ale) return -1;
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->getEpisodeFrameNumber();
    ALE_CATCH(-1)
}

// --- Screen Access ---

int ale_getScreenWidth(ALEInterface_handle ale) {
     if (!ale) return -1;
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->getScreen().width();
    ALE_CATCH(-1)
}

int ale_getScreenHeight(ALEInterface_handle ale) {
     if (!ale) return -1;
    ALE_TRY
        return static_cast<ALEInterface_c*>(ale)->getScreen().height();
    ALE_CATCH(-1)
}


int ale_getScreenGrayscale(ALEInterface_handle ale, unsigned char* output_buffer, size_t buffer_size) {
    if (!ale || !output_buffer) return -1;
    ALE_TRY
        ALEInterface_c* ale_ptr = static_cast<ALEInterface_c*>(ale);
        int width = ale_ptr->getScreen().width();
        int height = ale_ptr->getScreen().height();
        size_t required_size = static_cast<size_t>(width) * height;

        if (buffer_size < required_size) return -1; // Buffer too small

        // Use a temporary vector as required by the C++ API
        std::vector<unsigned char> temp_buffer;
        temp_buffer.reserve(required_size); // Pre-allocate for efficiency
        ale_ptr->getScreenGrayscale(temp_buffer);

        // Copy data to the C buffer
        std::memcpy(output_buffer, temp_buffer.data(), required_size);
        return static_cast<int>(required_size);

    ALE_CATCH(-1)
}

int ale_getScreenRGB(ALEInterface_handle ale, unsigned char* output_buffer, size_t buffer_size) {
     if (!ale || !output_buffer) return -1;
    ALE_TRY
        ALEInterface_c* ale_ptr = static_cast<ALEInterface_c*>(ale);
        int width = ale_ptr->getScreen().width();
        int height = ale_ptr->getScreen().height();
        size_t required_size = static_cast<size_t>(width) * height * 3; // RGB

        if (buffer_size < required_size) return -1; // Buffer too small

        // Use a temporary vector
        std::vector<unsigned char> temp_buffer;
        temp_buffer.reserve(required_size);
        ale_ptr->getScreenRGB(temp_buffer);

        // Copy data to the C buffer
        std::memcpy(output_buffer, temp_buffer.data(), required_size);
        return static_cast<int>(required_size);

    ALE_CATCH(-1)
}

// --- Audio Access ---

int ale_getAudio(ALEInterface_handle ale, uint8_t* output_buffer, size_t buffer_size) {
    if (!ale) return -1;
    ALE_TRY
        const std::vector<uint8_t>& audio = static_cast<ALEInterface_c*>(ale)->getAudio();
        size_t audio_size = audio.size();
        if (output_buffer && buffer_size >= audio_size) {
             std::memcpy(output_buffer, audio.data(), audio_size);
        }
         return static_cast<int>(audio_size);
    ALE_CATCH(-1)
}


// --- RAM Access ---

int ale_getRAMSize(ALEInterface_handle ale) {
    if (!ale) return -1;
     ALE_TRY
        return static_cast<int>(static_cast<ALEInterface_c*>(ale)->getRAM().size());
    ALE_CATCH(-1)
}


byte_t ale_getRAM(ALEInterface_handle ale, size_t memory_index) {
    if (!ale) return 0; // Return 0 on error? Or maybe 255?
     ALE_TRY
        const ale::ALERAM& ram = static_cast<ALEInterface_c*>(ale)->getRAM();
        if (memory_index < ram.size()) {
            // Assuming ALERAM has an operator[] or a .get() method
            return ram.get(memory_index); // Or ram[memory_index]; check ALERAM definition
        } else {
            return 0; // Index out of bounds
        }
    ALE_CATCH(0)
}

int ale_setRAM(ALEInterface_handle ale, size_t memory_index, byte_t value) {
    if (!ale) return -1;
     ALE_TRY
        // Check bounds before setting? The C++ API might do this.
        static_cast<ALEInterface_c*>(ale)->setRAM(memory_index, value);
        return 0;
    ALE_CATCH(-1)
}

// --- State Cloning and Restoration ---
ALEState_handle ale_cloneState(ALEInterface_handle ale, bool include_rng) {
    if (!ale) return nullptr;
    ALE_TRY
        // The C++ call returns by value, we need to store it on the heap for the handle
        ale::ALEState state = static_cast<ALEInterface_c*>(ale)->cloneState(include_rng);
        //std::string serialized = "";
        return static_cast<ALEState_handle>(new(std::nothrow) ALEState_c(state)); // Copy construct on heap
    ALE_CATCH(nullptr)
}

int ale_restoreState(ALEInterface_handle ale, const ALEState_handle state_handle) {
    if (!ale || !state_handle) return -1;
    ALE_TRY
        static_cast<ALEInterface_c*>(ale)->restoreState(*static_cast<ALEState_c*>(state_handle));
        return 0;
    ALE_CATCH(-1)
}

ALEState_handle ale_cloneSystemState(ALEInterface_handle ale) {
    if (!ale) return nullptr;
    ALE_TRY
        ale::ALEState state = static_cast<ALEInterface_c*>(ale)->cloneSystemState();
        return static_cast<ALEState_handle>(new(std::nothrow) ALEState_c(state));
    ALE_CATCH(nullptr)
}

int ale_restoreSystemState(ALEInterface_handle ale, const ALEState_handle state_handle) {
    // Directly map to restoreState in C++ implementation
    return ale_restoreState(ale, state_handle);
}

void ale_destroyState(ALEState_handle state_handle) {
    if (state_handle) {
        delete static_cast<ALEState_c*>(state_handle);
    }
}

// --- Screen Saving ---

int ale_saveScreenPNG(ALEInterface_handle ale, const char* filename) {
    if (!ale || !filename) return -1;
    ALE_TRY
        static_cast<ALEInterface_c*>(ale)->saveScreenPNG(filename);
        return 0;
    ALE_CATCH(-1)
}

ScreenExporter_handle ale_createScreenExporter(ALEInterface_handle ale, const char* path) {
    if (!ale || !path) return nullptr;
    ALE_TRY
        // The C++ call returns a raw pointer, we need to manage it.
        // We wrap it in our opaque struct type.
        // Assuming ScreenExporter has a virtual destructor if we delete ScreenExporter_c.
        // If ALEInterface retains ownership, this is wrong. The header doesn't specify ownership.
        // ASSUMING CALLER GETS OWNERSHIP based on "Ownership said object is passed to the caller".
         return static_cast<ScreenExporter_handle>(static_cast<ALEInterface_c*>(ale)->createScreenExporter(path));
         // If ScreenExporter needs to be wrapped:
         // ale::ScreenExporter* exporter = static_cast<ALEInterface_c*>(ale)->createScreenExporter(path);
         // return static_cast<ScreenExporter_handle>(reinterpret_cast<ScreenExporter_c*>(exporter));
    ALE_CATCH(nullptr)
}


void ale_destroyScreenExporter(ScreenExporter_handle exporter) {
    // Assuming the handle directly points to the object created by createScreenExporter
     if (exporter) {
        delete static_cast<ScreenExporter_c*>(exporter);
        // Or if it's just a reinterpret_cast:
        // delete reinterpret_cast<ale::ScreenExporter*>(exporter);
    }
}

// --- Static Utility Functions ---

int ale_isSupportedROM(const char* rom_file_path, char* output_md5_buffer, size_t buffer_size) {
    if (!rom_file_path || !output_md5_buffer || buffer_size < 33) return -1; // Need space for 32 hex + null
    ALE_TRY
        std::optional<std::string> md5 = ale::ALEInterface::isSupportedROM(rom_file_path);
        if (md5.has_value()) {
            // Check buffer size again (should be redundant but safe)
            if (buffer_size > md5.value().length()) {
                 std::strncpy(output_md5_buffer, md5.value().c_str(), buffer_size -1);
                 output_md5_buffer[std::min(buffer_size-1, md5.value().length())] = '\0'; // Ensure null termination
                 return 1; // Supported
            } else {
                // Buffer too small, but it is supported. Indicate error.
                output_md5_buffer[0] = '\0';
                return -1;
            }
        } else {
            output_md5_buffer[0] = '\0';
            return 0; // Not supported
        }
    ALE_CATCH(-1)
}

int ale_welcomeMessage(char* output_buffer, size_t buffer_size) {
    ALE_TRY
        std::string msg = ale::ALEInterface::welcomeMessage();
         if (output_buffer && buffer_size > 0) {
            size_t len_to_copy = msg.length();
            if (len_to_copy >= buffer_size) {
                len_to_copy = buffer_size - 1;
            }
            std::memcpy(output_buffer, msg.c_str(), len_to_copy);
            output_buffer[len_to_copy] = '\0';
        }
        return static_cast<int>(msg.length());
    ALE_CATCH(-1)
}


/*
void ale_disableBufferedIO() {
    ALE_TRY
        ale::ALEInterface::disableBufferedIO();
    ALE_CATCH()
}
*/


} // extern "C"
