/*******************************************************************************************
*
*   raylib game template
*
*   <Game title>
*   <Game description>
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include "easings.h"

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
#define maxTextureCount 5000
#define DEBUG 1
#define FIXED_COLUMNS_NUMBER 28
static int shotsCount = 0;

static int textureWidth = 0;
static int textureHeight = 0;
static float textureScale = 0;

static int stepWidth = 0;
static int stepHeight = 0;
static float objectSpacing = 10.f;

static int columnsCount = 0;
static int rowsCount = 0;

static Texture2D shotsTextures[maxTextureCount];
static RenderTexture2D bufferCanvas;

static const int screenWidth = 1280;
static const int screenHeight = 720;

static int renderWidth = 1920;
static int renderHeight = 1080;

//----------------------------------------------------------------------------------

static int animPlayerLength = 24 * 5;

static int yAnimOffset = 100;
static int yAnimLength = 24;
static int yAnimEaseType = 1;

static int fadeAnimLength = 24;
static int fadeEaseType = 0;

static float perObjectShift = 200.f;
static float perObjectShiftPower = 200.f;

static float objectStrokeWidth = 0.f;
static Color objectStrokeColor = {
    253, 249, 0, 255
};

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ClearRenderTexture(RenderTexture2D* target);
static void WriteImageFromTexture(Texture2D* texture, const char* filename);
static void InitRenderParameters(char* filepathImg);
static void ComputeLayout();
static int ComputeColumnCount(int renderWidth, int renderHeight, int textureWidth, int textureHeight);
static void DrawAnimTexture(int frameIndex);
static void ButtonExport(int animLength);


//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TRANSPARENT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Movie Shots Maker");

    FilePathList shotsFilePaths = LoadDirectoryFiles("D:/01 - SCINEMA/21 - Harry Potter/04 - Animation Assets/2 - All Shots HP3/HP3_shots");
    shotsCount = shotsFilePaths.count;

    char* filepathFirstImage = shotsFilePaths.paths[0];
    InitRenderParameters(filepathFirstImage);

    bufferCanvas = LoadRenderTexture(renderWidth, renderHeight);

#if (DEBUG == 0)
    for (unsigned int i = 0; i < shotsCount; i++)
    {
        char* filepath = shotsFilePaths.paths[i];
        Image img = LoadImage(filepath);
        ImageResize(&img, (int)(textureWidth * 0.2f), (int)(textureHeight * 0.2f));
        shotsTextures[i] = LoadTextureFromImage(img);

        UnloadImage(img);
    }    
#endif // DEBUG == 0
    

    // layout_name: controls initialization
    //----------------------------------------------------------------------------------
    bool windowBoxMainActive = true;
    bool animPlayerLengthValueBoxEditMode = false;
    bool yAnimOffsetValueBoxEditMode = false;
    bool yAnimLengthValueBoxEditMode = false;
    bool yAnimEaseDropdownBoxEditMode = false;
    bool fadeAnimLengthValueBoxEditMode = false;
    bool fadeEaseDropdownBoxEditMode = false;

    char* easingTypes = "OUT_SIN;OUT_CUBIC";

    bool animPlayerIsPlaying = true;
    float animPlayerFrame = 0.f;
    //----------------------------------------------------------------------------------

    SetTargetFPS(24);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        Vector2 anchorWindow = { GetScreenWidth() - 304, 0 };
        Vector2 anchorLayoutBox = { anchorWindow.x + 16, anchorWindow.y + 48 }; // 592, 96
        Vector2 anchorAnimBox = { anchorWindow.x + 16, anchorWindow.y + 256 };
        Vector2 anchorSequenceBox = { anchorWindow.x + 16, anchorWindow.y + 496 + 48 };

        if (animPlayerIsPlaying)
        {
            if (animPlayerFrame < animPlayerLength) {
                animPlayerFrame += 1.f;
            }
            else
            {
                animPlayerFrame = 0.f;
            }
        }


        //----------------------------------------------------------------------------------
        // Draw in render buffer
        //----------------------------------------------------------------------------------
        ComputeLayout();
        DrawAnimTexture(animPlayerFrame);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            ClearBackground(BLACK);

            // Draw render texture
            float aspectRatio = (float)renderHeight / (float)renderWidth;
            int previewWidth = min(GetScreenWidth(), (int)((float)GetScreenHeight() / aspectRatio));
            DrawTexturePro(bufferCanvas.texture, (Rectangle) { 0, 0, (float)bufferCanvas.texture.width, (float)-bufferCanvas.texture.height }, (Rectangle) { 0, 0, previewWidth, previewWidth * aspectRatio }, (Vector2) { 0, 0 }, 0.f, WHITE);
            
            {
                // raygui: controls drawing
                //----------------------------------------------------------------------------------
                if (windowBoxMainActive)
                {
                    int baseAnimPlayerSize = 24;

                    windowBoxMainActive = !GuiWindowBox((Rectangle) { anchorWindow.x + 0, anchorWindow.y + 0, 304, GetScreenHeight() - baseAnimPlayerSize }, "OPTIONS");
                    
                    if (yAnimEaseDropdownBoxEditMode || fadeEaseDropdownBoxEditMode) GuiLock();

                    // Layout settings
                    GuiGroupBox((Rectangle) { anchorLayoutBox.x + 0, anchorLayoutBox.y + 0, 272, 176 }, "Layout settings");
                    GuiSliderBar((Rectangle) { anchorLayoutBox.x + 136, anchorLayoutBox.y + 16, 120, 16 }, TextFormat("Spacing (%.1f)", objectSpacing), NULL, & objectSpacing, 0.f, 50.f);
                    GuiSliderBar((Rectangle) { anchorLayoutBox.x + 136, anchorLayoutBox.y + 40, 120, 16 }, "Stroke Width (0-10)", NULL, & objectStrokeWidth, 0, 10.f);
                    GuiLabel((Rectangle) { anchorLayoutBox.x + 32, anchorLayoutBox.y + 104, 88, 24 }, "Stroke Color");
                    GuiColorPicker((Rectangle) { anchorLayoutBox.x + 136, anchorLayoutBox.y + 72, 88, 88 }, "Stroke Color", & objectStrokeColor);

                    // Anim settings
                    GuiGroupBox((Rectangle) { anchorAnimBox.x + 0, anchorAnimBox.y + 0, 272, 208 + 48 }, "Animation settings");
                    if (GuiSpinner((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 16, 120, 24 }, "Y Animation Length ", & yAnimLength, 0, animPlayerLength, yAnimLengthValueBoxEditMode)) yAnimLengthValueBoxEditMode = !yAnimLengthValueBoxEditMode;
                    GuiLabel((Rectangle) { anchorAnimBox.x + 32, anchorAnimBox.y + 48, 120, 24 }, "Y Animation Ease");
                    if (GuiSpinner((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 80, 120, 24 }, "Y Animation Offset ", & yAnimOffset, 0, 2000, yAnimOffsetValueBoxEditMode)) yAnimOffsetValueBoxEditMode = !yAnimOffsetValueBoxEditMode;
                    
                    GuiLine((Rectangle) { anchorAnimBox.x + 16, anchorAnimBox.y + 112, 240, 16 }, NULL);
                
                    if (GuiSpinner((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 136, 120, 24 }, "Fade Length ", & fadeAnimLength, 0, animPlayerLength, fadeAnimLengthValueBoxEditMode)) fadeAnimLengthValueBoxEditMode = !fadeAnimLengthValueBoxEditMode;
                    GuiLabel((Rectangle) { anchorAnimBox.x + 64, anchorAnimBox.y + 168, 80, 24 }, "Fade Ease");

                    GuiLine((Rectangle) { anchorAnimBox.x + 16, anchorAnimBox.y + 200, 240, 16 }, NULL);

                    GuiSliderBar((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 224, 120, 16 }, TextFormat("Per object shift (%d)", (int)perObjectShift), NULL, & perObjectShift, 0.f, 1000.f);
                    
                    // Sequence settings
                    GuiGroupBox((Rectangle) { anchorSequenceBox.x + 0, anchorSequenceBox.y + 0, 272, 56 }, "Sequence settings");
                    if (GuiSpinner((Rectangle) { anchorSequenceBox.x + 136, anchorSequenceBox.y + 16, 120, 24 }, "Length (frames) ", & animPlayerLength, 0, 100000, animPlayerLengthValueBoxEditMode)) animPlayerLengthValueBoxEditMode = !animPlayerLengthValueBoxEditMode;
                    
                    if (GuiButton((Rectangle) { anchorWindow.x + 16, anchorWindow.y + 578 + 48, 272, 32 }, "Export animation")) ButtonExport(animPlayerLength);
                    
                    if (GuiDropdownBox((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 48, 120, 24 }, easingTypes, & yAnimEaseType, yAnimEaseDropdownBoxEditMode)) yAnimEaseDropdownBoxEditMode = !yAnimEaseDropdownBoxEditMode;
                    if (GuiDropdownBox((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 168, 120, 24 }, easingTypes, & fadeEaseType, fadeEaseDropdownBoxEditMode)) fadeEaseDropdownBoxEditMode = !fadeEaseDropdownBoxEditMode;

                    // Animation player
                    GuiStatusBar((Rectangle) { 0, GetScreenHeight() - baseAnimPlayerSize, 48, baseAnimPlayerSize }, TextFormat("%d", (int)animPlayerFrame));
                    if (GuiButton((Rectangle) { 48, GetScreenHeight() - baseAnimPlayerSize, baseAnimPlayerSize, baseAnimPlayerSize }, TextFormat("#%i#", animPlayerIsPlaying ? ICON_PLAYER_PAUSE : ICON_PLAYER_PLAY))) animPlayerIsPlaying = !animPlayerIsPlaying;
                    if (GuiButton((Rectangle) { 48 + baseAnimPlayerSize, GetScreenHeight() - baseAnimPlayerSize, baseAnimPlayerSize, baseAnimPlayerSize }, TextFormat("#%i#", ICON_RESTART))) animPlayerFrame = 0;
                    GuiSlider((Rectangle) { 48 + baseAnimPlayerSize * 2, GetScreenHeight() - baseAnimPlayerSize, GetScreenWidth() - baseAnimPlayerSize * 2, baseAnimPlayerSize }, NULL, NULL, &animPlayerFrame, 0.f, (float)animPlayerLength);
                }

                GuiUnlock();
                //----------------------------------------------------------------------------------
            }

            DrawFPS(10, 10);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload current screen data before closing
    for (int i = 0; i < shotsCount; i++)
    {
        UnloadTexture(shotsTextures[i]);
    }
    UnloadRenderTexture(bufferCanvas);

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
static void ClearRenderTexture(RenderTexture2D* target)
{
    BeginTextureMode(*target);
    ClearBackground(BLANK);
    EndTextureMode();
}

static void WriteImageFromTexture(Texture2D* texture, const char* filename)
{
    Image imgToExport = LoadImageFromTexture(*texture);
    ExportImage(imgToExport, TextFormat("./export/%s", filename));
    UnloadImage(imgToExport);
}

static void InitRenderParameters(char* filepathImg)
{
    Image img = LoadImage(filepathImg);
    textureWidth = img.width;
    textureHeight = img.height;
    UnloadImage(img);
#if (FIXED_COLUMNS_NUMBER == 0)
    columnsCount = ComputeColumnCount(renderWidth, renderHeight, textureWidth, textureHeight);
#else
    columnsCount = FIXED_COLUMNS_NUMBER;
#endif // (FIXED_COLUMNS_NUMBER == 0)

    ComputeLayout();
}

static void ComputeLayout()
{
#if (FIXED_COLUMNS_NUMBER == 0)
    stepWidth = (int)floorf((float)renderWidth / (float)columnsCount);
    stepHeight = (int)ceilf(((float)stepWidth / (float)textureWidth) * (float)textureHeight);
    textureScale = ((float)stepWidth / (float)textureWidth);
#else
    int rowsCount = (int)floorf((float)shotsCount / (float)columnsCount);
    stepWidth = (int)floorf(((float)renderWidth - (float)(columnsCount - 1) * objectSpacing) / (float)columnsCount);
    stepHeight = (int)ceilf(((float)stepWidth / (float)textureWidth) * (float)textureHeight);
    textureScale = ((float)stepWidth / (float)textureWidth);

    int newRenderHeight = (int)((stepHeight + objectSpacing) * rowsCount - objectSpacing) + 200;

    if (newRenderHeight != renderHeight)
    {
        renderHeight = newRenderHeight;
        UnloadRenderTexture(bufferCanvas);
        bufferCanvas = LoadRenderTexture(renderWidth, renderHeight);
    }
#endif // (FIXED_COLUMNS_NUMBER == 0)
}

static int ComputeColumnCount(int renderWidth, int renderHeight, int textureWidth, int textureHeight)
{
    for (unsigned int i = 1; i < 100; i++)
    {
        int stepWidth = (int)floorf((float)renderWidth / (float)i);
        float textureScale = (float)stepWidth / (float)textureWidth;
        int stepHeight = (int)ceilf(textureScale * (float)textureHeight);
        
        int y = (1 + (int)floorf((float)shotsCount / (float)i)) * stepHeight;

        if (y < renderHeight)
        {
            return i;
        }
        else
        {
            continue;
        }
    }
}

static void DrawAnimTexture(int frameIndex)
{
    ClearRenderTexture(&bufferCanvas);
    BeginTextureMode(bufferCanvas);
    {
        for (unsigned int i = 0; i < shotsCount; i++)
        {
            float elementAnimKeyframe = frameIndex - i * pow(perObjectShift / 5000.f, 2.f);
            float offset = objectStrokeWidth * 0.5f;

            float fadeAnimInterpolator = Clamp(elementAnimKeyframe / (float)fadeAnimLength, 0.f, 1.f);
            float fadeAnimProgress = fadeEaseType == 0 ? EaseOutSin(fadeAnimInterpolator) : EaseOutCubic(fadeAnimInterpolator);

            float opacity = Lerp(0, 255, fadeAnimProgress);

            float yAnimInterpolator = Clamp(elementAnimKeyframe / (float)yAnimLength, 0.f, 1.f);
            float yAnimProgress = yAnimEaseType == 0 ? EaseOutSin(yAnimInterpolator) : EaseOutCubic(yAnimInterpolator);

            int columnIndex = i % columnsCount;
            int rowIndex = (int)floorf((float)i / (float)columnsCount);

            int x = (columnIndex) * (stepWidth + objectSpacing);
            int y = (rowIndex) * (stepHeight + objectSpacing) + Lerp((float)yAnimOffset, 0.f, yAnimProgress) + offset * 2.f;

            int width = textureWidth * textureScale;
            int height = textureHeight * textureScale;

            Rectangle rectObject = { x, y, width, height };
#if (DEBUG == 0)
            Color tintColor = { WHITE.r, WHITE.g, WHITE.b, opacity };
            DrawTexturePro(shotsTextures[i], (Rectangle) { 0, 0, textureWidth * 0.2f, textureHeight * 0.2f }, rectObject, (Vector2) { 0, 0 }, 0.f, tintColor);
            //DrawTexture(shotsTextures[i], x, y, WHITE);
#else
            DrawRectangleRec(rectObject, (Color) { RED.r, RED.g, RED.b, opacity });
#endif // DEBUG

            Color strokeColor = { objectStrokeColor.r, objectStrokeColor.g, objectStrokeColor.b, opacity };
            DrawRectangleLinesEx((Rectangle) { rectObject.x - offset, rectObject.y - offset, rectObject.width + offset * 2.f, rectObject.height + offset * 2.f }, objectStrokeWidth, strokeColor);
        }
    }
    EndTextureMode();
}

static void ButtonExport(int animLength)
{
    RenderTexture2D bufferRender = LoadRenderTexture(renderWidth, renderHeight);
    for (unsigned int i = 0; i < animLength; i++)
    {
        DrawAnimTexture(i);
        ClearRenderTexture(& bufferRender);
        BeginTextureMode(bufferRender);
        {
            DrawTexture(bufferCanvas.texture, 0, 0, WHITE);
            // DrawTexturePro(bufferCanvas.texture, (Rectangle) { 0, 0, (float)bufferCanvas.texture.width, (float)bufferCanvas.texture.height }, (Rectangle) { 0, 0, (float)renderWidth, (float)renderHeight }, (Vector2) { 0, 0 }, 0.f, WHITE);
        }
        EndTextureMode();

        WriteImageFromTexture(&bufferRender.texture, TextFormat("anim_%05d.png", i));
    }
}