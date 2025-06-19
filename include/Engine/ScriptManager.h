#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <Objects/ScriptComponent.h>

class ScriptManager {
private:
    std::unordered_map<std::string, std::shared_ptr<ScriptComponent>> scripts_;
    ScriptManager() = default;

public:
    static ScriptManager& Instance() {
        static ScriptManager instance;
        return instance;
    }

    void AddScript(const std::shared_ptr<ScriptComponent>& script) {
        scripts_[script->GetName()] = script;
    }

    std::shared_ptr<ScriptComponent> GetScript(const std::string& name) {
        auto it = scripts_.find(name);
        if (it != scripts_.end()) return it->second;
        return nullptr;
    }

    std::vector<std::shared_ptr<ScriptComponent>> GetAllScripts() const {
        std::vector<std::shared_ptr<ScriptComponent>> result;
        for (const auto& [key, script] : scripts_) {
            result.push_back(script);
        }
        return result;
    }

    std::vector<std::string> GetScriptNames() const {
        std::vector<std::string> names;
        for (const auto& [key, _] : scripts_) {
            names.push_back(key);
        }
        return names;
    }
};
