#include <raylib.h>
#include "../../imgui/imgui.h"
#include "../../imgui/rlImGui.h"
#include "../../imgui/rlImGuiColors.h"
#include "../typedef.h"
#include "objectsUI.h"
#include "objects.h"
#include "objectHandling.h"
#include "GameEntity.h"
#include <vector>
#include <string>

bool RenderRemoveComponentButton()
{
    ImGui::SameLine();
    return ImGui::Button("X##RemoveComponent");
}

void ObjectUI::RenderGeneralUI(GameEntity **selectedEntity, std::vector<GameEntity *> &entities)
{
    ImGui::Begin("Entity Editor");

    if (*selectedEntity == nullptr)
    {
        if (ImGui::Button("Create Empty Entity"))
        {
            GameEntity *newEntity = new GameEntity();
            (newEntity)->AddComponent<TransformComponent>();
            entities.push_back(newEntity);
            *selectedEntity = newEntity;
        }
    }

    if (*selectedEntity)
    {
        // Always on top, looks nice
        if (auto transform = (*selectedEntity)->GetComponent<TransformComponent>())
        {
            ObjectUI::RenderTransformComponentUI(transform);
        }

        // Basically the dropdown menu where you can add components
        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            if (ImGui::MenuItem("Transform") && !(*selectedEntity)->GetComponent<TransformComponent>())
                (*selectedEntity)->AddComponent<TransformComponent>();
            if (ImGui::MenuItem("Cube") && !(*selectedEntity)->GetComponent<CubeComponent>())
                (*selectedEntity)->AddComponent<CubeComponent>();
            if (ImGui::MenuItem("Sphere") && !(*selectedEntity)->GetComponent<SphereComponent>())
                (*selectedEntity)->AddComponent<SphereComponent>();
            ImGui::EndPopup();
        }

        // Render the existing components, minus transform, as thats rendered on top
        if (auto cube = (*selectedEntity)->GetComponent<CubeComponent>())
        {
            ImGui::Text("Cube Component");
            if (RenderRemoveComponentButton())
            {
                (*selectedEntity)->RemoveComponent<CubeComponent>();
            }
            else
            {
                ObjectUI::RenderCubeComponentUI(cube);
            }
        }
        if (auto sphere = (*selectedEntity)->GetComponent<SphereComponent>())
        {
            ImGui::Text("Sphere Component");
            if (RenderRemoveComponentButton())
            {
                (*selectedEntity)->RemoveComponent<SphereComponent>();
            }
            else
            {
                ObjectUI::RenderSphereComponentUI(sphere);
            }
        }

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::Button("Delete Entity"))
        {
            for (auto &entity : entities)
            {
                if (entity == *selectedEntity)
                {
                    entity = entities.back();
                    entities.pop_back();
                    break;
                }
            }
            delete *selectedEntity;
            *selectedEntity = nullptr;
        }
    }
    else
    {
        ImGui::Text("No entity selected");
    }

    ImGui::End();

    ImGui::Begin("Entity Hierarchy");

    for (auto &entity : entities)
    {
        std::string entityName = "Entity " + std::to_string(reinterpret_cast<uintptr_t>(entity));

        bool isSelected = (*selectedEntity == entity);
        if (ImGui::Selectable(entityName.c_str(), isSelected))
        {
            *selectedEntity = entity;
        }
    }

    ImGui::End();
}

void ObjectUI::RenderTransformComponentUI(TransformComponent *transform)
{
    if (transform)
    {
        if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // TODO fix dragging
            ImGui::Text("Position");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputFloat3("##Position", &transform->position.x, "%.2f");

            ImGui::Text("Rotation");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputFloat3("##Rotation", &transform->rotation.x, "%.2f");

            ImGui::Text("Scale");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputFloat3("##Scale", &transform->scale.x, "%.2f");

            if (transform->scale.x <= 0)
                transform->scale.x = 0.1f;
            if (transform->scale.y <= 0)
                transform->scale.y = 0.1f;
            if (transform->scale.z <= 0)
                transform->scale.z = 0.1f;

            ImGui::BeginGroup();
            if (ImGui::Button("Uniform Scale"))
            {
                ImGui::OpenPopup("UniformScalePopup");
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Scale"))
            {
                transform->scale = {1.0f, 1.0f, 1.0f};
                if (ImGui::IsPopupOpen("UniformScalePopup"))
                {
                    ImGui::GetStateStorage()->SetFloat(
                        ImGui::GetID("##UniformScale"),
                        1.0f);
                }
            }
            ImGui::EndGroup();

            if (ImGui::BeginPopup("UniformScalePopup"))
            {
                static float uniformScale = 1.0f;
                ImGui::Text("Uniform Scale Factor");
                if (ImGui::InputFloat("##UniformScale", &uniformScale, 0.1f, 0.5f, "%.2f") && uniformScale > 0)
                {
                    transform->scale = {uniformScale, uniformScale, uniformScale};
                }

                if (ImGui::Button("Apply"))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }
    }
}

void ObjectUI::RenderCubeComponentUI(CubeComponent *cube)
{
    if (cube)
    {
        ImGui::Separator();
        ImGui::Text("Size");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputFloat3("##Size", &cube->size.x, "%.2f");

        // Make sure theres no negative sizing
        if (cube->size.x <= 0)
            cube->size.x = 0.1f;
        if (cube->size.y <= 0)
            cube->size.y = 0.1f;
        if (cube->size.z <= 0)
            cube->size.z = 0.1f;

        auto transform = cube->entity->GetComponent<TransformComponent>();
        if (transform)
        {
            Vector3 scaledSize = {
                cube->size.x * transform->scale.x,
                cube->size.y * transform->scale.y,
                cube->size.z * transform->scale.z};

            ImGui::TextDisabled("(Scaled: %.2f, %.2f, %.2f)",
                                scaledSize.x, scaledSize.y, scaledSize.z);
        }

        ImGui::Text("Color");
        ImGui::ColorEdit4("##Color", (float *)&cube->color);
    }
}

void ObjectUI::RenderSphereComponentUI(SphereComponent *sphere)
{
    if (sphere)
    {
        ImGui::Separator();
        ImGui::Text("Radius");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputFloat("##Radius", &sphere->radius, 0.1f, 0.5f, "%.2f");

        if (sphere->radius <= 0)
            sphere->radius = 0.1f;

        auto transform = sphere->entity->GetComponent<TransformComponent>();
        if (transform)
        {
            float avgScale = (transform->scale.x + transform->scale.y + transform->scale.z) / 3.0f;
            float scaledRadius = sphere->radius * avgScale;

            ImGui::TextDisabled("(Scaled: %.2f)", scaledRadius);
        }

        ImGui::Text("Color");
        ImGui::ColorEdit4("##Color", (float *)&sphere->color);
    }
}