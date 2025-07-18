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
 * @brief Sets the snap step used for gizmo rotation.
 *
 * This method updates the snap step value, which determines the incremental
 * rotation applied to the gizmo during dragging.
 *
 * @param degrees The desired snap step value for gizmo rotation.
 */
void GizmoSystem::SetRotationSnap(float degrees)
{
    rotationSnapDegrees = (degrees > 0.0f) ? degrees : 1.0f;
}

/**
 * @brief Sets the snap step used for gizmo scaling.
 *
 * This method updates the snap step value, which determines the incremental
 * scaling applied to the gizmo during dragging.
 *
 * @param step The desired snap step value for gizmo scaling.
 */
void GizmoSystem::SetScaleSnap(float step)
{
    scaleSnapStep = (step > 0.0f) ? step : 0.01f;
}

/**
 * @brief Sets the target position for the gizmo system.
 *
 * This method updates the target position pointer, which determines the current
 * editing target for the gizmo.
 *
 * @param position Pointer to the target position vector.
 */
void GizmoSystem::SetPositionTarget(Vector3 *position)
{
    targetPosition = position;
    mode = (position != nullptr) ? GizmoMode::POSITION : GizmoMode::NONE;
    isDragging = false;
    selectedAxis = -1;
}

/**
 * @brief Sets the target rotation for the gizmo system.
 *
 * This method updates the target rotation pointer, which determines the current
 * editing target for the gizmo.
 *
 * @param rotation Pointer to the target rotation quaternion.
 */
void GizmoSystem::SetRotationTarget(Quaternion *rotation)
{
    targetRotation = rotation;
    mode = (rotation != nullptr) ? GizmoMode::ROTATION : GizmoMode::NONE;
    isDragging = false;
    selectedAxis = -1;
}

void GizmoSystem::SetScaleTarget(Vector3 *scale)
{
    targetScale = scale;
    mode = (scale != nullptr) ? GizmoMode::SCALE : GizmoMode::NONE;
    isDragging = false;
    selectedAxis = -1;
}

/**
 * @brief Sets the target position and rotation for the gizmo system.
 *
 * This method updates the target position and rotation pointers, which determine
 * the current editing target for the gizmo.
 *
 * @param position Pointer to the target position vector.
 * @param rotation Pointer to the target rotation quaternion.
 * @param scale Pointer to the target scale vector.
 */
void GizmoSystem::SetTarget(Vector3 *position, Quaternion *rotation, Vector3 *scale)
{
    targetPosition = position;
    targetRotation = rotation;
    targetScale = scale;

    if (position && rotation && scale)
    {
        mode = GizmoMode::POSITION; // Default to position mode when all are available
    }
    else if (position)
    {
        mode = GizmoMode::POSITION;
    }
    else if (rotation)
    {
        mode = GizmoMode::ROTATION;
    }
    else if (scale)
    {
        mode = GizmoMode::SCALE;
    }
    else
    {
        mode = GizmoMode::NONE;
    }

    isDragging = false;
    selectedAxis = -1;
}

void GizmoSystem::SetMode(GizmoMode newMode)
{
    if (newMode == GizmoMode::POSITION && !targetPosition)
        return;
    if (newMode == GizmoMode::ROTATION && !targetRotation)
        return;
    if (newMode == GizmoMode::SCALE && !targetScale)
        return;

    mode = newMode;
    isDragging = false;
    selectedAxis = -1;
}

/**
 * @brief Deactivates the gizmo system.
 *
 * Resets the gizmo system to its default state, which means no target position
 * or rotation, mode set to NONE, and all other flags reset to their default
 * values.
 */
void GizmoSystem::Deactivate()
{
    mode = GizmoMode::NONE;
    targetPosition = nullptr;
    targetRotation = nullptr;
    targetScale = nullptr;
    isDragging = false;
    selectedAxis = -1;
    lastAppliedDelta = 0.0f;
}

