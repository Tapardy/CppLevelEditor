#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <raymath.h>

// Forward declaration, otherwise the component it doesn't know (kinda need it cuz templates have to be here)
class GameEntity;

enum class ComponentType
{
    Transform,
    Cube,
    Sphere,
};

// Pretty handy if we could only have 1 component of the same category per entity, breaks less things
enum class ComponentCategory
{
    // Transform is unique, always just the transform
    Transform,
    // Object is for meshes, cubes, etc etc etc
    Object,
};

class Component
{
public:
    virtual ~Component() = default;
    // Pointer to the entity that owns it
    GameEntity *entity = nullptr;
    const ComponentType type;
    const ComponentCategory category;

    // Ctor to the component type and category cannot be changed
    Component(ComponentType componentType, ComponentCategory componentCategory) : type(componentType), category(componentCategory) {}
};

// Has to be made in here, as it's a template
class GameEntity
{
public:
    GameEntity() = default;
    ~GameEntity() = default;

    // Yeah, still struggling myself here
    template <typename T, typename... Args>
    T *AddComponent(Args &&...args)
    {
        // Create a new component of type T and making it unique
        auto component = std::make_unique<T>(std::forward<Args>(args)...);

        // Just check all the components if theres one with the same category
        for (const auto &pair : components)
        {
            // More work, it could REALLY just have been a vector, might just rewrite my "genius idea" and use the vector
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

private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};

// TODO: probably make a component header for this
// Scale and rotation don't work yet. For rotation, I'll need to make rotation matrices
struct TransformComponent : Component
{
    TransformComponent() : Component(ComponentType::Transform, ComponentCategory::Transform) {};
    Vector3 position = {0, 0, 0};
    Vector3 rotation = {0, 0, 0};
    Vector3 scale = {1, 1, 1};

    Matrix GetTransformMatrix() const
    {
        // Create a translation matrix
        Matrix translation = MatrixTranslate(position.x, position.y, position.z);

        // Create rotation matrices for each axis
        Matrix rotationX = MatrixRotateX(rotation.x * DEG2RAD);
        Matrix rotationY = MatrixRotateY(rotation.y * DEG2RAD);
        Matrix rotationZ = MatrixRotateZ(rotation.z * DEG2RAD);

        // Create a scaling matrix
        Matrix scaling = MatrixScale(scale.x, scale.y, scale.z);

        // Combine all transformations: translation * rotation * scale
        // The order matters: first scale, then rotate, then translate, found that out the hard way
        Matrix transform = MatrixMultiply(scaling, MatrixMultiply(rotationX, MatrixMultiply(rotationY, MatrixMultiply(rotationZ, translation))));

        return transform;
    }
};

struct CubeComponent : Component
{
    CubeComponent() : Component(ComponentType::Cube, ComponentCategory::Object) {}
    Vector3 size = {1, 1, 1};
    Color color = RED;

    Vector3 GetScaledSize() const
    {
        auto transform = entity->GetComponent<TransformComponent>();
        if (transform)
        {
            return {
                size.x * transform->scale.x,
                size.y * transform->scale.y,
                size.z * transform->scale.z};
        }
        return size;
    }

    BoundingBox GetBoundingBox() const
    {
        auto transform = entity->GetComponent<TransformComponent>();
        if (!transform)
        {
            return {Vector3{0, 0, 0}, Vector3{0, 0, 0}};
        }

        Vector3 scaledSize = GetScaledSize();
        return {
            Vector3Subtract(transform->position, Vector3Scale(scaledSize, 0.5f)),
            Vector3Add(transform->position, Vector3Scale(scaledSize, 0.5f))};
    }
};

struct SphereComponent : Component
{
    SphereComponent() : Component(ComponentType::Sphere, ComponentCategory::Object) {}
    float radius = 1.0f;
    Color color = RED;

    float GetScaledRadius() const
    {
        auto transform = entity->GetComponent<TransformComponent>();
        if (transform)
        {
            float avgScale = (transform->scale.x + transform->scale.y + transform->scale.z) / 3.0f;
            return radius * avgScale;
        }
        return radius;
    }
};