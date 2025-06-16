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
    // Retourne un tableau des shared_ptr de tous les matériaux
    std::vector<std::shared_ptr<Material>> GetAllMaterials() const {
        std::vector<std::shared_ptr<Material>> mats;
        mats.reserve(materials_.size());
        for (const auto& [key, mat] : materials_) {
            mats.push_back(mat);
        }
        return mats;
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