/**
 * @brief Returns a unit vector representing the direction of the specified axis of the gizmo.
 *
 * This method returns a unit vector representing the direction of the specified axis
 *
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
 * direction of the specified axis.
 *
 * @param axis The axis index (0 = X, 1 = Y, 2 = Z).
 * @param camera The camera used to calculate the axis direction.
 * @return A Ray structure with the position set to the target position and the direction set to the axis direction.
 */
Ray GizmoSystem::GetAxisRay(int axis, Camera camera) const
{
    Ray axisRay;
    if (targetPosition)
    {
        axisRay.position = *targetPosition;
        axisRay.direction = TransformAxisDirection(axis, camera);
    }
    else
    {
        axisRay.position = {0, 0, 0};
        axisRay.direction = GetAxisDirection(axis);
    }
    return axisRay;
}

/**
 * @brief Draws an arrow from start to end position with specified dimensions
 *
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

    Vector3 shaftEnd = Vector3Add(start, Vector3Scale(direction, shaftLength));

    float currentRadius = highlighted ? radius * highlightScale : radius;
    float currentHeadRadius = highlighted ? headRadius * highlightScale : headRadius;

    DrawCylinderEx(start, shaftEnd, currentRadius, currentRadius, 8, color);
    DrawCylinderEx(shaftEnd, end, currentHeadRadius, 0.0f, 8, color);
}

/**
 * @brief Draws a rotation circle around the specified axis at the target position.
 *
 * This function visualizes a circular rotation gizmo around a given axis, centered on the target position.
 *
 * @param axis The axis around which the rotation circle is drawn.
 * @param color The color used to draw the circle.
 * @param highlighted Whether the circle should be drawn in a highlighted state.
 * @param camera The camera used for perspective transformations.
 */
void GizmoSystem::DrawRotationCircle(int axis, Color color, bool highlighted, Camera camera)
{
    if (!targetPosition)
        return;

    float gizmoScale = GetGizmoScale(camera);
    Vector3 center = *targetPosition;
    Vector3 axisDir = TransformAxisDirection(axis, camera);

    Vector3 up = {0, 1, 0};
    Vector3 right = Vector3Normalize(Vector3CrossProduct(axisDir, up));

    if (Vector3Length(right) < 0.01f)
    {
        up = {1, 0, 0};
        right = Vector3Normalize(Vector3CrossProduct(axisDir, up));
    }
    up = Vector3Normalize(Vector3CrossProduct(right, axisDir));

    float currentRadius = circleRadius * gizmoScale;
    float currentThickness = (highlighted ? circleThickness * highlightScale : circleThickness) * gizmoScale;

    for (int i = 0; i < circleSegments; i++)
    {
        float angle1 = (float)i / circleSegments * 2.0f * PI;
        float angle2 = (float)(i + 1) / circleSegments * 2.0f * PI;

        Vector3 p1 = Vector3Add(center, Vector3Add(Vector3Scale(right, cosf(angle1) * currentRadius), Vector3Scale(up, sinf(angle1) * currentRadius)));

        Vector3 p2 = Vector3Add(center, Vector3Add(Vector3Scale(right, cosf(angle2) * currentRadius), Vector3Scale(up, sinf(angle2) * currentRadius)));

        DrawCylinderEx(p1, p2, currentThickness, currentThickness, 4, color);
    }
}

Vector3 GizmoSystem::TransformAxisDirection(int axis, Camera camera) const
{
    Vector3 direction = GetAxisDirection(axis);

    if (mode == GizmoMode::SCALE && targetRotation)
    {
        // Just make it so it rotates with the object, looks nicer and less confusing, before sometimes youd scale the wrong axis
        direction = Vector3Transform(direction, QuaternionToMatrix(*targetRotation));
    }

    return direction;
}

/**
 * @brief Draws a scale axis with a line and a box at the end.
 *
 * @param axis The axis index determining the direction of the scale axis.
 * @param color The color used to draw the axis line and box.
 * @param highlighted Whether the axis should be drawn in a highlighted state.
 * @param camera The camera used to calculate the gizmo scale.
 */
