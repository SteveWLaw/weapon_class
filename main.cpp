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
        Weapon(&weaponSpriteSheet, 23, (Vector2){10, 3}, 3.0f), // col=3,row=0
        Weapon(&weaponSpriteSheet, 13, (Vector2){10, 3}, 1.5f)  // col=2,row=1
    };

    // Simple enemy used for the demo (rectangle that can be hit by a weapon)
    struct Enemy {
        Rectangle rect;
        bool hit{false};
        float hitTimer{0.f};
        void update(float dt) { if (hitTimer > 0.f) { hitTimer -= dt; if (hitTimer <= 0.f) hit = false; } }
        void draw() const { DrawRectangleRec(rect, hit ? RED : GRAY); if (hit) DrawText("HIT!", (int)rect.x, (int)(rect.y - 20), 20, BLACK); }
    };

    Enemy demoEnemy{{screenWidth/2 + 200, screenHeight/2 - 40, 40, 80}, false, 0.f};
    Enemy flippedEnemy{{screenWidth/2 - 180, screenHeight/2 + 40, 40, 80}, false, 0.f};  // Moved to match new weapon position

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
            // Determine warrior facing from input: hold LEFT to face left, otherwise face right
            int warriorFacing = IsKeyDown(KEY_LEFT) ? -1 : 1;

            // Update demo enemy timer
            demoEnemy.update(GetFrameTime());
            flippedEnemy.update(GetFrameTime());

            // Determine if attack input is active this frame
            bool attackActive = IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsKeyDown(KEY_SPACE);

            for (int i = 0; i < count; ++i)
            {
                Vector2 pos = {startX + i * spacing, baseY};
                // set per-weapon rotation based on input; rotate inwards depending on facing
                weapons[i].setRotation(attackActive ? (warriorFacing > 0 ? 35.f : -35.f) : 0.f);

                // Draw the weapon relative to the warrior position using current facing
                weapons[i].Draw(pos, warriorFacing, true);

                // Draw collision rectangle for debugging
                Rectangle crec = weapons[i].getCollisionRec();
                DrawRectangleLinesEx(crec, 2.0f, BLUE);

                // If attacking, test collision of the weapon vs the demo enemy
                if (attackActive)
                {
                    Rectangle cret = weapons[i].computeCollisionRec(pos, warriorFacing);
                    if (CheckCollisionRecs(cret, demoEnemy.rect))
                    {
                        demoEnemy.hit = true;
                        demoEnemy.hitTimer = 0.25f; // show hit flash for 0.25s
                    }
                }
            }

            // Flipped weapon demo (leftmost weapon flipped to face left)
            {
                Vector2 flippedPos = {startX - spacing + 100, baseY + 80};  // Moved right and down
                int flippedFacing = -1;  // Always face left for demo
                weapons[0].setRotation(attackActive ? -35.f : 0.f);
                weapons[0].Draw(flippedPos, flippedFacing, true);

                Rectangle flippedCroc = weapons[0].getCollisionRec();
                DrawRectangleLinesEx(flippedCroc, 2.0f, BLUE);

                // If attacking, test collision with the flipped enemy
                if (attackActive)
                {
                    Rectangle cret = weapons[0].computeCollisionRec(flippedPos, flippedFacing);
                    if (CheckCollisionRecs(cret, flippedEnemy.rect))
                    {
                        flippedEnemy.hit = true;
                        flippedEnemy.hitTimer = 0.25f;
                    }
                }
            }

            // Example: compute collision rect without drawing the weapon (non-mutating)
            Vector2 demoPos = {startX, baseY};
            Rectangle computed = weapons[0].computeCollisionRec(demoPos, warriorFacing);
            DrawRectangleLinesEx(computed, 2.0f, GREEN);
        }

        // Draw the demo enemies (after weapons so it's visible)
        demoEnemy.draw();
        flippedEnemy.draw();

        // Draw some text
        DrawText("Hello, Raylib!", 190, 200, 20, LIGHTGRAY);
        DrawText("Left-facing demo (left side)", screenWidth/2 - 280, screenHeight/2 + 100, 16, DARKGRAY);

        // End drawing
        EndDrawing();
    }

    // Unload shared texture
    UnloadTexture(weaponSpriteSheet);

    // Close the window and OpenGL context
    CloseWindow();

    return 0;
}