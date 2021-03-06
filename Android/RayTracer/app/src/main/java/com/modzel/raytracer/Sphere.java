package com.modzel.raytracer;

public class Sphere extends Shape {

    public float Radius;

    public Sphere(Vec3f center, float radius, Material material) {
        super(center, material);
        Radius = radius;
    }

    @Override
    public boolean RayIntersect(Vec3f origin, Vec3f direction, FloatWrapper distance, Vec3f hitPoint, Vec3f normal) {
        Vec3f L = Vec3f.Subtract(Center, origin);
        float tca = Vec3f.Dot(L, direction);
        float d2 = Vec3f.Dot(L, L) - tca*tca;
        if (d2 > Radius*Radius) return false;
        float thc = (float)Math.sqrt(Radius*Radius - d2);
        distance.Value = tca - thc;// thc jest zawsze nieujemne, więc tca - thc jest zawsze mniejsze od tca + thc
        if(distance.Value > 0)
        {
            hitPoint = Vec3f.Add(origin, Vec3f.Multiply(distance.Value, direction));
            // if(L.Norm() >= Radius)
            if(Vec3f.Dot(L, L) >= Radius*Radius)
                normal.CopyFrom(Vec3f.Subtract(hitPoint, Center).Normalize());
            else
                normal.CopyFrom(Vec3f.Subtract(Center, hitPoint).Normalize());
            return true;
        }
        distance.Value = tca + thc;
        if(distance.Value > 0)
        {
            hitPoint = Vec3f.Add(origin, Vec3f.Multiply(distance.Value, direction));
            // if(L.Norm() >= Radius)
            if(Vec3f.Dot(L, L) >= Radius*Radius)
                normal.CopyFrom(Vec3f.Subtract(hitPoint, Center).Normalize());
            else
                normal.CopyFrom(Vec3f.Subtract(Center, hitPoint).Normalize());
            return true;
        }
        return false;
    }
}