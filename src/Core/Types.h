#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <type_traits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;
using Quat = glm::quat;

namespace Engine
{
    using Vector2 = Vec2;
    using Vector3 = Vec3;
    using Vector4 = Vec4;
    using Matrix = Mat4;
    using Quaternion = Quat;

    struct Color
    {
        uint8_t r = 255;
        uint8_t g = 255;
        uint8_t b = 255;
        uint8_t a = 255;

        constexpr Color() = default;
        constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
            : r(r), g(g), b(b), a(a) {}

        template <typename RayColor,
                  typename = std::enable_if_t<!std::is_arithmetic_v<RayColor> && !std::is_same_v<std::decay_t<RayColor>, Color>>>
        constexpr Color(const RayColor &c)
            : r(static_cast<uint8_t>(c.r)), g(static_cast<uint8_t>(c.g)), b(static_cast<uint8_t>(c.b)), a(static_cast<uint8_t>(c.a)) {}

        template <typename RayColor,
                  typename = std::enable_if_t<!std::is_arithmetic_v<RayColor> && !std::is_same_v<std::decay_t<RayColor>, Color>>>
        operator RayColor() const { return RayColor{r, g, b, a}; }

        Vec4 ToFloat4() const
        {
            return Vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }
    };

    // just nice to have a coupel common colors without constantly having to look for the frikin rgb values
    namespace Colors
    {
        inline constexpr Color White{255, 255, 255, 255};
        inline constexpr Color Black{0, 0, 0, 255};
        inline constexpr Color Gray{130, 130, 130, 255};
        inline constexpr Color LightGray{200, 200, 200, 255};
        inline constexpr Color DarkGray{80, 80, 80, 255};
        inline constexpr Color Red{230, 41, 55, 255};
        inline constexpr Color Green{0, 228, 48, 255};
        inline constexpr Color Blue{0, 121, 241, 255};
        inline constexpr Color Yellow{253, 249, 0, 255};
        inline constexpr Color Orange{255, 161, 0, 255};
        inline constexpr Color RayWhite{245, 245, 245, 255};
        inline constexpr Color Clear{0, 0, 0, 0};
    }

    struct Camera3D
    {
        Vec3 position{10.0f, 10.0f, 10.0f};
        Vec3 target{0.0f, 0.0f, 0.0f};
        Vec3 up{0.0f, 1.0f, 0.0f};
        float fovy = 45.0f;
        bool isPerspective = true;

        Camera3D() = default;
        Camera3D(const Vec3 &pos, const Vec3 &tgt, const Vec3 &u = Vec3(0, 1, 0), float fov = 45.0f, bool perspective = true)
            : position(pos), target(tgt), up(u), fovy(fov), isPerspective(perspective) {}

        template <typename RayCamera,
                  typename = decltype(std::declval<RayCamera>().position)>
        Camera3D(const RayCamera &rc)
            : position(rc.position.x, rc.position.y, rc.position.z), target(rc.target.x, rc.target.y, rc.target.z), up(rc.up.x, rc.up.y, rc.up.z), fovy(rc.fovy), isPerspective(rc.projection == 0)
        {
        }

        template <typename RayCamera,
                  typename = decltype(std::declval<RayCamera>().position)>
        operator RayCamera() const
        {
            RayCamera rc{};
            rc.position.x = position.x;
            rc.position.y = position.y;
            rc.position.z = position.z;
            rc.target.x = target.x;
            rc.target.y = target.y;
            rc.target.z = target.z;
            rc.up.x = up.x;
            rc.up.y = up.y;
            rc.up.z = up.z;
            rc.fovy = fovy;
            rc.projection = isPerspective ? 0 : 1;
            return rc;
        }

        Mat4 GetViewMatrix() const
        {
            return glm::lookAt(position, target, up);
        }

        Mat4 GetProjectionMatrix(float aspect, float nearZ = 0.01f, float farZ = 1000.0f) const
        {
            if (isPerspective)
                return glm::perspective(glm::radians(fovy), aspect, nearZ, farZ);
            else
            {
                float halfH = fovy * 0.5f;
                float halfW = halfH * aspect;
                return glm::ortho(-halfW, halfW, -halfH, halfH, nearZ, farZ);
            }
        }
    };

    using Camera = Camera3D;

    struct Ray
    {
        Vec3 origin{0.0f, 0.0f, 0.0f};
        Vec3 direction{0.0f, 0.0f, 1.0f};

        Ray() = default;
        Ray(const Vec3 &orig, const Vec3 &dir) : origin(orig), direction(dir) {}

        template <typename RayRay,
                  typename = decltype(std::declval<RayRay>().direction)>
        Ray(const RayRay &r)
            : origin(r.position.x, r.position.y, r.position.z), direction(r.direction.x, r.direction.y, r.direction.z)
        {
        }

        template <typename RayRay,
                  typename = decltype(std::declval<RayRay>().direction)>
        operator RayRay() const
        {
            RayRay r{};
            r.position.x = origin.x;
            r.position.y = origin.y;
            r.position.z = origin.z;
            r.direction.x = direction.x;
            r.direction.y = direction.y;
            r.direction.z = direction.z;
            return r;
        }
    };

    struct RayHit
    {
        bool hit = false;
        float distance = 0.0f;
        Vec3 point{0.0f, 0.0f, 0.0f};
        Vec3 normal{0.0f, 0.0f, 0.0f};
    };

    struct BoundingBox
    {
        Vec3 min{0.0f, 0.0f, 0.0f};
        Vec3 max{0.0f, 0.0f, 0.0f};

        BoundingBox() = default;
        BoundingBox(const Vec3 &minV, const Vec3 &maxV) : min(minV), max(maxV) {}

        template <typename RayBox,
                  typename = decltype(std::declval<RayBox>().min)>
        BoundingBox(const RayBox &b)
            : min(b.min.x, b.min.y, b.min.z), max(b.max.x, b.max.y, b.max.z)
        {
        }

        template <typename RayBox,
                  typename = decltype(std::declval<RayBox>().min)>
        operator RayBox() const
        {
            RayBox b{};
            b.min.x = min.x;
            b.min.y = min.y;
            b.min.z = min.z;
            b.max.x = max.x;
            b.max.y = max.y;
            b.max.z = max.z;
            return b;
        }
    };

    struct Rectangle
    {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
    };
}