void GizmoSystem::DrawScaleAxis(int axis, Color color, bool highlighted, Camera camera)
{
    if (!targetPosition)
        return;

    float gizmoScale = GetGizmoScale(camera);
    Vector3 direction = TransformAxisDirection(axis, camera);
    Vector3 lineEnd = Vector3Add(*targetPosition, Vector3Scale(direction, axisLength * gizmoScale));

    float currentRadius = (highlighted ? axisRadius * highlightScale : axisRadius) * gizmoScale;
    float currentSphereRadius = (highlighted ? scaleBoxSize * highlightScale : scaleBoxSize) * 0.5f * gizmoScale;

    // Draw the axis line
    DrawCylinderEx(*targetPosition, lineEnd, currentRadius, currentRadius, 8, color);

    DrawSphere(lineEnd, currentSphereRadius, color);
}

void GizmoSystem::DrawUniformScaleCircle(Color color, bool highlighted, Camera camera)
{
    if (!targetPosition)
        return;

    float gizmoScale = GetGizmoScale(camera);
    float orbRadius = uniformScaleCircleRadius * gizmoScale;
    float scale = highlighted ? highlightScale : 1.0f;
    DrawSphere(*targetPosition, orbRadius * scale, color);
}

float GizmoSystem::GetGizmoScale(Camera camera) const
{
    if (!targetPosition)
        return 1.0f;

    float distance = Vector3Distance(camera.position, *targetPosition);
    return distance * gizmoSize;
}

/**
 * @brief Checks if the given axis is hovered by the mouse ray
 *
 * @param mouseRay The ray from the mouse position and direction
 * @param axis The axis to check (0 = X, 1 = Y, 2 = Z)
 * @param distance The distance from the mouse ray to the axis if it is hovered
 * @param camera The camera used to calculate the gizmo scale
 * @return True if the axis is hovered, false otherwise
 */
bool GizmoSystem::CheckAxisHover(const Ray &mouseRay, int axis, float &distance, Camera camera) const
{
    if (mode != GizmoMode::POSITION || !targetPosition)
        return false;

    float gizmoScale = GetGizmoScale(camera);
    Vector3 direction = TransformAxisDirection(axis, camera);
    Vector3 scaledArrowHeadLength = Vector3Scale(direction, (axisLength - arrowHeadLength) * gizmoScale);
    Vector3 arrowHeadStart = Vector3Add(*targetPosition, scaledArrowHeadLength);

    // Check collision with arrow head
    RayCollision headCollision = GetRayCollisionSphere(mouseRay, Vector3Add(arrowHeadStart, Vector3Scale(direction, arrowHeadLength * gizmoScale * 0.5f)), arrowHeadRadius * gizmoScale);
    if (headCollision.hit)
    {
        distance = headCollision.distance;
        return true;
    }

    // Check collision with axis shaft
    const int segments = 10;
    float minDist = FLT_MAX;
    bool hit = false;
    float shaftLength = (axisLength - arrowHeadLength) * gizmoScale;

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        Vector3 axisPoint = Vector3Add(*targetPosition, Vector3Scale(direction, shaftLength * t));

        Vector3 rayToAxis = Vector3Subtract(axisPoint, mouseRay.position);
        float projection = Vector3DotProduct(rayToAxis, mouseRay.direction);
        Vector3 closestOnRay = Vector3Add(mouseRay.position, Vector3Scale(mouseRay.direction, projection));

        float dist = Vector3Distance(axisPoint, closestOnRay);
        if (dist < arrowHeadRadius * gizmoScale && dist < minDist)
        {
            minDist = dist;
            distance = projection;
            hit = true;
        }
    }

    return hit;
}

/**
 * @brief Checks whether the mouse ray is hovering over the rotation circle for a given axis.
 *
 * @param mouseRay The ray originating from the mouse's screen position.
 * @param axis The axis around which the rotation circle is centered (0 = X, 1 = Y, 2 = Z).
 * @param distance Output parameter that receives the distance from the ray origin to the intersection point on the circle's plane if a hover is detected.
 * @param camera The camera used to calculate the gizmo scale.
 * @return True if the mouse ray is close enough to the rotation circle to be considered hovering, false otherwise.
 */
