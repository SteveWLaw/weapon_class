#include "raylib.h"
#include "weapon.h"

int main()
{
    // Initialize the window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Basic Raylib Window");

    // Load shared texture once and construct weapons that reference it
    Texture2D weaponSpriteSheet = LoadTexture("resources/Weapon-Asset-Part1.png");

    // Use flat frame indices (row-major) for a 10x3 sprite sheet: valid indices 0..29
    Weapon weapons[3] = {
        Weapon(&weaponSpriteSheet, 22, (Vector2){10, 3}, 1.0f), // col=2,row=2
        Weapon(&weaponSpriteSheet, 23, (Vector2){10, 3}, 3.0f),  // col=3,row=0
        Weapon(&weaponSpriteSheet, 13, (Vector2){10, 3}, 1.5f)  // col=2,row=1
    };

    float rotation = 0.0f;

    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose())
    {
        // Start drawing
        BeginDrawing();

        // Clear the background
        ClearBackground(RAYWHITE);

        // Draw the textures in a row, 200 units apart (use stored scale/rotation/origin)
        {
            const int count = sizeof(weapons) / sizeof(weapons[0]);
            const int spacing = 200;
            const float baseY = screenHeight / 2 - 50;
            const float startX = screenWidth / 2 - spacing; // centers the three: -spacing, 0, +spacing
            const float rotOffsets[3] = {0.0f, 45.0f, 90.0f};

            for (int i = 0; i < count; ++i)
            {
                Vector2 pos = { startX + i * spacing, baseY };
                // store per-weapon rotation using the setter, then draw with the simpler overload
                weapons[i].setRotation(rotation + rotOffsets[i]);
                weapons[i].Draw(pos);
            }
        }

        rotation += 5.0f;
        if (rotation > 360.0f)
        {
            rotation -= 360.0f;
        }

        // Draw some text
        DrawText("Hello, Raylib!", 190, 200, 20, LIGHTGRAY);

        // End drawing
        EndDrawing();
    }

    // Unload shared texture
    UnloadTexture(weaponSpriteSheet);

    // Close the window and OpenGL context
    CloseWindow();

    return 0;
}