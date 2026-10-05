#pragma once

#include "Types.h"
#include <algorithm>
#include <cmath>

namespace Engine::Math
{
    inline float ToRadians(float degrees) { return glm::radians(degrees); }
    inline float ToDegrees(float radians) { return glm::degrees(radians); }

    inline float Length(const Vec3 &v) { return glm::length(v); }
    inline float Distance(const Vec3 &a, const Vec3 &b) { return glm::distance(a, b); }
    inline Vec3 Normalize(const Vec3 &v)
    {
        float len = glm::length(v);
        return (len > 0.00001f) ? (v / len) : Vec3(0.0f);
    }
    inline float Dot(const Vec3 &a, const Vec3 &b) { return glm::dot(a, b); }
    inline Vec3 Cross(const Vec3 &a, const Vec3 &b) { return glm::cross(a, b); }

    inline Ray GetScreenToWorldRay(const Vec2 &mousePos, const Vec2 &viewportSize, const Camera3D &camera)
    {
        float x = (2.0f * mousePos.x) / viewportSize.x - 1.0f;
        float y = 1.0f - (2.0f * mousePos.y) / viewportSize.y;

        float aspect = viewportSize.x / (viewportSize.y > 0.0f ? viewportSize.y : 1.0f);
        Mat4 proj = camera.GetProjectionMatrix(aspect);
        Mat4 view = camera.GetViewMatrix();
        Mat4 invVP = glm::inverse(proj * view);

        Vec4 nearPoint = invVP * Vec4(x, y, -1.0f, 1.0f);
        Vec4 farPoint = invVP * Vec4(x, y, 1.0f, 1.0f);

        nearPoint /= nearPoint.w;
        farPoint /= farPoint.w;

        Ray ray;
        ray.origin = Vec3(nearPoint);
        ray.direction = glm::normalize(Vec3(farPoint - nearPoint));
        return ray;
    }

    inline RayHit IntersectRayAABB(const Ray &ray, const BoundingBox &box)
    {
        RayHit hit;
        hit.hit = false;

        float tmin = (box.min.x - ray.origin.x) / (std::fabs(ray.direction.x) > 0.00001f ? ray.direction.x : 0.00001f);
        float tmax = (box.max.x - ray.origin.x) / (std::fabs(ray.direction.x) > 0.00001f ? ray.direction.x : 0.00001f);

        if (tmin > tmax)
            std::swap(tmin, tmax);

        float tymin = (box.min.y - ray.origin.y) / (std::fabs(ray.direction.y) > 0.00001f ? ray.direction.y : 0.00001f);
        float tymax = (box.max.y - ray.origin.y) / (std::fabs(ray.direction.y) > 0.00001f ? ray.direction.y : 0.00001f);

        if (tymin > tymax)
            std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return hit;

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (box.min.z - ray.origin.z) / (std::fabs(ray.direction.z) > 0.00001f ? ray.direction.z : 0.00001f);
        float tzmax = (box.max.z - ray.origin.z) / (std::fabs(ray.direction.z) > 0.00001f ? ray.direction.z : 0.00001f);

        if (tzmin > tzmax)
            std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return hit;

        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        if (tmin >= 0.0f)
        {
            hit.hit = true;
            hit.distance = tmin;
            hit.point = ray.origin + ray.direction * tmin;
        }

        return hit;
    }

    inline RayHit IntersectRaySphere(const Ray &ray, const Vec3 &center, float radius)
    {
        RayHit hit;
        hit.hit = false;

        Vec3 oc = ray.origin - center;
        float b = glm::dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - radius * radius;
        float discriminant = b * b - c;

        if (discriminant > 0.0f)
        {
            float t = -b - std::sqrt(discriminant);
            if (t > 0.0f)
            {
                hit.hit = true;
                hit.distance = t;
                hit.point = ray.origin + ray.direction * t;
                hit.normal = glm::normalize(hit.point - center);
            }
        }

        return hit;
    }
}

