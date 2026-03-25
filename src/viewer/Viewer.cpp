#include "Viewer.hpp"
#include <cmath>
#include <algorithm>

// Konstruktor
Viewer::Viewer(const vector<AABB>& voxels) {
    mouseButtonIsPressed = false;
    mousePreviousX = 0.0f;
    mousePreviousY = 0.0f;

    // Hitung bounding box buat cari center
    Vector3D globalMin(1e18, 1e18, 1e18);
    Vector3D globalMax(-1e18, -1e18, -1e18);

    for (const AABB& box : voxels) {
        if (box.minCorner.x < globalMin.x) {
            globalMin.x = box.minCorner.x;
        }
        if (box.minCorner.y < globalMin.y) {
            globalMin.y = box.minCorner.y;
        }
        if (box.minCorner.z < globalMin.z) {
            globalMin.z = box.minCorner.z;
        }
        if (box.maxCorner.x > globalMax.x) {
            globalMax.x = box.maxCorner.x;
        }
        if (box.maxCorner.y > globalMax.y) {
            globalMax.y = box.maxCorner.y;
        }
        if (box.maxCorner.z > globalMax.z) {
            globalMax.z = box.maxCorner.z;
        }
    }

    // Titik tengah seluruh objek
    Vector3D centerPoint(
        (globalMin.x + globalMax.x) / 2.0,
        (globalMin.y + globalMax.y) / 2.0,
        (globalMin.z + globalMax.z) / 2.0
    );

    // Panjang diagonal objek buat nentuin jarak kamera awal
    double dx = globalMax.x - globalMin.x;
    double dy = globalMax.y - globalMin.y;
    double dz = globalMax.z - globalMin.z;
    double objectSize = sqrt(dx * dx + dy * dy + dz * dz);

    // Set kamera supaya ngeliat ke tengah objek dari jarak yang pas
    camera.target = centerPoint;
    camera.orbitRadius = objectSize * 1.2;
    camera.focalLength = 1.5;
    camera.theta = 0.5;
    camera.phi = 1.0;

    // Hitung posisi awal kamera
    updateCameraPosition();

    // Bangun semua edge dari semua voxel
    for (const AABB& box : voxels) {
        buildEdgesFromVoxel(box);
    }
}

// Bikin edges dari daftar voxel
// Huruf pertama: L/R, huruf kedua: B/T, huruf ketiga: Near/Far
void Viewer::buildEdgesFromVoxel(const AABB& box) {
    double minX = box.minCorner.x;
    double minY = box.minCorner.y;
    double minZ = box.minCorner.z;
    double maxX = box.maxCorner.x;
    double maxY = box.maxCorner.y;
    double maxZ = box.maxCorner.z;

    // 8 titik sudut kubus
    Vector3D LBN(minX, minY, minZ);
    Vector3D RBN(maxX, minY, minZ);
    Vector3D LTN(minX, maxY, minZ);
    Vector3D RTN(maxX, maxY, minZ);
    Vector3D LBF(minX, minY, maxZ);
    Vector3D RBF(maxX, minY, maxZ);
    Vector3D LTF(minX, maxY, maxZ);
    Vector3D RTF(maxX, maxY, maxZ);

    // 4 edge di sisi Near (depan)
    edges.push_back(Edge(LBN, RBN));
    edges.push_back(Edge(RBN, RTN));
    edges.push_back(Edge(RTN, LTN));
    edges.push_back(Edge(LTN, LBN));

    // 4 edge di sisi Far (belakang)
    edges.push_back(Edge(LBF, RBF));
    edges.push_back(Edge(RBF, RTF));
    edges.push_back(Edge(RTF, LTF));
    edges.push_back(Edge(LTF, LBF));

    // 4 edge penghubung Near ke Far (samping)
    edges.push_back(Edge(LBN, LBF));
    edges.push_back(Edge(RBN, RBF));
    edges.push_back(Edge(LTN, LTF));
    edges.push_back(Edge(RTN, RTF));
}

