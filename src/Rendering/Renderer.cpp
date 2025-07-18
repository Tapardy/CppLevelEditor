#include "Renderer.h"
#include <rlgl.h>
#include <raymath.h>

void Renderer::RenderComponents(const std::vector<GameEntity *> &entities, GameEntity *selectedEntity)
{
    for (auto entity : entities)
    {
        auto transform = entity->GetComponent<TransformComponent>();
        if (!transform)
            continue;

        rlPushMatrix();
        rlTranslatef(transform->position.x, transform->position.y, transform->position.z);

        Matrix transformMatrix = transform->GetTransformMatrix();
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

        rlPopMatrix();
    }
}
