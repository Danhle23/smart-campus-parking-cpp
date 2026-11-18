# 2. Ma trận yêu cầu — mã nguồn — kiểm thử

Ma trận này dùng để chứng minh mỗi yêu cầu của đề 11 đều có hiện thực và bằng
chứng chạy, tránh tình trạng chức năng chỉ xuất hiện trong báo cáo.

| ID | Yêu cầu/tiêu chí | CTDL/thuật toán | Vị trí mã chính | Bằng chứng test/demo |
|---|---|---|---|---|
| R01 | Đọc bản đồ từ file | CSV parser, transactional load | `DataLoader.cpp` | `DataLoader: lỗi file không làm hỏng graph cũ` |
| R02 | Đọc log camera nhiễu | CSV parser + chuẩn hóa | `DataLoader.cpp`, `Types.cpp` | `DataLoader: CSV có quote và camera log` |
| R03 | Ánh xạ mã thẻ → phiên gửi | Hash Table chaining | `HashTable.hpp`, `ParkingSystem.cpp` | `HashTable: collision, erase và rehash` |
| R04 | Chống trùng biển số | Hash Table `plate -> card` | `ParkingSystem::checkIn` | `Check-in/check-out và chống trùng` |
| R05 | Check-in và ghi thời gian | Hash + Graph | `ParkingSystem::checkIn` | Demo bước 2 |
| R06 | Check-out, giải phóng ô | Hash erase + cập nhật Graph | `ParkingSystem::checkOut` | `Check-in/check-out và chống trùng` |
| R07 | Tính phí tự động | Làm tròn số giờ bắt đầu | `FeeCalculator.cpp` | `FeeCalculator: mốc 59, 60, 61 phút` |
| R08 | Bãi đầy đưa xe vào Queue | Linked Queue FIFO | `LinkedQueue.hpp`, `enqueueRequest` | `Queue: bãi đầy, FIFO tự động nhận ô` |
| R09 | Không để ô tô chặn xe máy | Queue tách theo loại | `ParkingSystem.hpp` | `Queue tách theo loại xe` |
| R10 | Mô hình hóa bãi xe | Graph adjacency list | `ParkingGraph.hpp/.cpp` | Load 17 node, 34 cung có hướng từ dữ liệu mẫu |
| R11 | Tìm ô gần nhất theo mét | Dijkstra + Binary Min-Heap | `ParkingGraph::runDijkstra` | `Graph: Dijkstra đúng mét, BFS đúng số cạnh` |
| R12 | Minh họa BFS | BFS + Linked Queue | `bfsToNearestSlot` | Demo: BFS 20 m, Dijkstra 10 m |
| R13 | Tie-break có thể lặp lại | So sánh distance rồi ID | `ParkingGraph.cpp` | `Graph: tie-break theo mã slot` |
| R14 | Fuzzy Search top-3 | Levenshtein + Merge Sort | `FuzzySearch.cpp` | `Fuzzy Search top-3 active vehicle` |
| R15 | Chuẩn hóa biển số | Uppercase, bỏ ký tự phân cách | `Types.cpp` | `Chuẩn hóa chuỗi và Levenshtein` |
| R16 | Sinh viên tràn khu GV | Zone policy + overflow flag | `ParkingSystem::checkIn` | `Relocation: Heap chọn xe sớm nhất` |
| R17 | Chọn SV đỗ tràn sớm nhất reachable từ đúng cổng | Min-Heap; tạm pop/reinsert entry ở component khác; lazy deletion | `peekValidOverflow(type, gateId)` | `GATE_B` chọn/confirm `OVER_B`; yêu cầu sau từ `GATE_A` vẫn chọn được `OVER_A` |
| R18 | Đưa xe chắn ra/hoàn nguyên | Linked Stack LIFO | `createRelocationPlan` | Push `LM01, LM02`; pop `LM02, LM01` |
| R19 | Xác nhận trước khi di dời, không tính phí lúc chờ | Giao dịch hai pha; `entryTime = confirmedAt` | `confirmRelocation` | GV checkout sau 60 giây từ confirm chỉ chịu mức phí tối thiểu |
| R20 | Ưu tiên xe khu tạm | Linked Queue holding | `rehomeHolding` | `Khu tạm ưu tiên trước Queue` |
| R21 | Báo cáo lịch sử | Stable Merge Sort | `historySortedByTime/Fee` | `History dùng MergeSort` |
| R22 | Tính nhất quán toàn hệ thống | Invariant cho Hash/slot/Queue/Holding/pending/loại xe | `validateInvariants` | Tất cả integration test + demo |
| R23 | Không memory leak/UB | RAII + sanitizer | Toàn project | ASan/UBSan 24/24 PASS |
| R24 | Chịu tải bảng băm | Rehash load factor 0.75 | `HashTable::rehash` | Stress 10.000 khóa |
| R25 | Không để xe đến sau vượt pending relocation | Ưu tiên theo thời điểm đến + Heap không mất candidate | `hasLecturerWaiting`, `processReleasedCapacity` | `Relocation pending ưu tiên và Heap giữ ứng viên khi hủy plan` |

