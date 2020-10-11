package com.modzel.raytracer;

public abstract class PlainShape extends Shape {

    protected Vec3f Direction;

    public PlainShape(Vec3f center, Vec3f normal, Material material) {
        super(center, material);
        Direction = new Vec3f(normal);
    }

    public void SetDirection(Vec3f direction) {
        Direction = direction;
    }
}