// Posisi kamera (sudut pandang)
void Viewer::updateCameraPosition() {
    // Batas phi supaya kamera ga "kebalik"
    const double PI = 3.14159265358979323846;
    if (camera.phi < 0.1) {
        camera.phi = 0.1;
    }
    if (camera.phi > PI - 0.1) {
        camera.phi = PI - 0.1;
    }

    double sinPhi = sin(camera.phi);
    double cosPhi = cos(camera.phi);
    double sinTheta = sin(camera.theta);
    double cosTheta = cos(camera.theta);

    // Posisi kamera relatif terhadap target
    double offsetX = camera.orbitRadius * sinPhi * cosTheta;
    double offsetY = camera.orbitRadius * cosPhi;
    double offsetZ = camera.orbitRadius * sinPhi * sinTheta;

    // Posisi kamera = target + offset
    camera.eye.x = camera.target.x + offsetX;
    camera.eye.y = camera.target.y + offsetY;
    camera.eye.z = camera.target.z + offsetZ;
}

// View matriks dari posisi kamera saat ini
Matrix4x4 Viewer::computerViewMatrix() const {
    // Hitung tiga sumbu kamera
    // forward = arah pandang kamera (dari eye ke target)
    Vector3D forward = (camera.target - camera.eye).normalize();

    // right = arah kanan kamera
    Vector3D right = forward.cross(camera.up).normalize();

    // up kamera yang sudah dikoreksi
    Vector3D correctedUp = right.cross(forward);

    // Susun view matriks
    Matrix4x4 viewMatrix;

    viewMatrix.data[0][0] = right.x;
    viewMatrix.data[0][1] = right.y;
    viewMatrix.data[0][2] = right.z;
    viewMatrix.data[0][3] = -(right.x * camera.eye.x + right.y * camera.eye.y + right.z * camera.eye.z);
    
    viewMatrix.data[1][0] =  correctedUp.x;
    viewMatrix.data[1][1] =  correctedUp.y;
    viewMatrix.data[1][2] =  correctedUp.z;
    viewMatrix.data[1][3] = -(correctedUp.x * camera.eye.x + correctedUp.y * camera.eye.y + correctedUp.z * camera.eye.z);
    
    viewMatrix.data[2][0] = -forward.x;
    viewMatrix.data[2][1] = -forward.y;
    viewMatrix.data[2][2] = -forward.z;
    viewMatrix.data[2][3] =  (forward.x * camera.eye.x + forward.y * camera.eye.y + forward.z * camera.eye.z);

    viewMatrix.data[3][3] = 1.0;

    return viewMatrix;
}

// Proyeksi satu titik 3D ke koordinat layar 2D
bool Viewer::projectPoint(const Vector3D& point3D, const Matrix4x4& viewMatrix, Vector2f& outScreenPoint) const {
    // Tahap 1: transformasi ke camera space
    double inputX = point3D.x;
    double inputY = point3D.y;
    double inputZ = point3D.z;
    double inputW = 1.0;

    double cameraX = viewMatrix.data[0][0] * inputX +
                     viewMatrix.data[0][1] * inputY +
                     viewMatrix.data[0][2] * inputZ +
                     viewMatrix.data[0][3] * inputW;
 
    double cameraY = viewMatrix.data[1][0] * inputX +
                     viewMatrix.data[1][1] * inputY +
                     viewMatrix.data[1][2] * inputZ +
                     viewMatrix.data[1][3] * inputW;
 
    double cameraZ = viewMatrix.data[2][0] * inputX +
                     viewMatrix.data[2][1] * inputY +
                     viewMatrix.data[2][2] * inputZ +
                     viewMatrix.data[2][3] * inputW;

    // Kalau Z <= 0, titik ada di belakang kamera jadi gausah kegambar
    if (cameraZ <= 0.001) {
        return false;
    }

    // Tahap 2: perspective projection
    double projectedX = cameraX * camera.focalLength / cameraZ;
    double projectedY = cameraY * camera.focalLength / cameraZ;

    // Tahap 3: viewport mapping
    double screenX = (projectedX + 1.0) / 2.0 * (double)WINDOW_WIDTH;
    double screenY = (1.0 - projectedY) / 2.0 * (double)WINDOW_HEIGHT;

    outScreenPoint.x = (float)screenX;
    outScreenPoint.y = (float)screenY;

    return true;
}

