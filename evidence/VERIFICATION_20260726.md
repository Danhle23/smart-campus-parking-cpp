# Bằng chứng xác minh ngày 26/07/2026

## Môi trường

- Hệ điều hành: macOS 26.0.1, build 25A362
- Kiến trúc: arm64
- Trình biên dịch: Apple clang 17.0.0
- Chuẩn ngôn ngữ: C++17

## Build

Lệnh:

```bash
make -B all
```

Kết quả: exit code 0. Hai chương trình `build/smart_parking` và
`build/smart_parking_tests` được biên dịch với:

```text
-std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
```

Không có cảnh báo trong output biên dịch.

## Test

Lệnh:

```bash
make test
```

Kết quả cuối:

```text
[PASS] HashTable: collision, erase va rehash
[PASS] LinkedQueue: FIFO va vong doi rong-lai-rong
[PASS] LinkedStack: LIFO
[PASS] BinaryMinHeap: thu tu uu tien
[PASS] MergeSort: on dinh khi khoa bang nhau
[PASS] Chuan hoa chuoi va Levenshtein
[PASS] FeeCalculator: moc 59, 60, 61 phut
[PASS] DataLoader: CSV co quote va camera log
[PASS] Graph: Dijkstra dung met, BFS dung so canh
[PASS] Graph: tie-break theo ma slot
[PASS] DataLoader: loi file khong lam hong graph cu
[PASS] Check-in/check-out va chong trung
[PASS] Queue: ba xe cho duoc phuc vu FIFO
[PASS] Relocation: Heap chon xe som nhat, Stack dung thu tu
[PASS] Relocation: target checkout truoc confirm khong lam mat GV
[PASS] Relocation pending uu tien va Heap giu ung vien khi huy plan
[PASS] Khu GV: uu tien Queue Giang vien truoc Sinh vien
[PASS] Khu tam uu tien truoc Queue
[PASS] Fuzzy Search top-3 active vehicle
[PASS] O to: overflow, relocation, rehome va tinh phi end-to-end
[PASS] Queue tach theo loai xe, khong head-of-line blocking
[PASS] History dung MergeSort va invariant sau chuoi thao tac
[PASS] Input validation va graph unreachable
[PASS] Stress 10000 khoa HashTable

Ket qua: 24 PASS, 0 FAIL
```

## Demo

Lệnh:

```bash
make demo
```

Các mốc xác nhận:

```text
BFS_SO_CANH: GATE_A -> LM01 (20 m)
DIJKSTRA: GATE_A -> J0 -> JL -> LM01 (10 m)
Plan: MOVE-0001, di doi 59X112345 tai LM03
Thu tu dua xe chan ra (push Stack): LM01 LM02
Thu tu hoan nguyen (pop Stack): LM02 LM01
Camera: 59-X1.123.4S -> chuan hoa: 59X11234S
1. 59X112345 | distance=1 | similarity=88.9% | slot=KHU_TAM_DI_DOI
Invariant: PASS
```

Kết quả: exit code 0.

## Sanitizer

Lệnh:

```bash
make sanitize
```

Cờ bổ sung:

```text
-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
```

Kết quả cuối:

```text
Ket qua: 24 PASS, 0 FAIL
```

Exit code 0; AddressSanitizer và UndefinedBehaviorSanitizer không phát sinh
báo cáo trong các ca đã chạy.

## SHA-256 của các tệp trọng yếu

```text
d4cb716d154592ad2b4c7217a3c6a37db036349b8a362d9609036c90ad5e1f95  Makefile
40dde9bbac33f6d702e85a7335dca4e7733b1169ed149c3a46344deba66a6ebf  tests/test_main.cpp
8ed4a5dfcd49a81b54ccb4966060286f43e093ede4c4180915515f78ff96c787  src/ParkingSystem.cpp
4cb95bfb353a5940952cd35e8b04814ee269bfffc86802225db061c8d86b4f5a  include/services/ParkingSystem.hpp
619d1b27c98c6197e61079554acbce07372423dfbbd2fb68488e36544a295f31  data/fees.cfg
```

