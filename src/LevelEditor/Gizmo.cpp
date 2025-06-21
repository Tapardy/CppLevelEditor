#include "gizmo.h"
#include <float.h>

/**
 * @brief Default constructor for the GizmoSystem.
 *
 * Initializes the gizmo system in its default state, with no target position
 * and the mode set to NONE. All default values are now set in the header file.
 */
GizmoSystem::GizmoSystem()
{
    // All initialization is now handled by default member initializers in the header
}

/**
 * @brief Sets the snap step used for gizmo movement.
 *
 * This method updates the snap step value, which determines the incremental
 * movement applied to the gizmo during dragging, essentially making it snap to grid
 *
 * @param newSnapStep The desired snap step value for gizmo movement.
 */
void GizmoSystem::SetSnapStep(float newSnapStep)
{
    snapStep = (newSnapStep > 0.0f) ? newSnapStep : 0.01f;
}

/**
 * @brief Sets the target position for the gizmo system and determines the
 *        mode based on whether the target is set or not.
 *
 * If the target is set (i.e. not nullptr), the gizmo system is activated with
 * the mode set to POSITION. If the target is not set, the gizmo system is
 * deactivated and the mode is set to NONE.
 *
 * @param position The target position to set or nullptr to deactivate the
 *                 gizmo system.
 */
void GizmoSystem::SetTarget(Vector3 *position)
{
    targetPosition = position;
    mode = (position != nullptr) ? GizmoMode::POSITION : GizmoMode::NONE;
    isDragging = false;
    selectedAxis = -1;
}

/**
 * @brief Deactivates the gizmo system, resetting its state.
 *
 * This method effectively deactivates the gizmo,
 * making it inactive until a new target is set.
 */
void GizmoSystem::Deactivate()
{
    mode = GizmoMode::NONE;
    targetPosition = nullptr;
    isDragging = false;
    selectedAxis = -1;
    lastAppliedDelta = 0.0f;
}

/**
 * @brief Returns a unit vector representing the direction of the specified axis of the gizmo.
 * @param axis The axis to get the direction for (0 = X, 1 = Y, 2 = Z)
 * @return A unit vector representing the direction of the specified axis
 */
Vector3 GizmoSystem::GetAxisDirection(int axis) const
{
    Vector3 direction = {0, 0, 0};
    if (axis == 0)
        direction.x = 1.0f; // X-axis
    else if (axis == 1)
        direction.y = 1.0f; // Y-axis
    else if (axis == 2)
        direction.z = 1.0f; // Z-axis
    return direction;
}

/**
 * @brief Returns a ray representing the specified axis of the gizmo.
 *
 * The ray originates at the target position of the gizmo and extends in the
 * direction of the specified axis. The axis is represented as an integer where
 * 0 corresponds to the X-axis, 1 to the Y-axis, and 2 to the Z-axis.
 *
 * @param axis The axis index (0 = X, 1 = Y, 2 = Z).
 * @return A Ray structure with the position set to the target position and the
 * direction set to the axis direction.
 */
Ray GizmoSystem::GetAxisRay(int axis) const
{
    Ray axisRay;
    axisRay.position = *targetPosition;
    axisRay.direction = GetAxisDirection(axis);
    return axisRay;
}

/**
 * @brief Draws an arrow from start to end position with specified dimensions
 * @param start Starting position of the arrow
 * @param end Ending position of the arrow
 * @param radius Radius of the arrow shaft
 * @param headLength Length of the arrow head
 * @param headRadius Radius of the arrow head base
 * @param color Color of the arrow
 * @param highlighted Whether to draw the arrow in highlighted state
 */
void GizmoSystem::DrawArrow(Vector3 start, Vector3 end, float radius, float headLength, float headRadius, Color color, bool highlighted)
{
    Vector3 direction = Vector3Normalize(Vector3Subtract(end, start));
    float totalLength = Vector3Distance(start, end);
    float shaftLength = totalLength - headLength;

    // Calculate shaft end position
    Vector3 shaftEnd = Vector3Add(start, Vector3Scale(direction, shaftLength));

    // Apply highlight scaling if needed
    float currentRadius = highlighted ? radius * highlightScale : radius;
    float currentHeadRadius = highlighted ? headRadius * highlightScale : headRadius;

    // Draw shaft as cylinder
    DrawCylinderEx(start, shaftEnd, currentRadius, currentRadius, 8, color);

    // Draw arrow head as cone
    DrawCylinderEx(shaftEnd, end, currentHeadRadius, 0.0f, 8, color);
}

