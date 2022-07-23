#pragma once

#include <imgui/imgui.h>

namespace ht {
enum class UniformType : unsigned char { Float, Vec2, Vec3, Vec4, Color };

// clang-format off
struct UniformItemFloat {
	float value;
	void Update() { ImGui::DragFloat("", &value, 0.1f); }
	static constexpr const char* GetShaderType() { return "float"; }
	static constexpr UniformType EnumType = UniformType::Float;
};
struct UniformItemVec2 {
	glm::vec2 value;
	void Update() { ImGui::DragFloat2("", &value.x, 0.1f); }
	static constexpr const char* GetShaderType() { return "vec2"; }
	static constexpr UniformType EnumType = UniformType::Vec2;
};
struct UniformItemVec3 {
	glm::vec3 value;
	void Update() { ImGui::DragFloat3("", &value.x, 0.1f); }
	static constexpr const char* GetShaderType() { return "vec3"; }
	static constexpr UniformType EnumType = UniformType::Vec3;
};
struct UniformItemVec4 {
	glm::vec4 value;
	void Update() { ImGui::DragFloat4("", &value.x, 0.1f); }
	static constexpr const char* GetShaderType() { return "vec4"; }
	static constexpr UniformType EnumType = UniformType::Vec4;
};
struct UniformItemColor {
	glm::vec4 value;
	void Update() { ImGui::ColorEdit4("", &value.x, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float); }
	static constexpr const char* GetShaderType() { return "vec4"; }
	static constexpr UniformType EnumType = UniformType::Color;
};
// clang-format on

using UniformItem = std::variant<UniformItemFloat,
                                 UniformItemVec2,
                                 UniformItemVec3,
                                 UniformItemVec4,
                                 UniformItemColor>;

struct GetUniformType {
    template <typename T>
    std::string operator()(T& uniform) {
        return uniform.GetShaderType();
    }
};

static UniformItem UniformItemFromType(UniformType type) {
    if (type == UniformType::Vec2) {
        return UniformItemVec2{};
    } else if (type == UniformType::Vec3) {
        return UniformItemVec3{};
    } else if (type == UniformType::Vec4) {
        return UniformItemVec4{};
    } else if (type == UniformType::Color) {
        return UniformItemColor{};
    }
    return UniformItemFloat{};
}

struct UniformData {
    std::string mName;
    UniformItem mItem;

    void Serialize(dubu::ReadBuffer& buffer) {
        buffer >> mName;
        UniformType type;
        buffer >> type;
        mItem = UniformItemFromType(type);
        std::visit(overloaded{[&buffer](auto& data) { buffer >> data; }},
                   mItem);
    }

    void Serialize(dubu::WriteBuffer& buffer) const {
        buffer << mName;

        std::visit(overloaded{[&buffer](const auto& data) {
                       buffer << data.EnumType;
                       buffer << data;
                   }},
                   mItem);
    }
};

}  // namespace ht
