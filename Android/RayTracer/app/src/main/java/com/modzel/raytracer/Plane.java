package com.modzel.raytracer;

class Plane extends PlainShape {

    public Plane(Vec3f center, Vec3f direction, Material material) {
        super(center, direction, material);
    }

    @Override
    public boolean RayIntersect(Vec3f origin, Vec3f direction, float distance, Vec3f hitPoint, Vec3f normal) {
        final float cosDd = Vec3f.Dot(Direction, direction);
        if(cosDd == 0) return false; // the ray does not hit the plane, because they are parallel
        distance = Vec3f.Dot( Direction, Vec3f.Subtract(Center, origin) ) / cosDd;
        if(distance > 0)
        {
            hitPoint = Vec3f.Add(origin, Vec3f.Multiply(distance, direction));
            if(cosDd < 0) // the ray comes from the side of the plane that is pointed by its Direction vector
                normal = new Vec3f(Direction);
            else // the ray comes from the other side of the plane
                normal = Vec3f.Inverse(Direction);
            return true;
        }
        return false;
    }
}