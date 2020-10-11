package com.modzel.raytracer;

public class Vec4f {

    public float X, Y, Z, W;

    public Vec4f(float x, float y, float z, float w) {
        X = x; Y = y; Z = z; W = w;
    }
    public Vec4f(Vec4f o) {
        X = o.X; Y = o.Y; Z = o.Z; W = o.W;
    }
}
