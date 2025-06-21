#pragma once
#include <raylib.h>
#include <raymath.h>

enum class GizmoMode
{
    NONE,
    POSITION
};

class GizmoSystem
{
private:
    float GetMovementAlongAxis(const Ray &mouseRay, int axis, Camera camera) const;
    GizmoMode mode = GizmoMode::NONE;
    Color axisColors[3] = {RED, GREEN, BLUE}; // X, Y, Z axes
    Vector3 *targetPosition = nullptr;
    Vector3 dragStartPos = {0, 0, 0};
    Vector3 dragStartMouseWorld = {0, 0, 0};
    bool isDragging = false;
    int selectedAxis = -1;
    float axisLength = 2.0f;
    float arrowHeadLength = 0.4f;
    float arrowHeadRadius = 0.15f;
    float axisRadius = 0.025f;
    float highlightScale = 1.2f;
    float lastAppliedDelta = 0.0f;
    float snapStep = 0.10f;
    float accumulatedMovement = 0.0f;
    float dragStartMovement = 0.0f;

    Ray GetAxisRay(int axis) const;
    bool CheckAxisHover(const Ray &mouseRay, int axis, float &distance) const;
    Vector3 GetAxisDirection(int axis) const;
    Vector3 ProjectMouseToAxis(const Ray &mouseRay, int axis, Camera camera) const;
    void DrawArrow(Vector3 start, Vector3 end, float radius, float headLength, float headRadius, Color color, bool highlighted = false);

public:
    GizmoSystem();
    void SetTarget(Vector3 *position);
    void Deactivate();
    bool Update(Camera camera, Ray mouseRay, Vector3 &position);
    void Render(Camera camera, Ray mouseRay);
    bool CheckForAxisClick(const Ray &mouseRay) const;
    bool IsMouseOverGizmo(const Ray &mouseRay) const;
    bool IsActive() const { return targetPosition != nullptr && mode != GizmoMode::NONE; }
    void SetSnapStep(float newSnapStep);
    void GetSnapStep(float *step) const { *step = snapStep; }
    Vector3 *GetTargetAddress() const { return targetPosition; }
};