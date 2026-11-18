# 4. Đối chiếu với bài giảng môn học

Đã đọc 5 deck trong `Bài giảng lý thuyết-20260716.zip`, tổng cộng 380 slide.

## Nội dung được dùng trực tiếp

| Bài giảng | Slide liên quan | Phần áp dụng trong project |
|---|---:|---|
| `Danh-sach-lien-ket.pptx` | 15–73 | Node, DSLK đơn, tìm/xóa và giải phóng |
| `Danh-sach-lien-ket.pptx` | 118–131 | Merge Sort trên danh sách/dữ liệu động |
| `Danh-sach-lien-ket.pptx` | 153–172 | Stack, LIFO, push/pop `O(1)` |
| `Danh-sach-lien-ket.pptx` | 173–191 | Queue, FIFO, front/rear, linked queue `O(1)` |
| `Heap-sort_Min.pptx` | 2–17 | Min-Heap và quan hệ cha/con |
| `Heap-sort_Shell-sort.pptx` | 23–27 | Sift/Shift, heap và `O(n log n)` |
| `Merge_Sort_Radix_Sort.pptx` | 1–18 | Merge Sort `O(n log n)`, tính ổn định và bộ nhớ phụ |
| `Tim-kiem-sap-xep.ppt` | 3–20 | Tìm tuyến tính/nhị phân và cách phân tích Big O |
| `Tim-kiem-sap-xep.ppt` | 78–91 | Tư duy phân hoạch và đánh giá thuật toán |

## Nội dung đề yêu cầu nhưng ZIP chưa dạy

- Hash Table và xử lý collision/rehash.
- Biểu diễn Graph, BFS và Dijkstra.
- Priority Queue như một ADT.
- Fuzzy Search, String Matching và Levenshtein.
- Parser CSV, kiểm thử, chia module và RAII.

Vì các phần này xuất hiện trực tiếp trong đề 11, project tự hiện thực và giải thích
đầy đủ trong báo cáo dù không có trong ZIP bài giảng đã cung cấp.

## Các điểm giả mã cần hiện thực hóa cẩn thận

Một số đoạn trên slide mang tính giả mã/rút gọn. Khi chuyển sang C++ chạy thật,
project đã làm rõ các điểm sau:

1. Stack linked-list cần cập nhật `top` và ownership trước khi trả kết quả.
2. Điều kiện Queue rỗng phải so sánh từng biến rõ ràng trong cú pháp C++.
3. Linked Queue phải nối node mới từ `rear` để enqueue `O(1)`.
4. `DeQueue` phần tử cuối phải reset cả `front` và `rear`.
5. Heap phải thống nhất hoàn toàn công thức chỉ số 0-based hoặc 1-based.

Project sửa bằng:

- Queue giữ `unique_ptr head` và raw non-owning `tail`; nối qua `tail->next`.
- Khi dequeue phần tử cuối, `tail` được đặt `nullptr`.
- Stack pop cập nhật top trước khi node bị hủy tự động.
- Heap 0-based dùng `left=2*i+1`, `right=2*i+2`, `parent=(i-1)/2`.
- Toàn bộ node sở hữu được quản lý bằng RAII, không trộn `malloc/free` với
  `std::string`.

## Điểm khác phong cách bài giảng cần giải thích khi bảo vệ

Bài giảng chủ yếu dùng `typedef struct`, hàm tự do, mảng và con trỏ thô. Project
dùng class/template/namespace/RAII nhưng **không thay thế thuật toán bằng STL**.

Lý do:

- Template giúp một Binary Heap dùng được cả Dijkstra và overflow priority.
- Class giữ invariant và ngăn UI sửa trực tiếp cấu trúc bên trong.
- `unique_ptr` vẫn là danh sách liên kết tự cài nhưng có ownership rõ, tránh leak.
- Test trực tiếp từng ADT chứng minh code không chỉ gọi thư viện có sẵn.

Nếu giảng viên yêu cầu phong cách hoàn toàn thủ tục, có thể bổ sung phụ lục ánh xạ:

| Tên trong bài giảng | Tên trong project |
|---|---|
| `Init` | constructor mặc định |
| `Empty` | `empty()` |
| `EnQueue` | `enqueue()` |
| `DeQueue` | `dequeue()` |
| `Push`, `Pop`, `Top` | `push()`, `pop()`, `top()` |
| `Shift` | `siftUp()`, `siftDown()` |
| `Search` | `find()` |
| `Remove` | `erase()` |