bool GizmoSystem::CheckCircleHover(const Ray &mouseRay, int axis, float &distance, Camera camera) const
{
    if (mode != GizmoMode::ROTATION || !targetPosition)
        return false;

    float gizmoScale = GetGizmoScale(camera);
    Vector3 center = *targetPosition;
    Vector3 axisDir = TransformAxisDirection(axis, camera);

    // Create plane perpendicular to axis
    float denom = Vector3DotProduct(mouseRay.direction, axisDir);
    if (fabs(denom) < 0.0001f)
        return false;

    Vector3 rayToPlane = Vector3Subtract(center, mouseRay.position);
    float t = Vector3DotProduct(rayToPlane, axisDir) / denom;
    if (t < 0.001f)
        return false;

    Vector3 intersection = Vector3Add(mouseRay.position, Vector3Scale(mouseRay.direction, t));
    Vector3 toIntersection = Vector3Subtract(intersection, center);
    float distFromCenter = Vector3Length(toIntersection);

    // Check if intersection is near the circle
    float scaledRadius = circleRadius * gizmoScale;
    float tolerance = circleThickness * gizmoScale * 8.0f;
    if (fabs(distFromCenter - scaledRadius) < tolerance)
    {
        distance = t;
        return true;
    }

    return false;
}

/**
 * @brief Checks whether the mouse ray is hovering over the scale box at the end of the given axis.
 *
 * @param mouseRay The ray from the mouse's screen position.
 * @param axis The axis around which the scale box is centered (0 = X, 1 = Y, 2 = Z).
 * @param distance Output parameter that receives the distance from the ray origin to the intersection point on the scale box's plane if a hover is detected.
 * @param camera The camera used to calculate the gizmo scale.
 * @return True if the mouse ray is close enough to the scale box to be considered hovering, false otherwise.
 */
bool GizmoSystem::CheckScaleBoxHover(const Ray &mouseRay, int axis, float &distance, Camera camera) const
{
    if (mode != GizmoMode::SCALE || !targetPosition)
        return false;

    float gizmoScale = GetGizmoScale(camera);
    Vector3 direction = TransformAxisDirection(axis, camera);
    Vector3 boxCenter = Vector3Add(*targetPosition, Vector3Scale(direction, axisLength * gizmoScale));

    RayCollision collision = GetRayCollisionSphere(mouseRay, boxCenter, scaleBoxSize * gizmoScale);
    if (collision.hit)
    {
        distance = collision.distance;
        return true;
    }

    // Check axis line
    const int segments = 10;
    float minDist = FLT_MAX;
    bool hit = false;

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        Vector3 axisPoint = Vector3Add(*targetPosition, Vector3Scale(direction, axisLength * gizmoScale * t));

        Vector3 rayToAxis = Vector3Subtract(axisPoint, mouseRay.position);
        float projection = Vector3DotProduct(rayToAxis, mouseRay.direction);
        Vector3 closestOnRay = Vector3Add(mouseRay.position, Vector3Scale(mouseRay.direction, projection));

        float dist = Vector3Distance(axisPoint, closestOnRay);
        if (dist < axisRadius * gizmoScale * 2.0f && dist < minDist)
        {
            minDist = dist;
            distance = projection;
            hit = true;
        }
    }

    return hit;
}

bool GizmoSystem::CheckUniformScaleCircleHover(const Ray &mouseRay, float &distance, Camera camera) const
{
    if (mode != GizmoMode::SCALE || !targetPosition)
        return false;

    float gizmoScale = GetGizmoScale(camera);
    Vector3 center = *targetPosition;
    float scaledRadius = uniformScaleCircleRadius * gizmoScale;

    RayCollision collision = GetRayCollisionSphere(mouseRay, center, scaledRadius + uniformScaleCircleThickness * gizmoScale * 3.0f);
    if (collision.hit)
    {
        Vector3 hitPoint = Vector3Add(mouseRay.position, Vector3Scale(mouseRay.direction, collision.distance));
        Vector3 toHit = Vector3Subtract(hitPoint, center);
        float distFromCenter = Vector3Length(toHit);

        float tolerance = uniformScaleCircleThickness * gizmoScale * 4.0f;
        if (fabs(distFromCenter - scaledRadius) < tolerance)
        {
            distance = collision.distance;
            return true;
        }
    }

    return false;
}

