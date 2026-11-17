#include "Application.h"

#include "core/ConfigLoader.h"
#include "core/StringUtils.h"
#include "tools/Profiler.h"
#include "tools/ScopeMarker.h"
#include "world/EnemySpawner.h"
#include "world/LevelFile.h"

#include <raylib.h>

#include <algorithm>
#include <cstdlib>
#include <string>

namespace 
{
    constexpr float kAspect = static_cast<float>(Config::kWindowWidth) / Config::kWindowHeight;

    // Setup default camera point to see full platform with exact pitch.
    Vector3 DefaultCameraPoint() 
    {
        const float half = Config::kFloorSide * Config::kFloorTileSize * 0.5f;
        const BoundingBox floor{ Vector3{ -half, 0.0f, -half }, Vector3{ half, Config::kPlayerHeight, half } };
        return GameCamera::FitPosition(floor, Config::kCameraPitchDeg, Config::kCameraFovYDeg, kAspect);
    }
}

Application::Application()
    : AppWindow(Config::kWindowWidth, Config::kWindowHeight, Config::kWindowTitle, Config::kTargetFps),
      AppParams(),
      AppRegistry(AppParams),
      AppCommands(AppRegistry),
      AppConsole(AppCommands),
      AppEventLog(AppParams),
      AppEnemyModel(Config::kEnemyModelFile, Config::kEnemyHeight),
      AppWorld(AppEventLog, DefaultCameraPoint()),
      AppCamera(DefaultCameraPoint(), Config::kCameraFovYDeg, Config::kCameraSpeed),
      AppCameraController(AppCamera),
      AppEditor(AppWorld, AppCamera, AppEventLog) 
{
    // We setup the exit rule for the app on "ESC"
    // when we not in console.
    SetExitKey(KEY_NULL);

    // The spawner is a game object like any other: the world updates it in its normal loop.
    AppWorld.Add<EnemySpawner>(AppParams, AppWorld, AppEnemyModel);

    RegisterCommands();
 
    AppCamera.SetPosition(AppWorld.CameraSpawnPosition());
}

void Application::RegisterCommands() 
{
    AppCommands.Register("ReloadConfig", "Reload all parameters from the config file (same as F5)",
                         [this](const CommandProcessor::Args&) { return ReloadConfig(); });
    AppCommands.Register("clear", "Clear the console output",
                         [this](const CommandProcessor::Args&) { AppConsole.Clear(); return CommandResult{}; });
    AppCommands.Register("fps", "fps <n> - target frame rate, 0 = uncapped (for frame time measurements)",
                         [](const CommandProcessor::Args& args) 
                         {
                             if (args.size() != 1) return CommandResult{ false, "usage: fps <n>   (0 = uncapped)" };
                             char* end = nullptr;
                             const long value = std::strtol(args[0].c_str(), &end, 10);
                             if (end == args[0].c_str() || *end != '\0' || value < 0 || value > 10000) 
                                 return CommandResult{ false, "fps must be an integer 0..10000" };
                             SetTargetFPS(static_cast<int>(value));
                             return CommandResult{ true, value == 0 ? "frame rate uncapped" : "target fps = " + std::to_string(value) };
                         });
}

CommandResult Application::ReloadConfig() 
{
    ConfigLoadResult result = ConfigLoader::Load(Config::kConfigPath, AppParams);
    if (!result.fileFound) 
    {
        ConfigLoadResult fallback = ConfigLoader::Load("params.cfg", AppParams);
        if (fallback.fileFound) result = fallback;
    }

    AppEventLog.Log(result.Success()
        ? "Config reloaded (" + std::to_string(result.applied) + " params)"
        : "Config reload FAILED (see console)");
    return { result.Success(), result.Summary() };
}

void Application::SaveLevel() 
{
    const LevelIoResult result = LevelFile::Save(AppWorld, Config::kLevelPath);
    AppEventLog.Log(result.text);
    AppConsole.Print(result.text, !result.ok);
}

void Application::LoadLevel() 
{
    const LevelIoResult result = LevelFile::Load(AppWorld, Config::kLevelPath);
    AppEventLog.Log(result.text);
    AppConsole.Print(result.text, !result.ok);
    if (result.ok) AppCamera.SetPosition(AppWorld.CameraSpawnPosition());
}

