package com.modzel.raytracer;

class Ellipse extends PlainShape {

    public Vec3f Focus2; // Center == Focus1
    public float FocusDistanceSum;

    /* additionalFocusesDistance - a value that is added to the distance between ellipse's
    focuses to avoid situation, when user given FocusDistanceSum is less than actual distance
    between focuses, so the ellipse does not exist */

    public Ellipse(Vec3f center1, Vec3f center2, float additionalFocusesDistance, Vec3f direction, Material material) {
        super(center1, direction, material);
        Focus2 = new Vec3f(center2);
        FocusDistanceSum = Vec3f.Subtract(center1, center2).Norm() + additionalFocusesDistance;
    }

    @Override
    public boolean RayIntersect(Vec3f origin, Vec3f direction, FloatWrapper distance, Vec3f hitPoint, Vec3f normal) {
        final float cosDd = Vec3f.Dot(Direction, direction);
        if(cosDd == 0) return false; // the ray does not hit the ellipse, because they are parallel
        distance.Value = Vec3f.Dot( Direction, Vec3f.Subtract(Center, origin) ) / cosDd;
        if(distance.Value <= 0) return false;
        hitPoint.CopyFrom(Vec3f.Add(origin, Vec3f.Multiply(distance.Value, direction)));
        if(Vec3f.Subtract(hitPoint, Center).Norm() + Vec3f.Subtract(hitPoint, Focus2).Norm() <= FocusDistanceSum)
        {
            if(cosDd < 0) // the ray comes from the side of the ellipse that is pointed by its Direction vector
                normal.CopyFrom(Direction);
            else // the ray comes from the other side of the ellipse
                normal.CopyFrom(Vec3f.Inverse(Direction));
            return true;
        }
        return false;
    }
}