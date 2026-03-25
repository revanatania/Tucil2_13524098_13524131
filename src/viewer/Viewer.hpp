#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "../geometry/AABB.hpp"
#include "../geometry/Vector3D.hpp"

using namespace sf;
using namespace std;

/* ===== STRUCT MATRIKS ===== */
// Matriks 4X4 buat transformasi 3D
struct Matrix4x4 {
    // data[baris][kolom]
    double data[4][4];

    // Konstruktor default: isi dengan 0
    Matrix4x4() {
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                data[row][col] = 0.0;
            }
        }
    }

    // Matriks identitas
    static Matrix4x4 identity() {
        Matrix4x4 result;
        result.data[0][0] = 1.0;
        result.data[1][1] = 1.0;
        result.data[2][2] = 1.0;
        result.data[3][3] = 1.0;
        return result;
    }

    // Perkalian matriks
    Matrix4x4 operator*(const Matrix4x4 other) const {
        Matrix4x4 result;
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                double sum = 0.0;
                for (int k = 0; k < 4; k++) {
                    sum = sum + (data[row][k] * other.data[k][col]);
                }
                result.data[row][col] = sum;
            }
        }
        return result;
    }
};

/* ===== STRUCT CAMERA ===== */
// Struct camera buat nyimpen informasi tentang posisi dan orientasi kamera
struct Camera {
    // Posisi kamera di dunia 3D
    Vector3D eye;

    // Titik yang dilihat kamera
    Vector3D target;

    // Arah atas dari kamera atau sumbu Y
    Vector3D up;

    // Jarak kamera ke target
    double orbitRadius;

    // Sudut horizontal kamera: cursor pindah kiri kanan
    double theta;

    // Sudut vertikal kamera: cursor pindah atas bawah
    double phi;

    // Panjang fokus kamera: zoom in dan zoom out
    double focalLength;

    // Konstruktor default
    Camera() {
        target = Vector3D(0.0, 0.0, 0.0);
        up = Vector3D(0.0, 1.0, 0.0);
        orbitRadius = 3.0;
        theta = 0.5;
        phi = 1.0;
        focalLength = 1.5;

        // Posisi awal kamera
        eye = Vector3D(0.0, 0.0, orbitRadius);
    }
};

/* ===== STRUCT EDGE ===== */
// Satu garis yang ngehubungin dua titik, buat ngegambar kerangka voxel
struct Edge {
    // Dua titik ujung garis
    Vector3D pointA;
    Vector3D pointB;

    Edge(Vector3D a, Vector3D b) {
        this->pointA = a;
        this->pointB = b;
    }
};

/* ===== CLASS VIEWER ===== */
// Digunain buat ngurus semua proses viewer
class Viewer {
    public:
    // Lebar dan tinggi jendela (dalam pixel)
    static const int WINDOW_WIDTH = 900;
    static const int WINDOW_HEIGHT = 700;

    // Konsutrktor
    Viewer(const vector<AABB>& voxels);

    // Loop utama ngejalanin viewer
    void run();

    private:
    // Daftar semua edge yang bakal digambar
    vector<Edge> edges;

    // State kamera saat ini
    Camera camera;

    // State mouse untuk deteksi drag
    bool mouseButtonIsPressed;
    float mousePreviousX;
    float mousePreviousY;

    // Bangun daftar edge dari satu voxel (satu kubus ada 12 edge)
    void buildEdgesFromVoxel(const AABB& box);

    // Hitung posisi kamera dari spherical coordinates
    void updateCameraPosition();

    // Hitung view matriks dari posisi kamera saat ini
    Matrix4x4 computerViewMatrix() const;

    // Proyeksikan satu titik 3D ke koordinat layar 2D
    bool projectPoint(const Vector3D& point3D, const Matrix4x4& viewMatrix, Vector2f& outScreenPoint) const;

    // Gambar semua edge ke render target
    void drawEdges(RenderWindow& window, const Matrix4x4& viewMatrix);

    // Handle semua event 
    void handleEvents(RenderWindow& window, const Event& event);
};