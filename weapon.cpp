#include "weapon.h"
#include <cmath>
#include <cfloat>

void Weapon::Draw(Vector2 warriorPos, int facing, bool showBounds)
{
    int facingDirection = (facing >= 0) ? 1 : -1;

    float srcW = sourceRect.width;
    float srcH = sourceRect.height;
    float w = srcW * scale;
    float h = srcH * scale;

    Vector2 offset = (facingDirection >= 0) ? attachmentOffsetRight : attachmentOffsetLeft;
    Vector2 drawOrigin = (facingDirection >= 0) ? Vector2{origin.x * scale, origin.y * scale} : Vector2{(srcW - origin.x) * scale, origin.y * scale};

    // Apply thrust offset if using thrust attack
    Vector2 thrustMovement{0.f, 0.f};
    if (attackType == AttackType::Thrust)
    {
        thrustMovement = {thrustOffset * facingDirection, 0.f};
    }

    // Pivot is the attachment point where weapon connects to warrior
    Vector2 pivot = {warriorPos.x + offset.x + thrustMovement.x, warriorPos.y + offset.y + thrustMovement.y};

    // Destination top-left: position so pivot aligns with drawOrigin
    float destX = pivot.x - drawOrigin.x;
    float destY = pivot.y - drawOrigin.y;

    // For drawing only: offset texture down by its height
    float drawDestY = destY + h;

    // For left-facing, flip the source rectangle width to mirror the texture (keep same x to use same sprite)
    Rectangle srcRect = (facingDirection >= 0) ? sourceRect : Rectangle{sourceRect.x, sourceRect.y, -sourceRect.width, sourceRect.height};

    // When source width is negative, destination x needs adjustment
    float drawDestX = (facingDirection >= 0) ? destX : destX + w;

    Rectangle dest = {drawDestX, drawDestY, w, h};

    DrawTexturePro(*texture, srcRect, dest, drawOrigin, rotation, WHITE);

    // Compute and update collision rectangle (AABB of rotated sprite if rotation exists)
    collisionRect = computeCollisionRec(warriorPos, facing);

    if (showBounds)
    {
        // draw AABB collision rectangle (red)
        DrawRectangleLinesEx(collisionRect, 2.0f, RED);

        // draw pivot point (attachment) and draw origin marker
        DrawCircleV(pivot, 4.0f, {0, 255, 255, 255});
        DrawCircleV(Vector2{destX + drawOrigin.x, destY + drawOrigin.y}, 3.0f, BLUE);
    }
}

Rectangle Weapon::computeCollisionRec(Vector2 warriorPos, int facing) const
{
    int facingDirection = (facing >= 0) ? 1 : -1;
    float srcW = sourceRect.width;
    float srcH = sourceRect.height;
    float w = srcW * scale;
    float h = srcH * scale;

    Vector2 offset = (facingDirection >= 0) ? attachmentOffsetRight : attachmentOffsetLeft;

    // Pivot (attachment point) in world-space
    Vector2 pivot = {warriorPos.x + offset.x, warriorPos.y + offset.y};

    // Determine drawOrigin (mirrored for facing) in destination-space
    Vector2 drawOrigin = (facingDirection >= 0) ? Vector2{origin.x * scale, origin.y * scale} : Vector2{(srcW - origin.x) * scale, origin.y * scale};

    // Destination top-left so that pivot == dest + drawOrigin
    float destX = pivot.x - drawOrigin.x;
    float destY = pivot.y - drawOrigin.y;

    // Corners of the unrotated dest rectangle (use positive width/height for AABB calc)
    Vector2 corners[4] = {
        {destX, destY},           // top-left
        {destX + w, destY},       // top-right
        {destX + w, destY + h},   // bottom-right
        {destX, destY + h}        // bottom-left
    };

    // If rotation is effectively zero, return the simple rect
    if (std::abs(rotation) < 1e-3f)
    {
        return Rectangle{destX, destY, w, h};
    }

    // Rotate corners around pivot and compute AABB
    float rad = rotation * (3.14159265358979323846f / 180.0f);
    float c = std::cos(rad);
    float s = std::sin(rad);

    float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;

    for (int i = 0; i < 4; ++i)
    {
        float dx = corners[i].x - pivot.x;
        float dy = corners[i].y - pivot.y;
        float rx = pivot.x + dx * c - dy * s;
        float ry = pivot.y + dx * s + dy * c;
        if (rx < minX) minX = rx;
        if (ry < minY) minY = ry;
        if (rx > maxX) maxX = rx;
        if (ry > maxY) maxY = ry;
    }

    return Rectangle{minX, minY, maxX - minX, maxY - minY};
}
