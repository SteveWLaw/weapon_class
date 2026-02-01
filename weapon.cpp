#include "weapon.h"

void Weapon::Draw(Vector2 position, bool showBounds)
{
    // Use stored members (set via setters) when calling the full Draw implementation
    Draw(position, scale, rotation, origin, showBounds);
}

void Weapon::Draw(Vector2 position, float scale, float rotation, Vector2 origin, bool showBounds)
{
    Rectangle dest = { position.x, position.y, sourceRec.width * scale, sourceRec.height * scale };

    DrawTexturePro(
        *texture,
        sourceRec,
        dest,
        Vector2{ origin.x * scale, origin.y * scale },
        rotation,
        WHITE
    );

    // Optionally draw bounding rectangle for debugging
    if (showBounds)
    {
        DrawRectangleLinesEx(dest, 2.0f, RED);
    }
}
