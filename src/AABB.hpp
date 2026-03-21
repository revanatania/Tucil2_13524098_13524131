#pragma once
#include "Vector3D.hpp"

struct AABB {
    /* ===== VARIABEL GLOBAL ===== */
    // Titik sudut terkecil dari kubus (paling kiri-bawah-belakang)
    Vector3D minCorner;
    // Titik sudut terbesar dari kubus (paling kanan-atas-depan)
    Vector3D maxCorner;
    // Titik tengah kubus
    Vector3D center;
    // Setengah panjang sisi kubus
    double halfSideLength;

    /* ===== KONSTRUKTOR ===== */
    // Konstruktor utama: bikin AABB dari titik tengah dan setengah panjang sisi
    AABB(Vector3D center, double halfSideLength) {
        this->center = center;
        this->halfSideLength = halfSideLength;

        // Titik paling kiri-bawah-belakang: titik pusat - panjang setengah sisi
        this->minCorner = Vector3D(
            center.x - halfSideLength,
            center.y - halfSideLength,
            center.z - halfSideLength
        );

        // Titik paling kanan-atas-depan: titik pusat + panjang setengah sisi
        this->maxCorner = Vector3D(
            center.x + halfSideLength,
            center.y + halfSideLength,
            center.z + halfSideLength
        );
    }

    // Konstruktor default: tanpa parameter, set semuanya 0
    AABB() {
        this->center = Vector3D(0, 0, 0);
        this->halfSideLength = 0;
        this->minCorner = Vector3D(0, 0, 0);
        this->maxCorner = Vector3D(0, 0, 0);
    }

    /* ====== METHOD ===== */
    // Cari sisi terpanjang dari tiga dimensi, terus pakai dia jadi panjang semua dimensi
    static AABB fromBounds(Vector3D minPoint, Vector3D maxPoint) {
        double lengthX = maxPoint.x - minPoint.x;
        double lengthY = maxPoint.y - minPoint.y;
        double lengthZ = maxPoint.z - minPoint.z;

        double longestSide = lengthX;
        if (lengthY > longestSide) {
            longestSide = lengthY;
        }
        if (lengthZ > longestSide) {
            longestSide = lengthZ;
        }

        // Padding supaya titik yang pas di pinggiran pun bisa tetep ke-include
        double halfSideLength = (longestSide / 2.0) * 1.001;

        Vector3D centerPoint(
            (minPoint.x + maxPoint.x) / 2.0,
            (minPoint.y + maxPoint.y) / 2.0,
            (minPoint.z + maxPoint.z) / 2.0
        );

        return AABB(centerPoint, halfSideLength);
    }

    // Bagi AABB menjadi 8 octant (anak) yang lebih kecil
    void subdivide(AABB children[8]) const {
        double childHalfSideLength = halfSideLength / 2.0;

        double centerX = center.x;
        double centerY = center.y;
        double centerZ = center.z;

        // 8 kombinasi arah vektor dari titik pusat parent
        children[0] = AABB(Vector3D(centerX - childHalfSideLength, centerY - childHalfSideLength, centerZ - childHalfSideLength), childHalfSideLength);
        children[1] = AABB(Vector3D(centerX - childHalfSideLength, centerY - childHalfSideLength, centerZ + childHalfSideLength), childHalfSideLength);
        children[2] = AABB(Vector3D(centerX - childHalfSideLength, centerY + childHalfSideLength, centerZ - childHalfSideLength), childHalfSideLength);
        children[3] = AABB(Vector3D(centerX - childHalfSideLength, centerY + childHalfSideLength, centerZ + childHalfSideLength), childHalfSideLength);
        children[4] = AABB(Vector3D(centerX + childHalfSideLength, centerY - childHalfSideLength, centerZ - childHalfSideLength), childHalfSideLength);
        children[5] = AABB(Vector3D(centerX + childHalfSideLength, centerY - childHalfSideLength, centerZ + childHalfSideLength), childHalfSideLength);
        children[6] = AABB(Vector3D(centerX + childHalfSideLength, centerY + childHalfSideLength, centerZ - childHalfSideLength), childHalfSideLength);
        children[7] = AABB(Vector3D(centerX + childHalfSideLength, centerY + childHalfSideLength, centerZ + childHalfSideLength), childHalfSideLength);
    }

    // Buat ngecek ada titik ini ga dalam kubus
    bool containsPoint(Vector3D point) const {
        if (point.x < minCorner.x || point.x > maxCorner.x) {
            return false;
        }
        if (point.y < minCorner.y || point.y > maxCorner.y) {
            return false;
        }
        if (point.z < minCorner.z || point.z > maxCorner.z) {
            return false;
        }
        return true;
    }
};