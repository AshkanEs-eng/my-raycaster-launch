#include "raylib.h"
#include <math.h>

// ====================
// تنظیمات نقشه
// ====================
#define MAP_WIDTH 8
#define MAP_HEIGHT 8

#define MINIMAP_TILE_SIZE 20
#define MINIMAP_OFFSET_X 10
#define MINIMAP_OFFSET_Y 40

// ====================
// Game State
// ====================
typedef enum
{
    STATE_PLAY,
    STATE_EDIT
} GameState;

GameState currentState = STATE_PLAY;

// ====================
// وضعیت بازیکن
// ====================
Vector2 playerPos = {1.5f, 5.5f};
Vector2 playerDir = {0.0f, -1.0f};
Vector2 cameraPlane = {0.66f, 0.0f};

// ====================
// حرکت
// ====================
float playerSpeed = 2.0f;
float rotationSpeed = 2.0f;

// ====================
// نقشه
// ====================
int worldMap[MAP_HEIGHT][MAP_WIDTH] =
    {
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1}};

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Raycasting Project - Final");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // ====================
        // تغییر State
        // ====================
        if (IsKeyPressed(KEY_M))
        {
            currentState = (currentState == STATE_PLAY) ? STATE_EDIT : STATE_PLAY;
        }

        // ====================
        // PLAY MODE
        // ====================
        if (currentState == STATE_PLAY)
        {
            float moveSpeed = playerSpeed * deltaTime;
            float rot = rotationSpeed * deltaTime;

            // چرخش
            if (IsKeyDown(KEY_A))
            {
                float oldDirX = playerDir.x;
                playerDir.x = playerDir.x * cosf(rot) - playerDir.y * sinf(rot);
                playerDir.y = oldDirX * sinf(rot) + playerDir.y * cosf(rot);

                float oldPlaneX = cameraPlane.x;
                cameraPlane.x = cameraPlane.x * cosf(rot) - cameraPlane.y * sinf(rot);
                cameraPlane.y = oldPlaneX * sinf(rot) + cameraPlane.y * cosf(rot);
            }

            if (IsKeyDown(KEY_D))
            {
                float oldDirX = playerDir.x;
                playerDir.x = playerDir.x * cosf(-rot) - playerDir.y * sinf(-rot);
                playerDir.y = oldDirX * sinf(-rot) + playerDir.y * cosf(-rot);

                float oldPlaneX = cameraPlane.x;
                cameraPlane.x = cameraPlane.x * cosf(-rot) - cameraPlane.y * sinf(-rot);
                cameraPlane.y = oldPlaneX * sinf(-rot) + cameraPlane.y * cosf(-rot);
            }

            // حرکت جلو / عقب
            if (IsKeyDown(KEY_W))
            {
                float nextX = playerPos.x + playerDir.x * moveSpeed;
                float nextY = playerPos.y + playerDir.y * moveSpeed;

                if (worldMap[(int)playerPos.y][(int)nextX] == 0)
                    playerPos.x = nextX;
                if (worldMap[(int)nextY][(int)playerPos.x] == 0)
                    playerPos.y = nextY;
            }

            if (IsKeyDown(KEY_S))
            {
                float nextX = playerPos.x - playerDir.x * moveSpeed;
                float nextY = playerPos.y - playerDir.y * moveSpeed;

                if (worldMap[(int)playerPos.y][(int)nextX] == 0)
                    playerPos.x = nextX;
                if (worldMap[(int)nextY][(int)playerPos.x] == 0)
                    playerPos.y = nextY;
            }
        }

        // ====================
        // EDIT MODE
        // ====================
        if (currentState == STATE_EDIT)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                Vector2 mouse = GetMousePosition();

                int mw = MAP_WIDTH * MINIMAP_TILE_SIZE;
                int mh = MAP_HEIGHT * MINIMAP_TILE_SIZE;

                if (mouse.x >= MINIMAP_OFFSET_X &&
                    mouse.x < MINIMAP_OFFSET_X + mw &&
                    mouse.y >= MINIMAP_OFFSET_Y &&
                    mouse.y < MINIMAP_OFFSET_Y + mh)
                {
                    int mapX = (int)((mouse.x - MINIMAP_OFFSET_X) / MINIMAP_TILE_SIZE);
                    int mapY = (int)((mouse.y - MINIMAP_OFFSET_Y) / MINIMAP_TILE_SIZE);

                    // محافظت از دیوارهای مرزی
                    if (mapX > 0 && mapX < MAP_WIDTH - 1 &&
                        mapY > 0 && mapY < MAP_HEIGHT - 1)
                    {
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            worldMap[mapY][mapX] = 1;

                        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                            worldMap[mapY][mapX] = 0;
                    }
                }
            }
        }

        // ====================
        // رسم
        // ====================
        BeginDrawing();
        ClearBackground(BLACK);

        // --- Raycasting ---
        for (int x = 0; x < screenWidth; x++)
        {
            double cameraX = 2.0 * x / screenWidth - 1.0;
            double rayDirX = playerDir.x + cameraPlane.x * cameraX;
            double rayDirY = playerDir.y + cameraPlane.y * cameraX;

            int mapX = (int)playerPos.x;
            int mapY = (int)playerPos.y;

            double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);

            int stepX, stepY;
            double sideDistX, sideDistY;

            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (playerPos.x - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - playerPos.x) * deltaDistX;
            }

            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (playerPos.y - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - playerPos.y) * deltaDistY;
            }

            int hit = 0, side;

            while (!hit)
            {
                if (sideDistX < sideDistY)
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                if (worldMap[mapY][mapX] == 1)
                    hit = 1;
            }

            double perpWallDist =
                (side == 0) ? (sideDistX - deltaDistX)
                            : (sideDistY - deltaDistY);

            int lineHeight = (int)(screenHeight / perpWallDist);
            int drawStart = -lineHeight / 2 + screenHeight / 2;
            int drawEnd = lineHeight / 2 + screenHeight / 2;

            if (drawStart < 0)
                drawStart = 0;
            if (drawEnd >= screenHeight)
                drawEnd = screenHeight - 1;

            Color wallColor = BLUE;
            if (side == 1)
            {
                wallColor.r *= 0.7f;
                wallColor.g *= 0.7f;
                wallColor.b *= 0.7f;
            }

            DrawLine(x, 0, x, drawStart, SKYBLUE);
            DrawLine(x, drawStart, x, drawEnd, wallColor);
            DrawLine(x, drawEnd, x, screenHeight, DARKGREEN);
        }

        // --- MiniMap ---
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                DrawRectangle(
                    MINIMAP_OFFSET_X + x * MINIMAP_TILE_SIZE,
                    MINIMAP_OFFSET_Y + y * MINIMAP_TILE_SIZE,
                    MINIMAP_TILE_SIZE,
                    MINIMAP_TILE_SIZE,
                    worldMap[y][x] ? DARKGRAY : RAYWHITE);
                DrawRectangleLines(
                    MINIMAP_OFFSET_X + x * MINIMAP_TILE_SIZE,
                    MINIMAP_OFFSET_Y + y * MINIMAP_TILE_SIZE,
                    MINIMAP_TILE_SIZE,
                    MINIMAP_TILE_SIZE,
                    BLACK);
            }
        }

        // بازیکن روی MiniMap
        int px = MINIMAP_OFFSET_X + playerPos.x * MINIMAP_TILE_SIZE;
        int py = MINIMAP_OFFSET_Y + playerPos.y * MINIMAP_TILE_SIZE;
        DrawCircle(px, py, 4, RED);
        DrawLine(px, py, px + playerDir.x * 10, py + playerDir.y * 10, RED);

        DrawText(
            currentState == STATE_PLAY ? "PLAY MODE (M)" : "EDIT MODE (M)",
            10, 10, 20,
            currentState == STATE_PLAY ? GREEN : RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
