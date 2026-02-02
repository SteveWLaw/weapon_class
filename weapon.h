#include "raylib.h"

class Weapon
{
public:
    // Constructor that uses a shared texture pointer (does not take ownership)
    Weapon(Texture2D *sharedTexture, Vector2 position, Vector2 sheetSize = {10, 3}, float scale = 1.0f)
        : matrixSize(sheetSize), scale(scale)
    {
        texture = sharedTexture;

        // Calculate single-cell width/height
        width = static_cast<float>(texture->width) / matrixSize.x;
        height = static_cast<float>(texture->height) / matrixSize.y;

        // Use column * cellWidth and row * cellHeight
        sourceRec = {static_cast<int>(position.x) * width,
                     static_cast<int>(position.y) * height,
                     width, height};

        origin = {0.f, height}; // bottom-left by default (x=0, y=height)
    }

    // Convenience constructor: choose a frame by flat index (row-major) using shared texture
    Weapon(Texture2D *sharedTexture, int frameIndex, Vector2 sheetSize = {10, 3}, float scale = 1.0f)
        : matrixSize(sheetSize), scale(scale)
    {
        texture = sharedTexture;

        width = static_cast<float>(texture->width) / matrixSize.x;
        height = static_cast<float>(texture->height) / matrixSize.y;

        int cols = static_cast<int>(matrixSize.x);
        int col = frameIndex % cols;
        int row = frameIndex / cols;

        sourceRec = {col * width, row * height, width, height};
        origin = {0.f, height};
    }

    ~Weapon() = default;

    // Single Draw API: draw the weapon relative to a warrior position and a facing (-1 left, 1 right)
    void Draw(Vector2 warriorPos, int facing, bool showBounds = false);

    // Offsets (per-facing) determine how the weapon is positioned relative to the warrior
    const void setOffsets(Vector2 rightOffset, Vector2 leftOffset) { offsetRight = rightOffset; offsetLeft = leftOffset; }

    // Compute collision rectangle for a warrior position and facing WITHOUT mutating internal state
    Rectangle computeCollisionRec(Vector2 warriorPos, int facing) const;

    const Rectangle &getCollisionRec() const { return collisionRec; }

    const void setRotation(float newRotation) { rotation = newRotation; }
    const void setScale(float newScale) { scale = newScale; }
    const void setOrigin(Vector2 newOrigin) { origin = newOrigin; }
    const void setFacing(int f) { facing = (f >= 0 ? 1 : -1); }
    const int getFacing() const { return facing; }
    const Vector2 getOrigin() const { return origin; }
    const float getWidth() const { return width * scale; }
    const float getHeight() const { return height * scale; }
    const float getscale() const { return scale; }

private:
    Texture2D *texture{nullptr};
    Rectangle sourceRec{0, 0, 0, 0};
    Vector2 matrixSize{};
    float rotation{0.0f};
    float width{0.0f};
    float height{0.0f};
    float scale{1.0f};
    Vector2 origin{0.f, 0.f}; // origin stored in pixel units (scaled at draw time)
    // Per-facing offsets (defaults chosen to match provided example)
    Vector2 offsetRight{35.f, 55.f};
    Vector2 offsetLeft{25.f, 55.f};
    Rectangle collisionRec{0.f, 0.f, 0.f, 0.f};
    int facing{1}; // 1 = facing right (default), -1 = facing left (flipped)
};