/**
 * @brief Calculates the movement along a given axis based on the mouse ray intersection with a plane perpendicular to the axis
 *
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
 * @brief Calculates the scale along a given axis based on the mouse ray intersection with a plane perpendicular to the axis
 *
 * @param mouseRay The ray from the mouse position and direction
 * @param axis The axis to calculate the scale for (0 = X, 1 = Y, 2 = Z)
 * @param camera The active camera
 * @return The scale relative to the starting position
 */
float GizmoSystem::GetScaleAlongAxis(const Ray &mouseRay, int axis, Camera camera) const
{
    if (!targetPosition)
        return 0.0f;

    Vector3 axisDir = TransformAxisDirection(axis, camera);

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

    float movement = Vector3DotProduct(diff, axisDir);

    return movement * 0.5f;
}

float GizmoSystem::GetUniformScaleAmount(const Ray &mouseRay, Camera camera) const
{
    if (!targetPosition)
        return 1.0f;

    Vector2 currMousePos = GetMousePosition();
    float deltaY = dragStartScreenPos.y - currMousePos.y;

    float sensitivity = 0.005f;
    float scaleFactor = 1.0f + deltaY * sensitivity;

    scaleFactor = fmaxf(scaleFactor, 0.01f);

    return dragStartMovement * scaleFactor;
}

/**
 * @brief Normalizes an angle to the range [-PI, PI].
 *
 * This function adjusts the provided angle by adding or subtracting
 * multiples of 2*PI to ensure it falls within the standard range for
 * angles, which is between -PI and PI.
 *
 * @param angle The angle in radians to be normalized.
 * @return The normalized angle in radians within the range [-PI, PI].
 */
float GizmoSystem::NormalizeAngle(float angle) const
{
    while (angle > PI)
        angle -= 2.0f * PI;
    while (angle < -PI)
        angle += 2.0f * PI;
    return angle;
}

/**
 * @brief Calculates the signed angle between two vectors in radians.
 *
 * This function calculates the angle between two vectors in 3D space.
 *
 * @param a The first vector.
 * @param b The second vector.
 * @param normal The normal vector of the plane in which the angle is calculated.
 * @return The signed angle between the two vectors in radians.
 */
float GizmoSystem::GetAngleBetweenVectors(Vector3 a, Vector3 b, Vector3 normal) const
{
    Vector3 normA = Vector3Normalize(a);
    Vector3 normB = Vector3Normalize(b);

    float dot = Vector3DotProduct(normA, normB);
    dot = fmaxf(-1.0f, fminf(1.0f, dot)); // Clamp to avoid floating point errors

    float angle = acosf(dot);

    Vector3 cross = Vector3CrossProduct(normA, normB);
    if (Vector3DotProduct(cross, normal) < 0)
    {
        angle = -angle;
    }

    return angle;
}

/**
 * @brief Calculates the point on the circle where the mouse ray intersects with the plane of the circle.
 *
 * This function is used by the rotation gizmo to determine the point on the circle that the user is
 * currently hovering over.
 *
 * @param mouseRay The ray from the camera to the mouse position.
 * @param axis The axis of rotation.
 * @param camera The camera.
 * @return The point on the circle where the mouse ray intersects with the plane of the circle.
 */
Vector3 GizmoSystem::ProjectMouseToCircle(const Ray &mouseRay, int axis, Camera camera) const
{
    if (!targetPosition)
        return {0, 0, 0};

    Vector3 center = *targetPosition;
    Vector3 axisDir = GetAxisDirection(axis);

    float denom = Vector3DotProduct(mouseRay.direction, axisDir);
    if (fabs(denom) < 0.0001f)
        return dragStartMouseOnCircle;

    Vector3 rayToPlane = Vector3Subtract(center, mouseRay.position);
    float t = Vector3DotProduct(rayToPlane, axisDir) / denom;
    if (t < 0.001f)
        return dragStartMouseOnCircle;

    Vector3 intersection = Vector3Add(mouseRay.position, Vector3Scale(mouseRay.direction, t));
    Vector3 toIntersection = Vector3Subtract(intersection, center);

    float length = Vector3Length(toIntersection);
    if (length < 0.001f)
        return dragStartMouseOnCircle;

    return Vector3Scale(toIntersection, circleRadius / length);
}

