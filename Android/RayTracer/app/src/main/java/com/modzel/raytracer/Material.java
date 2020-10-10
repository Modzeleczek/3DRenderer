package com.modzel.raytracer;

public class Material {

    public float RefractiveIndex;
    public Vec4f Albedo;
    public Vec3f DiffuseColor;
    public float SpecularExponent;

    public Material(float refractiveIndex, Vec4f albedo, Vec3f color, float specularExponent) {
        RefractiveIndex = refractiveIndex;
        Albedo = albedo;
        DiffuseColor = color;
        SpecularExponent = specularExponent;
    }
    public Material() {
        RefractiveIndex = 1;
        Albedo = new Vec4f(1, 0, 0, 0);
        DiffuseColor = new Vec3f(0, 0, 0);
        SpecularExponent = 0;
    }
}
