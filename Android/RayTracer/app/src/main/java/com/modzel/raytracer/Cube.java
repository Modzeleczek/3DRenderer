package com.modzel.raytracer;

// WIP
public class Cube extends Shape {

    public float Edge;

    public Cube(Vec3f center, float edge, Material material) {
        super(center, material);
        Edge = edge;
    }

    @Override
    public boolean RayIntersect(Vec3f origin, Vec3f direction, FloatWrapper distance, Vec3f hitPoint, Vec3f normal) {
        return false;
    }
};