/**
 * @brief Calculates the rotation around the given axis that the user is currently attempting to apply with the mouse.
 *
 * This function is used by the rotation gizmo to determine the rotation the user is
 * currently trying to apply.
 *
 * @param mouseRay The ray from the camera to the mouse position.
 * @param axis The axis of rotation.
 * @param camera The camera.
 * @return The rotation around the axis in radians.
 */
float GizmoSystem::GetRotationAroundAxis(const Ray &mouseRay, int axis, Camera camera) const
{
    if (!targetPosition)
        return 0.0f;

    Vector3 axisDir = GetAxisDirection(axis);
    Vector3 currentMouseOnCircle = ProjectMouseToCircle(mouseRay, axis, camera);

    return GetAngleBetweenVectors(dragStartMouseOnCircle, currentMouseOnCircle, axisDir);
}

/**
 * @brief Updates the gizmo system with the current mouse state and applies any transformations to the target object.
 *
 * This function should be called once per frame, and should be provided with the current mouse ray and the camera.
 * The method will return true if the gizmo system changed the target object in any way.
 *
 * @param camera The camera used to render the scene.
 * @param mouseRay The ray from the camera to the mouse position.
 * @param position The position of the target object to be updated.
 * @param rotation The rotation of the target object to be updated.
 * @param scale The scale of the target object to be updated.
 * @param transformComponent The TransformComponent of the target object to be updated, mainly for rotations.
 * @return true if the gizmo system changed the target object, false otherwise.
 */