bool Application::ShouldExit() const 
{
    return WindowShouldClose() || (IsKeyPressed(KEY_ESCAPE) && !AppConsole.IsOpen());
}

void Application::HandleHotkeys() 
{
    if (IsKeyPressed(KEY_F5)) 
    {
        const CommandResult result = ReloadConfig();
        AppConsole.Print(result.text, !result.ok);
    }

    // When we in console, we dont trigger actions.
    if (AppConsole.IsOpen()) return;

    if (IsKeyPressed(KEY_F1)) AppEditor.Toggle();
    if (IsKeyPressed(KEY_F2)) SaveLevel();
    if (IsKeyPressed(KEY_F3)) LoadLevel();
    if (IsKeyPressed(KEY_F4)) 
    {
        AppRenderer.SetCullingEnabled(!AppRenderer.IsCullingEnabled());
        AppEventLog.Log(AppRenderer.IsCullingEnabled() ? "Frustum culling ON" : "Frustum culling OFF");
    }
    if (IsKeyPressed(KEY_F6)) 
    {
        AppRenderer.ToggleFrustumFreeze();
        AppEventLog.Log(AppRenderer.IsFrustumFrozen() ? "Frustum frozen" : "Frustum released");
    }
    // Return back to default camera position
    if (IsKeyPressed(KEY_R)) AppCamera.SetPosition(AppWorld.CameraSpawnPosition()); 
}

void Application::Run() 
{
    while (!ShouldExit()) 
    {
        Profiler::Instance().BeginFrame();
        PROFILE_SCOPE("Frame", "Frame");

        const float dt = std::min(GetFrameTime(), 0.1f);
        Update(dt);
        Render();
    }
}

// Create default application actions.
void Application::Update(float dt) 
{
    PROFILE_SCOPE("Update", "Logic");

    { PROFILE_SCOPE("Console", "Input"); AppConsole.Update(dt); }
    { PROFILE_SCOPE("Hotkeys", "Input"); HandleHotkeys(); }
    { PROFILE_SCOPE("Camera input", "Input"); if (!AppConsole.IsOpen()) AppCameraController.Update(dt); }
    { PROFILE_SCOPE("Camera look-at", "Logic"); AppCamera.LookAt(AppWorld.PlayerPosition()); }
    { PROFILE_SCOPE("Editor", "Input"); if (!AppConsole.IsOpen()) AppEditor.Update(); }
    { PROFILE_SCOPE("World update", "Logic"); AppWorld.Update(dt); }
    { PROFILE_SCOPE("Event log", "Logic"); AppEventLog.Update(); }
}

void Application::Render() 
{
    PROFILE_SCOPE("Render", "Render");

    { PROFILE_SCOPE("BeginDrawing", "Render"); BeginDrawing(); ClearBackground(Color{ 25, 28, 35, 255 }); }
    { PROFILE_SCOPE("Scene 3D", "Render"); DrawScene(); }
    { PROFILE_SCOPE("UI", "UI"); DrawOverlays(); }
    { PROFILE_SCOPE("EndDrawing", "Render"); EndDrawing(); }
}

void Application::DrawScene() 
{
    AppRenderer.BeginScene(AppCamera.GetCamera());
    { PROFILE_SCOPE("Frustum culling", "Render"); AppWorld.Render(AppRenderer, AppEditor.IsActive()); }
    { PROFILE_SCOPE("Draw visible", "Render"); AppRenderer.DrawVisible(); }
    { PROFILE_SCOPE("Debug gizmos", "Render"); AppRenderer.DrawFrustumDebug(); AppEditor.DrawGizmos(); }
    AppRenderer.EndScene();
}

void Application::DrawOverlays() const 
{
    { PROFILE_SCOPE("Event log", "UI"); AppEventLog.Draw(0, 0); }
    { PROFILE_SCOPE("Culling overlay", "UI"); AppCullingOverlay.Draw(AppRenderer, 0, 400); }
    { PROFILE_SCOPE("Profiler overlay", "UI"); AppProfilerOverlay.Draw(GetScreenWidth() - ProfilerOverlay::Width() - 10, 10); }
    { PROFILE_SCOPE("Editor HUD", "UI"); AppEditor.DrawHud(); }
    { PROFILE_SCOPE("Console", "UI"); AppConsole.Draw(); }
}
