#include "raylib.h"
#include "weapon.h"

int main()
{
    // Initialize the window
    const int screenWidth = 1200;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Weapon System Demo - Three Characters");

    // Load shared texture once and construct weapons that reference it
    Texture2D weaponSpriteSheet = LoadTexture("resources/Weapon-Asset-Part1.png");

    // Shared spear configuration
    float spearCellWidth = weaponSpriteSheet.width / 10.0f;
    float spearCellHeight = weaponSpriteSheet.height / 3.0f;
    Vector2 spearOrigin = {spearCellWidth * 0.33f, spearCellHeight * 0.66f};

    // Character 1: Sword wielder
    Weapon swordWeapon(&weaponSpriteSheet, 22, (Vector2){10, 3}, 2.0f);
    swordWeapon.setAttackType(AttackType::Swing);
    Rectangle warrior1 = {150, screenHeight / 2 - 60, 40, 120};

    // Character 2: Spear wielder (ready stance)
    Weapon spearReady(&weaponSpriteSheet, 11, (Vector2){10, 3}, 2.0f);
    spearReady.setAttackType(AttackType::Thrust);
    spearReady.setOrigin(spearOrigin);
    Rectangle warrior2 = {screenWidth / 2 - 20, screenHeight / 2 - 60, 40, 120};

    // Character 3: Spear wielder (idle stance)
    Weapon spearIdle(&weaponSpriteSheet, 11, (Vector2){10, 3}, 2.0f);
    spearIdle.setAttackType(AttackType::Thrust);
    spearIdle.setOrigin(spearOrigin);
    Rectangle warrior3 = {screenWidth - 230, screenHeight / 2 - 60, 40, 120};

    // Attack animation state for each character
    float attackTimer1 = 0.f;
    float attackTimer2 = 0.f;
    float attackTimer3 = 0.f;
    const float attackDuration = 0.3f;

    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // TAB triggers attacks for all characters
        if (IsKeyPressed(KEY_TAB))
        {
            if (attackTimer1 <= 0.f)
                attackTimer1 = attackDuration;
            if (attackTimer2 <= 0.f)
                attackTimer2 = attackDuration;
            if (attackTimer3 <= 0.f)
                attackTimer3 = attackDuration;
        }

        // Update attack timers
        if (attackTimer1 > 0.f)
        {
            attackTimer1 -= dt;
            if (attackTimer1 < 0.f)
                attackTimer1 = 0.f;
        }
        if (attackTimer2 > 0.f)
        {
            attackTimer2 -= dt;
            if (attackTimer2 < 0.f)
                attackTimer2 = 0.f;
        }
        if (attackTimer3 > 0.f)
        {
            attackTimer3 -= dt;
            if (attackTimer3 < 0.f)
                attackTimer3 = 0.f;
        }

        // Helper lambda to calculate attack progress (triangle wave for thrust)
        auto calculateProgress = [attackDuration](float timer, bool isThrust) -> float
        {
            if (timer <= 0.f)
                return 0.f;

            float t = timer / attackDuration;
            float normalized = 1.0f - t;

            if (isThrust)
            {
                // Triangle wave: 0 -> 1 -> 0
                if (normalized < 0.5f)
                    return normalized * 2.0f;
                else
                    return (1.0f - normalized) * 2.0f;
            }
            else
            {
                // Simple linear: 0 -> 1
                return normalized;
            }
        };

        float attackProgress1 = calculateProgress(attackTimer1, false); // Sword
        float attackProgress2 = calculateProgress(attackTimer2, true);  // Spear
        float attackProgress3 = calculateProgress(attackTimer3, true);  // Spear

        // Start drawing
        BeginDrawing();

        // Clear the background
        ClearBackground(RAYWHITE);

        int facing = 1; // All facing right for this demo

        // === CHARACTER 1: SWORD (SWING ATTACK) ===
        {
            DrawRectangleRec(warrior1, BLUE);
            Vector2 weaponPos = {warrior1.x + warrior1.width, warrior1.y + warrior1.height / 2};

            if (attackTimer1 > 0.f)
            {
                float maxRotation = 45.f;
                swordWeapon.setRotation(attackProgress1 * maxRotation);
            }
            else
            {
                swordWeapon.setRotation(0.f);
            }
            swordWeapon.setThrustOffset(0.f);

            swordWeapon.Draw(weaponPos, facing, true);

            DrawText("SWORD", (int)warrior1.x - 10, (int)warrior1.y - 30, 20, DARKBLUE);
            DrawText("(Swing)", (int)warrior1.x - 15, (int)warrior1.y - 10, 16, DARKGRAY);
        }

        // === CHARACTER 2: SPEAR IN READY STANCE ===
        {
            DrawRectangleRec(warrior2, GREEN);
            Vector2 weaponPos = {warrior2.x + warrior2.width, warrior2.y + warrior2.height / 2};

            float thrustRotation = 45.f;
            float maxThrust = 50.f;

            // Always in ready stance (rotated), thrusts when attacking
            spearReady.setRotation(thrustRotation);
            if (attackTimer2 > 0.f)
            {
                spearReady.setThrustOffset(attackProgress2 * maxThrust);
            }
            else
            {
                spearReady.setThrustOffset(0.f);
            }

            spearReady.Draw(weaponPos, facing, true);

            DrawText("SPEAR (READY)", (int)warrior2.x - 40, (int)warrior2.y - 30, 20, DARKGREEN);
            DrawText("(Always 45°)", (int)warrior2.x - 30, (int)warrior2.y - 10, 16, DARKGRAY);
        }

        // === CHARACTER 3: SPEAR IN IDLE STANCE ===
        {
            DrawRectangleRec(warrior3, ORANGE);
            Vector2 weaponPos = {warrior3.x + warrior3.width, warrior3.y + warrior3.height / 2};

            float thrustRotation = 45.f;
            float maxThrust = 50.f;

            // Idle normally, rotates and thrusts when attacking
            if (attackTimer3 > 0.f)
            {
                spearIdle.setRotation(thrustRotation);
                spearIdle.setThrustOffset(attackProgress3 * maxThrust);
            }
            else
            {
                spearIdle.setRotation(0.f);
                spearIdle.setThrustOffset(0.f);
            }

            spearIdle.Draw(weaponPos, facing, true);

            DrawText("SPEAR (IDLE)", (int)warrior3.x - 35, (int)warrior3.y - 30, 20, ORANGE);
            DrawText("(Rotate on attack)", (int)warrior3.x - 50, (int)warrior3.y - 10, 16, DARKGRAY);
        }

        // Draw instructions
        DrawText("Press TAB to attack with all weapons", 10, 10, 24, BLACK);
        DrawText("Three weapon usage examples: Swing, Ready Thrust, Idle Thrust", 10, 40, 20, DARKGRAY);

        // End drawing
        EndDrawing();
    }

    // Unload shared texture
    UnloadTexture(weaponSpriteSheet);

    // Close the window and OpenGL context
    CloseWindow();

    return 0;
}