namespace Engine
{
    inline Vec3 Vector3Normalize(const Vec3 &v) { return Math::Normalize(v); }
    inline Vec3 Vector3Subtract(const Vec3 &a, const Vec3 &b) { return a - b; }
    inline Vec3 Vector3Add(const Vec3 &a, const Vec3 &b) { return a + b; }
    inline float Vector3Distance(const Vec3 &a, const Vec3 &b) { return Math::Distance(a, b); }
    inline Vec3 Vector3Scale(const Vec3 &v, float s) { return v * s; }
    inline Vec3 Vector3CrossProduct(const Vec3 &a, const Vec3 &b) { return Math::Cross(a, b); }
    inline float Vector3DotProduct(const Vec3 &a, const Vec3 &b) { return Math::Dot(a, b); }
    inline float Vector3Length(const Vec3 &v) { return Math::Length(v); }
    inline Vec3 Vector3Transform(const Vec3 &v, const Mat4 &m) { return Vec3(m * Vec4(v, 1.0f)); }
    inline Mat4 QuaternionToMatrix(const Quat &q) { return glm::mat4_cast(q); }
}

// If using raylib, we do conversions, cuz we no longer rely on it for math etc
#ifdef RAYLIB_H

namespace Engine
{
    inline ::Vector3 ToRaylib(const Vec3 &v) { return ::Vector3{v.x, v.y, v.z}; }
    inline Vec3 FromRaylib(const ::Vector3 &v) { return Vec3(v.x, v.y, v.z); }

    inline ::Vector2 ToRaylib(const Vec2 &v) { return ::Vector2{v.x, v.y}; }
    inline Vec2 FromRaylib(const ::Vector2 &v) { return Vec2(v.x, v.y); }

    inline ::Color ToRaylib(const Engine::Color &c) { return ::Color{c.r, c.g, c.b, c.a}; }
    inline Engine::Color FromRaylib(const ::Color &c) { return Engine::Color(c.r, c.g, c.b, c.a); }

    inline ::Matrix ToRaylib(const Mat4 &m)
    {
        ::Matrix r;
        r.m0 = m[0][0];
        r.m1 = m[0][1];
        r.m2 = m[0][2];
        r.m3 = m[0][3];
        r.m4 = m[1][0];
        r.m5 = m[1][1];
        r.m6 = m[1][2];
        r.m7 = m[1][3];
        r.m8 = m[2][0];
        r.m9 = m[2][1];
        r.m10 = m[2][2];
        r.m11 = m[2][3];
        r.m12 = m[3][0];
        r.m13 = m[3][1];
        r.m14 = m[3][2];
        r.m15 = m[3][3];
        return r;
    }

    inline Mat4 FromRaylib(const ::Matrix &m)
    {
        return Mat4(
            m.m0, m.m1, m.m2, m.m3,
            m.m4, m.m5, m.m6, m.m7,
            m.m8, m.m9, m.m10, m.m11,
            m.m12, m.m13, m.m14, m.m15);
    }

    inline ::Quaternion ToRaylib(const Quat &q) { return ::Quaternion{q.x, q.y, q.z, q.w}; }
    inline Quat FromRaylib(const ::Quaternion &q) { return Quat(q.w, q.x, q.y, q.z); }

    inline ::Camera3D ToRaylib(const Engine::Camera3D &c)
    {
        ::Camera3D rc;
        rc.position = ToRaylib(c.position);
        rc.target = ToRaylib(c.target);
        rc.up = ToRaylib(c.up);
        rc.fovy = c.fovy;
        rc.projection = c.isPerspective ? 0 : 1;
        return rc;
    }

    inline ::RayCollision GetRayCollisionBox(const Engine::Ray &ray, const Engine::BoundingBox &box)
    {
        Engine::RayHit hit = Engine::Math::IntersectRayAABB(ray, box);
        ::RayCollision rc = {0};
        rc.hit = hit.hit;
        rc.distance = hit.distance;
        rc.point = ::Vector3{hit.point.x, hit.point.y, hit.point.z};
        rc.normal = ::Vector3{hit.normal.x, hit.normal.y, hit.normal.z};
        return rc;
    }

    inline ::RayCollision GetRayCollisionSphere(const Engine::Ray &ray, const Vec3 &center, float radius)
    {
        Engine::RayHit hit = Engine::Math::IntersectRaySphere(ray, center, radius);
        ::RayCollision rc = {0};
        rc.hit = hit.hit;
        rc.distance = hit.distance;
        rc.point = ::Vector3{hit.point.x, hit.point.y, hit.point.z};
        rc.normal = ::Vector3{hit.normal.x, hit.normal.y, hit.normal.z};
        return rc;
    }
}

#endif
