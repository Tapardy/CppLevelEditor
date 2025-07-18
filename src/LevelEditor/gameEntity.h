#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <raymath.h>
#include <string>

// Forward declaration, otherwise the component it doesn't know (kinda need it cuz templates have to be here)
class GameEntity;

// Pretty handy if we could only have 1 component of the same category per entity, breaks less things
enum class ComponentCategory
{
    Object,
};

class Component
{
public:
    virtual ~Component() = default;
    // Pointer to the entity that owns it
    GameEntity *entity = nullptr;
    const ComponentCategory category;

    Component(ComponentCategory componentCategory) : category(componentCategory) {}
};

struct EntityTransform
{
    Vector3 position = {0, 0, 0};
    Vector3 scale = {1, 1, 1};
    Quaternion rotation = QuaternionIdentity();
    Vector3 eulerAngles = {0, 0, 0};
    bool useEulerStorage = true;

    Matrix GetTransformMatrix() const
    {
        Matrix scaling = MatrixScale(scale.x, scale.y, scale.z);
        Matrix rotationMatrix = QuaternionToMatrix(rotation);
        // Scale first, then rotate!!!!
        return MatrixMultiply(scaling, rotationMatrix);
    }

    // Rotate around world axes
    void RotateAroundWorldAxis(Vector3 axis, float angleDegrees)
    {
        Quaternion deltaRotation = QuaternionFromAxisAngle(Vector3Normalize(axis), angleDegrees * DEG2RAD);
        rotation = QuaternionNormalize(QuaternionMultiply(deltaRotation, rotation));
        if (useEulerStorage)
        {
            UpdateEulerFromQuaternion();

            if (fabs(axis.x - 1.0f) < 0.001f)
                eulerAngles.x += angleDegrees;
            else if (fabs(axis.y - 1.0f) < 0.001f)
                eulerAngles.y += angleDegrees;
            else if (fabs(axis.z - 1.0f) < 0.001f)
                eulerAngles.z += angleDegrees;
        }
    }

    // Rotate around local axes
    void RotateAroundLocalAxis(Vector3 axis, float angleDegrees)
    {
        Quaternion deltaRotation = QuaternionFromAxisAngle(Vector3Normalize(axis), angleDegrees * DEG2RAD);
        rotation = QuaternionNormalize(QuaternionMultiply(rotation, deltaRotation));
        if (useEulerStorage)
            UpdateEulerFromQuaternion();
    }

    void ResetRotation()
    {
        rotation = QuaternionIdentity();
        eulerAngles = {0, 0, 0};
    }

    // Get forward, right, up vectors (useful for understanding current rotation)
    Vector3 GetForward() const { return Vector3Transform({0, 0, -1}, QuaternionToMatrix(rotation)); }
    Vector3 GetRight() const { return Vector3Transform({1, 0, 0}, QuaternionToMatrix(rotation)); }
    Vector3 GetUp() const { return Vector3Transform({0, 1, 0}, QuaternionToMatrix(rotation)); }

    Vector3 GetEulerAngles() const
    {
        if (useEulerStorage)
            return eulerAngles;

        Matrix m = QuaternionToMatrix(rotation);
        Vector3 rad = {0};

        if (fabsf(m.m1) < 0.99999f)
        {
            rad.x = asinf(-m.m1);       // pitch (X)
            rad.y = atan2f(m.m2, m.m0); // yaw (Y)
            rad.z = atan2f(m.m5, m.m9); // roll (Z)
        }
        else
        {
            rad.x = asinf(-m.m1);
            rad.y = atan2f(-m.m8, m.m10);
            rad.z = 0;
        }

        return {rad.x * RAD2DEG, rad.y * RAD2DEG, rad.z * RAD2DEG};
    }

    void SetEulerAngles(Vector3 degrees)
    {
        eulerAngles = degrees;
        UpdateQuaternionFromEuler();
    }

    void SetEulerAngles(float x, float y, float z) { SetEulerAngles({x, y, z}); }

    // EULER ANGLE FUNCTIONS FOR UI INTEGRATION (kinda nice to have yk)
    void AddEulerAngles(Vector3 degrees)
    {
        eulerAngles = Vector3Add(eulerAngles, degrees);
        WrapEulerAngles();
        UpdateQuaternionFromEuler();
    }

    void AddEulerAngles(float x, float y, float z) { AddEulerAngles({x, y, z}); }

