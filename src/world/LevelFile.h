#pragma once

#include <string>

class GameWorld;

struct LevelIoResult 
{
    bool ok = false;
    std::string text;
};

// level.map - a small human readable text format, one object per line:
//
//     # level.map v1
//     player <x> <y> <z>                       player spawn (centre of the player box)
//     camera <x> <y> <z>                       camera point
//     cube   <cx> <cy> <cz> <sx> <sy> <sz>     static box: centre + size
//     wall   <cx> <cy> <cz> <sx> <sy> <sz>
//
// Only what the editor can place is stored; the floor and the enemies are rebuilt by the game.
// Loading is atomic: the whole file is parsed first and the world is touched only if it is valid.
class LevelFile 
{
    public:
    static LevelIoResult Save(const GameWorld& world, const std::string& path);
    static LevelIoResult Load(GameWorld& world, const std::string& path);
};
