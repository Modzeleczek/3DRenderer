package com.modzel.raytracer;

public class Circle extends PlainShape {

    public float Radius;

    public Circle(Vec3f center, float radius, Vec3f direction, Material material) {
        super(center, direction, material);
        Radius = radius;
    }

    @Override
    public boolean RayIntersect(Vec3f origin, Vec3f direction, float distance, Vec3f hitPoint, Vec3f normal) {
        final float cosDd = Vec3f.Dot(Direction, direction);
        if(cosDd == 0) return false; // the ray does not hit the circle, because they are parallel
        distance = Vec3f.Dot( Direction, Vec3f.Subtract(Center, origin) ) / cosDd;
        if(distance <= 0) return false;
        hitPoint = Vec3f.Add(origin, Vec3f.Multiply(distance, direction));
        // (hitPoint - Center).Norm() <= Radius
        final Vec3f fromCenterToP = Vec3f.Subtract(hitPoint, Center);
        if(Vec3f.Dot(fromCenterToP, fromCenterToP) <= Radius*Radius)
        {
            if(cosDd < 0) // the ray comes from the side of the circle that is pointed by its Direction vector
                normal = new Vec3f(Direction);
            else // the ray comes from the other side of the circle
                normal = Vec3f.Inverse(Direction);
            return true;
        }
        return false;
    }
}