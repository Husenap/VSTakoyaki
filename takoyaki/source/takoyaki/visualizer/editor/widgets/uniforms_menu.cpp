#include "uniforms_menu.hpp"

static int CharacterFilter(ImGuiInputTextCallbackData* data) {
    ImWchar c        = data->EventChar;
    int     numChars = (*(int*)data->UserData);
    if (std::isalpha(c)) return 0;
    if (std::isdigit(c) && numChars > 0) return 0;
    if (c == '_') return 0;
    return 1;
}

namespace ht {

const char* UniformsName           = "Uniforms";
const char* AddNewUniformPopupName = "AddNewUniform";
const char* UniformDragAndDropName = "REORDER_UNIFORMS_DRAG_AND_DROP";

void UniformsMenu::Update() {
    size_t numUniforms = mUniforms.size();

    if (mVisibility) {
        if (ImGui::Begin(UniformsName, &mVisibility)) {
            DrawUniforms();

            DrawAddUniformPopup();
        }
        ImGui::End();
    }

    if (numUniforms != mUniforms.size()) {
        if (mUniformsChangedHandler) mUniformsChangedHandler();
    }
}

void UniformsMenu::DrawUniforms() {
    if (ImGui::Button("Add Uniform")) {
        if (!ImGui::IsPopupOpen(AddNewUniformPopupName)) {
            mNameBuffer.fill('\0');
            mSelectedType = UniformType::Float;
            ImGui::OpenPopup(AddNewUniformPopupName);
        }
    }

    SwapData uniformSwap{-1, -1};
    for (int i = 0; i < mUniforms.size();) {
        ImGui::PushID(i);

        auto& uniform = mUniforms[i];

        ImGui::BeginGroup();
        bool wasButtonPressed = ImGui::Button("X");
        ImGui::SameLine();
        std::visit(overloaded{[](auto& item) { item.Update(); }},
                   uniform.mItem);
        ImGui::SameLine();

        ImGui::Text(uniform.mName.c_str());

        const static int dragDropFlags =
            ImGuiDragDropFlags_SourceAllowNullID |
            ImGuiDragDropFlags_AcceptBeforeDelivery;
        if (ImGui::BeginDragDropSource(dragDropFlags)) {
            ImGui::SetDragDropPayload(UniformDragAndDropName, &i, sizeof(int));
            ImGui::Text(uniform.mName.c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::EndGroup();

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload(UniformDragAndDropName)) {
                int payload_index = *(const int*)payload->Data;
                uniformSwap       = {payload_index, i};
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();

        if (wasButtonPressed) {
            mUniforms.erase(mUniforms.begin() + i);
        } else {
            ++i;
        }
    }

    if (uniformSwap.mSourceIndex != -1 && uniformSwap.mTargetIndex != -1) {
        UniformData data = mUniforms[uniformSwap.mSourceIndex];
        mUniforms.erase(mUniforms.begin() + uniformSwap.mSourceIndex);
        mUniforms.insert(mUniforms.begin() + uniformSwap.mTargetIndex, data);
    }
}

void UniformsMenu::DrawAddUniformPopup() {
    if (ImGui::BeginPopupModal(
            AddNewUniformPopupName, nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::SetWindowSize({0.f, 0.f});
        const std::array<const char*, 5> types{
            "float", "vec2", "vec3", "vec4", "color"};
        ImGui::Combo(
            "Type", (int*)&mSelectedType, types.data(), (int)types.size());

        int numChars = static_cast<int>(std::string(mNameBuffer.data()).size());
        ImGui::InputText("Variable Name",
                         mNameBuffer.data(),
                         mNameBuffer.size(),
                         ImGuiInputTextFlags_CallbackCharFilter,
                         CharacterFilter,
                         &numChars);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.f, 0.f));
        ImGui::Columns(2, nullptr, false);
        if (ImGui::Button("Create", {-FLT_EPSILON, 0.0})) {
            HandleNewUniform();
            ImGui::CloseCurrentPopup();
        }
        ImGui::NextColumn();
        if (ImGui::Button("Cancel", {-FLT_EPSILON, 0.0})) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::Columns(1);
        ImGui::PopStyleVar();

        ImGui::EndPopup();
    }
}

void UniformsMenu::HandleNewUniform() {
    std::string name = mNameBuffer.data();
    if (name.empty()) {
        return;
    }

    auto pred = [&n = name](const UniformData& data) {
        return n == data.mName;
    };
    if (std::any_of(mUniforms.begin(), mUniforms.end(), pred)) {
        return;
    }

    mUniforms.push_back({name.data(), UniformItemFromType(mSelectedType)});
}

void UniformsMenu::RegisterCommands(RenderCommandList<RenderCommand>& cmds,
                                    const ShaderProgram&              program) {
    for (auto& uniform : mUniforms) {
        auto pred = [&](auto& item) {
            cmds.Push<Commands::Uniform>(
                program.mProgram, uniform.mName, item.value);
        };
        std::visit(overloaded{pred}, uniform.mItem);
    }
}

void UniformsMenu::OpenFile(std::string_view file) {
    mUniforms.clear();

    std::string      uniformsFile = std::string(file) + ".uniforms";
    dubu::FileBuffer fb(uniformsFile, dubu::FileBuffer::Mode::Read);
    if (fb.IsOpen()) fb >> mUniforms;
}

void UniformsMenu::SaveFile(std::string_view file) {
    std::string      uniformsFile = std::string(file) + ".uniforms";
    dubu::FileBuffer fb(uniformsFile, dubu::FileBuffer::Mode::Write);
    if (fb.IsOpen()) fb << mUniforms;
}

std::string UniformsMenu::GetUniformDeclarations() {
    std::string output = "";

    for (const auto& uniform : mUniforms) {
        output += "uniform ";
        output += std::visit(GetUniformType{}, uniform.mItem);
        output += " ";
        output += uniform.mName;
        output += ";\n";
    }

    return output;
}

}  // namespace ht
