#include "world/LevelFile.h"

#include "core/StringUtils.h"
#include "world/GameWorld.h"

#include <raylib.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

namespace 
{
    struct StaticRecord 
    {
        ObjectKind kind;
        Vector3 center;
        Vector3 size;
    };

    bool ParseFloat(const std::string& text, float& value) 
    {
        char* end = nullptr;
        value = std::strtof(text.c_str(), &end);
        return end != text.c_str() && *end == '\0';
    }

    bool ParseVector(const std::vector<std::string>& tokens, std::size_t first, Vector3& v) 
    {
        return ParseFloat(tokens[first], v.x) && ParseFloat(tokens[first + 1], v.y) && ParseFloat(tokens[first + 2], v.z);
    }

    std::string Line(const char* name, Vector3 a) 
    {
        char buffer[160];
        std::snprintf(buffer, sizeof(buffer), "%s %.4f %.4f %.4f\n", name, a.x, a.y, a.z);
        return buffer;
    }

    std::string Line(const char* name, Vector3 a, Vector3 b) 
    {
        char buffer[220];
        std::snprintf(buffer, sizeof(buffer), "%s %.4f %.4f %.4f %.4f %.4f %.4f\n", name, a.x, a.y, a.z, b.x, b.y, b.z);
        return buffer;
    }
}

LevelIoResult LevelFile::Save(const GameWorld& world, const std::string& path) 
{
    std::string content = "# level.map v1\n";
    content += Line("player", world.PlayerPosition());
    content += Line("camera", world.CameraSpawnPosition());

    int saved = 0;
    for (const auto& object : world.Objects()) 
    {
        const ObjectKind kind = object->Kind();
        if (object->IsDestroyed() || (kind != ObjectKind::StaticCube && kind != ObjectKind::StaticWall)) continue;

        const BoundingBox b = object->Bounds();
        const Vector3 center{ (b.min.x + b.max.x) * 0.5f, (b.min.y + b.max.y) * 0.5f, (b.min.z + b.max.z) * 0.5f };
        const Vector3 size{ b.max.x - b.min.x, b.max.y - b.min.y, b.max.z - b.min.z };
        content += Line(kind == ObjectKind::StaticCube ? "cube" : "wall", center, size);
        ++saved;
    }

    std::ofstream file(std::filesystem::u8path(path), std::ios::binary | std::ios::trunc);
    if (!file) return { false, "Cannot write " + path };
    file << content;
    if (!file) return { false, "Write error: " + path };
    return { true, "Level saved to " + path + " (" + std::to_string(saved) + " objects)" };
}

LevelIoResult LevelFile::Load(GameWorld& world, const std::string& path) 
{
    std::ifstream file{ std::filesystem::u8path(path) };
    if (!file) return { false, "Cannot open " + path };

    Vector3 player = world.PlayerPosition();
    Vector3 camera = world.CameraSpawnPosition();
    bool hasPlayer = false;
    bool hasCamera = false;
    std::vector<StaticRecord> statics;

    std::string raw;
    int lineNo = 0;
    while (std::getline(file, raw)) 
    {
        ++lineNo;
        if (lineNo == 1 && raw.compare(0, 3, "\xEF\xBB\xBF") == 0) raw.erase(0, 3);   // UTF-8 BOM
        const auto comment = raw.find('#');
        if (comment != std::string::npos) raw.erase(comment);

        const std::string line = StringUtils::Trim(raw);
        if (line.empty()) continue;
        const std::vector<std::string> t = StringUtils::SplitWhitespace(line);
        const std::string where = path + ", line " + std::to_string(lineNo) + ": ";

        if (t[0] == "player" && t.size() == 4) 
        {
            if (!ParseVector(t, 1, player)) return { false, where + "bad number" };
            hasPlayer = true;
        }
        else if (t[0] == "camera" && t.size() == 4) 
        {
            if (!ParseVector(t, 1, camera)) return { false, where + "bad number" };
            hasCamera = true;
        }
        else if ((t[0] == "cube" || t[0] == "wall") && t.size() == 7) 
        {
            StaticRecord record{ t[0] == "cube" ? ObjectKind::StaticCube : ObjectKind::StaticWall, {}, {} };
            if (!ParseVector(t, 1, record.center) || !ParseVector(t, 4, record.size)) return { false, where + "bad number" };
            if (record.size.x <= 0.0f || record.size.y <= 0.0f || record.size.z <= 0.0f) return { false, where + "size must be positive" };
            statics.push_back(record);
        }
        else 
        {
            return { false, where + "unknown or malformed record '" + t[0] + "'" };
        }
    }

    // Touch the world only when file is valid.
    world.ClearLevel();
    if (hasPlayer) world.SetPlayerSpawn(player);
    if (hasCamera) world.SetCameraSpawn(camera);
    for (const StaticRecord& r : statics) world.AddStatic(r.kind, r.center, r.size);

    return { true, "Level loaded from " + path + " (" + std::to_string(statics.size()) + " objects)" };
}
