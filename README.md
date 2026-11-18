# Smart Campus Parking — Custom C++17 Data Structures, Routing & Transactional Rollback Engine

[![C++17 & ASan/UBSan CI](https://github.com/Danhle23/smart-campus-parking-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/Danhle23/smart-campus-parking-cpp/actions/workflows/ci.yml)
![Language](https://img.shields.io/badge/C%2B%2B-17%20Strict%20(-Wconversion%20-Wshadow)-2563EB)
![Memory Safety](https://img.shields.io/badge/ASan%20%2B%20UBSan-0%20Leaks%20%2F%200%20UB-10B981)
![Tests](https://img.shields.io/badge/Tests-24%2F24%20Passing-059669)

A high-reliability C++17 campus parking routing, fuzzy license-plate recovery, and narrow-lane vehicle relocation engine developed at **VNUHCM – University of Science (Faculty of Electronics & Telecommunications, Control & Automation Track)**.

Engineered from scratch **without STL container shortcuts** (`no std::unordered_map`, `no std::priority_queue`, `no std::stack`, `no std::queue`, `no std::sort`) to guarantee deterministic complexity bounds, explicit memory ownership, and transactional state rollback verified under **AddressSanitizer (`ASan`)** and **UndefinedBehaviorSanitizer (`UBSan`)**.

---

## Core Architectural Modules

| Subsystem | Custom Implementation | Engineering Purpose |
| :--- | :--- | :--- |
| **Session & Plate Index** | Custom open/chained **Hash Table** (`include/ds/`) | $O(1)$ average lookup mapping `card_id ↔ session` and `license_plate ↔ card_id`. |
| **Shortest-Path Routing** | Adjacency-list Graph + **BFS** & **Dijkstra** over custom **Binary Min-Heap** (`src/ParkingGraph.cpp`) | Computes minimum-distance accessible bays across multi-zone campus topologies. |
| **Noisy OCR Plate Recovery** | Normalization + **Levenshtein Edit Distance** + custom **Merge Sort** (`src/FuzzySearch.cpp`) | Recovers top-3 candidate vehicles when gate ANPR cameras suffer motion blur or glare. |
| **Narrow-Lane Blocking** | Custom **Linked Stack** + **Two-Phase Relocation & Rollback** (`src/ParkingSystem.cpp`) | Safely extracts blocking vehicles in LIFO order and atomically rolls back state if any invariant fails. |
| **Overflow Dispatch** | Role-aware **Linked FIFO Queues** + Min-Heap eviction selector | Separates faculty/student/vehicle-class queues during peak saturation. |

---

## Build, Verification & Memory Sanitizers

```bash
# 1. Compile with strict warnings (-Wall -Wextra -Wpedantic -Wconversion -Wshadow) & run 24 unit suites
make test

# 2. Run deterministic end-to-end invariant simulation
make demo

# 3. Run under AddressSanitizer + UndefinedBehaviorSanitizer
make sanitize
```

---

## Engineering Lead & Documentation

- **Team Lead & Core Architect:** Lê Thanh Danh (`@Danhle23`) — VNUHCM - University of Science (HCMUS)
- **Full Technical Report (29-Page PDF):** [`output/pdf/BaoCao_DoAn11_SmartCampusParking_FINAL.pdf`](./output/pdf/BaoCao_DoAn11_SmartCampusParking_FINAL.pdf)
- **Design & Test Matrix Docs:** [`docs/`](./docs/)
