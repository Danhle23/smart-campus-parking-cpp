# 3. Kịch bản demo và bảo vệ (khoảng 7 phút)

> Bản phân công 6 người và lời nói đầy đủ nằm tại
> `KICH_BAN_THUYET_TRINH_7_PHUT.md` ở thư mục gốc. Tệp này giữ vai trò kịch bản
> kỹ thuật tổng quát và ngân hàng câu hỏi phản biện.

## 0:00–0:40 — Mở bài

> Đề tài mô phỏng bãi xe thông minh nhiều khu vực. Điểm trọng tâm không phải CRUD
> mà là kết hợp Hash Table, Queue, Graph, Dijkstra, Fuzzy Search, Priority Queue
> và Stack trong một trạng thái nhất quán.

Chỉ nhanh cấu trúc `include/ds`, `ParkingGraph` và `ParkingSystem`.

## 0:40–1:25 — BFS và Dijkstra

Chạy:

```bash
make demo
```

Ở bước 1, chỉ ra:

- BFS chọn `GATE_A -> LM01`, một cạnh nhưng dài 20 m.
- Dijkstra chọn `GATE_A -> J0 -> JL -> LM01`, ba cạnh nhưng chỉ 10 m.

Câu nói chốt:

> Vì graph có trọng số khoảng cách khác nhau, BFS chỉ tối ưu số cạnh; Dijkstra
> mới tối ưu tổng mét.

## 1:25–2:15 — Check-in và Hash Table

Demo đưa hai Giảng viên vào `LM01`, `LM02`; ba Sinh viên vào `SM01..SM03`.

Giải thích:

- Hash `card -> session` dùng để check-out trung bình `O(1)`.
- Hash `plate -> card` chống một biển số vào hai lần.
- Mỗi bucket là DSLK đơn; rehash ở load factor 0.75.

## 2:15–2:55 — Bãi đầy và Queue

- Sinh viên tiếp theo đỗ tràn vào `LM03`.
- Xe sau nữa không còn ô nên vào Queue vị trí 1.
- Queue tách theo vai trò và loại xe để giữ FIFO trong nhóm và tránh head-of-line
  blocking giữa ô tô/xe máy.

## 2:55–4:15 — Priority Queue + Stack

Giảng viên thứ ba đến khi khu GV kín:

1. Min-Heap chọn `SV-OVER` vì đây là sinh viên đỗ tràn sớm nhất reachable từ
   cổng của Giảng viên; ứng viên được chọn không bị pop khi mới lập plan.
2. Xe ở `LM03` bị `LM01`, `LM02` chắn.
3. Thứ tự push: `LM01`, `LM02`.
4. Thứ tự pop/hoàn nguyên: `LM02`, `LM01`.
5. Hệ thống chỉ lập plan trước; khi xác nhận mới chuyển sinh viên vào khu tạm và
   cấp `LM03` cho Giảng viên.
6. `entryTime` của Giảng viên bắt đầu tại lúc xác nhận/cấp ô, nên 10 giây chờ
   xác nhận trong demo không bị tính phí.

Câu nói chốt:

> Priority Queue chọn đúng xe cần di dời; Stack giải quyết thứ tự thao tác vật lý.
> Hai CTDL có hai vai trò khác nhau, không dùng Stack để chọn xe sớm nhất.

## 4:15–5:00 — Fuzzy Search

Camera đọc `59-X1.123.4S`; sau chuẩn hóa thành `59X11234S`.

- Levenshtein so sánh với các biển số đang trong bãi.
- Kết quả đúng `59X112345` có distance 1.
- Trả top-3; hòa thì sắp theo biển số để kết quả luôn lặp lại được.

Nêu Big O: `O(Nmn)` cho `N` biển số; DP dùng hai hàng nên bộ nhớ `O(min(m,n))`.

## 5:00–5:45 — Check-out và ưu tiên

- `SV001` ra sau 61 phút, phí 10.000 VND theo cấu hình hai giờ bắt đầu.
- Ô `SM01` trống: xe ở khu tạm được đưa về trước.
- `SV002` ra: xe trong Queue mới được nhận vào.
- Chỉ ra thời gian chờ ngoài cổng và chờ xác nhận relocation không bị tính phí.

## 5:45–6:25 — Invariant và Big O

Nêu sáu invariant quan trọng:

1. Hai Hash Table đồng bộ.
2. Session và ô occupied trỏ ngược đúng.
3. Ô occupied phải enabled và tương thích loại xe.
4. Xe `Holding` không chiếm slot và xuất hiện đúng một lần trong Queue khu tạm
   đúng loại.
5. Pending lecturer không có session/không nằm Queue; target vẫn là Sinh viên
   overflow hợp lệ cùng loại xe.
6. Active session = occupied + holding.

Big O nói ngắn:

- Hash trung bình `O(1)`.
- Queue/Stack `O(1)`.
- Heap push/pop `O(log n)`.
- Dijkstra lazy heap `O((V+E) log E)`.
- Merge Sort `O(n log n)`.

## 6:25–7:00 — Bằng chứng chất lượng

