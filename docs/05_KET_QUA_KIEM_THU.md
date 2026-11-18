# 5. Kết quả kiểm thử đã xác nhận

Ngày chạy: **2026-07-24**  
Môi trường: macOS Apple Silicon, Apple Clang 17, C++17.

## Build warning nghiêm ngặt

```bash
make -B all
```

Cờ biên dịch:

```text
-std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
```

Kết quả: build thành công, **0 warning**.

## Test thường

```bash
./build/smart_parking_tests
```

Kết quả:

```text
Ket qua: 24 PASS, 0 FAIL
```

Phạm vi gồm unit test CTDL, thuật toán, parser, nghiệp vụ tích hợp, invariant và
stress 10.000 khóa Hash Table.

Regression test mới xác nhận đồng thời ba điều: pending relocation được phục vụ
trước Queue đến sau khi một ô Giảng viên khác trống; xe Sinh viên chờ không vượt
nhu cầu Giảng viên; và hủy plan không làm mất ứng viên overflow sớm nhất trong
Heap. Bộ test relocation cũng kiểm tra phí Giảng viên bắt đầu từ thời điểm
confirm/cấp ô, không tính khoảng chờ xác nhận.

Test relocation còn dựng tạm bản đồ hai cổng `GATE_A`, `GATE_B` thuộc hai
component rời nhau. `OVER_A` đến sớm hơn nhưng không reachable từ `GATE_B`, nên
Heap chọn `OVER_B` và confirm thành công; sau đó `GATE_A` vẫn lập được plan cho
`OVER_A`, chứng minh entry tạm pop đã được reinsert; toàn hệ thống vẫn giữ invariant.
Ca này nằm trong test relocation hiện có nên tổng số test vẫn là 24.

## AddressSanitizer + UndefinedBehaviorSanitizer

```bash
make sanitize
```

Cờ bổ sung:

```text
-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
```

Kết quả:

```text
Ket qua: 24 PASS, 0 FAIL
```

Không có báo cáo use-after-free, heap-buffer-overflow, double-free hoặc undefined
behavior.

## Demo tự động

```bash
./build/smart_parking --demo
```

Các điểm đã quan sát:

- BFS: 20 m; Dijkstra: 10 m.
- Sinh viên đỗ tràn đúng `LM03`.
- Xe tiếp theo vào Queue FIFO.
- Min-Heap chọn đúng xe `SV-OVER`.
- Stack push `LM01, LM02`, pop `LM02, LM01`.
- Fuzzy Search nhận `59-X1.123.4S` gần `59X112345` với distance 1.
- Check-out 61 phút: 10.000 VND theo cấu hình mẫu.
- Xe khu tạm nhận ô trước xe trong Queue.
- Invariant cuối demo: PASS.

Các integration test còn gọi `validateInvariants()` để kiểm tra Hash hai chiều,
slot enabled/tương thích loại xe, Holding xuất hiện đúng một lần trong Queue khu
tạm, và pending plan không trùng session/Queue đồng thời vẫn trỏ đến target
overflow hợp lệ cùng loại.

## Cách tái kiểm tra trước khi nộp

```bash
make -B all
make test
make demo
make sanitize
```

Nếu sửa source hoặc dữ liệu mẫu, phải chạy lại đủ bốn lệnh và cập nhật ngày/kết
quả trong tài liệu này.