// Gambar semua edge ke window
void Viewer::drawEdges(RenderWindow& window, const Matrix4x4& viewMatrix) {
    // Warna garis putih agak transparan
    Color edgeColor(200, 220, 255, 180);

    for (const Edge& edge : edges) {
        Vector2f screenPointA;
        Vector2f screenPointB;

        // Proyeksikan kedua ujung edge
        bool pointAVisible = projectPoint(edge.pointA, viewMatrix, screenPointA);
        bool pointBVisible = projectPoint(edge.pointB, viewMatrix, screenPointB);

        // Gambar kalau kedua ujung terlihat aja
        if (pointAVisible && pointBVisible) {
            Vertex line[2];
            line[0].position = screenPointA;
            line[0].color = edgeColor;
            line[1].position = screenPointB;
            line[1].color = edgeColor;

            window.draw(line, 2, Lines);
        }
    }
}

// Handle semua event SFML
void Viewer::handleEvents(RenderWindow& window, const Event& event) {
    // Tutup jendela
    if (event.type == Event::Closed) {
        window.close();
    }

    // Tekan tombol kiri mouse = mulai drag
    if (event.type == Event::MouseButtonPressed) {
        if (event.mouseButton.button == Mouse::Left) {
            mouseButtonIsPressed = true;
            mousePreviousX = (float)event.mouseButton.x;
            mousePreviousY = (float)event.mouseButton.y;
        }
    }

    // Lepas tombol mouse = stop drag
    if (event.type == Event::MouseButtonReleased) {
        if (event.mouseButton.button == Mouse::Left) {
            mouseButtonIsPressed = false;
        }
    }

    // Gerak mouse saat tombol ditekan = rotasi kamera
    if (event.type == Event::MouseMoved) {
        if (mouseButtonIsPressed) {
            float currentX = (float)event.mouseMove.x;
            float currentY = (float)event.mouseMove.y;

            // Seberapa jauh mouse bergerak sejak frame sebelumnya
            float deltaX = currentX - mousePreviousX;
            float deltaY = currentY - mousePreviousY;

            // Sensitivitas rotasi
            double rotationSensitivity = 0.005;

            // Gerak horizontal
            camera.theta += (double)deltaX * rotationSensitivity;

            // Gerak vertikal
            camera.phi -= (double)deltaY * rotationSensitivity;

            // Update posisi kamera
            updateCameraPosition();

            mousePreviousX = currentX;
            mousePreviousY = currentY;
        }

        // Scroll mouse = zoom in dan out
        if (event.type == Event::MouseWheelScrolled) {
            double zoomSensitivity = 0.1;
            camera.orbitRadius -= (double)event.mouseWheelScroll.delta * (camera.orbitRadius * 0.1);
        }

        // Batasan radius zoom
        if (camera.orbitRadius < 0.1) {
            camera.orbitRadius = 0.1;
        }
        if (camera.orbitRadius > 1000.0) {
            camera.orbitRadius = 1000.0;
        }

        updateCameraPosition();
    }

    // Escape buat tutup window
    if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Escape) {
            window.close();
        }
    }
}

// Run
void Viewer::run() {
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "3D Viewer Object Voxelizer");

    // Batas frame 60 fps
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        // Handle antrian event
        Event event;
        while (window.pollEvent(event)) {
            handleEvents(window, event);
        }

        // Hitung view matriks sekali per frame
        Matrix4x4 viewMatrix = computerViewMatrix();

        // Clear layar pake warna gelap
        window.clear(Color(25, 25, 25));

        // Gambar semua edge
        drawEdges(window, viewMatrix);

        // Tampilin hasil ke layar
        window.display();
    }
}