/**
 * @brief Checks if the given axis is hovered by the mouse ray
 * @param mouseRay The ray from the mouse position and direction
 * @param axis The axis to check (0 = X, 1 = Y, 2 = Z)
 * @param distance The distance from the mouse ray to the axis if it is hovered
 * @return True if the axis is hovered, false otherwise
 */
bool GizmoSystem::CheckAxisHover(const Ray &mouseRay, int axis, float &distance) const
{
    if (!targetPosition)
        return false;

    Vector3 axisDir = GetAxisDirection(axis);
    Vector3 arrowHeadStart = Vector3Add(*targetPosition, Vector3Scale(axisDir, axisLength - arrowHeadLength));

    // Check collision with arrow head (cone)
    RayCollision headCollision = GetRayCollisionSphere(mouseRay, Vector3Add(arrowHeadStart, Vector3Scale(axisDir, arrowHeadLength * 0.5f)), arrowHeadRadius);
    if (headCollision.hit)
    {
        distance = headCollision.distance;
        return true;
    }

    // Check collision with axis shaft using cylinder approximation
    const int segments = 10;
    float minDist = FLT_MAX;
    bool hit = false;
    float shaftLength = axisLength - arrowHeadLength;

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        Vector3 axisPoint = Vector3Add(*targetPosition, Vector3Scale(axisDir, shaftLength * t));

        Vector3 rayToAxis = Vector3Subtract(axisPoint, mouseRay.position);
        float projection = Vector3DotProduct(rayToAxis, mouseRay.direction);
        Vector3 closestOnRay = Vector3Add(mouseRay.position, Vector3Scale(mouseRay.direction, projection));

        float dist = Vector3Distance(axisPoint, closestOnRay);
        if (dist < arrowHeadRadius && dist < minDist)
        {
            minDist = dist;
            distance = projection;
            hit = true;
        }
    }

    return hit;
}

/**
 * @brief Calculates the movement along a given axis based on the mouse ray intersection with a plane perpendicular to the axis
 * @param mouseRay The ray from the mouse position and direction
 * @param axis The axis to calculate the movement for (0 = X, 1 = Y, 2 = Z)
 * @param camera The active camera
 * @return The movement along the axis
 */
float GizmoSystem::GetMovementAlongAxis(const Ray &mouseRay, int axis, Camera camera) const
{
    if (!targetPosition)
        return 0.0f;

    Vector3 axisDir = GetAxisDirection(axis);

    // Create a plane perpendicular to the axis
    Vector3 up = {0, 1, 0};
    Vector3 planeNormal = Vector3Normalize(Vector3CrossProduct(axisDir, up));

    // Fallback if axis is parallel to 'up'
    if (Vector3Length(planeNormal) < 0.01f)
        planeNormal = Vector3Normalize(Vector3CrossProduct(axisDir, Vector3{1, 0, 0}));

    Vector3 planePoint = dragStartPos;

    float denom = Vector3DotProduct(mouseRay.direction, planeNormal);
    if (fabs(denom) < 0.0001f)
        return dragStartMovement + lastAppliedDelta;

    Vector3 rayToPlane = Vector3Subtract(planePoint, mouseRay.position);
    float t = Vector3DotProduct(rayToPlane, planeNormal) / denom;
    if (t < 0.001f)
        return dragStartMovement + lastAppliedDelta;

    Vector3 intersection = Vector3Add(mouseRay.position, Vector3Scale(mouseRay.direction, t));
    Vector3 diff = Vector3Subtract(intersection, dragStartPos);

    return Vector3DotProduct(diff, axisDir);
}

/**
 * @brief Updates the gizmo system based on the current mouse state and camera
 * @param camera The active camera
 * @param mouseRay The ray from the mouse position and direction
 * @param position The current position of the gizmo target
 * @return True if the gizmo target position changed, false otherwise
 */
