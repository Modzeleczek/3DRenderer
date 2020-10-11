package com.modzel.raytracer;

public class Vec3f {

    public float X, Y, Z;

    public Vec3f(float x, float y, float z) {
        X = x; Y = y; Z = z;
    }
    public Vec3f(Vec3f o) {
        X = o.X; Y = o.Y; Z = o.Z;
    }
    public Vec3f() {
        X = 0; Y = 0; Z = 0;
    }

    public Vec3f Add(Vec3f v) {
        X += v.X;
        Y += v.Y;
        Z += v.Z;
        return this;
    }

    public static Vec3f Cross(Vec3f v1, Vec3f v2) {
        return new Vec3f(v1.Y * v2.Z - v1.Z * v2.Y, v1.Z * v2.X - v1.X * v2.Z, v1.X * v2.Y - v1.Y * v2.X);
    }

    public float Norm() {
        return (float) Math.sqrt((double) (X * X + Y * Y + Z * Z));
    }

    public Vec3f Normalize() {
        final float norm = Norm();
        X /= norm;
        Y /= norm;
        Z /= norm;
        return this;
    }

    public float NormalizeReturnNorm() {
        final float norm = Norm();
        X /= norm;
        Y /= norm;
        Z /= norm;
        return norm;
    }

    public static Vec3f Reflect(Vec3f I, Vec3f N) {
        /* Rotation of I vector by 180 degrees around N vector using quaternion.
        qr = cos(180/2) = 0, s = sin(180/2) = 1
        qxyz = (N.X * 1, N.Y * 1, N.Z * 1) = N
        |N| = |I| = 1
        I' = 2.0f * (N*I) * N + (0 - (N*N)) * I = 2.0f * (N*I) * N + (0 - (1^2)) * I = 2.0f * (N*I) * N - I
        The rotated I' vector is still pointing at the point, where the ray hit, so it has to be inversed in order to be fully reflected.
        result - -I' = I - 2.0f * (N*I) * N */
        return Subtract(I, Multiply(N, 2.f * Dot(I, N)));
    }

    static Vec3f Refract(Vec3f I, Vec3f N, float eta_t, float eta_i) { // Snell's law
        float cosi = -Math.max (-1.f, Math.min (1.f, Dot(I, N)));
        if (cosi < 0)
            return Refract(I, Inverse(N), eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
        float eta = eta_i / eta_t;
        float k = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0 ? new Vec3f(1, 0, 0)
                : Add(Multiply(I, eta), Multiply(N, (eta * cosi - (float)Math.sqrt(k)))); // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
    }

    public void RotateX(float angle) {
        RotateX((float) Math.sin(angle), (float) Math.cos(angle));
    }

    public void RotateX(float sinA, float cosA) {
        float y = Y, z = Z;
        Y = y * cosA - z * sinA;
        Z = y * sinA + z * cosA;
    }

    public void RotateY(float angle) {
        RotateY((float) Math.sin(angle), (float) Math.cos(angle));
    }

    public void RotateY(float sinA, float cosA) {
        float x = X, z = Z;
        X = x * cosA + z * sinA;
        Z = -x * sinA + z * cosA;
    }

    public void RotateZ(float angle) {
        RotateZ((float) Math.sin(angle), (float) Math.cos(angle));
    }

    public void RotateZ(float sinA, float cosA) {
        float x = X, y = Y;
        X = x * cosA - y * sinA;
        Y = x * sinA + y * cosA;
    }

    public void RotateAxisMatrix(Vec3f axis, float angle) {
        // https://www.continuummechanics.org/rotationmatrix.html
        float vx = X, vy = Y, vz = Z, c = (float) Math.cos(angle), s = (float) Math.sin(angle);
        X = (c + (1 - c) * axis.X * axis.X) * vx + ((1 - c) * axis.X * axis.Y - s * axis.Z) * vy + ((1 - c) * axis.X * axis.Z + s * axis.Y) * vz;
        Y = ((1 - c) * axis.X * axis.Y + s * axis.Z) * vx + (c + (1 - c) * axis.Y * axis.Y) * vy + ((1 - c) * axis.Y * axis.Z - s * axis.X) * vz;
        Z = ((1 - c) * axis.X * axis.Z - s * axis.Y) * vx + ((1 - c) * axis.Y * axis.Z + s * axis.X) * vy + (c + (1 - c) * axis.Z * axis.Z) * vz;
    }

    public void RotateAxisQuaternion(Vec3f axis, float angle) {
        // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
        // q = cos(angle/2) + (axis.x*i + axis.y*j + axis.z*k)*sin(angle/2)
        final float qr = (float) Math.cos(angle / 2.f), s = (float) Math.sin(angle / 2.f);
        Vec3f qxyz = new Vec3f(axis.X * s, axis.Y * s, axis.Z * s);

        // https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
        Vec3f newValue = Add( Multiply(2.0f, Multiply(Dot(qxyz, this), qxyz)),
                            Add( Multiply(qr * qr - Dot(qxyz, qxyz), this),
                                Multiply(2.0f, Multiply(qr, Cross(qxyz, this))) ) );
        X = newValue.X;
        Y = newValue.Y;
        Z = newValue.Z;
    }

    public Vec3b AsVec3b() {
        Vec3b result = new Vec3b((byte)0, (byte)0, (byte)0);
        if (X > 1.f) result.R = (byte)255;
        else result.R = (byte)(255 * X);
        if (Y > 1.f) result.G = (byte)255;
        else result.G = (byte)(255 * Y);
        if (Z > 1.f) result.B = (byte)255;
        else result.B = (byte)(255 * Z);
        return result;
    }

    public static float Dot(Vec3f v1, Vec3f v2) {
        return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
    }

    public static Vec3f Add(Vec3f v1, Vec3f v2) {
        return new Vec3f(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
    }

    public static Vec3f Subtract(Vec3f v1, Vec3f v2) {
        return new Vec3f(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);
    }

    public static Vec3f Multiply(Vec3f v, float factor) {
        return new Vec3f(v.X * factor, v.Y * factor, v.Z * factor);
    }

    public static Vec3f Multiply(float factor, Vec3f v) {
        return Multiply(v, factor);
    }

    public static Vec3f Inverse(Vec3f v) {
        return Multiply(v, -1.f);
    }

    @Override
    public String toString() {
        return '(' + Float.toString(X) + ',' + Float.toString(Y) + ',' + Float.toString(Z) + ')';
    }
}