```bash
make test
make sanitize
```

Cho thấy `24 PASS, 0 FAIL`, không warning và sanitizer không báo lỗi.

Kết thúc:

> Project tự cài toàn bộ CTDL được chấm, dùng RAII để an toàn bộ nhớ và có test
> trực tiếp từng CTDL lẫn luồng tích hợp.

---

## Câu hỏi phản biện thường gặp

### 1. Vì sao không dùng `unordered_map`?

Đây là đồ án CTDL nên Hash Table separate chaining được tự cài. `std::vector` chỉ
giữ danh sách bucket; collision, linked chain, erase và rehash đều do project xử lý.

### 2. Vì sao cần hai Hash Table?

`card -> session` tối ưu check-out; `plate -> card` chống trùng biển số. Nếu chỉ
có bảng theo thẻ, kiểm tra trùng biển số phải quét toàn bộ `O(n)`.

### 3. Vì sao Hash Table có trường hợp xấu `O(n)`?

Nếu nhiều khóa rơi cùng bucket, tìm kiếm phải duyệt cả chain. Hàm FNV-1a và
rehash giảm xác suất này nhưng không xóa được trường hợp xấu lý thuyết.

### 4. Vì sao không dùng BFS cho đường ngắn nhất?

BFS đúng khi cạnh đồng trọng số. Bản đồ lưu mét khác nhau nên phải dùng Dijkstra.
Demo 20 m so với 10 m là phản ví dụ trực tiếp.

### 5. Tại sao Dijkstra không có decrease-key?

Khi tìm được distance tốt hơn, code push entry mới. Khi pop entry cũ, nếu distance
không còn khớp mảng thì bỏ. Heap có thể giữ `O(E)` entry, nên cận chặt dùng trong
báo cáo là `O((V+E) log E)`.

### 6. Tại sao Heap có bản ghi overflow cũ?

Xóa tùy ý giữa heap tốn thêm chỉ mục. Project dùng lazy deletion: khi xem đỉnh,
entry đã checkout/di dời mới bị loại vĩnh viễn. Entry hợp lệ nhưng unreachable
từ cổng hiện tại chỉ bị pop tạm rồi push lại; entry reachable được chọn chỉ được
`peek`. Vì vậy hủy plan hoặc xét từ một cổng khác không làm mất ứng viên.

### 7. Stack đóng vai trò gì?

Stack không chọn xe cần chuyển. Nó lưu các xe chắn bị đưa ra theo thứ tự gần cửa
đến sâu, sau đó pop để hoàn nguyên theo thứ tự ngược.

### 8. Nếu di dời thất bại giữa chừng?

`confirmRelocation` thay đổi theo giao dịch: nếu cấp ô hoặc chèn Hash thất bại,
code xóa session mới, trả lại ô và khôi phục session sinh viên.

### 9. Vì sao không tự đổi `S` thành `5` trước khi so khớp?

Nếu đổi cứng, ta che mất chính lỗi OCR cần đo. Levenshtein chuẩn xem đó là một
phép thay thế; dễ chứng minh và không đưa giả định camera không có trong đề.

### 10. Có memory leak không?

Node sở hữu dùng `unique_ptr`; project chạy sạch dưới AddressSanitizer và
UndefinedBehaviorSanitizer. Test riêng còn ép collision, erase chain và rehash.

### 11. Tại sao code dùng class/template khác bài giảng thủ tục?

Logic CTDL vẫn đúng bài giảng nhưng được đóng gói bằng C++17 để quản lý ownership,
tránh copy nhầm và rò rỉ. Các thao tác `insert/find/erase`, `enqueue/dequeue`,
`push/pop`, `siftUp/siftDown` vẫn được tự hiện thực và có test độc lập.

### 12. Giới hạn hiện tại là gì?

Chưa có database/camera thật và chưa lưu snapshot qua lần khởi động. Đây là mô
phỏng CTDL; dữ liệu bắt buộc của đề (bản đồ và camera log) đã đọc từ file.

### 13. Nếu một ô Giảng viên khác trống khi plan đang chờ thì sao?

Yêu cầu trong pending plan đến trước nên được cấp ô đó trước Queue Giảng viên đến
sau; plan được hủy mà không di dời Sinh viên. `entryTime` bắt đầu tại thời điểm
cấp ô, còn ứng viên overflow vẫn ở Heap để phục vụ lần điều phối kế tiếp.

### 14. Nếu Heap top nằm ở component mà cổng Giảng viên không đi tới được?

`peekValidOverflow(type, gateId)` kiểm tra đường đi theo đúng thứ tự Heap. Entry
hợp lệ nhưng unreachable được đưa vào buffer tạm; hệ thống chọn entry reachable
đầu tiên rồi push lại toàn bộ buffer. Do đó vẫn giữ ưu tiên sớm nhất trong phạm
vi có thể tiếp cận và không làm mất xe của component khác. Trường hợp xấu kiểm
tra `o` entry có chi phí `O(o(V+E) log E + o log o)`; có thể tối ưu bằng cache
reachability/Dijkstra theo cổng nếu bản đồ rất lớn.
