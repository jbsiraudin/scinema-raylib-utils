/*******************************************************************************************
 *
 *   made with raylib - (@raysan5)
 *
 *   Custom render engine for image to fluid to image video transition - @jbsiraudin
 *
 ********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

 //----------------------------------------------------------------------------------
 // Local Variables Definition (local to this module)
 //----------------------------------------------------------------------------------
 // Render size for internal buffers
static const int renderWidth = 1920;
static const int renderHeight = 1080;

// init size of window, preview size not affecting render size
static const int screenWidth = 1920 / 2;
static const int screenHeight = 1080 / 2;


//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ClearRenderTexture(RenderTexture2D* target);
static void WriteImageFromTexture(Texture2D* texture, const char* filename);


//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Fluid Transition Tool");

    // -- input images --
    Image inImage = LoadImage("./resources/input/in_image2.png");
    ImageResize(&inImage, renderWidth, renderHeight);
    Texture2D inTexture = LoadTextureFromImage(inImage);
    UnloadImage(inImage);

    // Load noise textures
    //--------------------------------------------------------------------------------------
    char* noiseTexturesNames = "Bayer;Blue Noise;Gray Noise;Small Gray Noise";

    Image bayer = LoadImage("./resources/textures/bayer.png");
    Image blueNoise = LoadImage("./resources/textures/bluenoise.png");
    Image grayNoise = LoadImage("./resources/textures/graynoise.png");
    Image grayNoiseSmall = LoadImage("./resources/textures/graynoisesmall.png");

    Texture2D noiseTextures[4] = {
        LoadTextureFromImage(bayer),
        LoadTextureFromImage(blueNoise),
        LoadTextureFromImage(grayNoise),
        LoadTextureFromImage(grayNoiseSmall),
    };

    float noiseTexturesResolutions[4][2] = {
        {(float)noiseTextures[0].width, (float)noiseTextures[0].height},
        {(float)noiseTextures[1].width, (float)noiseTextures[1].height},
        {(float)noiseTextures[2].width, (float)noiseTextures[2].height},
        {(float)noiseTextures[3].width, (float)noiseTextures[3].height},
    };

    UnloadImage(bayer);
    UnloadImage(blueNoise);
    UnloadImage(grayNoise);
    UnloadImage(grayNoiseSmall);

    // Create render targets
    //--------------------------------------------------------------------------------------

    // Buffer to use as the user canvas to define the initial "engine" of the sim
    RenderTexture2D bufferFluidEngineInput = LoadRenderTexture(renderWidth, renderHeight);
    // ClearRenderTexture(&bufferFluidEngineInput);

    // Buffer for the user canvas to define static areas
    RenderTexture2D bufferFluidStaticInput = LoadRenderTexture(renderWidth, renderHeight);

    // Buffers for the fake fluid dynamics shader
    // canvas for the main buffer input that loops on itself
    RenderTexture2D bufferFluidDynamicsCanvas = LoadRenderTexture(renderWidth, renderHeight);
    RenderTexture2D bufferFluidDynamicsCanvasCopy = LoadRenderTexture(renderWidth, renderHeight);
    // Advection field for the buffer to bake
    RenderTexture2D bufferFluidDynamicsDisplacement = LoadRenderTexture(renderWidth, renderHeight);

    // Load shaders and uniform locations
    //--------------------------------------------------------------------------------------

    // Fluid simulation shader
    Shader fluidDynamicsShader = LoadShader(0, "resources/shaders/simple_cfd.frag");
    Shader baseShader = LoadShader(0, "resources/shaders/base.frag");

    // Get shader locations for required uniforms
    int iFrameLoc = GetShaderLocation(fluidDynamicsShader, "iFrame");
    // No need for loc for the first sampler2D, we're using the default slot of shader mode
    // int iChannel0Loc = GetShaderLocation(fluidDynamicsShader, "iChannel0");
    int iChannel1Loc = GetShaderLocation(fluidDynamicsShader, "iChannel1");
    int iChannel2Loc = GetShaderLocation(fluidDynamicsShader, "iChannel2");
    int iChannelResolution0Loc = GetShaderLocation(fluidDynamicsShader, "iChannelResolution0");
    int iChannelResolution1Loc = GetShaderLocation(fluidDynamicsShader, "iChannelResolution1");
    int iRotNumLoc = GetShaderLocation(fluidDynamicsShader, "iRotNum");
    int iItNumLoc = GetShaderLocation(fluidDynamicsShader, "iItNum");
    int iFlowStrengthLoc = GetShaderLocation(fluidDynamicsShader, "iFlowStrength");
    int iFlowDirectionLoc = GetShaderLocation(fluidDynamicsShader, "iFlowDirection");

    float renderResolution[2] = { (float)renderWidth, (float)renderHeight };
    SetShaderValue(fluidDynamicsShader, iChannelResolution0Loc, renderResolution, SHADER_UNIFORM_VEC2);

    int iRotNum = 5;
    int iItNum = 20;
    float iFlowStrength = 2.0f;
    float iFlowDirection[2] = { -1.0f, 1.0f };
    //--------------------------------------------------------------------------------------
    int frameIndex = 0;
    int noiseTexturesIndex = 1;
    bool dropDownEditMode = false;
    const int marginLeft = 100;
    //--------------------------------------------------------------------------------------

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(24); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        // Set shader required uniform values
        SetShaderValue(fluidDynamicsShader, iFrameLoc, &frameIndex, SHADER_UNIFORM_INT);
        SetShaderValue(fluidDynamicsShader, iChannelResolution1Loc, noiseTexturesResolutions[noiseTexturesIndex], SHADER_UNIFORM_VEC2);
        SetShaderValue(fluidDynamicsShader, iRotNumLoc, &iRotNum, SHADER_UNIFORM_INT);
        SetShaderValue(fluidDynamicsShader, iItNumLoc, &iItNum, SHADER_UNIFORM_INT);
        SetShaderValue(fluidDynamicsShader, iFlowStrengthLoc, &iFlowStrength, SHADER_UNIFORM_FLOAT);
        SetShaderValue(fluidDynamicsShader, iFlowDirectionLoc, &iFlowDirection, SHADER_UNIFORM_VEC2);

        // Write inside our buffers
        BeginTextureMode(bufferFluidDynamicsCanvasCopy);
        {
            //DrawTexture(bufferFluidDynamicsCanvas.texture, 0, 0, WHITE);
            DrawTextureRec(bufferFluidDynamicsCanvas.texture, (Rectangle) { 0, 0, (float)bufferFluidDynamicsCanvas.texture.width, (float)-bufferFluidDynamicsCanvas.texture.height }, (Vector2) { 0, 0 }, WHITE);
        }
        EndTextureMode();


        BeginTextureMode(bufferFluidDynamicsCanvas);
        {
            BeginShaderMode(fluidDynamicsShader);
            {
                // We are drawing bufferFluidDynamicsCanvas.texture using default sampler2D iChannel0 but
                // an additional texture units is enabled for other sampler2D
                DrawTexture(bufferFluidDynamicsCanvasCopy.texture, 0, 0, WHITE);

                // WARNING: Additional samplers are enabled for all draw calls in the batch,
                // EndShaderMode() forces batch drawing and consequently resets active textures
                // to let other sampler2D to be activated on consequent drawings (if required)
                // SetShaderValue(fluidDynamicsShader, iChannel1Loc, &noiseTextures[noiseTexturesIndex], SHADER_UNIFORM_SAMPLER2D);
                // SetShaderValueTexture(fluidDynamicsShader, iChannel0Loc, bufferFluidDynamicsCanvas.texture);
                SetShaderValueTexture(fluidDynamicsShader, iChannel1Loc, noiseTextures[noiseTexturesIndex]);
                SetShaderValueTexture(fluidDynamicsShader, iChannel2Loc, inTexture);
            }
            EndShaderMode();
        }
        EndTextureMode();

        // Draw
        //----------------------------------------------------------------------------------

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            // Draw render texture
            DrawTexture(bufferFluidDynamicsCanvas.texture, 0, 0, WHITE);
            //DrawTextureRec(bufferFluidDynamicsCanvas.texture, (Rectangle) { 0, 0, (float)bufferFluidDynamicsCanvas.texture.width, (float)-bufferFluidDynamicsCanvas.texture.height }, (Vector2) { 0, 0 }, WHITE);

            /* BeginShaderMode(baseShader);
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), GREEN);
            EndShaderMode(); */

            // Draw GUI controls
            //------------------------------------------------------------------------------
            Rectangle rect = (Rectangle){ GetScreenWidth() - 260, 10, 250, 200 };
            DrawRectangle(rect.x, rect.y, rect.width, rect.height, RAYWHITE);
            DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, Fade(LIGHTGRAY, 0.6f));

            DrawTextureEx(noiseTextures[noiseTexturesIndex], (Vector2) { rect.x + marginLeft, rect.y + 40 }, 0.0f, 60.0f / noiseTexturesResolutions[noiseTexturesIndex][0], RAYWHITE);
            //------------------------------------------------------------------------------
            // Check all possible events that require GuiLock
            if (dropDownEditMode)
            {
                GuiLock();
            }
            GuiSetStyle(SPINNER, TEXT_PADDING, 4);
            GuiSpinner((Rectangle) { rect.x + marginLeft, rect.y + 110, 120, 20 }, "Rotational Steps", & iRotNum, 0, 10, false);
            GuiSpinner((Rectangle) { rect.x + marginLeft, rect.y + 140, 120, 20 }, "Simulation Steps", & iItNum, 1, 50, false);
            GuiSetStyle(SLIDER, TEXT_PADDING, 4);
            GuiSliderBar((Rectangle) { rect.x + marginLeft, rect.y + 170, 120, 20 }, "Flow Strength", TextFormat("%.2f", iFlowStrength), & iFlowStrength, 0.0f, 5.0f);

            GuiUnlock();
            GuiSetStyle(DROPDOWNBOX, TEXT_PADDING, 4);
            GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
            GuiSetStyle(LABEL, TEXT_PADDING, 1);
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
            GuiLabel((Rectangle) { rect.x + marginLeft - 120, rect.y + 10, 120, 20 }, "Noise Type");
            if (GuiDropdownBox((Rectangle) { rect.x + marginLeft, rect.y + 10, 120, 20 }, noiseTexturesNames, & noiseTexturesIndex, dropDownEditMode))
            {
                dropDownEditMode = !dropDownEditMode;
            }
            //------------------------------------------------------------------------------

            DrawText(TextFormat("Shader is ready %i", IsShaderReady(fluidDynamicsShader)), 10, 30, 10, GRAY);

            DrawFPS(10, 10);
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
        frameIndex++;
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(fluidDynamicsShader);
    UnloadShader(baseShader);
    for (int i = 0; i < 4; i++)
    {
        UnloadTexture(noiseTextures[i]);
    }
    UnloadRenderTexture(bufferFluidEngineInput);
    UnloadRenderTexture(bufferFluidStaticInput);
    UnloadRenderTexture(bufferFluidDynamicsCanvas);
    UnloadRenderTexture(bufferFluidDynamicsDisplacement);
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

static void ClearRenderTexture(RenderTexture2D* target)
{
    BeginTextureMode(*target);
    ClearBackground(BLANK);
    EndTextureMode();
}

static void WriteImageFromTexture(Texture2D* texture, const char* filename)
{
    ExportImage(LoadImageFromTexture(*texture), TextFormat("export/%s", filename));
}