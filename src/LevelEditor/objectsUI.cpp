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
        // TODO: improve if chain, it's ugly

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
        ImGui::InputFloat3("Position", &transform->position.x);
        ImGui::InputFloat3("Rotation", &transform->rotation.x);
        ImGui::InputFloat3("Scale", &transform->scale.x);
    }
}

void ObjectUI::RenderCubeComponentUI(CubeComponent *cube)
{
    if (cube)
    {
        ImGui::InputFloat3("Size", &cube->size.x);
        ImGui::ColorEdit4("Color", (float *)&cube->color);
    }
}

void ObjectUI::RenderSphereComponentUI(SphereComponent *sphere)
{
    if (sphere)
    {
        ImGui::InputFloat("Radius", &sphere->radius);
        ImGui::ColorEdit4("Color", (float *)&sphere->color);
    }
}