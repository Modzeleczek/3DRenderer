package com.modzel.raytracer;

public abstract class Shape {

    public Vec3f Center;
    public Material Surface;

    public Shape(Vec3f center, Material material) {
        Center = new Vec3f(center);
        Surface = new Material(material);
    }
    public abstract boolean RayIntersect(Vec3f origin, Vec3f direction, FloatWrapper distance, Vec3f hitPoint,
                                         Vec3f normal);
}