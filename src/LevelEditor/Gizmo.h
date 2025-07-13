#pragma once

#include "../LevelEditor/gameEntity.h"

enum class GizmoMode
{
    NONE,
    POSITION,
    ROTATION,
    SCALE
};
class GizmoSystem
{
public:
    GizmoSystem();
    void SetPositionTarget(Vector3 *position);
    void SetRotationTarget(Quaternion *rotation);
    void SetScaleTarget(Vector3 *scale);
    void SetTarget(Vector3 *position, Quaternion *rotation = nullptr, Vector3 *scale = nullptr);
    void Deactivate();
    bool Update(Camera camera, Ray mouseRay, Vector3 &position, Quaternion &rotation, Vector3 &scale, TransformComponent *transformComponent);
    void Render(Camera camera, Ray mouseRay);
    bool CheckForAxisClick(const Ray &mouseRay) const;
    bool IsMouseOverGizmo(const Ray &mouseRay) const;
    bool IsActive() const { return (targetPosition != nullptr || targetRotation != nullptr || targetScale != nullptr) && mode != GizmoMode::NONE; }
    void SetSnapStep(float newSnapStep);
    void SetRotationSnap(float degrees);
    void SetScaleSnap(float step);
    void GetSnapStep(float *step) const { *step = snapStep; }
    void GetRotationSnap(float *degrees) const { *degrees = rotationSnapDegrees; }
    void GetScaleSnap(float *step) const { *step = scaleSnapStep; }
    Vector3 *GetTargetPositionAddress() const { return targetPosition; }
    Quaternion *GetTargetRotationAddress() const { return targetRotation; }
    Vector3 *GetTargetScaleAddress() const { return targetScale; }
    void SetMode(GizmoMode newMode);
    GizmoMode GetMode() const { return mode; }

private:
    float GetMovementAlongAxis(const Ray &mouseRay, int axis, Camera camera) const;
    float GetRotationAroundAxis(const Ray &mouseRay, int axis, Camera camera) const;
    float GetScaleAlongAxis(const Ray &mouseRay, int axis, Camera camera) const;
    float GetUniformScaleAmount(const Ray &mouseRay, Camera camera) const;
    GizmoMode mode = GizmoMode::NONE;
    Color axisColors[3] = {RED, GREEN, BLUE}; // X, Y, Z axes

    Vector3 *targetPosition = nullptr;
    Vector3 dragStartPos = {0, 0, 0};
    Vector3 dragStartMouseWorld = {0, 0, 0};

    Quaternion *targetRotation = nullptr;
    Quaternion dragStartRotation = {0, 0, 0, 1};
    Vector3 dragStartMouseOnCircle = {0, 0, 0};
    float dragStartAngle = 0.0f;

    Vector3 *targetScale = nullptr;
    Vector3 dragStartScale = {1, 1, 1};
    Vector2 dragStartScreenPos;
    bool isDragging = false;
    int selectedAxis = -1; // -1 = none, 0-2 = X,Y,Z axes, 3 = uniform scale
    float axisLength = 2.0f;
    float arrowHeadLength = 0.4f;
    float arrowHeadRadius = 0.15f;
    float axisRadius = 0.025f;
    float circleRadius = 1.5f;
    float circleThickness = 0.05f;
    int circleSegments = 64;
    float highlightScale = 1.2f;
    float scaleBoxSize = 0.2f;

    float uniformScaleCircleRadius = 0.2f;
    float uniformScaleCircleThickness = 0.08f;
    float lastAppliedDelta = 0.0f;
    float snapStep = 0.10f;
    float rotationSnapDegrees = 15.0f;
    float scaleSnapStep = 0.1f;
    float accumulatedMovement = 0.0f;
    float dragStartMovement = 0.0f;

    Ray GetAxisRay(int axis) const;
    bool CheckAxisHover(const Ray &mouseRay, int axis, float &distance) const;
    bool CheckCircleHover(const Ray &mouseRay, int axis, float &distance) const;
    bool CheckScaleBoxHover(const Ray &mouseRay, int axis, float &distance) const;
    bool CheckUniformScaleCircleHover(const Ray &mouseRay, float &distance) const;
    Vector3 GetAxisDirection(int axis) const;
    Vector3 ProjectMouseToAxis(const Ray &mouseRay, int axis, Camera camera) const;
    Vector3 ProjectMouseToCircle(const Ray &mouseRay, int axis, Camera camera) const;
    void DrawArrow(Vector3 start, Vector3 end, float radius, float headLength, float headRadius, Color color, bool highlighted = false);
    void DrawRotationCircle(int axis, Color color, bool highlighted = false);
    void DrawScaleAxis(int axis, Color color, bool highlighted = false);
    void DrawUniformScaleCircle(Color color, bool highlighted = false);
    float NormalizeAngle(float angle) const;
    float GetAngleBetweenVectors(Vector3 a, Vector3 b, Vector3 normal) const;
};