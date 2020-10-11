package com.modzel.raytracer;

public class Vec3b {

    public byte R, G, B;

    public Vec3b(byte r, byte g, byte b) {
        R = r; G = g; B = b;
    }
    public Vec3b(Vec3b o) {
        R = o.R; G = o.G; B = o.B;
    }
}
