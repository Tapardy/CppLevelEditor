#include <raylib.h>
#include "../../imgui/imgui.h"
#include "../../imgui/rlImGui.h"
#include "../../imgui/rlImGuiColors.h"
#include "../imgui/imfilebrowser.h"
#include "../typedef.h"
#include "objectsUI.h"
#include "GameEntity.h"
#include <vector>
#include <string>

ImGui::FileBrowser fileDialog;

bool RenderRemoveComponentButton()
{
    ImGui::SameLine();
    return ImGui::Button("X##RemoveComponent");
}

bool ObjectUI::IsGizmoClicked(Camera camera, Ray mouseRay, GizmoSystem &gizmoSystem)
{
    return gizmoSystem.CheckForAxisClick(mouseRay, camera);
}

void ObjectUI::UpdateAndRenderGizmos(Camera camera, GameEntity *selectedEntity, Ray mouseRay, GizmoSystem &gizmoSystem)
{
    if (!selectedEntity)
    {
        gizmoSystem.Deactivate();
        return;
    }

    static Vector3 *lastPositionTarget = nullptr;
    static Quaternion *lastRotationTarget = nullptr;
    static Vector3 *lastScaleTarget = nullptr;

    bool targetsChanged = (lastPositionTarget != &selectedEntity->EntityTransform.position) || (lastRotationTarget != &selectedEntity->EntityTransform.rotation) || (lastScaleTarget != &selectedEntity->EntityTransform.scale);

    if (targetsChanged)
    {
        gizmoSystem.SetTarget(&selectedEntity->EntityTransform.position, &selectedEntity->EntityTransform.rotation, &selectedEntity->EntityTransform.scale);
        lastPositionTarget = &selectedEntity->EntityTransform.position;
        lastRotationTarget = &selectedEntity->EntityTransform.rotation;
        lastScaleTarget = &selectedEntity->EntityTransform.scale;
    }
    gizmoSystem.Update(camera, mouseRay, selectedEntity->EntityTransform.position, selectedEntity->EntityTransform.rotation, selectedEntity->EntityTransform.scale, &selectedEntity->EntityTransform);

    gizmoSystem.Render(camera, mouseRay);
}

