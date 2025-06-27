#include <raylib.h>
#include "../../imgui/imgui.h"
#include "../../imgui/rlImGui.h"
#include "../../imgui/rlImGuiColors.h"
#include "../typedef.h"
#include "objectsUI.h"
#include "GameEntity.h"
#include <vector>
#include <string>

GizmoSystem ObjectUI::gizmoSystem;
bool RenderRemoveComponentButton()
{
    ImGui::SameLine();
    return ImGui::Button("X##RemoveComponent");
}

bool ObjectUI::IsGizmoClicked(Camera camera, Ray mouseRay)
{
    return gizmoSystem.CheckForAxisClick(mouseRay);
}

bool ObjectUI::IsGizmoHovered(Ray mouseRay)
{
    return gizmoSystem.IsMouseOverGizmo(mouseRay);
}

void ObjectUI::UpdateAndRenderGizmos(Camera camera, GameEntity *selectedEntity, Ray mouseRay)
{
    if (!selectedEntity)
    {
        gizmoSystem.Deactivate();
        return;
    }

    auto transform = selectedEntity->GetComponent<TransformComponent>();
    if (!transform)
    {
        gizmoSystem.Deactivate();
        return;
    }

    static Vector3 *lastTarget = nullptr;
    if (lastTarget != &transform->position)
    {
        gizmoSystem.SetTarget(&transform->position);
        lastTarget = &transform->position;
    }

    gizmoSystem.Update(camera, mouseRay, transform->position);
    gizmoSystem.Render(camera, mouseRay);
}

void ObjectUI::RenderGeneralUI(GameEntity **selectedEntity, std::vector<GameEntity *> &entities)
{
    // ImGui::DockSpaceOverViewport(ImGuiDockNodeFlags_PassthruCentralNode);

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
            ImGui::Text("Position");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

            if (gizmoSystem.IsActive() && gizmoSystem.GetTargetAddress() == &transform->position)
            {
                ImGui::InputFloat3("##Position", &transform->position.x, "%.2f");
                ImGui::SameLine();
                ImGui::TextDisabled("[Gizmo]");
            }
            else
            {
                ImGui::InputFloat3("##Position", &transform->position.x, "%.2f");
            }

            // EULER ANGLE ROTATION UI
            ImGui::Separator();
            ImGui::Text("Rotation");

            // Get current euler angles for display/editing
            Vector3 eulerAngles = transform->GetEulerAngles();

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::InputFloat3("##Rotation", &eulerAngles.x, "%.2f"))
            {
                transform->SetEulerAngles(eulerAngles);
            }

            ImGui::TextDisabled("(Pitch, Yaw, Roll in degrees)");

            ImGui::Separator();
            ImGui::Text("Quick Rotations:");

            // World space rotation buttons
            ImGui::Text("World Space:");
            ImGui::BeginGroup();
            if (ImGui::Button("X+15°"))
                transform->RotateAroundWorldAxis({1, 0, 0}, 15.0f);
            ImGui::SameLine();
            if (ImGui::Button("X-15°"))
                transform->RotateAroundWorldAxis({1, 0, 0}, -15.0f);
            ImGui::SameLine();
            if (ImGui::Button("Y+15°"))
                transform->RotateAroundWorldAxis({0, 1, 0}, 15.0f);
            ImGui::SameLine();
            if (ImGui::Button("Y-15°"))
                transform->RotateAroundWorldAxis({0, 1, 0}, -15.0f);
            ImGui::SameLine();
            if (ImGui::Button("Z+15°"))
                transform->RotateAroundWorldAxis({0, 0, 1}, 15.0f);
            ImGui::SameLine();
            if (ImGui::Button("Z-15°"))
                transform->RotateAroundWorldAxis({0, 0, 1}, -15.0f);
            ImGui::EndGroup();

            ImGui::BeginGroup();
            if (ImGui::Button("X+90°"))
                transform->RotateAroundWorldAxis({1, 0, 0}, 90.0f);
            ImGui::SameLine();
            if (ImGui::Button("X-90°"))
                transform->RotateAroundWorldAxis({1, 0, 0}, -90.0f);
            ImGui::SameLine();
            if (ImGui::Button("Y+90°"))
                transform->RotateAroundWorldAxis({0, 1, 0}, 90.0f);
            ImGui::SameLine();
            if (ImGui::Button("Y-90°"))
                transform->RotateAroundWorldAxis({0, 1, 0}, -90.0f);
            ImGui::SameLine();
            if (ImGui::Button("Z+90°"))
                transform->RotateAroundWorldAxis({0, 0, 1}, 90.0f);
            ImGui::SameLine();
            if (ImGui::Button("Z-90°"))
                transform->RotateAroundWorldAxis({0, 0, 1}, -90.0f);
            ImGui::EndGroup();

            // Local space rotation buttons
            ImGui::Text("Local Space:");
            ImGui::BeginGroup();
            if (ImGui::Button("Pitch+15°"))
                transform->RotateAroundLocalAxis({1, 0, 0}, 15.0f);
            ImGui::SameLine();
            if (ImGui::Button("Pitch-15°"))
                transform->RotateAroundLocalAxis({1, 0, 0}, -15.0f);
            ImGui::SameLine();
            if (ImGui::Button("Yaw+15°"))
                transform->RotateAroundLocalAxis({0, 1, 0}, 15.0f);
            ImGui::SameLine();
            if (ImGui::Button("Yaw-15°"))
                transform->RotateAroundLocalAxis({0, 1, 0}, -15.0f);
            ImGui::SameLine();
            if (ImGui::Button("Roll+15°"))
                transform->RotateAroundLocalAxis({0, 0, 1}, 15.0f);
            ImGui::SameLine();
            if (ImGui::Button("Roll-15°"))
                transform->RotateAroundLocalAxis({0, 0, 1}, -15.0f);
            ImGui::EndGroup();

            // Utility buttons
            ImGui::Separator();
            if (ImGui::Button("Reset Rotation"))
            {
                transform->ResetRotation();
            }
            ImGui::SameLine();
            if (ImGui::Button("Snap to Grid"))
            {
                // Snap to nearest 15-degree increments
                Vector3 current = transform->GetEulerAngles();
                Vector3 snapped = {
                    roundf(current.x / 15.0f) * 15.0f,
                    roundf(current.y / 15.0f) * 15.0f,
                    roundf(current.z / 15.0f) * 15.0f};
                transform->SetEulerAngles(snapped);
            }

            if (ImGui::CollapsingHeader("Orientation Vectors"))
            {
                Vector3 forward = transform->GetForward();
                Vector3 right = transform->GetRight();
                Vector3 up = transform->GetUp();

                ImGui::Text("Forward: (%.2f, %.2f, %.2f)", forward.x, forward.y, forward.z);
                ImGui::Text("Right:   (%.2f, %.2f, %.2f)", right.x, right.y, right.z);
                ImGui::Text("Up:      (%.2f, %.2f, %.2f)", up.x, up.y, up.z);
            }

            ImGui::Separator();
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