## Danh sách 24 test hiện tại

1. HashTable: collision, erase và rehash.
2. LinkedQueue: FIFO và vòng đời rỗng → có dữ liệu → rỗng.
3. LinkedStack: LIFO.
4. BinaryMinHeap: thứ tự ưu tiên.
5. MergeSort: ổn định khi khóa bằng nhau.
6. Chuẩn hóa chuỗi và Levenshtein.
7. FeeCalculator: 59/60/61 phút và thời gian âm.
8. CSV có quote và camera log.
9. Dijkstra theo mét so với BFS theo số cạnh.
10. Tie-break theo mã slot và từ chối cạnh âm.
11. Transactional load khi file cạnh lỗi.
12. Check-in/check-out, trùng thẻ và trùng biển số.
13. Ba xe chờ được phục vụ đúng FIFO và đúng queue position.
14. Heap chọn giữa hai overflow, Stack tạo đúng hai thứ tự; với hai cổng ở hai
    component rời nhau, bỏ qua xe đến sớm hơn nhưng unreachable, chọn target
    reachable, rồi assert yêu cầu từ component kia vẫn chọn được entry đã reinsert.
15. Xe mục tiêu checkout trước confirm: plan hủy nhưng không mất yêu cầu GV.
16. Pending relocation ưu tiên trước xe đến sau; hủy plan không làm mất ứng viên
    overflow sớm nhất trong Heap.
17. Ô khu GV ưu tiên Queue Giảng viên trước Queue Sinh viên.
18. Xe khu tạm được ưu tiên trước Queue.
19. Fuzzy Search top-3 đầy đủ, tie-break và giới hạn kết quả.
20. Ô tô end-to-end: overflow, relocation, rehome và phí 61 phút.
21. Queue ô tô không chặn xe máy.
22. History Merge Sort và invariant sau chuỗi thao tác.
23. Input sai, ngày 31/02 và graph không tới được.
24. Stress 10.000 khóa Hash Table.

## Checklist trước khi nộp

- [x] CTDL cốt lõi tự cài đặt.
- [x] Code chia `.hpp/.cpp`, không dùng `bits/stdc++.h` hoặc VLA.
- [x] Có file dữ liệu mẫu và kiểm tra lỗi dòng.
- [x] Có demo cố định, không phụ thuộc nhập tay.
- [x] Có test cho happy path, edge case và integration.
- [x] Build warning sạch.
- [x] ASan/UBSan sạch.
- [x] Có phân tích Big O liên hệ trực tiếp với code.
- [x] Có kịch bản bảo vệ và câu hỏi phản biện.
- [x] Điền học phần, lớp, giảng viên và danh sách 8 thành viên.
- [x] Tạo báo cáo Typst/PDF và kịch bản thuyết trình nhóm.
- [x] Bổ sung họ tên và MSSV đầy đủ của Tống Duy Quyết, Trần Ngọc Quang Minh,
      Lê Ngọc Hiếu.
- [x] Ghi rõ phí trong `fees.cfg` là cấu hình mẫu có thể thay đổi.
- [x] Ghi rõ phạm vi dùng thư viện chuẩn và các ADT do nhóm tự hiện thực.
- [x] Render, kiểm tra trực quan báo cáo và đóng gói source tái lập.