    void UpdateQuaternionFromEuler()
    {
        float pitch = eulerAngles.x * DEG2RAD;
        float yaw = eulerAngles.y * DEG2RAD;
        float roll = eulerAngles.z * DEG2RAD;

        Quaternion qZ = QuaternionFromAxisAngle({0, 0, 1}, roll);
        Quaternion qX = QuaternionFromAxisAngle({1, 0, 0}, pitch);
        Quaternion qY = QuaternionFromAxisAngle({0, 1, 0}, yaw);

        rotation = QuaternionMultiply(QuaternionMultiply(qY, qX), qZ);
        rotation = QuaternionNormalize(rotation);
    }

    void UpdateEulerFromQuaternion()
    {
        eulerAngles = GetEulerAngles();
    }

    void WrapEulerAngles()
    {
        eulerAngles.x = fmodf(eulerAngles.x + 180.0f, 360.0f) - 180.0f;
        eulerAngles.y = fmodf(eulerAngles.y + 180.0f, 360.0f) - 180.0f;
        eulerAngles.z = fmodf(eulerAngles.z + 180.0f, 360.0f) - 180.0f;
    }

    void SetEulerStorageMode(bool enable)
    {
        if (enable != useEulerStorage)
        {
            if (enable)
                UpdateEulerFromQuaternion();
            useEulerStorage = enable;
        }
    }
};

class GameEntity
{
public:
    GameEntity() : name("Entity"), EntityTransform() {}
    ~GameEntity() = default;

    // Default to "Entity" as name
    std::string name;
    EntityTransform EntityTransform;

    template <typename T, typename... Args>
    T *AddComponent(Args &&...args)
    {
        // Create a new component of type T and making it unique
        auto component = std::make_unique<T>(std::forward<Args>(args)...);

        // Just check all the components if theres one with the same category
        for (const auto &pair : components)
        {
            const auto &existingComponent = pair.second;
            if (existingComponent->category == component->category)
            {
                return nullptr;
            }
        }

        // Can't be null, so no reference, so we make it a pointer instead of direct
        T *componentPtr = component.get();
        componentPtr->entity = this;

        // Store the uPointer in the map using the type index as the key
        components[std::type_index(typeid(T))] = std::move(component);

        return componentPtr;
    }

    template <typename T>
    T *GetComponent()
    {
        auto component = components.find(std::type_index(typeid(T)));

        if (component != components.end())
        {
            // Return as pointer of T
            return static_cast<T *>(component->second.get());
        }

        return nullptr;
    }

    template <typename T>
    T *RemoveComponent()
    {
        auto component = components.find(std::type_index(typeid(T)));

        if (component != components.end())
        {
            T *componentPtr = static_cast<T *>(component->second.get());
            componentPtr->entity = nullptr;
            components.erase(component);

            return componentPtr;
        }
        return nullptr;
    }

    template <typename T>
    bool HasComponent()
    {
        return components.find(std::type_index(typeid(T))) != components.end();
    }

    void SetName(std::string name) { this->name = name; }
    const std::string &GetName() const { return name; }

private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};

struct CubeComponent : Component
{
    CubeComponent() : Component(ComponentCategory::Object) {}
    Vector3 size = {1, 1, 1};
    Color color = GRAY;

    Vector3 GetScaledSize() const
    {
        if (entity)
        {
            return {
                size.x * entity->EntityTransform.scale.x,
                size.y * entity->EntityTransform.scale.y,
                size.z * entity->EntityTransform.scale.z};
        }
        return size;
    }

    BoundingBox GetBoundingBox() const
    {
        if (!entity)
        {
            return {Vector3{0, 0, 0}, Vector3{0, 0, 0}};
        }

        Vector3 scaledSize = GetScaledSize();
        return {
            Vector3Subtract(entity->EntityTransform.position, Vector3Scale(scaledSize, 0.5f)),
            Vector3Add(entity->EntityTransform.position, Vector3Scale(scaledSize, 0.5f))};
    }
};

struct SphereComponent : Component
{
    SphereComponent() : Component(ComponentCategory::Object) {}
    float radius = 1.0f;
    Color color = GRAY;

    float GetScaledRadius() const
    {
        if (entity)
        {
            float avgScale = (entity->EntityTransform.scale.x + entity->EntityTransform.scale.y + entity->EntityTransform.scale.z) / 3.0f;
            return radius * avgScale;
        }
        return radius;
    }
};

struct MeshComponent : Component
{
    MeshComponent() : Component(ComponentCategory::Object) {}

    Mesh *mesh = nullptr;
    Material *material = nullptr;

    Vector3 GetPosition() const
    {
        if (entity)
            return entity->EntityTransform.position;
        return {0, 0, 0};
    }
    Quaternion GetRotation() const
    {
        if (entity)
            return entity->EntityTransform.rotation;
        return QuaternionIdentity();
    }
    Vector3 GetScale() const
    {
        if (entity)
            return entity->EntityTransform.scale;
        return {1, 1, 1};
    }
};
