#include <stdio.h>
#include <stdlib.h>
#include "ale_c_interface.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rom_file>\n", argv[0]);
        return 1;
    }
    const char* rom_path = argv[1];

    // Check if ROM is supported
    char md5_buffer[33];
    int supported = ale_isSupportedROM(rom_path, md5_buffer, sizeof(md5_buffer));
    if (supported == 1) {
        printf("ROM is supported. MD5: %s\n", md5_buffer);
    } else if (supported == 0) {
        printf("ROM is not officially supported by ALE.\n");
        // Proceed anyway? Or exit?
    } else {
        fprintf(stderr, "Error checking ROM support.\n");
        // return 1; // Decide if this is fatal
    }

    // Create ALE instance
    ALEInterface_handle ale = ale_create();
    if (!ale) {
        fprintf(stderr, "Failed to create ALE instance.\n");
        return 1;
    }

    // Set some options (optional)
    ale_setBool(ale, "display_screen", true);
    ale_setFloat(ale, "repeat_action_probability", 0.0f); // Deterministic

    // Load the ROM
    if (ale_loadROM(ale, rom_path) != 0) {
        fprintf(stderr, "Failed to load ROM: %s\n", rom_path);
        ale_destroy(ale);
        return 1;
    }

    // Get legal actions
    int num_actions = ale_getLegalActionSet(ale, NULL, 0); // Get count first
    if (num_actions < 0) {
         fprintf(stderr, "Failed to get action set size.\n");
         ale_destroy(ale);
         return 1;
    }
    Action* legal_actions = (Action*)malloc(num_actions * sizeof(Action));
    if (!legal_actions) {
        fprintf(stderr, "Failed to allocate memory for actions.\n");
        ale_destroy(ale);
        return 1;
    }
    if (ale_getLegalActionSet(ale, legal_actions, num_actions) != num_actions) {
         fprintf(stderr, "Failed to get action set.\n");
         free(legal_actions);
         ale_destroy(ale);
         return 1;
    }

    printf("Legal actions (%d): ", num_actions);
    for (int i = 0; i < num_actions; ++i) {
        printf("%d ", legal_actions[i]);
    }
    printf("\n");

    // Simple game loop: perform 100 random actions
    reward_t total_reward = 0;
    for (int i = 0; i < 100; ++i) {
        if (ale_game_over(ale, true)) {
            printf("Game over at step %d\n", i);
            ale_reset_game(ale);
        }

        // Choose a random legal action
        Action action = legal_actions[rand() % num_actions];
        reward_t reward = ale_act(ale, action);
        total_reward += reward;

        // Optional: Get screen data (example - requires large buffer)
        /*
        int width = ale_getScreenWidth(ale);
        int height = ale_getScreenHeight(ale);
        if (width > 0 && height > 0) {
            size_t rgb_size = (size_t)width * height * 3;
            unsigned char* screen_buffer = (unsigned char*)malloc(rgb_size);
            if (screen_buffer) {
                if (ale_getScreenRGB(ale, screen_buffer, rgb_size) > 0) {
                    // Process screen_buffer...
                }
                free(screen_buffer);
            }
        }
        */
    }

    printf("Total reward over 100 steps: %f\n", total_reward);

    // Clean up
    free(legal_actions);
    ale_destroy(ale);

    return 0;
}
