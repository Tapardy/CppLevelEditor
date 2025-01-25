#pragma once

#include <raylib.h>

// Namespace it so it's obvious its mine, rather than some other library
// Might remove the namespace if stuff gets annoying to call like the enum needing Object:: in front
namespace Object
{
    // This should allow me to call different draw functions
    enum ObjectType
    {
        PLANE,
        CUBE,
        SPHERE,
        TRIANGLE,
        CYLINDER,
        CAPSULE,
        MESH
    };

    struct Plane
    {
        Vector3 centerPos;
        Vector2 size;
        Color color;

        // Not allowed to change
        static constexpr ObjectType tag = PLANE;
    };

    struct Cube
    {
        Vector3 position;
        Vector3 rotation;
        Vector3 size;
        Color color;
        static constexpr ObjectType tag = CUBE;

        static Cube CreateCube(Vector3 position, Vector3 rotation, Vector3 size, Color color)
        {
            return Cube{position, rotation, size, color};
        }
        // Simple return statmement so I never have to write this again
        // Could separate this into a diff class, so the dynamic one can be used, not sure if that's the best idea
        BoundingBox GetBoundingBox() const
        {
            return {
                {position.x - size.x / 2, position.y - size.y / 2, position.z - size.z / 2},
                {position.x + size.x / 2, position.y + size.y / 2, position.z + size.z / 2}};
        }
    };

    struct Sphere
    {
        Vector3 centerPos;
        float radius;
        Color color;

        static constexpr ObjectType tag = SPHERE;
    };

    struct Cylinder
    {
        Vector3 position;
        float radiusTop;
        float radiusBottom;
        float height;
        int slices;
        Color color;

        static constexpr ObjectType tag = CYLINDER;
    };

    struct Capsule
    {
        Vector3 startPos;
        Vector3 endPos;
        float radius;
        int slices;
        int rings;
        Color color;

        static constexpr ObjectType tag = CAPSULE;
    };

    // Not sure if needed, might implement later
    struct Mesh
    {
    };

    // Example of a more dynamic object
    // Could have issues when the object is different/ uses different parameters like radius, need to look into that
    // Triangles also need a different structure, could look into making this something like decals or mesh objects later
    struct LevelObject
    {
        Vector3 position;
        Vector3 rotation;
        Vector3 size;
        Color color;
        ObjectType tag;
    };
}