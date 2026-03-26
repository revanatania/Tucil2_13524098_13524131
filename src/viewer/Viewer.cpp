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

    // Set kamera supaya ngeliat ke tengah objek dari luar
    camera.target = centerPoint;
    camera.orbitRadius = objectSize * 1.5;
    camera.focalLength = 1.2;
    camera.theta = 0.5;
    camera.phi = 1.0;

    // Hitung posisi awal kamera
    updateCameraPosition();

    // Bangun semua face dari semua voxel
    for (const AABB& box : voxels) {
        buildFacesFromVoxel(box);
    }
}

// Bangun 6 face dari satu voxel (kubus)
// Tiap face punya 4 titik sudut dan 1 normal
// Urutan 4 titik tiap face: counter-clockwise dilihat dari luar
void Viewer::buildFacesFromVoxel(const AABB& box) {
    double x0 = box.minCorner.x;
    double y0 = box.minCorner.y;
    double z0 = box.minCorner.z;
    double x1 = box.maxCorner.x;
    double y1 = box.maxCorner.y;
    double z1 = box.maxCorner.z;

    // 8 titik sudut kubus
    Vector3D v000(x0, y0, z0);
    Vector3D v100(x1, y0, z0);
    Vector3D v010(x0, y1, z0);
    Vector3D v110(x1, y1, z0);
    Vector3D v001(x0, y0, z1);
    Vector3D v101(x1, y0, z1);
    Vector3D v011(x0, y1, z1);
    Vector3D v111(x1, y1, z1);

    // Bottom face: normal ke arah bawah (0, -1, 0)
    Face3D bottom;
    bottom.corners[0] = v000;
    bottom.corners[1] = v001;
    bottom.corners[2] = v101;
    bottom.corners[3] = v100;
    bottom.normal = Vector3D(0.0, -1.0, 0.0);
    allFaces.push_back(bottom);

    // Top face: normal ke arah atas (0, 1, 0)
    Face3D top;
    top.corners[0] = v010;
    top.corners[1] = v110;
    top.corners[2] = v111;
    top.corners[3] = v011;
    top.normal = Vector3D(0.0, 1.0, 0.0);
    allFaces.push_back(top);

    // Front face: normal ke arah depan (0, 0, -1)
    Face3D front;
    front.corners[0] = v000;
    front.corners[1] = v100;
    front.corners[2] = v110;
    front.corners[3] = v010;
    front.normal = Vector3D(0.0, 0.0, -1.0);
    allFaces.push_back(front);

    // Back face: normal ke arah belakang (0, 0, 1)
    Face3D back;
    back.corners[0] = v001;
    back.corners[1] = v011;
    back.corners[2] = v111;
    back.corners[3] = v101;
    back.normal = Vector3D(0.0, 0.0, 1.0);
    allFaces.push_back(back);

    // Left face: normal ke arah kiri (-1, 0, 0)
    Face3D left;
    left.corners[0] = v000;
    left.corners[1] = v010;
    left.corners[2] = v011;
    left.corners[3] = v001;
    left.normal = Vector3D(-1.0, 0.0, 0.0);
    allFaces.push_back(left);
 
    // Right face: normal ke arah kanan (1, 0, 0)
    Face3D right;
    right.corners[0] = v100;
    right.corners[1] = v101;
    right.corners[2] = v111;
    right.corners[3] = v110;
    right.normal = Vector3D(1.0, 0.0, 0.0);
    allFaces.push_back(right);
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
Matrix4x4 Viewer::computeViewMatrix() const {
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
    
    viewMatrix.data[1][0] = correctedUp.x;
    viewMatrix.data[1][1] = correctedUp.y;
    viewMatrix.data[1][2] = correctedUp.z;
    viewMatrix.data[1][3] = -(correctedUp.x * camera.eye.x + correctedUp.y * camera.eye.y + correctedUp.z * camera.eye.z);
    
    viewMatrix.data[2][0] = -forward.x;
    viewMatrix.data[2][1] = -forward.y;
    viewMatrix.data[2][2] = -forward.z;
    viewMatrix.data[2][3] = -(forward.x * camera.eye.x + forward.y * camera.eye.y + forward.z * camera.eye.z);

    viewMatrix.data[3][3] = 1.0;

    return viewMatrix;
}

// Proyeksi satu titik 3D ke koordinat layar 2D
bool Viewer::projectPoint(const Vector3D& point3D, const Matrix4x4& viewMatrix, Vector2f& outScreenPoint, double& outCameraZ) const {
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

    outCameraZ = cameraZ;

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

// Gambar semua face sebagai solid polygon
// Digunakan backface culling, depth sorting, dan lighting
void Viewer::drawFaces(RenderWindow& window, const Matrix4x4& viewMatrix) {
    // Arah cahaya ngikutin posisi kamera
    Vector3D toCamera = (camera.eye - camera.target).normalize();
    Vector3D lightUp = Vector3D(0.0, 1.0, 0.0);
    Vector3D lightDirection = (toCamera + lightUp * 0.5).normalize();

    // Arah dari objek ke kamera
    Vector3D cameraDirection = (camera.eye - camera.target).normalize();

    /* Tahap 1: kumpulin face yang lolos backface culling, proyeksiin, dan hitung depth */
    // Struct sementara buat nyimpen face yang udah siap digambar
    struct ReadyFace {
        Vector2f screenCorners[4];
        double depth;
        double brightness;
        bool valid;
    };

    vector<ReadyFace> readyFaces;
    readyFaces.reserve(allFaces.size());

    for (const Face3D& face : allFaces) {
        // Backface culling: dot product normal face dengan arah kamera
        double facingCamera = face.normal.dot(cameraDirection);
        if (facingCamera <= 0.0) {
            continue;
        }

        // Proyeksikan 4 titik sudut face ke layar
        ReadyFace ready;
        ready.valid = true;
        double totalDepth = 0.0;
 
        for (int i = 0; i < 4; i++) {
            double cameraZ = 0.0;
            bool visible = projectPoint(face.corners[i], viewMatrix, ready.screenCorners[i], cameraZ);
            if (!visible) {
                ready.valid = false;
                break;
            }
            totalDepth += cameraZ;
        }
 
        if (!ready.valid) {
            continue;
        }

        // Depth = rata-rata Z keempat titik
        ready.depth = totalDepth / 4.0;

        // Hitung lighting: dot product normal face dengan arah cahaya
       double lightDot = face.normal.dot(lightDirection);
        if (lightDot < 0.0) {
            lightDot = 0.0;
        }
        // ambient = cahaya dasar yang selalu ada = 0.4
        // diffuse = cahaya yang bergantung pada sudut normal = 0.6
        ready.brightness = 0.4 + 0.6 * lightDot; 

        readyFaces.push_back(ready);
    }

    /* Tahap 2: sort dari yang paling jauh ke yang paling dekat*/
    sort(readyFaces.begin(), readyFaces.end(), [](const ReadyFace& faceA, const ReadyFace& faceB) {
            return faceA.depth > faceB.depth;
        }
    );

    /* Tahap 3: gambar tiap face sebagai ConvexShape*/
    for (const ReadyFace& ready : readyFaces) {
        // Warna dasar abu-abu kebiruan
        // Dikalikan brightness untuk efek lighting
        float baseR = 180.0f;
        float baseG = 200.0f;
        float baseB = 220.0f;
 
        float r = (float)(baseR * ready.brightness);
        float g = (float)(baseG * ready.brightness);
        float b = (float)(baseB * ready.brightness);
 
        // Clamp supaya tidak melebihi 255
        if (r > 255.0f) {
            r = 255.0f;
        }
        if (g > 255.0f) {
            g = 255.0f;
        }
        if (b > 255.0f) {
            b = 255.0f;
        }
 
        Color faceColor((Uint8)r, (Uint8)g, (Uint8)b, 255);
 
        // Buat polygon dengan 4 titik
        ConvexShape polygon;
        polygon.setPointCount(4);
        polygon.setPoint(0, ready.screenCorners[0]);
        polygon.setPoint(1, ready.screenCorners[1]);
        polygon.setPoint(2, ready.screenCorners[2]);
        polygon.setPoint(3, ready.screenCorners[3]);
        polygon.setFillColor(faceColor);
 
        // Garis tepi tipis gelap supaya batas antar voxel terlihat
        polygon.setOutlineColor(Color(30, 30, 40, 200));
        polygon.setOutlineThickness(0.5f);
 
        window.draw(polygon);
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
    }

    // Scroll mouse = zoom in dan out
    if (event.type == Event::MouseWheelScrolled) {
        double zoomSensitivity = 0.1;
        camera.orbitRadius -= (double)event.mouseWheelScroll.delta * zoomSensitivity * camera.orbitRadius;

        // Batasan radius zoom
        if (camera.orbitRadius < 0.1) {
            camera.orbitRadius = 0.1;
        }
        if (camera.orbitRadius > 9999999.0) {
            camera.orbitRadius = 9999999.0;
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
        Matrix4x4 viewMatrix = computeViewMatrix();

        // Clear layar pake warna gelap
        window.clear(Color(25, 25, 25));

        // Gambar semua faces
        drawFaces(window, viewMatrix);

        // Tampilin hasil ke layar
        window.display();
    }
}