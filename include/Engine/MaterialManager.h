#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <Engine/Material.h>

class MaterialManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
    MaterialManager() = default;

public:
    static MaterialManager& Instance() {
        static MaterialManager instance;
        return instance;
    }

    void AddMaterial(const std::shared_ptr<Material>& material) {
        materials_[material->name] = material;
    }

    std::shared_ptr<Material> GetMaterial(const std::string& name) {
        auto it = materials_.find(name);
        if (it != materials_.end()) return it->second;
        return nullptr;
    }

    std::vector<std::string> GetMaterialNames() const {
        std::vector<std::string> names;
        for (const auto& [key, mat] : materials_) {
            names.push_back(key);
        }
        return names;
    }
};
