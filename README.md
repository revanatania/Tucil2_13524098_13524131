# Tucil2_13524098_13524131

Program konversi model 3D `.obj` menjadi model voxel menggunakan algoritma **Divide and Conquer** berbasis struktur data **Octree**, dibuat untuk Tugas Kecil 2 IF2211 Strategi Algoritma Semester II 2025/2026.

---

## Deskripsi Program

Program membaca file `.obj`, membangun Octree dengan membagi ruang 3D secara rekursif menggunakan Divide and Conquer, lalu menghasilkan file `.obj` baru yang tersusun dari kubus-kubus seragam (voxel) pada permukaan objek.

**Fitur:**
- Voxelisasi permukaan objek 3D menggunakan Octree
- Voxel uniform diambil dari leaf node pada depth maksimum
- Pruning otomatis untuk region kosong
- Concurrency menggunakan `std::async` untuk paralelisasi subdivisi Octree
- 3D viewer interaktif berbasis SFML dengan solid rendering, backface culling, painter's algorithm, dan diffuse lighting

---

## Requirement

- C++17 atau lebih baru
- Compiler `g++` (GCC 9+)
- SFML 2.5 atau lebih baru

### Install SFML

**Linux (Ubuntu/Debian):**
```bash
sudo apt install libsfml-dev
```

**Windows:**
1. Download SFML dari https://www.sfml-dev.org/download.php
2. Pilih versi yang sesuai dengan compiler (MinGW/MSVC)
3. Ekstrak dan set path ke folder SFML

---

## Cara Kompilasi

Jalankan perintah berikut dari folder `src/`:

**Linux:**
```bash
g++ -std=c++17 -O2 -pthread \
  main.cpp \
  geometry/Intersection.cpp \
  geometry/Mesh.cpp \
  io/objParser.cpp \
  io/objWriter.cpp \
  octree/Octree.cpp \
  voxel/Voxelizer.cpp \
  viewer/Viewer.cpp \
  -lsfml-graphics -lsfml-window -lsfml-system \
  -o voxelizer
```

**Windows (MinGW):**
```bash
g++ -std=c++17 -O2 -pthread main.cpp geometry/Intersection.cpp geometry/Mesh.cpp io/objParser.cpp io/objWriter.cpp octree/Octree.cpp voxel/Voxelizer.cpp viewer/Viewer.cpp -lsfml-graphics -lsfml-window -lsfml-system -o voxelizer.exe
```

---

## Cara Menjalankan

```
./voxelizer <input.obj> <maxDepth> [outputPath] [parallelLevel] [--show]
```

| Argumen | Keterangan | Wajib |
|---------|------------|-------|
| `input.obj` | Path ke file `.obj` input | ✓ |
| `maxDepth` | Kedalaman maksimum Octree (integer ≥ 1) | ✓ |
| `outputPath` | Path file `.obj` output (default: `<nama>-voxelized.obj` di folder input) | |
| `parallelLevel` | Kedalaman sampai mana paralelisasi digunakan (default: 2) | |
| `--show` | Buka 3D viewer interaktif setelah konversi selesai | |

### Contoh

```bash
# Voxelisasi sederhana
./voxelizer ../test/pumpkin.obj 5

# Dengan output path eksplisit
./voxelizer ../test/cow.obj 6 ../test/cow-voxelized.obj

# Dengan viewer
./voxelizer ../test/pumpkin.obj 6 ../test/pumpkin-voxelized.obj 2 --show

# Depth tinggi dengan paralelisasi agresif
./voxelizer ../test/cow.obj 8 ../test/cow-depth8.obj 4 --show
```

### Kontrol Viewer

| Aksi | Kontrol |
|------|---------|
| Rotasi | Drag mouse kiri |
| Zoom in/out | Scroll mouse |
| Tutup viewer | Escape atau tombol X |

---

## Struktur Repository

```
Tucil2_13524098_13524131
├── bin
│   └── voxelizer.exe
├── doc
│   └── LaporanTucil2_13524098_13524131.pdf
├── src
│   ├── geometry
│   │   ├── AABB.hpp
│   │   ├── Intersection.cpp
│   │   ├── Intersection.hpp
│   │   ├── Mesh.cpp
│   │   ├── Mesh.hpp
│   │   ├── Triangle.hpp
│   │   └── Vector3D.hpp
│   ├── io
│   │   ├── objParser.cpp
│   │   ├── objParser.hpp
│   │   ├── objWriter.cpp
│   │   └── objWriter.hpp
│   ├── main.cpp
│   ├── octree
│   │   ├── Octree.cpp
│   │   ├── Octree.hpp
│   │   └── octreeNode.hpp
│   ├── viewer
│   │   ├── Viewer.cpp
│   │   └── Viewer.hpp
│   └── voxel
│       ├── Voxelizer.cpp
│       └── Voxelizer.hpp
└── test
│   ├── cow-depth6.obj
│   ├── cow-depth8.obj
│   ├── cow.obj
│   ├── line-depth6.obj
│   ├── line.obj
│   ├── pumpkin-depth5.obj
│   ├── pumpkin.obj
│   ├── teapot-depth5.obj
│   └── teapot.obj 
├── README.md
```

---

## Author

| Nama | NIM |
|------|-----|
| Reva Natania Sitohang | 13524098 |
| Amanda Aurellia Salsabilla | 13524131 |

IF2211 Strategi Algoritma — Semester II 2025/2026
Institut Teknologi Bandung
