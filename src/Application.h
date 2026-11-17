#pragma once

#include "camera/CameraController.h"
#include "camera/GameCamera.h"
#include "console/CommandProcessor.h"
#include "console/Console.h"
#include "core/Config.h"
#include "core/ParameterRegistry.h"
#include "core/Window.h"
#include "editor/LevelEditor.h"
#include "render/Renderer.h"
#include "tools/CullingOverlay.h"
#include "tools/EventLog.h"
#include "tools/ProfilerOverlay.h"
#include "world/GameWorld.h"
#include "world/ModelAsset.h"

class Application 
{
    public:
    // Functions
    Application();
    void Run();

    private:
    // Functions
    bool ShouldExit() const;
    void Update(float dt);
    void HandleHotkeys();
    CommandResult ReloadConfig(); // shared by F5 and the console command
    void SaveLevel();
    void LoadLevel();
    void RegisterCommands();
    void Render();
    void DrawScene();
    void DrawOverlays() const;

    // Variables (order matters: the window must exist before the model is uploaded to the GPU)
    Window AppWindow;
    SimulationParams AppParams;
    ParameterRegistry AppRegistry;
    CommandProcessor AppCommands;
    Console AppConsole;
    EventLog AppEventLog;
    ModelAsset AppEnemyModel;
    GameWorld AppWorld;
    GameCamera AppCamera;
    CameraController AppCameraController;
    Renderer AppRenderer;
    LevelEditor AppEditor;
    ProfilerOverlay AppProfilerOverlay;
    CullingOverlay AppCullingOverlay;
};