bool GizmoSystem::Update(Camera camera, Ray mouseRay, Vector3 &position, Quaternion &rotation, Vector3 &scale, EntityTransform *entityTransform)
{
    if (mode == GizmoMode::NONE)
        return false;

    bool changed = false;
    bool isPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool isReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    if (isPressed && !isDragging)
    {
        float minDistance = FLT_MAX;
        int closestAxis = -1;

        if (mode == GizmoMode::SCALE)
        {
            float distance;
            if (CheckUniformScaleCircleHover(mouseRay, distance, camera))
            {
                minDistance = distance;
                closestAxis = 3;
            }
        }

        for (int i = 0; i < 3; i++)
        {
            float distance;
            bool hit = false;

            if (mode == GizmoMode::POSITION)
            {
                hit = CheckAxisHover(mouseRay, i, distance, camera);
            }
            else if (mode == GizmoMode::ROTATION)
            {
                hit = CheckCircleHover(mouseRay, i, distance, camera);
            }
            else if (mode == GizmoMode::SCALE)
            {
                hit = CheckScaleBoxHover(mouseRay, i, distance, camera);
            }

            if (hit && distance < minDistance && distance > 0)
            {
                minDistance = distance;
                closestAxis = i;
            }
        }

        if (closestAxis != -1)
        {
            isDragging = true;
            selectedAxis = closestAxis;

            if (mode == GizmoMode::POSITION && targetPosition)
            {
                dragStartPos = *targetPosition;
                dragStartMovement = GetMovementAlongAxis(mouseRay, selectedAxis, camera);
            }
            else if (mode == GizmoMode::ROTATION && targetRotation && targetPosition)
            {
                dragStartRotation = *targetRotation;
                dragStartMouseOnCircle = ProjectMouseToCircle(mouseRay, selectedAxis, camera);
                dragStartAngle = 0.0f;
            }
            else if (mode == GizmoMode::SCALE && targetScale && targetPosition)
            {
                dragStartScale = *targetScale;
                dragStartPos = *targetPosition;

                if (selectedAxis == 3)
                {
                    dragStartScreenPos = GetMousePosition();
                    dragStartMovement = GetUniformScaleAmount(mouseRay, camera);
                }
                else
                {
                    dragStartMovement = GetScaleAlongAxis(mouseRay, selectedAxis, camera);
                }
            }

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
        if (mode == GizmoMode::POSITION && targetPosition)
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
        else if (mode == GizmoMode::ROTATION && entityTransform)
        {
            float currentAngle = GetRotationAroundAxis(mouseRay, selectedAxis, camera);
            float totalAngleDegrees = currentAngle * RAD2DEG;
            float snappedAngleDegrees = floorf((totalAngleDegrees + rotationSnapDegrees * 0.5f) / rotationSnapDegrees) * rotationSnapDegrees;

            const float epsilon = 0.1f;
            if (fabs(snappedAngleDegrees - lastAppliedDelta) > epsilon)
            {
                Vector3 axisDir = GetAxisDirection(selectedAxis);
                entityTransform->RotateAroundWorldAxis(axisDir, snappedAngleDegrees - lastAppliedDelta);
                lastAppliedDelta = snappedAngleDegrees;
                changed = true;
            }
        }
        else if (mode == GizmoMode::SCALE && targetScale)
        {
            if (selectedAxis == 3)
            {
                float currentMovement = GetUniformScaleAmount(mouseRay, camera);
                float totalRawDelta = currentMovement - dragStartMovement;
                float snappedTotal = floorf((totalRawDelta + scaleSnapStep * 0.5f) / scaleSnapStep) * scaleSnapStep;

                const float epsilon = 0.001f;
                if (fabs(snappedTotal - lastAppliedDelta) > epsilon)
                {
                    float scaleFactor = 1.0f + snappedTotal;
                    scaleFactor = fmaxf(scaleFactor, 0.01f);

                    Vector3 newScale = {
                        dragStartScale.x * scaleFactor,
                        dragStartScale.y * scaleFactor,
                        dragStartScale.z * scaleFactor};

                    scale = newScale;
                    *targetScale = newScale;
                    lastAppliedDelta = snappedTotal;
                    changed = true;
                }
            }
            else
            {
                float currentMovement = GetScaleAlongAxis(mouseRay, selectedAxis, camera);
                float totalRawDelta = currentMovement - dragStartMovement;
                float snappedTotal = floorf((totalRawDelta + scaleSnapStep * 0.5f) / scaleSnapStep) * scaleSnapStep;

                const float epsilon = 0.001f;
                if (fabs(snappedTotal - lastAppliedDelta) > epsilon)
                {
                    Vector3 newScale = dragStartScale;
                    float scaleFactor = 1.0f + snappedTotal;
                    scaleFactor = fmaxf(scaleFactor, 0.01f);

                    switch (selectedAxis)
                    {
                    case 0:
                        newScale.x = dragStartScale.x * scaleFactor;
                        break;
                    case 1:
                        newScale.y = dragStartScale.y * scaleFactor;
                        break;
                    case 2:
                        newScale.z = dragStartScale.z * scaleFactor;
                        break;
                    }

                    scale = newScale;
                    *targetScale = newScale;
                    lastAppliedDelta = snappedTotal;
                    changed = true;
                }
            }
        }
    }

    return changed;
}

/**
 * @brief Renders the gizmo in the 3D scene.
 *
 * Renders the gizmo in the 3D scene using the provided camera and mouse ray.
 * Highlights the axis being hovered over or dragged.
 *
 * @param camera The camera used for rendering.
 * @param mouseRay The ray from the mouse used to determine hover/drag state.
 */
void GizmoSystem::Render(Camera camera, Ray mouseRay)
{
    if (mode == GizmoMode::NONE)
        return;

    float gizmoScale = GetGizmoScale(camera);

    int hoveredAxis = -1;
    if (!isDragging)
    {
        float minDistance = FLT_MAX;

        if (mode == GizmoMode::SCALE)
        {
            float distance;
            if (CheckUniformScaleCircleHover(mouseRay, distance, camera))
            {
                minDistance = distance;
                hoveredAxis = 3;
            }
        }

        for (int i = 0; i < 3; i++)
        {
            float distance;
            bool hit = false;

            if (mode == GizmoMode::POSITION)
            {
                hit = CheckAxisHover(mouseRay, i, distance, camera);
            }
            else if (mode == GizmoMode::ROTATION)
            {
                hit = CheckCircleHover(mouseRay, i, distance, camera);
            }
            else if (mode == GizmoMode::SCALE)
            {
                hit = CheckScaleBoxHover(mouseRay, i, distance, camera);
            }

            if (hit && distance < minDistance && distance > 0)
            {
                minDistance = distance;
                hoveredAxis = i;
            }
        }
    }

    if (mode == GizmoMode::POSITION && targetPosition)
    {
        for (int i = 0; i < 3; i++)
        {
            bool isHighlighted = (isDragging && selectedAxis == i) || (!isDragging && hoveredAxis == i);
            Vector3 direction = TransformAxisDirection(i, camera);
            Vector3 endPoint = Vector3Add(*targetPosition, Vector3Scale(direction, axisLength * gizmoScale));
            DrawArrow(*targetPosition, endPoint, axisRadius * gizmoScale,
                      arrowHeadLength * gizmoScale, arrowHeadRadius * gizmoScale, axisColors[i], isHighlighted);
        }

        DrawSphere(*targetPosition, axisRadius * gizmoScale * 2.0f, WHITE);
    }
    else if (mode == GizmoMode::ROTATION && targetPosition)
    {
        for (int i = 0; i < 3; i++)
        {
            bool isHighlighted = (isDragging && selectedAxis == i) || (!isDragging && hoveredAxis == i);
            DrawRotationCircle(i, axisColors[i], isHighlighted, camera);
        }

        DrawSphere(*targetPosition, circleThickness * gizmoScale * 3.0f, WHITE);
    }
    else if (mode == GizmoMode::SCALE && targetPosition)
    {
        for (int i = 0; i < 3; i++)
        {
            bool isHighlighted = (isDragging && selectedAxis == i) || (!isDragging && hoveredAxis == i);
            DrawScaleAxis(i, axisColors[i], isHighlighted, camera);
        }

        bool isUniformHighlighted = (isDragging && selectedAxis == 3) || (!isDragging && hoveredAxis == 3);
        DrawUniformScaleCircle(WHITE, isUniformHighlighted, camera);

        DrawSphere(*targetPosition, axisRadius * gizmoScale * 2.0f, WHITE);
    }
}

/**
 * @brief Checks if the given mouse ray intersects with any of the gizmo axes.
 *
 * Returns true if the given mouseRay intersects with any of the gizmo axes.
 * Will not return true if the gizmo is not active or if the mode is NONE.
 *
 * @param mouseRay The ray from the mouse to check.
 * @param camera The camera used for rendering.
 * @return True if the ray intersects with any of the gizmo axes.
 */
bool GizmoSystem::CheckForAxisClick(const Ray &mouseRay, Camera camera) const
{
    if (mode == GizmoMode::NONE)
        return false;

    if (mode == GizmoMode::SCALE)
    {
        float distance;
        if (CheckUniformScaleCircleHover(mouseRay, distance, camera))
            return true;
    }

    for (int i = 0; i < 3; i++)
    {
        float distance;
        bool hit = false;

        if (mode == GizmoMode::POSITION)
        {
            hit = CheckAxisHover(mouseRay, i, distance, camera);
        }
        else if (mode == GizmoMode::ROTATION)
        {
            hit = CheckCircleHover(mouseRay, i, distance, camera);
        }
        else if (mode == GizmoMode::SCALE)
        {
            hit = CheckScaleBoxHover(mouseRay, i, distance, camera);
        }

        if (hit)
            return true;
    }
    return false;
}

/**
 * @brief Checks if the given mouse ray intersects with any of the gizmo axes.
 *
 * Returns true if the given mouseRay intersects with any of the gizmo axes.
 * Will not return true if the gizmo is not active or if the mode is NONE.
 *
 * @param mouseRay The ray from the mouse to check.
 * @return True if the ray intersects with any of the gizmo axes.
 */
bool GizmoSystem::IsMouseOverGizmo(const Ray &mouseRay, Camera camera) const
{
    return CheckForAxisClick(mouseRay, camera);
}