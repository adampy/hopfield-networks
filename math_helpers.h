#pragma once

#include <cmath>

struct Mat4
{
    float m[16];
};

inline Mat4 mat4_identity()
{
    Mat4 r{};
    r.m[0] = 1.0f;
    r.m[5] = 1.0f;
    r.m[10] = 1.0f;
    r.m[15] = 1.0f;
    return r;
}

inline Mat4 mat4_mul(const Mat4& a, const Mat4& b)
{
    Mat4 r{};
    for (int c = 0; c < 4; ++c)
    {
        for (int rrow = 0; rrow < 4; ++rrow)
        {
            r.m[c * 4 + rrow] =
                a.m[0 * 4 + rrow] * b.m[c * 4 + 0] +
                a.m[1 * 4 + rrow] * b.m[c * 4 + 1] +
                a.m[2 * 4 + rrow] * b.m[c * 4 + 2] +
                a.m[3 * 4 + rrow] * b.m[c * 4 + 3];
        }
    }
    return r;
}

inline Mat4 mat4_translate(float x, float y, float z)
{
    Mat4 r = mat4_identity();
    r.m[12] = x;
    r.m[13] = y;
    r.m[14] = z;
    return r;
}

inline Mat4 mat4_rotate_axis_angle(float angleRad, float ax, float ay, float az)
{
    float len = std::sqrt(ax * ax + ay * ay + az * az);
    if (len <= 0.0f) return mat4_identity();
    ax /= len;
    ay /= len;
    az /= len;

    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    float t = 1.0f - c;

    Mat4 r{};
    r.m[0]  = t * ax * ax + c;
    r.m[1]  = t * ax * ay + s * az;
    r.m[2]  = t * ax * az - s * ay;
    r.m[3]  = 0.0f;

    r.m[4]  = t * ax * ay - s * az;
    r.m[5]  = t * ay * ay + c;
    r.m[6]  = t * ay * az + s * ax;
    r.m[7]  = 0.0f;

    r.m[8]  = t * ax * az + s * ay;
    r.m[9]  = t * ay * az - s * ax;
    r.m[10] = t * az * az + c;
    r.m[11] = 0.0f;

    r.m[12] = 0.0f;
    r.m[13] = 0.0f;
    r.m[14] = 0.0f;
    r.m[15] = 1.0f;
    return r;
}

inline Mat4 mat4_perspective(float fovYRad, float aspect, float zNear, float zFar)
{
    Mat4 r{};
    float f = 1.0f / std::tan(fovYRad * 0.5f);
    r.m[0] = f / aspect;
    r.m[5] = f;
    r.m[10] = (zFar + zNear) / (zNear - zFar);
    r.m[11] = -1.0f;
    r.m[14] = (2.0f * zFar * zNear) / (zNear - zFar);
    return r;
}
