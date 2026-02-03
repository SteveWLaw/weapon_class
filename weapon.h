#include "raylib.h"

class Weapon
{
public:
    // Constructor that uses a shared texture pointer (does not take ownership)
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

    // Convenience constructor: choose a frame by flat index (row-major) using shared texture
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

    // Single Draw API: draw the weapon relative to a warrior position and a facing (-1 left, 1 right)
    void Draw(Vector2 warriorPos, int facing, bool showBounds = false);

    // Offsets (per-facing) determine how the weapon is positioned relative to the warrior
    const void setOffsets(Vector2 rightOffset, Vector2 leftOffset) { attachmentOffsetRight = rightOffset; attachmentOffsetLeft = leftOffset; }

    // Compute collision rectangle for a warrior position and facing WITHOUT mutating internal state
    Rectangle computeCollisionRec(Vector2 warriorPos, int facing) const;

    const Rectangle &getCollisionRect() const { return collisionRect; }

    const void setRotation(float newRotation) { rotation = newRotation; }
    const void setScale(float newScale) { scale = newScale; }
    const void setOrigin(Vector2 newOrigin) { origin = newOrigin; }
    const void setFacing(int f) { facing = (f >= 0 ? 1 : -1); }

private:
    Texture2D *texture{nullptr};
    Rectangle sourceRect{0, 0, 0, 0};
    Vector2 spriteSheetGridSize{};
    float rotation{0.0f};
    float frameCellWidth{0.0f};
    float frameCellHeight{0.0f};
    float scale{1.0f};
    Vector2 origin{0.f, 0.f}; // origin stored in pixel units (scaled at draw time)
    // Per-facing offsets (defaults chosen to match provided example)
    Vector2 attachmentOffsetRight{35.f, 55.f};
    Vector2 attachmentOffsetLeft{25.f, 55.f};
    Rectangle collisionRect{0.f, 0.f, 0.f, 0.f};
    int facing{1}; // 1 = facing right (default), -1 = facing left (flipped)
};