bool GizmoSystem::Update(Camera camera, Ray mouseRay, Vector3 &position)
{
    if (!targetPosition || mode == GizmoMode::NONE)
        return false;

    bool changed = false;

    bool isPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool isReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    if (isPressed && !isDragging)
    {
        float minDistance = FLT_MAX;
        int closestAxis = -1;

        for (int i = 0; i < 3; i++)
        {
            float distance;
            if (CheckAxisHover(mouseRay, i, distance) && distance < minDistance && distance > 0)
            {
                minDistance = distance;
                closestAxis = i;
            }
        }

        if (closestAxis != -1)
        {
            isDragging = true;
            selectedAxis = closestAxis;
            dragStartPos = *targetPosition;
            dragStartMovement = GetMovementAlongAxis(mouseRay, selectedAxis, camera);
            lastAppliedDelta = 0.0f;
        }
    }
    else if (isReleased && isDragging)
    {
        isDragging = false;
        selectedAxis = -1;
        lastAppliedDelta = 0.0f;
    }
    else if (isDragging && selectedAxis != -1)
    {
        float currentMovement = GetMovementAlongAxis(mouseRay, selectedAxis, camera);
        float totalRawDelta = currentMovement - dragStartMovement;
        float snappedTotal = floorf((totalRawDelta + snapStep * 0.5f) / snapStep) * snapStep;

        const float epsilon = 0.0001f;
        if (fabs(snappedTotal - lastAppliedDelta) > epsilon)
        {
            Vector3 newPosition = dragStartPos;
            switch (selectedAxis)
            {
            case 0:
                newPosition.x = dragStartPos.x + snappedTotal;
                break;
            case 1:
                newPosition.y = dragStartPos.y + snappedTotal;
                break;
            case 2:
                newPosition.z = dragStartPos.z + snappedTotal;
                break;
            }
            position = newPosition;
            *targetPosition = newPosition;
            lastAppliedDelta = snappedTotal;
            changed = true;
        }
    }

    return changed;
}

/**
 * Renders the gizmo in the 3D scene using the provided camera and mouse ray.
 * Highlights the axis being hovered over or dragged, if any, and draws the
 * gizmo's axes as arrows with cone heads. Displays the current axis
 * being dragged along with snapping information if dragging is active.
 *
 * @param camera The camera used for rendering.
 * @param mouseRay The ray from the mouse used to determine hover/drag state.
 */
void GizmoSystem::Render(Camera camera, Ray mouseRay)
{
    if (!targetPosition || mode == GizmoMode::NONE)
        return;

    int hoveredAxis = -1;
    if (!isDragging)
    {
        float minDistance = FLT_MAX;
        for (int i = 0; i < 3; i++)
        {
            float distance;
            if (CheckAxisHover(mouseRay, i, distance) && distance < minDistance && distance > 0)
            {
                minDistance = distance;
                hoveredAxis = i;
            }
        }
    }

    // Draw arrows for each axis
    for (int i = 0; i < 3; i++)
    {
        bool isHighlighted = (isDragging && selectedAxis == i) || (!isDragging && hoveredAxis == i);

        Vector3 direction = GetAxisDirection(i);
        Vector3 endPoint = Vector3Add(*targetPosition, Vector3Scale(direction, axisLength));

        DrawArrow(*targetPosition, endPoint, axisRadius, arrowHeadLength, arrowHeadRadius, axisColors[i], isHighlighted);
    }

    // Draw center sphere
    DrawSphere(*targetPosition, axisRadius * 2.0f, WHITE);

    if (isDragging)
    {
        const char *axisNames[] = {"X", "Y", "Z"};
        DrawText(TextFormat("Dragging %s axis (snapStep=%.3f)", axisNames[selectedAxis], snapStep), 10, 30, 20, BLACK);
    }
}

/**
 * Returns true if the given mouseRay intersects with any of the gizmo axes.
 * Will not return true if the gizmo is not active or if the mode is NONE.
 * @param mouseRay The ray from the mouse to check.
 * @return True if the ray intersects with any of the gizmo axes.
 */
bool GizmoSystem::CheckForAxisClick(const Ray &mouseRay) const
{
    if (!targetPosition || mode == GizmoMode::NONE)
        return false;

    for (int i = 0; i < 3; i++)
    {
        float distance;
        if (CheckAxisHover(mouseRay, i, distance))
        {
            return true;
        }
    }
    return false;
}

/**
 * Returns true if the given mouseRay intersects with any of the gizmo axes.
 * Will not return true if the gizmo is not active or if the mode is NONE.
 * @param mouseRay The ray from the mouse to check.
 * @return True if the ray intersects with any of the gizmo axes.
 */
bool GizmoSystem::IsMouseOverGizmo(const Ray &mouseRay) const
{
    return CheckForAxisClick(mouseRay);
}