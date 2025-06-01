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

    GizmoMode mode;
    Color axisColors[3];
    Vector3 *targetPosition;
    Vector3 dragStartPos;
    Vector3 dragStartMouseWorld;
    bool isDragging;
    int selectedAxis;
    float axisLength;
    float handleRadius;
    float highlightScale;
    float lastAppliedDelta;
    float snapStep;
    float accumulatedMovement;
    float dragStartMovement;

    Ray GetAxisRay(int axis) const;
    bool CheckAxisHover(const Ray &mouseRay, int axis, float &distance) const;
    Vector3 GetAxisDirection(int axis) const;
    Vector3 ProjectMouseToAxis(const Ray &mouseRay, int axis, Camera camera) const;

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