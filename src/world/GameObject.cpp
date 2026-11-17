#include "world/GameObject.h"

const char* ObjectKindName(ObjectKind kind) 
{
    switch (kind) 
    {
        case ObjectKind::Floor:       return "Floor tiles";
        case ObjectKind::StaticCube:  return "Cubes 1x1x1";
        case ObjectKind::StaticWall:  return "Walls 10x10x1";
        case ObjectKind::PlayerSpawn: return "Player";
        case ObjectKind::CameraSpawn: return "Camera point";
        case ObjectKind::Enemy:       return "Enemies";
        case ObjectKind::Spawner:     return "Spawner";
        default:                      return "?";
    }
}
