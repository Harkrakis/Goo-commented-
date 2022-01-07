#include "Vector.h"
#include <math.h>
Vec2 Vec2_Set(float x, float y)
{
    Vec2 v = { .x = x, .y = y };
    return v;
}

Vec2 Vec2_Perp(Vec2 v)
{
    return Vec2_Set(v.y, -v.x);
}

Vec2 Vec2_Add(Vec2 v1, Vec2 v2)
{
    v1.x+=v2.x;
    v1.y+=v2.y;
    return v1;
}

Vec2 Vec2_Sub(Vec2 v1, Vec2 v2)
{
    v1.x-=v2.x;
    v1.y-=v2.y;
    return v1;
}

Vec2 Vec2_Scale(Vec2 v, float s)
{
    v.x*=s;
    v.y*=s;
    return v;
}

float Vec2_Length(Vec2 v)
{
    return sqrt(v.x*v.x+v.y*v.y);
}

Vec2 Vec2_Normalize(Vec2 v)
{
    float tmp = Vec2_Length(v);
    v.x=v.x / tmp;
    v.y=v.y/ tmp;
    return v;
}

float Vec2_Distance(Vec2 v1, Vec2 v2)
{
    return Vec2_Length(Vec2_Sub(v1,v2));
}

