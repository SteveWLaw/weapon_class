/**
 * @file weapon.h
 * @brief Weapon rendering and collision system for 2D sprite-based games
 */

#include "raylib.h"

/**
 * @enum AttackType
 * @brief Defines different weapon attack behavior patterns
 */
enum class AttackType
{
    Swing,  ///< Rotate about origin point (swords, axes)
    Thrust  ///< Rotate then move linearly forward (spears, polearms)
};

/**
 * @class Weapon
 * @brief Represents a weapon sprite with rendering, rotation, and collision detection
 * 
 * The Weapon class handles sprite rendering from a shared texture atlas, supports
 * multiple attack patterns, and provides collision detection with rotation support.
 * Weapons do not own their texture and are designed for efficient reuse.
 */
class Weapon
{
public:
    /**
     * @brief Constructs a weapon from sprite sheet grid coordinates
     * @param sharedTexture Pointer to the sprite sheet texture (not owned by weapon)
     * @param position Grid position (column, row) in the sprite sheet
     * @param sheetSize Grid dimensions of sprite sheet in cells (columns, rows)
     * @param scale Render scale multiplier (1.0f = original size)
     */
    Weapon(Texture2D *sharedTexture, Vector2 position, Vector2 sheetSize = {10, 3}, float scale = 1.0f)
        : spriteSheetGridSize(sheetSize), scale(scale)
    {
        texture = sharedTexture;

        // Calculate single-cell width/height
        frameCellWidth = static_cast<float>(texture->width) / spriteSheetGridSize.x;
        frameCellHeight = static_cast<float>(texture->height) / spriteSheetGridSize.y;

        // Use column * cellWidth and row * cellHeight
        sourceRect = {static_cast<int>(position.x) * frameCellWidth,
                      static_cast<int>(position.y) * frameCellHeight,
                      frameCellWidth, frameCellHeight};

        origin = {0.f, frameCellHeight}; // bottom-left by default (x=0, y=height)
    }

    /**
     * @brief Constructs a weapon from a flat frame index in the sprite sheet
     * @param sharedTexture Pointer to the sprite sheet texture (not owned by weapon)
     * @param frameIndex Zero-based frame index in row-major order
     * @param sheetSize Grid dimensions of sprite sheet in cells (columns, rows)
     * @param scale Render scale multiplier (1.0f = original size)
     */
    Weapon(Texture2D *sharedTexture, int frameIndex, Vector2 sheetSize = {10, 3}, float scale = 1.0f)
        : spriteSheetGridSize(sheetSize), scale(scale)
    {
        texture = sharedTexture;

        frameCellWidth = static_cast<float>(texture->width) / spriteSheetGridSize.x;
        frameCellHeight = static_cast<float>(texture->height) / spriteSheetGridSize.y;

        int cols = static_cast<int>(spriteSheetGridSize.x);
        int col = frameIndex % cols;
        int row = frameIndex / cols;

        sourceRect = {col * frameCellWidth, row * frameCellHeight, frameCellWidth, frameCellHeight};
        origin = {0.f, frameCellHeight};
    }

    ~Weapon() = default;

    /**
     * @brief Draws the weapon sprite relative to a character position
     * @param warriorPos Character's world position (attachment point)
     * @param facing Direction character is facing (-1 = left, 1 = right)
     * @param showBounds If true, draws debug visualization (collision box, pivot, origin)
     */
    void Draw(Vector2 warriorPos, int facing, bool showBounds = false);

    /**
     * @brief Sets the attachment offset for each facing direction
     * @param rightOffset Offset from character position when facing right
     * @param leftOffset Offset from character position when facing left
     */
    const void setOffsets(Vector2 rightOffset, Vector2 leftOffset)
    {
        attachmentOffsetRight = rightOffset;
        attachmentOffsetLeft = leftOffset;
    }

    /**
     * @brief Computes collision rectangle without modifying internal state
     * @param warriorPos Character's world position
     * @param facing Direction character is facing (-1 = left, 1 = right)
     * @return Axis-aligned bounding box (AABB) for the rotated weapon sprite
     */
    Rectangle computeCollisionRec(Vector2 warriorPos, int facing) const;

    /**
     * @brief Gets the cached collision rectangle from last Draw() call
     * @return Reference to the collision rectangle
     */
    const Rectangle &getCollisionRect() const { return collisionRect; }

    /**
     * @brief Sets the weapon's rotation angle
     * @param newRotation Rotation in degrees (clockwise)
     */
    const void setRotation(float newRotation) { rotation = newRotation; }
    
    /**
     * @brief Sets the weapon's render scale
     * @param newScale Scale multiplier (1.0f = original size)
     */
    const void setScale(float newScale) { scale = newScale; }
    
    /**
     * @brief Sets the weapon's origin point for rotation
     * @param newOrigin Origin in sprite-local coordinates (pixels, unscaled)
     */
    const void setOrigin(Vector2 newOrigin) { origin = newOrigin; }
    
    /**
     * @brief Sets the weapon's facing direction
     * @param f Facing direction (>= 0 for right, < 0 for left)
     */
    const void setFacing(int f) { facing = (f >= 0 ? 1 : -1); }

    /**
     * @brief Sets the weapon's attack behavior pattern
     * @param type Attack type (Swing or Thrust)
     */
    const void setAttackType(AttackType type) { attackType = type; }
    
    /**
     * @brief Sets the thrust offset for thrust-type attacks
     * @param offset Distance in pixels to thrust forward
     */
    const void setThrustOffset(float offset) { thrustOffset = offset; }
    
    /**
     * @brief Gets the weapon's current attack type
     * @return Current attack behavior pattern
     */
    const AttackType getAttackType() const { return attackType; }
    
    /**
     * @brief Sets the weapon's damage value
     * @param dmg Damage amount
     */
    const void setDamage(float dmg) { damage = dmg; }
    
    /**
     * @brief Gets the weapon's damage value
     * @return Current damage amount
     */
    const float getDamage() const { return damage; }

private:
    Texture2D *texture{nullptr};              ///< Pointer to shared sprite sheet texture (not owned)
    Rectangle sourceRect{0, 0, 0, 0};         ///< Source rectangle in sprite sheet (pixels)
    Vector2 spriteSheetGridSize{};            ///< Grid dimensions of sprite sheet (columns, rows)
    float rotation{0.0f};                     ///< Current rotation angle in degrees
    float frameCellWidth{0.0f};               ///< Width of single sprite cell (pixels)
    float frameCellHeight{0.0f};              ///< Height of single sprite cell (pixels)
    float scale{1.0f};                        ///< Render scale multiplier
    Vector2 origin{0.f, 0.f};                 ///< Rotation origin in sprite-local coordinates (pixels, unscaled)
    Vector2 attachmentOffsetRight{35.f, 55.f};///< Offset from character when facing right
    Vector2 attachmentOffsetLeft{25.f, 55.f}; ///< Offset from character when facing left
    Rectangle collisionRect{0.f, 0.f, 0.f, 0.f}; ///< Cached collision AABB from last Draw() call
    int facing{1};                            ///< Current facing direction (1 = right, -1 = left)
    
    AttackType attackType{AttackType::Swing}; ///< Current attack behavior pattern
    float thrustOffset{0.f};                  ///< Linear thrust distance for Thrust attacks (pixels)
    float damage{10.f};                       ///< Damage dealt by this weapon
};
