package com.modzel.raytracer;

public class Light {

    public Vec3f Position;
    public float Intensity;

    public Light(Vec3f position, float intensity) {
        Position = new Vec3f(position);
        Intensity = intensity;
    }
}
