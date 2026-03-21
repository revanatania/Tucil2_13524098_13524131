#pragma once
#include <cmath>
#include <string>

using namespace std;

struct Vector3D {
    // Vektor 3 dimensi
    double x, y, z;
    
    /* ===== KONSTRUKTOR ===== */
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}
    
    /* ===== OPERASI ARITMATIKA DASAR ===== */
    // Operasi aritmatika
    Vector3D operator+(const Vector3D& o) const {
        return {x + o.x, y + o.y, z + o.z};
    }

    Vector3D operator-(const Vector3D& o) const {
        return {x - o.x, y - o.y, z - o.z};
    }

    Vector3D operator*(double s) const {
        return {x * s, y * s, z * s};
    }

    Vector3D operator/(double s) const {
        return {x / s, y / s, z / s};
    }

    // Operator gabungan
    Vector3D& operator+=(const Vector3D& o) {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }

    Vector3D& operator-=(const Vector3D& o) {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }

    Vector3D& operator*=(double s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    // Operator perbandingan
    bool operator==(const Vector3D& o) const {
        return (x == o.x && y == o.y && z == o.z);
    }

    bool operator!=(const Vector3D& o) const {
        return !(*this == o);
    }

    /* ===== OPERASI VEKTOR ===== */
    // Dot product
    double dot(const Vector3D& o) const {
        return (x * o.x + y * o.y + z * o.z);
    }

    // Cross product
    Vector3D cross(const Vector3D& o) const {
        return {
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        };
    }

    // Panjang vektor
    double length() const {
        return sqrt(x * x + y * y + z * z);
    }

    // Panjang kuadrat vektor
    double lengthSqr() const {
        return (x * x + y * y + z * z);
    }

    // Normalisasi vektor
    Vector3D normalize() const {
        double len = length();
        if (len < 1e-10) {
            return {0, 0, 0};
        } else {
            return {x / len, y / len, z / len};
        }
    }

    /* ===== FUNGSI LAINNYA ===== */
    // Cari nilai minimum per komponen dari dua vektor
    static Vector3D min(const Vector3D& a, const Vector3D& b) {
        return {
            std::fmin(a.x, b.x),
            std::fmin(a.y, b.y),
            std::fmin(a.z, b.z)
        };
    }

    // Cari nilai maksimum per komponen dari dua vektor
    static Vector3D max(const Vector3D& a, const Vector3D& b) {
        return {
            fmax(a.x, b.x),
            fmax(a.y, b.y),
            fmax(a.z, b.z)
        };
    }

    // Mengubah vektor menjadi string
    string toString() const {
        return "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ")";
    }
};

// Ini buat kasus di mana skalar duluan baru dikali sama vektornya
inline Vector3D operator*(double s, const Vector3D& v) {
    return {s * v.x, s * v.y, s * v.z};
}