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

    // Create a sword (frame 22) with swing attack
    Weapon sword(&weaponSpriteSheet, 22, (Vector2){10, 3}, 2.0f);
    sword.setAttackType(AttackType::Swing);

    // Create a spear (frame 11 - row 1, column 1) with thrust attack
    Weapon spear(&weaponSpriteSheet, 11, (Vector2){10, 3}, 2.0f);
    spear.setAttackType(AttackType::Thrust);
    // Set origin for spear - 1/3 along diagonal from bottom-left to top-right
    float spearCellWidth = weaponSpriteSheet.width / 10.0f;
    float spearCellHeight = weaponSpriteSheet.height / 3.0f;
    spear.setOrigin({spearCellWidth * 0.33f, spearCellHeight * 0.66f});

    // Track which weapon is active (toggle with TAB)
    bool usingSword = true;

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

    // Attack animation state
    float attackTimer = 0.f;
    const float attackDuration = 0.3f;
    bool isReady = false; // Ready state for spear (rotated but not thrusting)

    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Toggle weapon with TAB
        if (IsKeyPressed(KEY_TAB))
        {
            usingSword = !usingSword;
        }

        // Get reference to active weapon
        Weapon &weapon = usingSword ? sword : spear;

        // Start drawing
        BeginDrawing();

        // Clear the background
        ClearBackground(RAYWHITE);

        // Warrior demo
        {
            // Determine warrior facing from input: hold LEFT to face left, otherwise face right
            int warriorFacing = IsKeyDown(KEY_LEFT) ? -1 : 1;

            // Update demo enemy timer
            demoEnemy.update(dt);

            // Track attack button state
            bool attackPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE);
            bool attackReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsKeyReleased(KEY_SPACE);
            bool attacking = attackTimer > 0.f;

            // State machine for spear: idle -> ready (on button press) -> attack (on button release) -> idle
            // For sword: just idle <-> attack
            if (weapon.getAttackType() == AttackType::Thrust)
            {
                // Spear logic: press to ready, release to attack
                if (attackPressed && !attacking)
                {
                    isReady = true;
                }

                if (attackReleased && isReady && !attacking)
                {
                    attackTimer = attackDuration;
                    isReady = false;
                }

                // Cancel ready if attack started
                if (attacking)
                {
                    isReady = false;
                }
            }
            else
            {
                // Sword logic: simple press to attack
                isReady = false;
                if (attackPressed && !attacking)
                {
                    attackTimer = attackDuration;
                }
            }

            // Update attack timer
            if (attacking)
            {
                attackTimer -= dt;
                if (attackTimer < 0.f)
                    attackTimer = 0.f;
            }

            // Calculate attack progress (0 to 1, then back to 0)
            float attackProgress = 0.f;
            if (attacking)
            {
                float t = attackTimer / attackDuration;
                float normalized = 1.0f - t; // 0 at start, 1 at end

                // Make thrust go out and return: 0 -> 1 -> 0 (triangle wave)
                if (normalized < 0.5f)
                    attackProgress = normalized * 2.0f; // 0 to 1 in first half
                else
                    attackProgress = (1.0f - normalized) * 2.0f; // 1 to 0 in second half
            }

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

            // Set weapon rotation and thrust based on attack type
            if (weapon.getAttackType() == AttackType::Swing)
            {
                // Swing: rotate to max angle during attack
                float maxRotation = 45.f;
                weapon.setRotation(attacking ? (attackProgress * maxRotation * warriorFacing) : 0.f);
                weapon.setThrustOffset(0.f);
            }
            else // AttackType::Thrust
            {
                // Thrust: three states
                // - Idle: no rotation, no thrust
                // - Ready: 45° rotation, no thrust
                // - Attack: 45° rotation, thrust forward and back
                float thrustRotation = 45.f * warriorFacing;
                float maxThrust = 50.f; // pixels to thrust forward

                if (attacking)
                {
                    weapon.setRotation(thrustRotation);
                    weapon.setThrustOffset(attackProgress * maxThrust);
                }
                else if (isReady)
                {
                    weapon.setRotation(thrustRotation);
                    weapon.setThrustOffset(0.f);
                }
                else // idle
                {
                    weapon.setRotation(0.f);
                    weapon.setThrustOffset(0.f);
                }
            }

            // Draw the weapon
            weapon.Draw(weaponPos, warriorFacing, true);

            // Draw collision rectangle for debugging
            Rectangle crec = weapon.getCollisionRect();
            DrawRectangleLinesEx(crec, 2.0f, BLUE);

            // If attacking, test collision of the weapon vs the demo enemy
            if (attacking)
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
        const char *attackInstr = usingSword ? "SPACE/CLICK: Attack" : "HOLD SPACE/CLICK: Ready, RELEASE: Thrust";
        DrawText(TextFormat("TAB: Switch weapon | LEFT: Face left | %s", attackInstr), 10, 10, 18, DARKGRAY);
        DrawText(usingSword ? "Weapon: SWORD (Swing)" : "Weapon: SPEAR (Thrust)", 10, 35, 20, DARKGREEN);
        if (isReady)
            DrawText("[READY TO THRUST]", 10, 60, 20, ORANGE);

        // End drawing
        EndDrawing();
    }

    // Unload shared texture
    UnloadTexture(weaponSpriteSheet);

    // Close the window and OpenGL context
    CloseWindow();

    return 0;
}