void ObjectUI::RenderGeneralUI(GameEntity **selectedEntity, std::vector<GameEntity *> &entities, GizmoSystem &gizmoSystem)
{
    // ImGui::DockSpaceOverViewport(ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::Begin("Entity Editor");

    if (*selectedEntity == nullptr)
    {
        if (ImGui::Button("Create Empty Entity"))
        {
            GameEntity *newEntity = new GameEntity();
            entities.push_back(newEntity);
            *selectedEntity = newEntity;
        }
    }

    if (*selectedEntity)
    {
        // Always on top, looks nice
        ObjectUI::RenderTransformComponentUI(selectedEntity, gizmoSystem);

        // Basically the dropdown menu where you can add components
        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            if (ImGui::MenuItem("Cube") && !(*selectedEntity)->GetComponent<CubeComponent>())
                (*selectedEntity)->AddComponent<CubeComponent>();
            if (ImGui::MenuItem("Sphere") && !(*selectedEntity)->GetComponent<SphereComponent>())
                (*selectedEntity)->AddComponent<SphereComponent>();
            if (ImGui::MenuItem("Mesh") && !(*selectedEntity)->GetComponent<ModelComponent>())
                (*selectedEntity)->AddComponent<ModelComponent>();
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
        if (auto model = (*selectedEntity)->GetComponent<ModelComponent>())
        {
            ImGui::Text("Model Component");
            if (RenderRemoveComponentButton())
            {
                (*selectedEntity)->RemoveComponent<ModelComponent>();
            }
            else
            {
                ObjectUI::RenderModelComponentUI(model);
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

void ObjectUI::RenderTransformComponentUI(GameEntity **selectedEntity, GizmoSystem &gizmoSystem)
{
    GameEntity *entity = *selectedEntity;
    if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Gizmo Mode:");
        ImGui::SameLine();

        GizmoMode currentMode = gizmoSystem.GetMode();
        const char *modeNames[] = {"None", "Position", "Rotation", "Scale"};
        int currentModeIndex = static_cast<int>(currentMode);

        if (ImGui::Combo("##GizmoMode", &currentModeIndex, modeNames, 4))
        {
            gizmoSystem.SetMode(static_cast<GizmoMode>(currentModeIndex));
        }

        if (ImGui::CollapsingHeader("Gizmo Settings"))
        {
            float snapStep, rotationSnap, scaleSnap;
            gizmoSystem.GetSnapStep(&snapStep);
            gizmoSystem.GetRotationSnap(&rotationSnap);
            gizmoSystem.GetScaleSnap(&scaleSnap);

            if (ImGui::InputFloat("Position Snap", &snapStep, 0.01f, 0.1f, "%.3f"))
            {
                gizmoSystem.SetSnapStep(snapStep);
            }

            if (ImGui::InputFloat("Rotation Snap (degrees)", &rotationSnap, 1.0f, 15.0f, "%.1f"))
            {
                gizmoSystem.SetRotationSnap(rotationSnap);
            }

            if (ImGui::InputFloat("Scale Snap", &scaleSnap, 0.01f, 0.1f, "%.3f"))
            {
                gizmoSystem.SetScaleSnap(scaleSnap);
            }
        }

        ImGui::Separator();

        // Position UI
        ImGui::Text("Position");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        if (gizmoSystem.IsActive() &&
            gizmoSystem.GetMode() == GizmoMode::POSITION &&
            gizmoSystem.GetTargetPositionAddress() == &entity->EntityTransform.position)
        {
            ImGui::InputFloat3("##Position", &entity->EntityTransform.position.x, "%.2f");
            ImGui::SameLine();
            ImGui::TextDisabled("[Position Gizmo]");
        }
        else
        {
            ImGui::InputFloat3("##Position", &entity->EntityTransform.position.x, "%.2f");
        }

        // EULER ANGLE ROTATION UI
        ImGui::Separator();
        ImGui::Text("Rotation");

        if (gizmoSystem.IsActive() &&
            gizmoSystem.GetMode() == GizmoMode::ROTATION &&
            gizmoSystem.GetTargetRotationAddress() == &entity->EntityTransform.rotation)
        {
            ImGui::TextDisabled("[Rotation Gizmo Active]");
        }

        // Get current euler angles for display/editing
        Vector3 eulerAngles = entity->EntityTransform.GetEulerAngles();
        if (ImGui::InputFloat3("##Rotation", &eulerAngles.x, "%.2f"))
        {
            if (!(gizmoSystem.IsActive() && gizmoSystem.GetMode() == GizmoMode::ROTATION))
            {
                entity->EntityTransform.SetEulerAngles(eulerAngles);
            }
        }

        ImGui::TextDisabled("(Pitch, Yaw, Roll in degrees)");

        ImGui::Separator();
        ImGui::Text("Quick Rotations:");

        // World space rotation buttons
        ImGui::Text("World Space:");
        ImGui::BeginGroup();
        if (ImGui::Button("X+15°"))
            entity->EntityTransform.RotateAroundWorldAxis({1, 0, 0}, 15.0f);
        ImGui::SameLine();
        if (ImGui::Button("X-15°"))
            entity->EntityTransform.RotateAroundWorldAxis({1, 0, 0}, -15.0f);
        ImGui::SameLine();
        if (ImGui::Button("Y+15°"))
            entity->EntityTransform.RotateAroundWorldAxis({0, 1, 0}, 15.0f);
        ImGui::SameLine();
        if (ImGui::Button("Y-15°"))
            entity->EntityTransform.RotateAroundWorldAxis({0, 1, 0}, -15.0f);
        ImGui::SameLine();
        if (ImGui::Button("Z+15°"))
            entity->EntityTransform.RotateAroundWorldAxis({0, 0, 1}, 15.0f);
        ImGui::SameLine();
        if (ImGui::Button("Z-15°"))
            entity->EntityTransform.RotateAroundWorldAxis({0, 0, 1}, -15.0f);
        ImGui::EndGroup();

        ImGui::BeginGroup();
        if (ImGui::Button("X+90°"))
            entity->EntityTransform.RotateAroundWorldAxis({1, 0, 0}, 90.0f);
        ImGui::SameLine();
        if (ImGui::Button("X-90°"))
            entity->EntityTransform.RotateAroundWorldAxis({1, 0, 0}, -90.0f);
        ImGui::SameLine();
        if (ImGui::Button("Y+90°"))
            entity->EntityTransform.RotateAroundWorldAxis({0, 1, 0}, 90.0f);
        ImGui::SameLine();
        if (ImGui::Button("Y-90°"))
            entity->EntityTransform.RotateAroundWorldAxis({0, 1, 0}, -90.0f);
        ImGui::SameLine();
        if (ImGui::Button("Z+90°"))
            entity->EntityTransform.RotateAroundWorldAxis({0, 0, 1}, 90.0f);
        ImGui::SameLine();
        if (ImGui::Button("Z-90°"))
            entity->EntityTransform.RotateAroundWorldAxis({0, 0, 1}, -90.0f);
        ImGui::EndGroup();

        // Local space rotation buttons
        ImGui::Text("Local Space:");
        ImGui::BeginGroup();

        if (ImGui::Button("Pitch+15°"))
            entity->EntityTransform.RotateAroundLocalAxis({1, 0, 0}, 15.0f);
        ImGui::SameLine();
        if (ImGui::Button("Pitch-15°"))
            entity->EntityTransform.RotateAroundLocalAxis({1, 0, 0}, -15.0f);
        ImGui::SameLine();
        if (ImGui::Button("Yaw+15°"))
            entity->EntityTransform.RotateAroundLocalAxis({0, 1, 0}, 15.0f);
        ImGui::SameLine();
        if (ImGui::Button("Yaw-15°"))
            entity->EntityTransform.RotateAroundLocalAxis({0, 1, 0}, -15.0f);
        ImGui::SameLine();
        if (ImGui::Button("Roll+15°"))
            entity->EntityTransform.RotateAroundLocalAxis({0, 0, 1}, 15.0f);
        ImGui::SameLine();
        if (ImGui::Button("Roll-15°"))
            entity->EntityTransform.RotateAroundLocalAxis({0, 0, 1}, -15.0f);

        ImGui::EndGroup();

        // Utility buttons
        ImGui::Separator();
        if (ImGui::Button("Reset Rotation"))
        {
            entity->EntityTransform.ResetRotation();
        }
        ImGui::SameLine();
        if (ImGui::Button("Snap to Grid"))
        {
            // Snap to nearest 15-degree increments
            Vector3 current = entity->EntityTransform.GetEulerAngles();
            Vector3 snapped = {
                roundf(current.x / 15.0f) * 15.0f,
                roundf(current.y / 15.0f) * 15.0f,
                roundf(current.z / 15.0f) * 15.0f};
            entity->EntityTransform.SetEulerAngles(snapped);
        }

        if (ImGui::CollapsingHeader("Orientation Vectors"))
        {
            Vector3 forward = entity->EntityTransform.GetForward();
            Vector3 right = entity->EntityTransform.GetRight();
            Vector3 up = entity->EntityTransform.GetUp();

            ImGui::Text("Forward: (%.2f, %.2f, %.2f)", forward.x, forward.y, forward.z);
            ImGui::Text("Right:   (%.2f, %.2f, %.2f)", right.x, right.y, right.z);
            ImGui::Text("Up:      (%.2f, %.2f, %.2f)", up.x, up.y, up.z);
        }

        ImGui::Separator();
        ImGui::Text("Scale");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        if (gizmoSystem.IsActive() &&
            gizmoSystem.GetMode() == GizmoMode::SCALE &&
            gizmoSystem.GetTargetScaleAddress() == &entity->EntityTransform.scale)
        {
            ImGui::InputFloat3("##Scale", &entity->EntityTransform.scale.x, "%.2f");
            ImGui::SameLine();
            ImGui::TextDisabled("[Scale Gizmo]");
        }
        else
        {
            ImGui::InputFloat3("##Scale", &entity->EntityTransform.scale.x, "%.2f");
        }

        if (entity->EntityTransform.scale.x <= 0)
            entity->EntityTransform.scale.x = 0.1f;
        if (entity->EntityTransform.scale.y <= 0)
            entity->EntityTransform.scale.y = 0.1f;
        if (entity->EntityTransform.scale.z <= 0)
            entity->EntityTransform.scale.z = 0.1f;

        ImGui::BeginGroup();
        if (ImGui::Button("Uniform Scale"))
        {
            ImGui::OpenPopup("UniformScalePopup");
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Scale"))
        {
            entity->EntityTransform.scale = {1.0f, 1.0f, 1.0f};
        }
        ImGui::EndGroup();

        if (ImGui::BeginPopup("UniformScalePopup"))
        {
            static float uniformScale = 1.0f;
            ImGui::Text("Uniform Scale Factor");
            if (ImGui::InputFloat("##UniformScale", &uniformScale, 0.1f, 0.5f, "%.2f") && uniformScale > 0)
            {
                entity->EntityTransform.scale = {uniformScale, uniformScale, uniformScale};
            }

            if (ImGui::Button("Apply"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
}

void ObjectUI::RenderCubeComponentUI(CubeComponent *cube)
{
    GameEntity *entity = cube->entity;
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

        Vector3 scaledSize = {
            cube->size.x * entity->EntityTransform.scale.x,
            cube->size.y * entity->EntityTransform.scale.y,
            cube->size.z * entity->EntityTransform.scale.z};

        ImGui::TextDisabled("(Scaled: %.2f, %.2f, %.2f)", scaledSize.x, scaledSize.y, scaledSize.z);

        ImGui::Text("Color");
        ImVec4 colorVec = rlImGuiColors::Convert(cube->color);
        if (ImGui::ColorEdit4("##Color", (float *)&colorVec))
        {
            cube->color = rlImGuiColors::Convert(colorVec);
        }
    }
}

void ObjectUI::RenderSphereComponentUI(SphereComponent *sphere)
{
    GameEntity *entity = sphere->entity;
    if (sphere)
    {
        ImGui::Separator();
        ImGui::Text("Radius");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputFloat("##Radius", &sphere->radius, 0.1f, 0.5f, "%.2f");

        if (sphere->radius <= 0)
            sphere->radius = 0.1f;

        float avgScale = (entity->EntityTransform.scale.x + entity->EntityTransform.scale.y + entity->EntityTransform.scale.z) / 3.0f;
        float scaledRadius = sphere->radius * avgScale;
        ImGui::TextDisabled("(Scaled: %.2f)", scaledRadius);

        ImGui::Text("Color");
        ImVec4 colorVec = rlImGuiColors::Convert(sphere->color);
        if (ImGui::ColorEdit4("##Color", (float *)&colorVec))
        {
            sphere->color = rlImGuiColors::Convert(colorVec);
        }
    }
}

void ObjectUI::RenderModelComponentUI(ModelComponent *model)
{
    if (!model)
        return;

    GameEntity *entity = model->entity;

    ImGui::Separator();

    static bool fileBrowserInitialized = false;
    if (!fileBrowserInitialized)
    {
        fileDialog.SetTitle("Select Model File");
        fileDialog.SetTypeFilters({".obj", ".iqm", ".gltf", ".glb", ".vox"});
        fileBrowserInitialized = true;
    }

    ImGui::Text("Model File");

    std::string currentFile = model->filePath.empty() ? "No file selected" : model->filePath;
    ImGui::TextWrapped("Current: %s", currentFile.c_str());

    if (ImGui::Button("Browse..."))
    {
        fileDialog.Open();
    }

    fileDialog.Display();

    if (fileDialog.HasSelected())
    {
        std::string selectedPath = fileDialog.GetSelected().string();

        if (model->LoadModelFromFile(selectedPath))
        {
            model->filePath = selectedPath;
            ImGui::OpenPopup("ModelLoadSuccess");
        }
        else
        {
            ImGui::OpenPopup("ModelLoadError");
        }

        fileDialog.ClearSelected();
    }

    if (!model->filePath.empty())
    {
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
        {
            model->ClearModel();
        }
    }

    // I just like seeing this
    if (!model->filePath.empty() && model->IsLoaded())
    {
        ImGui::Separator();
        ImGui::Text("Model Info:");
        ImGui::TextDisabled("Vertices: %d", model->GetVertexCount());
        ImGui::TextDisabled("Triangles: %d", model->GetTriangleCount());
    }

    ImGui::Separator();
    ImGui::Text("Material");
    // TODO: Implement
}
