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
    Weapon weapon(&weaponSpriteSheet, 22, (Vector2){10, 3}, 2.0f);

    // Simple warrior rectangle for demo
    Rectangle warrior = {screenWidth / 2 - 20, screenHeight / 2 - 60, 40, 120};

    // Simple enemy used for the demo (rectangle that can be hit by a weapon)
    struct Enemy
    {
        Rectangle rect;
        bool hit{false};
        float hitTimer{0.f};
        void update(float dt)
        {
            if (hitTimer > 0.f)
            {
                hitTimer -= dt;
                if (hitTimer <= 0.f)
                    hit = false;
            }
        }
        void draw() const
        {
            DrawRectangleRec(rect, hit ? RED : GRAY);
            if (hit)
                DrawText("HIT!", (int)rect.x, (int)(rect.y - 20), 20, BLACK);
        }
    };

    Enemy demoEnemy{{screenWidth / 2 + 150, screenHeight / 2 - 40, 40, 80}, false, 0.f};

    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose())
    {
        // Start drawing
        BeginDrawing();

        // Clear the background
        ClearBackground(RAYWHITE);

        // Warrior demo
        {
            // Determine warrior facing from input: hold LEFT to face left, otherwise face right
            int warriorFacing = IsKeyDown(KEY_LEFT) ? -1 : 1;

            // Update demo enemy timer
            demoEnemy.update(GetFrameTime());

            // Determine if attack input is active this frame
            bool attackActive = IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsKeyDown(KEY_SPACE);

            // Draw warrior rectangle
            DrawRectangleRec(warrior, BLUE);

            // Position weapon at left or right edge of warrior based on facing
            Vector2 weaponPos;
            if (warriorFacing > 0)
            {
                // Facing right: weapon on right edge
                weaponPos = {warrior.x + warrior.width, warrior.y + warrior.height / 2};
            }
            else
            {
                // Facing left: weapon on left edge
                weaponPos = {warrior.x, warrior.y + warrior.height / 2};
            }

            // Set weapon rotation based on attack
            weapon.setRotation(attackActive ? (warriorFacing > 0 ? 35.f : -35.f) : 0.f);

            // Draw the weapon
            weapon.Draw(weaponPos, warriorFacing, true);

            // Draw collision rectangle for debugging
            Rectangle crec = weapon.getCollisionRect();
            DrawRectangleLinesEx(crec, 2.0f, BLUE);

            // If attacking, test collision of the weapon vs the demo enemy
            if (attackActive)
            {
                Rectangle cret = weapon.computeCollisionRec(weaponPos, warriorFacing);
                if (CheckCollisionRecs(cret, demoEnemy.rect))
                {
                    demoEnemy.hit = true;
                    demoEnemy.hitTimer = 0.25f; // show hit flash for 0.25s
                }
            }
        }

        // Draw the demo enemy (after weapons so it's visible)
        demoEnemy.draw();

        // Draw instructions
        DrawText("Press LEFT arrow to face left, SPACE/CLICK to attack", 10, 10, 20, DARKGRAY);

        // End drawing
        EndDrawing();
    }

    // Unload shared texture
    UnloadTexture(weaponSpriteSheet);

    // Close the window and OpenGL context
    CloseWindow();

    return 0;
}