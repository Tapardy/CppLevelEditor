#include "Renderer.h"
#include <rlgl.h>
#include <raymath.h>

void Renderer::RenderComponents(const std::vector<GameEntity *> &entities, GameEntity *selectedEntity)
{
    for (auto entity : entities)
    {
        rlPushMatrix();
        rlTranslatef(entity->EntityTransform.position.x, entity->EntityTransform.position.y, entity->EntityTransform.position.z);

        Matrix transformMatrix = entity->EntityTransform.GetTransformMatrix();
        rlMultMatrixf(MatrixToFloat(transformMatrix));

        if (auto cube = entity->GetComponent<CubeComponent>())
        {
            DrawCubeV(Vector3{0, 0, 0}, cube->size, cube->color);
            if (entity == selectedEntity)
                DrawCubeWiresV(Vector3{0, 0, 0}, Vector3{cube->size.x + 0.02f, cube->size.y + 0.02f, cube->size.z + 0.02f}, BLACK);
        }
        else if (auto sphere = entity->GetComponent<SphereComponent>())
        {
            DrawSphere(Vector3{0, 0, 0}, sphere->radius, sphere->color);
            if (entity == selectedEntity)
                DrawSphereWires(Vector3{0, 0, 0}, sphere->radius + 0.01f, 16, 16, BLACK);
        }
        else if (auto model = entity->GetComponent<ModelComponent>())
        {
            if (model->IsLoaded())
            {
                DrawModel(model->model, Vector3{0, 0, 0}, 1.0f, WHITE);
            }
        }

        rlPopMatrix();
    }
}
