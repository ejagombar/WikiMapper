#include "controlData.hpp"
#include "../lib/json.hpp"
#include <fstream>
#include <vector>

using json = nlohmann::json;

inline void to_json(json &j, const glm::vec3 &v) { j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}}; }

void from_json(const json &j, glm::vec3 &v) {
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
    v.z = j.at("z").get<float>();
}

// SimParameters -------------------------------
void to_json(json &j, const SimParameters &p) {
    j = json{{"repulsionStrength", p.repulsionStrength}, {"attractionStrength", p.attractionStrength},
             {"centeringForce", p.centeringForce},       {"timeStep", p.timeStep},
             {"forceMultiplier", p.forceMultiplier},     {"maxForce", p.maxForce},
             {"targetDistance", p.targetDistance},       {"maxVelocity", p.maxVelocity}};
}

void from_json(const json &j, SimParameters &p) {
    j.at("repulsionStrength").get_to(p.repulsionStrength);
    j.at("attractionStrength").get_to(p.attractionStrength);
    j.at("centeringForce").get_to(p.centeringForce);
    j.at("timeStep").get_to(p.timeStep);
    j.at("forceMultiplier").get_to(p.forceMultiplier);
    j.at("maxForce").get_to(p.maxForce);
    j.at("targetDistance").get_to(p.targetDistance);
    j.at("maxVelocity").get_to(p.maxVelocity);
}

// EngineControlData -------------------------------
void to_json(json &j, const EngineControlData &e) {
    j = json{{"vSync", e.vSync},
             {"backgroundButtonToggle", e.backgroundButtonToggle},
             {"cameraFov", e.cameraFov},
             {"mouseSensitivity", e.mouseSensitivity},
             {"initGraphData", e.initGraphData.load()},
             {"customVals", std::vector<float>(std::begin(e.customVals), std::end(e.customVals))}};
}

void from_json(const json &j, EngineControlData &e) {
    e.vSync = j.at("vSync").get<bool>();
    e.backgroundButtonToggle = j.at("backgroundButtonToggle").get<bool>();
    e.cameraFov = j.at("cameraFov").get<float>();
    e.mouseSensitivity = j.at("mouseSensitivity").get<float>();
    e.initGraphData.store(j.at("initGraphData").get<bool>());

    auto arr = j.at("customVals").get<std::vector<float>>();
    for (size_t i = 0; i < std::min(arr.size(), size_t(6)); ++i)
        e.customVals[i] = arr[i];
}

// ControlData -------------------------------
void to_json(json &j, const ControlData &c) {
    j = json{{"simParameters", c.sim.parameters.load()}, {"engine", c.engine}};
}

void from_json(const json &j, ControlData &c) {
    if (j.contains("simParameters"))
        c.sim.parameters.store(j.at("simParameters").get<SimParameters>());
    if (j.contains("engine"))
        j.at("engine").get_to(c.engine);
}

// ApplicationData -----------------------------
void to_json(json &j, const ApplicationData &a) {
    j = json{{"dataSource", static_cast<int>(a.dataSource.sourceType)},
             {"dbUrl", a.dataSource.dbUrl},
             {"dbUsername", a.dataSource.dbUsername},
             {"dbPassword", a.dataSource.dbPassword},
             {"serverUrl", a.dataSource.serverUrl},
             {"background", a.background}};
}

void from_json(const json &j, ApplicationData &a) {
    a.dataSource.sourceType = static_cast<dbInterfaceType>(j.at("dataSource").get<int>());
    a.dataSource.dbUrl = j.at("dbUrl").get<std::string>();
    a.dataSource.dbUsername = j.at("dbUsername").get<std::string>();
    a.dataSource.dbPassword = j.at("dbPassword").get<std::string>();
    a.dataSource.serverUrl = j.at("serverUrl").get<std::string>();
    a.background = j.at("background").get<int32_t>();
}

// IO Helpers -------------------------------
bool ControlData::SaveControlData(const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    json j;
    j["simParameters"] = sim.parameters.load();
    j["engine"] = engine;
    j["app"] = app;

    file << j.dump(4);
    return true;
}

bool ControlData::LoadControlData(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

    json j;
    file >> j;

    if (j.contains("simParameters")) {
        SimParameters params = j["simParameters"].get<SimParameters>();
        sim.parameters.store(params);
    }

    if (j.contains("engine")) {
        from_json(j["engine"], engine);
    }

    if (j.contains("app")) {
        from_json(j["app"], app);
    }

    return true;
}
