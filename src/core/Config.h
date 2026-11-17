#pragma once

// Parameters from the "parameter table"
struct SimulationParams 
{
    float spawnPeriodSec = 0.4f; // enemy spawn period, seconds
    float minEnemySpeed = 3.0f; // units / second
    float maxEnemySpeed = 8.0f; // units / second
    int maxEnemies = 15; // max alive at once
    float logEntryLifetimeSec = 3.0f; // how long a log line stays visible
};


#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH "params.cfg"
#endif

// Folder with the models (enemy.glb). Searched relative to the working directory as well.
#ifndef ASSETS_DIR
#define ASSETS_DIR "assets"
#endif

// The level written by F2 and read by F3 (and at start-up, if it exists).
#ifndef LEVEL_FILE_PATH
#define LEVEL_FILE_PATH "level.map"
#endif

// Fixed scene / engine constants.
namespace Config 
{
    constexpr const char* kConfigPath = CONFIG_FILE_PATH;
    constexpr const char* kAssetsDir = ASSETS_DIR;
    constexpr const char* kLevelPath = LEVEL_FILE_PATH;

    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 720;
    constexpr int kTargetFps = 60; // 0 = uncapped (use the "fps" console command to change it at runtime)
    constexpr const char* kWindowTitle = "Raylib Engine Project";

    // Floor: N x N tiles, every tile is a separate game object (so it can be culled on its own)
    constexpr int kFloorSide = 100;
    constexpr float kFloorTileSize = 1.0f;

    // Player (the thing the enemies run towards): 1 x 2 x 1 box
    constexpr float kPlayerWidth = 1.0f;
    constexpr float kPlayerHeight = 2.0f;

    // Camera
    constexpr float kCameraPitchDeg = 45.0f; // pitch of the DEFAULT camera point, must be 30..60
    constexpr float kCameraFovYDeg = 45.0f;
    constexpr float kCameraSpeed = 30.0f; // units / second
    constexpr float kCameraSpawnHeight = 20.0f; // editor: camera point = clicked point + this height

    // Enemies
    constexpr const char* kEnemyModelFile = "enemy.glb";
    constexpr float kEnemyHeight = 1.8f;
    constexpr float kSpawnRadius = 45.0f;   // circle around the player
    constexpr float kDestroyDistance = 1.2f;    // self-destruct distance to the player

    // Editor objects, (width X, height Y, depth Z)
    constexpr float kCubeSizeX = 1.0f,  kCubeSizeY = 1.0f,  kCubeSizeZ = 1.0f;
    constexpr float kWallSizeX = 10.0f, kWallSizeY = 10.0f, kWallSizeZ = 1.0f;  
}
