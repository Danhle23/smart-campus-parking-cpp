#import "@preview/codly:1.3.0": *
#import "@preview/codly-languages:0.1.1": *
#import "config.typ": *

#set document(
  title: "Báo cáo Đề tài 11 - Hệ thống bãi giữ xe thông minh và dẫn đường",
  author: "Nhóm Lê Thanh Danh - 24207086",
  keywords: (
    "Cấu trúc dữ liệu và giải thuật",
    "Hash Table",
    "Queue",
    "Graph",
    "Dijkstra",
    "Min-Heap",
    "Stack",
    "Levenshtein",
    "Merge Sort",
    "C++17",
  ),
)

#set page(
  paper: "a4",
  margin: (top: 2cm, bottom: 2cm, left: 2.5cm, right: 2cm),
)

#set text(
  font: "Times New Roman",
  size: body_size,
  lang: "vi",
  fill: ink,
  top-edge: 0.8em,
  bottom-edge: -0.2em,
)

#set par(
  justify: true,
  leading: 0.68em,
  spacing: 0.45em,
  first-line-indent: (amount: 1cm, all: true),
  linebreaks: "optimized",
  justification-limits: (
    spacing: (min: 90%, max: 118%),
    tracking: (min: -0.003em, max: 0.008em),
  ),
)

#set list(indent: 1.05cm, body-indent: 0.35cm, spacing: 0.32em)
#set enum(indent: 1.05cm, body-indent: 0.35cm, spacing: 0.32em)

#set raw(tab-size: 2, theme: auto)
#show raw.where(block: false): set text(
  font: "DejaVu Sans Mono",
  size: 10.5pt,
  fill: navy,
)
#show raw.where(block: true): set text(
  font: "DejaVu Sans Mono",
  size: code_size,
  fill: ink,
)

#show: codly-init.with()
#codly(
  languages: codly-languages,
  default-color: gray_500,
  number-format: none,
  display-name: false,
  display-icon: false,
  smart-indent: true,
  breakable: false,
  fill: gray_100,
  zebra-fill: none,
  stroke: 0.5pt + gray_300,
  radius: 1.5pt,
  inset: (x: 8pt, y: 4pt),
)

#show figure.where(kind: table): set block(breakable: true)
#show figure.where(kind: "code"): set block(breakable: false)
#show link: set text(fill: navy)
#show ref: set text(fill: navy)

#set figure(numbering: "1")
#set figure.caption(separator: [. ])
#show figure.caption: c => [
  #set par(first-line-indent: 0pt, justify: false)
  #align(center)[
    #if c.numbering == none [
      #text(size: 10.5pt, weight: "bold")[#c.body]
    ] else [
      #text(size: 10.5pt)[
        #text(weight: "bold")[#c.supplement #c.counter.display(c.numbering).]
        #c.body
      ]
    ]
  ]
  #v(0.15cm)
]

#set heading(numbering: "1.1")
#show heading.where(level: 1): it => {
  pagebreak(weak: true)
  block(
    width: 100%,
    above: 0cm,
    below: 0.42cm,
    breakable: false,
  )[
    #set par(first-line-indent: 0pt, justify: false)
    #align(center)[
      #text(size: 16pt, weight: "bold", fill: navy)[
        #if it.numbering == none [
          #upper(it.body)
        ] else [
          CHƯƠNG #counter(heading).display("1")
          #v(4pt)
          #upper(it.body)
        ]
      ]
      #v(4pt)
      #line(length: 3.2cm, stroke: 0.8pt + navy)
    ]
  ]
}

#show heading.where(level: 2): it => block(
  width: 100%,
  above: 0.38cm,
  below: 0.16cm,
  breakable: false,
)[
  #set par(first-line-indent: 0pt, justify: false)
  #text(size: 13.5pt, weight: "bold", fill: ink)[
    #if it.numbering == none [
      #it.body
    ] else [
      #counter(heading).display(it.numbering) #h(0.14cm) #it.body
    ]
  ]
]

#show heading.where(level: 3): it => block(
  width: 100%,
  above: 0.25cm,
  below: 0.1cm,
  breakable: false,
)[
  #set par(first-line-indent: 0pt, justify: false)
  #text(size: 13pt, weight: "bold", style: "italic", fill: ink)[
    #if it.numbering == none [
      #it.body
    ] else [
      #counter(heading).display(it.numbering) #h(0.14cm) #it.body
    ]
  ]
]

// ===========================================================================
// BÌA NGOÀI
// ===========================================================================

#set page(header: none, footer: none)

#box(
  width: 100%,
  height: 24.7cm,
  stroke: 0.8pt + ink,
  inset: (x: 0.9cm, y: 0.8cm),
)[
  #set par(first-line-indent: 0pt, justify: false)
  #align(center)[
    #text(size: 13.5pt, weight: "bold")[#meta.university_parent]
    #v(2pt)
    #text(size: 14.5pt, weight: "bold")[#meta.university]
    #v(2pt)
    #text(size: 13pt, weight: "bold")[#meta.faculty]
    #v(0.18cm)
    #image("assets/hcmus-logo-official.png", width: 2.8cm)
    #v(0.12cm)
    #line(length: 4.2cm, stroke: 0.65pt + ink)

    #v(0.72cm)
    #text(size: 15pt, weight: "bold")[BÁO CÁO ĐỒ ÁN MÔN HỌC]
    #v(0.22cm)
    #text(size: 13.5pt, weight: "bold", fill: navy)[#meta.course]

    #v(1.3cm)
    #text(size: 14pt, weight: "bold")[#meta.project_no]
    #v(0.32cm)
    #text(size: 19pt, weight: "bold", fill: navy)[#meta.project_name]
    #v(0.28cm)
    #text(size: 13pt, style: "italic")[#meta.project_name_en]
    #v(0.22cm)
    #line(length: 7.5cm, stroke: 0.7pt + navy)

    #v(1.25cm)
    #grid(
      columns: (4.1cm, 8.8cm),
      column-gutter: 0.35cm,
      row-gutter: 0.18cm,
      align: (right, left),
      text(weight: "bold")[Giảng viên:],
      [#meta.lecturer],
      text(weight: "bold")[Lớp học phần:],
      [#meta.class_name],
      text(weight: "bold")[Ngôn ngữ:],
      [#meta.language],
      text(weight: "bold")[Nhóm trưởng:],
      [#meta.leader — #meta.leader_id],
    )

    #v(1fr)
    #text(size: 12pt)[Thành phố Hồ Chí Minh, tháng #meta.report_month năm #meta.report_year]
  ]
]

// ===========================================================================
// BÌA TRONG VÀ THÔNG TIN NHÓM
// ===========================================================================

#pagebreak()

#align(center)[
  #set par(first-line-indent: 0pt, justify: false)
  #text(size: 13pt, weight: "bold")[#meta.university_parent]
  #linebreak()
  #text(size: 14pt, weight: "bold")[#meta.university]
  #linebreak()
  #text(size: 12.5pt, weight: "bold")[#meta.faculty]
  #v(0.2cm)
  #image("assets/hcmus-logo-official.png", width: 2.4cm)
  #v(0.15cm)
  #line(length: 4cm, stroke: 0.65pt + ink)
  #v(0.45cm)
  #text(size: 15pt, weight: "bold")[BÁO CÁO ĐỒ ÁN MÔN HỌC]
  #v(0.25cm)
  #text(size: 18pt, weight: "bold", fill: navy)[#meta.project_name]
  #v(0.18cm)
  #text(size: 12.5pt, style: "italic")[#meta.project_name_en]
]

#v(0.55cm)

#grid(
  columns: (3.7cm, 1fr),
  column-gutter: 0.35cm,
  row-gutter: 0.15cm,
  [#info_label[Học phần:]], [#meta.course],
  [#info_label[Lớp:]], [#meta.class_name],
  [#info_label[Giảng viên:]], [#meta.lecturer],
  [#info_label[Đề tài:]], [#meta.project_no],
)

#v(0.35cm)

#figure(
  readable_table[
    #table(
      columns: (0.8cm, 2.7cm, 1fr, 2.6cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (center, center, left, left),
      fill: table_fill,
      table.header(
        head([STT]), head([MSSV]), head([Họ và tên]), head([Vai trò]),
      ),
      cell([1]), cell([24207086]), cell([Lê Thanh Danh]), cell([Nhóm trưởng]),
      cell([2]), cell([24207108]), cell([Nguyễn Hồng Tiến]), cell([Thành viên]),
      cell([3]), cell([24207033]), cell([Phạm Minh Phát]), cell([Thành viên]),
      cell([4]), cell([24207067]), cell([Lê Quốc Bảo]), cell([Thành viên]),
      cell([5]), cell([22207056]), cell([Lương Hải Long]), cell([Thành viên]),
      cell([6]), cell([24207038]), cell([Tống Duy Quyết]), cell([Thành viên]),
      cell([7]), cell([24207025]), cell([Trần Ngọc Quang Minh]), cell([Thành viên]),
      cell([8]), cell([20207033]), cell([Lê Ngọc Hiếu]), cell([Thành viên]),
    )
  ],
  kind: "front-table",
  supplement: none,
  numbering: none,
  caption: [Danh sách nhóm thực hiện],
) <tbl:members>

#v(1fr)
#align(center)[Thành phố Hồ Chí Minh, tháng #meta.report_month năm #meta.report_year]

// ===========================================================================
// FRONT MATTER
// ===========================================================================

#pagebreak()
#set page(footer: front_footer)
#counter(page).update(1)

#front_title[Lời mở đầu]

Đồ án môn học là cơ hội để nhóm chuyển các khái niệm Cấu trúc dữ liệu và Giải
thuật thành một hệ thống có trạng thái, quy tắc nghiệp vụ và bằng chứng chạy
thật. Nhóm lựa chọn mô hình bãi giữ xe trong khuôn viên trường vì bài toán này
đòi hỏi đồng thời tra cứu nhanh, xử lý hàng chờ, tối ưu đường đi, xếp hạng ưu
tiên và bảo toàn tính nhất quán sau nhiều thao tác liên tiếp.

Nhóm xin cảm ơn ThS. Trần Tuấn Kiệt đã hướng dẫn học phần và cung cấp nền tảng
lý thuyết cho quá trình phân tích, thiết kế và kiểm thử. Báo cáo tập trung giải
thích lý do lựa chọn cấu trúc dữ liệu, liên hệ trực tiếp với mã nguồn và trình
bày giới hạn của bằng chứng thay vì chỉ mô tả chức năng.

Do phạm vi học phần và thời gian thực hiện có giới hạn, hệ thống hiện là mô
phỏng console chạy trên dữ liệu tệp; chưa thay thế một hệ thống kiểm soát ra vào
thực tế. Nhóm mong nhận được góp ý để tiếp tục hoàn thiện chất lượng kỹ thuật và
cách trình bày.

#v(1.2cm)
#align(right)[
  #set par(first-line-indent: 0pt, justify: false)
  #text(weight: "bold")[Đại diện nhóm thực hiện]
  #v(1.5cm)
  #text(weight: "bold")[Lê Thanh Danh]
]

#pagebreak()
#front_title[Cam kết]

Nhóm cam kết báo cáo này phản ánh đúng phạm vi đồ án đã thực hiện. Các kết quả
biên dịch, kiểm thử, demo và kiểm tra bộ nhớ được ghi theo những lần chạy đã xác
nhận; nhóm không diễn giải chúng thành bảo đảm tuyệt đối cho mọi đầu vào.

Nhóm cam kết:

- không sao chép nội dung của nhóm khác và không làm sai lệch kết quả thực thi;
- ghi nguồn đối với lý thuyết, thuật toán và tài liệu đã tham khảo;
- phân biệt rõ phần tự hiện thực với tiện ích hạ tầng của C++17;
- cung cấp mã nguồn, dữ liệu mẫu và lệnh chạy để giảng viên có thể tái lập;
- chịu trách nhiệm về tính chính xác của thông tin thành viên trước khi nộp.

#v(1.4cm)
#align(right)[
  #set par(first-line-indent: 0pt, justify: false)
  Thành phố Hồ Chí Minh, tháng #meta.report_month năm #meta.report_year
  #v(0.25cm)
  #text(weight: "bold")[Đại diện nhóm (ký và ghi rõ họ tên)]
  #v(1.8cm)
  #text(weight: "bold")[Lê Thanh Danh]
]

#pagebreak()
#front_title[Tóm tắt]

Đồ án xây dựng một hệ thống mô phỏng quản lý bãi xe máy và ô tô trong khuôn viên
trường đại học. Hệ thống xử lý dữ liệu xe vào/ra, tính phí theo cấu hình, phân
khu Sinh viên—Giảng viên, dẫn đường đến ô phù hợp, điều phối khi bãi đầy và tìm
biển số gần đúng từ log camera bị nhiễu.

Trọng tâm kỹ thuật là phối hợp các cấu trúc dữ liệu tự hiện thực: Hash Table
separate chaining cho hai chỉ mục mã thẻ và biển số; Linked Queue cho xe chờ;
Linked Stack cho thứ tự di dời trong dãy hẹp; Binary Min-Heap cho Dijkstra và
ưu tiên xe đỗ tràn; Graph adjacency list cho bản đồ; Merge Sort cho kết quả cần
thứ tự ổn định. Levenshtein Distance dùng hai hàng bộ nhớ để xếp hạng độ gần của
biển số. Các thao tác thay đổi trạng thái đi qua một lớp điều phối và được kiểm
tra bằng invariant.

Phiên bản xác nhận ngày #meta.evidence_date biên dịch không có cảnh báo với bộ
cờ nghiêm ngặt, đạt 24/24 nhóm kiểm thử, kết thúc demo bằng
`Invariant: PASS`; ASan và UBSan không phát hiện lỗi trong các ca đã thực thi.
Ma trận đặc tả gồm 25 yêu cầu nguyên tử R01–R25, trong khi chương trình test gộp
các yêu cầu liên quan thành 24 nhóm executable; vì vậy hai con số không biểu
diễn cùng một đơn vị đếm.

#v(0.35cm)
#set par(first-line-indent: 0pt, justify: false)
#text(weight: "bold")[Từ khóa:] Hash Table; Queue; Graph; Dijkstra; Binary
Min-Heap; Stack; Levenshtein; Merge Sort; C++17.

#pagebreak()
#front_title[Abstract]

#block[
  #set text(lang: "en")
  #set par(first-line-indent: (amount: 1cm, all: true), justify: true)
  This project develops a console-based simulation for motorcycle and car
  parking on a university campus. The system processes check-in and check-out
  data, calculates configurable fees, separates student and lecturer zones,
  finds a suitable parking slot, handles full-capacity situations, and performs
  fuzzy licence-plate retrieval from noisy camera logs.

  The implementation coordinates custom data structures rather than treating
  them as isolated demonstrations. Separate-chaining hash tables maintain card
  and plate indices; linked queues preserve waiting order; a linked stack
  restores blocker vehicles in a narrow lane; binary min-heaps support Dijkstra
  routing and overflow priority; an adjacency-list graph models the parking
  map; and Merge Sort provides deterministic ordering. A two-row Levenshtein
  dynamic programme ranks plate candidates, while system-wide invariants detect
  inconsistent state transitions.

  The build verified on #meta.evidence_date completed without warnings under
  the configured strict flags, all 24 executable test groups passed, the
  deterministic demo ended with `Invariant: PASS`, and ASan/UBSan reported no
  issue in the executed cases. These results are reproducible evidence for the
  tested scope, not a proof that every possible input is defect-free.

  #v(0.35cm)
  #set par(first-line-indent: 0pt, justify: false)
  #text(weight: "bold")[Keywords:] data structures; shortest path; hash table;
  queue; heap; stack; edit distance; C++17.
]

#pagebreak()
#front_title[Mục lục]
#block[
  #set text(size: 10.4pt)
  #set par(first-line-indent: 0pt, leading: 0.42em, spacing: 0.12em)
  #outline(title: none, depth: 2, indent: auto)
]

#pagebreak()
#front_title[Danh mục chữ viết tắt]

#figure(
  readable_table[
    #table(
      columns: (2.8cm, 5.0cm, 7.2cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (center, left, left),
      fill: table_fill,
      table.header(head([Ký hiệu]), head([Thuật ngữ]), head([Ý nghĩa trong đồ án])),
      cell([ADT]), cell([Abstract Data Type]), cell([Kiểu dữ liệu trừu tượng]),
      cell([ASan]), cell([AddressSanitizer]), cell([Phát hiện lỗi truy cập bộ nhớ trong ca chạy]),
      cell([BFS]), cell([Breadth-First Search]), cell([Tìm đường ít cạnh trên đồ thị không trọng số]),
      cell([CSV]), cell([Comma-Separated Values]), cell([Định dạng dữ liệu bản đồ và camera]),
      cell([DP]), cell([Dynamic Programming]), cell([Quy hoạch động cho edit distance]),
      cell([FIFO]), cell([First In, First Out]), cell([Quy tắc của hàng đợi]),
      cell([LIFO]), cell([Last In, First Out]), cell([Quy tắc của ngăn xếp]),
      cell([STL]), cell([Standard Template Library]), cell([Tiện ích thư viện chuẩn C++]),
      cell([UBSan]), cell([UndefinedBehaviorSanitizer]), cell([Phát hiện hành vi không xác định]),
    )
  ],
  kind: "front-table",
  supplement: none,
  numbering: none,
  caption: [Các chữ viết tắt sử dụng trong báo cáo],
) <tbl:abbreviations>

#v(0.7cm)
#front_title[Danh mục hình]
#block[
  #set text(size: 9.8pt)
  #set par(first-line-indent: 0pt, leading: 0.38em, spacing: 0.1em)
  #outline(title: none, target: figure.where(kind: "diagram"))
]

#v(0.8cm)
#front_title[Danh mục bảng]
#block[
  #set text(size: 9.8pt)
  #set par(first-line-indent: 0pt, leading: 0.38em, spacing: 0.1em)
  #outline(title: none, target: figure.where(kind: table))
]

// ===========================================================================
// PHẦN NỘI DUNG — ĐÁNH SỐ Ả RẬP
// ===========================================================================

#pagebreak()
#set page(footer: body_footer)
#counter(page).update(1)
#counter(heading).update(0)

= Giới thiệu bài toán

== Bối cảnh

Bãi giữ xe trong khuôn viên trường có ba đặc trưng làm bài toán phức tạp hơn một
ứng dụng CRUD thông thường. Thứ nhất, xe vào và ra liên tục nên trạng thái ô đỗ,
mã thẻ và biển số phải được cập nhật nhất quán. Thứ hai, bản đồ lối đi có khoảng
cách khác nhau; tuyến có ít cạnh hơn chưa chắc ngắn hơn theo mét. Thứ ba, bãi có
thể quá tải, camera có thể đọc sai biển số và sinh viên có thể phải đỗ tạm trong
khu Giảng viên.

Đồ án chọn mô hình dữ liệu tệp và giao diện console để tập trung vào Cấu trúc dữ
liệu và Giải thuật. Camera, cổng tự động và cơ sở dữ liệu được mô phỏng bằng
log, lệnh và cấu trúc trong bộ nhớ; báo cáo không tuyên bố đây là hệ thống sẵn
sàng triển khai thực tế.

== Mục tiêu

- Quản lý check-in, check-out, thời gian và phí gửi xe.
- Tra cứu trung bình $O(1)$ theo mã thẻ và phát hiện biển số trùng.
- Tìm ô trống phù hợp có tổng khoảng cách nhỏ nhất.
- Giữ FIFO khi bãi đầy và tránh head-of-line blocking giữa xe máy, ô tô.
- Tìm ba biển số gần nhất khi log camera có lỗi ký tự.
- Điều phối sinh viên đỗ tràn và xử lý dãy hẹp có xe chắn.
- Chứng minh phạm vi đã kiểm tra bằng test, invariant và sanitizer.

== Phạm vi đầu vào và đầu ra

#figure(
  readable_table[
    #table(
      columns: (3.2cm, 5.8cm, 6.0cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (left, left, left),
      fill: table_fill,
      table.header(head([Nhóm]), head([Đầu vào]), head([Đầu ra])),
      cell([Bản đồ]), cell([`nodes.csv`, `edges.csv`]), cell([Graph, tuyến đường và ô đích]),
      cell([Nghiệp vụ]), cell([Yêu cầu check-in/check-out]), cell([Phiên gửi, phí, Queue hoặc relocation plan]),
      cell([Camera]), cell([`camera_logs.csv`, chuỗi truy vấn]), cell([Top-3 biển số gần nhất]),
      cell([Kiểm thử]), cell([Mã nguồn và dữ liệu mẫu]), cell([24 nhóm test, demo và sanitizer]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Phạm vi dữ liệu vào và đầu ra của hệ thống],
) <tbl:scope>

== Tiêu chí thành công

Một chức năng chỉ được xem là hoàn thành khi có đủ ba lớp bằng chứng: mã nguồn
thực hiện đúng vai trò, test hoặc demo có thể chạy lại, và trạng thái cuối không
vi phạm invariant. Cách đánh giá này giúp tránh trường hợp một thuật toán chạy
đúng riêng lẻ nhưng làm sai đồng bộ giữa ô đỗ, mã thẻ và biển số.

= Yêu cầu và cơ sở lý thuyết

== Yêu cầu cơ bản và nâng cao

#figure(
  readable_table[
    #table(
      columns: (2.5cm, 6.4cm, 6.1cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (left, left, left),
      fill: table_fill,
      table.header(head([Mức]), head([Chức năng]), head([CTDL/giải thuật chính])),
      cell([Cơ bản]),
      cell([Đọc tệp; check-in/out; tính phí; tra cứu; bãi đầy; dẫn đường]),
      cell([Hash Table, Linked Queue, Graph, BFS, Dijkstra]),
      cell([Nâng cao]),
      cell([Camera nhiễu; đỗ tràn; dãy hẹp; rollback; invariant; lịch sử]),
      cell([Levenshtein, Binary Min-Heap, Linked Stack, Merge Sort]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Một chương trình bao gồm cả yêu cầu cơ bản và nâng cao],
) <tbl:levels>

== Liên hệ bài giảng và tài liệu

Các bài giảng của học phần bao quát danh sách liên kết, Stack, Queue, Min-Heap,
Merge Sort và phân tích độ phức tạp #link(<ref-lecture-1>)[[1]]–#link(<ref-lecture-3>)[[3]].
Nhóm dùng các nguyên lý này để tự cài `LinkedQueue`, `LinkedStack`,
`BinaryMinHeap` và `MergeSort`. Hash Table, Graph, Dijkstra và Levenshtein là
thành phần trực tiếp của Đề tài 11 nên được phân tích bổ sung. Cách đánh giá
độ phức tạp của Hash Table và thuật toán đồ thị đối chiếu với tài liệu giải
thuật chuẩn #link(<ref-clrs>)[[4]].

#note_block(
  [Ranh giới sử dụng STL],
  [Các CTDL được chấm đều do nhóm tự hiện thực. `std::string`, `std::vector`,
   `std::unique_ptr`, stream và `chrono` chỉ đóng vai trò hạ tầng của C++17
   #link(<ref-cpp17>)[[5]]. Vì chưa có văn bản quy định cấm toàn bộ thư viện
   chuẩn, báo cáo không sử dụng tuyên bố tuyệt đối “không dùng STL”.],
)

== Thuật ngữ và ký hiệu

#figure(
  readable_table[
    #table(
      columns: (2.2cm, 5.0cm, 7.8cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (center, left, left),
      fill: table_fill,
      table.header(head([Ký hiệu]), head([Đại lượng]), head([Phạm vi sử dụng])),
      cell([$n$]), cell([Số phiên/khóa]), cell([Hash, Queue, Heap, Merge Sort]),
      cell([$B$]), cell([Số bucket]), cell([Bộ nhớ tổng của Hash Table]),
      cell([$V, E$]), cell([Số đỉnh, số cạnh]), cell([BFS và Dijkstra]),
      cell([$N$]), cell([Số biển số ứng viên]), cell([Fuzzy Search]),
      cell([$m, ell$]), cell([Độ dài hai chuỗi]), cell([Levenshtein]),
      cell([$o$]), cell([Số entry overflow]), cell([Chọn ứng viên reachable]),
      cell([$k$]), cell([Số xe chắn]), cell([Kế hoạch relocation dãy hẹp]),
      cell([$S$]), cell([Số ô cần quét]), cell([Xác định xe cùng lane]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Hệ ký hiệu dùng nhất quán trong báo cáo],
) <tbl:notation>

= Phân tích và thiết kế hệ thống

== Kiến trúc phân lớp

#figure(
  align(center)[
    #grid(
      columns: (12.8cm,),
      row-gutter: 0.18cm,
      diagram_box([LỚP TRÌNH BÀY], [Console menu · Demo tự động · Hiển thị kết quả]),
      align(center)[#text(size: 13pt, fill: gray_700)[↓ yêu cầu / kết quả]],
      diagram_box(
        [LỚP ỨNG DỤNG],
        [`ParkingSystem` · `FeeCalculator` · `FuzzySearch` · `DataLoader`],
        fill: navy_light,
        stroke: navy,
      ),
      align(center)[#text(size: 13pt, fill: gray_700)[↓ thao tác qua giao diện ADT]],
      diagram_box(
        [LỚP CTDL VÀ GIẢI THUẬT],
        [Hash · Queue · Stack · Min-Heap · Merge Sort · Graph · BFS · Dijkstra],
      ),
      align(center)[#text(size: 13pt, fill: gray_700)[↓ đọc / ghi có kiểm tra]],
      diagram_box([LỚP DỮ LIỆU], [CSV bản đồ · Log camera · Cấu hình phí · Lịch sử]),
    )
  ],
  kind: "diagram",
  supplement: [Hình],
  caption: [Kiến trúc phân lớp và chiều phụ thuộc của hệ thống],
) <fig:architecture>

`ParkingSystem` là đầu mối duy nhất thay đổi trạng thái nghiệp vụ. Giao diện
không sửa trực tiếp Hash Table hoặc ô đỗ; nhờ đó một thao tác không thể cập nhật
mã thẻ mà quên cập nhật biển số hay liên kết ngược từ ô đỗ.

== Mô hình dữ liệu

#figure(
  readable_table[
    #table(
      columns: (3.0cm, 5.5cm, 6.5cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (left, left, left),
      fill: table_fill,
      table.header(head([Thực thể]), head([Trường trọng tâm]), head([Vai trò])),
      cell([`ParkingNode`]),
      cell([id, type, zone, vehicle, lane, depth, occupiedCardId]),
      cell([Biểu diễn cổng, giao lộ và ô đỗ trên Graph]),
      cell([`ParkingSession`]),
      cell([card, plate, role, type, entryTime, slot, status, overflow]),
      cell([Trạng thái một xe đã được nhận vào hệ thống]),
      cell([`VehicleRequest`]),
      cell([Thông tin xe, cổng, thời điểm và sequence]),
      cell([Yêu cầu check-in hoặc node đang chờ trong Queue]),
      cell([`RelocationPlan`]),
      cell([Target, slot, blocker order, restore order, lecturer request]),
      cell([Giao dịch di dời hai pha cần xác nhận]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Các thực thể dữ liệu chính],
) <tbl:models>

== Chính sách phân khu và ưu tiên

1. Sinh viên ưu tiên khu Sinh viên; chỉ đỗ tràn khu Giảng viên khi khu Sinh viên
   hết ô phù hợp và không có Giảng viên cùng loại xe đang chờ.
2. Giảng viên chỉ sử dụng khu Giảng viên.
3. Khi ô Sinh viên trống, xe ở trạng thái `Holding` được đưa về trước Queue cổng.
4. Khi ô Giảng viên trống, ưu tiên pending relocation, sau đó Queue Giảng viên;
   chỉ khi không còn nhu cầu Giảng viên mới nhận sinh viên đỗ tràn.
5. Mọi tie-break sử dụng mã ô hoặc sequence để kết quả demo có thể lặp lại.

== Luồng check-in có nhánh quyết định

#figure(
  align(center)[
    #grid(
      columns: (3.1cm, 0.55cm, 3.1cm, 0.55cm, 3.1cm, 0.55cm, 3.1cm),
      column-gutter: 0.04cm,
      align: center + horizon,
      diagram_box([1. VALIDATE], [Chuẩn hóa · chống trùng]),
      text(size: 14pt, fill: gray_700)[→],
      diagram_box([2. ROUTE], [Dijkstra đến ô đúng loại]),
      text(size: 14pt, fill: gray_700)[→],
      diagram_box([3. DECIDE], [Đúng khu · overflow · hết chỗ]),
      text(size: 14pt, fill: gray_700)[→],
      diagram_box([4. COMMIT], [Hash · slot · history], fill: navy_light, stroke: navy),
    )
    #v(0.2cm)
    #grid(
      columns: (1fr, 1fr, 1fr, 1fr),
      column-gutter: 0.15cm,
      diagram_box([REJECT], [Dữ liệu sai hoặc trùng]),
      diagram_box([PARKED], [Cấp ô trong khu ưu tiên]),
      diagram_box([OVERFLOW], [Sinh viên vào khu Giảng viên]),
      diagram_box([QUEUE], [Giữ FIFO theo vai trò/loại]),
    )
  ],
  kind: "diagram",
  supplement: [Hình],
  caption: [Luồng check-in và bốn kết quả nghiệp vụ],
) <fig:checkin>

== Check-out và relocation

Check-out tra phiên qua Hash Table, kiểm tra thời gian, tính phí, giải phóng ô,
xóa đồng bộ hai chỉ mục và ghi lịch sử. Sức chứa vừa giải phóng được xử lý theo
chính sách ưu tiên trước khi nhận xe tiếp theo.

Relocation được chia hai pha. `createRelocationPlan` chỉ chọn ứng viên và tạo
thứ tự di dời; chưa thay đổi ô. `confirmRelocation` mới đưa sinh viên vào khu
tạm, cấp ô cho Giảng viên và đặt `entryTime` của Giảng viên tại thời điểm xác
nhận. Nếu giao dịch thất bại, các thay đổi được hoàn tác.

#figure(
  align(center)[
    #grid(
      columns: (3.0cm, 0.5cm, 3.0cm, 0.5cm, 3.0cm, 0.5cm, 3.0cm),
      align: center + horizon,
      diagram_box([PREPARE], [Chọn target và blockers]),
      text(size: 14pt, fill: gray_700)[→],
      diagram_box([PLAN], [Không thay đổi trạng thái]),
      text(size: 14pt, fill: gray_700)[→],
      diagram_box([CONFIRM], [Commit đồng bộ], fill: navy_light, stroke: navy),
      text(size: 14pt, fill: gray_700)[↔],
      diagram_box([ROLLBACK], [Khôi phục nếu lỗi]),
    )
  ],
  kind: "diagram",
  supplement: [Hình],
  caption: [Giao dịch relocation hai pha và nhánh rollback],
) <fig:relocation>

= Thiết kế cấu trúc dữ liệu

== Bảng lựa chọn

#figure(
  readable_table[
    #table(
      columns: (2.7cm, 4.4cm, 4.0cm, 3.9cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (left, left, left, left),
      fill: table_fill,
      table.header(head([CTDL]), head([Vai trò]), head([Lý do chọn]), head([Không chọn])),
      cell([Hash Table]), cell([Hai chỉ mục card và plate]), cell([Tra cứu trung bình $O(1)$]), cell([Quét vector $O(n)$]),
      cell([Linked Queue]), cell([Xe chờ và khu tạm]), cell([FIFO; hai đầu $O(1)$]), cell([Stack không giữ FIFO]),
      cell([Binary Min-Heap]), cell([Dijkstra và overflow]), cell([Lấy min; push/pop $O(log n)$]), cell([Quét tuyến tính $O(n)$]),
      cell([Linked Stack]), cell([Xe chắn trong dãy hẹp]), cell([Hoàn nguyên theo LIFO]), cell([Queue cho sai thứ tự]),
      cell([Adjacency list]), cell([Bản đồ bãi xe thưa]), cell([Bộ nhớ $O(V+E)$]), cell([Ma trận $O(V^2)$]),
      cell([Merge Sort]), cell([Fuzzy và lịch sử]), cell([Ổn định; $O(n log n)$]), cell([Quick Sort có cận xấu $O(n^2)$]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Lựa chọn cấu trúc dữ liệu và phương án thay thế],
) <tbl:ds-choice>

== Hash Table separate chaining

Hệ thống duy trì `cardId -> ParkingSession` và
`normalizedPlate -> cardId`. Mỗi bucket là danh sách liên kết đơn. Khi load
factor dự kiến vượt 0,75, bảng được rehash sang số bucket mới. Xóa dùng con trỏ
tới `unique_ptr` liên kết nên không cần tombstone; quyền sở hữu node được giải
phóng tự động khi liên kết bị thay thế. ADT nằm tại
`include/ds/HashTable.hpp`; hai chỉ mục được tích hợp trong
`include/services/ParkingSystem.hpp` và `src/ParkingSystem.cpp`.

Với $n$ phần tử và $B$ bucket, load factor hiện tại và load factor dự kiến sau
một lần chèn được xác định bởi:

#math.equation(
  block: true,
  numbering: "(1)",
)[
  $alpha = n / B, quad alpha^+ = (n + 1) / B.$
] <eq:hash-load>

Hiện thực gọi `rehash(2B+1)` khi $alpha^+ > 0.75$. Công thức
#ref(<eq:hash-load>) giải thích trực tiếp điều kiện ở dòng chèn thay vì chỉ mô
tả ngưỡng bằng lời.

#figure(
  ```cpp
  if ((size_ + 1.0) / buckets_.size() > maxLoadFactor_) {
      rehash(buckets_.size() * 2 + 1);
  }
  const auto index = bucketIndex(key, buckets_.size());
  auto node = std::make_unique<Node>(key, std::move(value));
  node->next = std::move(buckets_[index]);
  buckets_[index] = std::move(node);
  ++size_;
  ```,
  kind: "code",
  supplement: [Mã nguồn],
  caption: [Chèn node và kích hoạt rehash trước khi vượt load factor],
) <code:hash-insert>

== Queue, Heap và Stack

Queue được tách theo cặp `(vai trò, loại xe)`. Việc tách này giữ FIFO trong từng
nhóm đồng thời ngăn một xe không tương thích ở đầu hàng chặn sức chứa phù hợp.
Heap dùng cùng một template nhưng hai comparator: Dijkstra ưu tiên distance,
còn overflow ưu tiên entry time, sequence và card ID. Stack chỉ giải quyết thứ
tự vật lý; không dùng để chọn xe đỗ tràn. Ba ADT tương ứng nằm tại
`include/ds/LinkedQueue.hpp`, `BinaryMinHeap.hpp` và `LinkedStack.hpp`.

#figure(
  align(center)[
    #grid(
      columns: (1fr, 1fr, 1fr),
      column-gutter: 0.25cm,
      diagram_box([QUEUE — FIFO], [Xe chờ theo vai trò và loại xe]),
      diagram_box([MIN-HEAP], [Ứng viên ưu tiên nhỏ nhất]),
      diagram_box([STACK — LIFO], [Đưa xe chắn ra và hoàn nguyên]),
    )
    #v(0.16cm)
    #align(center)[
      #text(size: 9.5pt, fill: gray_700)[Ba cấu trúc cùng thuộc lớp ADT; không phụ thuộc tuần tự vào nhau.]
    ]
  ],
  kind: "diagram",
  supplement: [Hình],
  caption: [Ba cấu trúc dữ liệu có vai trò nghiệp vụ độc lập],
) <fig:qhs>

== Quản lý bộ nhớ

Node của Hash Table, Queue và Stack được sở hữu bằng `std::unique_ptr`; đối
tượng không chia sẻ quyền sở hữu và được hủy theo chuỗi liên kết. Các class
không để lộ con trỏ node ra ngoài ADT. Graph và Heap lưu giá trị trong vector,
không giữ con trỏ đến phần tử có thể bị invalid sau khi vector tăng dung lượng.
Đây là lý do thiết kế giúp giảm rủi ro leak và dangling pointer, nhưng kết luận
cuối cùng vẫn phải dựa trên test và sanitizer.

= Thiết kế giải thuật

== Dijkstra và đối chứng BFS

Dijkstra khởi tạo khoảng cách của cổng bằng 0, lấy node có khoảng cách nhỏ nhất,
relax cạnh có trọng số dương và lưu parent để dựng đường. Hiện thực dùng lazy
entry thay cho decrease-key: khi có khoảng cách tốt hơn, một entry mới được
push; entry cũ bị bỏ lúc pop nếu không còn khớp mảng distance
#link(<ref-clrs>)[[4]]. Hiện thực cụ thể nằm tại `src/ParkingGraph.cpp`;
adjacency list và giao diện Graph được khai báo trong
`include/graph/ParkingGraph.hpp`.

Với đỉnh nguồn $s$, thuật toán đặt $d(s)=0$, các đỉnh còn lại bằng vô cực và
thực hiện phép relax trên cạnh $(u,v)$ có trọng số $w(u,v)>0$:

#math.equation(
  block: true,
  numbering: "(1)",
)[
  $d(v) <- min(d(v), d(u) + w(u,v)).$
] <eq:dijkstra-relax>

Gọi $tilde(d)=d(u)+w(u,v)$ và $p(v)$ là node cha hiện tại. Cập nhật được thực
hiện khi:

#math.equation(
  block: true,
  numbering: "(1)",
)[
  $tilde(d) < d(v) or
  [tilde(d) = d(v) and (p(v) = bot or id(u) < id(p(v)))].$
] <eq:dijkstra-tie>

Nhánh thứ hai chọn mã node cha nhỏ hơn khi hai đường có cùng khoảng cách. Nó
không thay đổi giá trị tối ưu nhưng làm kết quả demo xác định.

#figure(
  ```cpp
  for (const Edge& edge : adjacency_[current.nodeIndex]) {
      if (!edge.enabled || !nodes_[edge.to].enabled) continue;
      if (current.distance > INF - edge.distanceMeters) continue;

      const auto candidate = current.distance + edge.distanceMeters;
      const bool strictlyBetter = candidate < result.distance[edge.to];
      const bool sameButBetterParent =
          candidate == result.distance[edge.to] &&
          (result.parent[edge.to] == NO_PARENT ||
           nodes_[current.nodeIndex].id <
               nodes_[result.parent[edge.to]].id);

      if (strictlyBetter || sameButBetterParent) {
          result.distance[edge.to] = candidate;
          result.parent[edge.to] = current.nodeIndex;
          frontier.push({candidate, edge.to, nodes_[edge.to].id});
      }
  }
  ```,
  kind: "code",
  supplement: [Mã nguồn],
  caption: [Phép relax có guard chống tràn số và tie-break xác định],
) <code:dijkstra>

#figure(
  align(center)[
    #grid(
      columns: (2.0cm, 1.7cm, 2.0cm),
      align: center + horizon,
      diagram_box([GATE_A], [Cổng vào]),
      align(center)[#text(size: 10pt, fill: gray_700)[── 20 m ──→]],
      diagram_box([LM01], [Ô xe máy]),
    )
    #v(0.28cm)
    #grid(
      columns: (2.0cm, 1.45cm, 2.0cm, 1.45cm, 2.0cm, 1.45cm, 2.0cm),
      align: center + horizon,
      diagram_box([GATE_A], [Cổng vào]),
      align(center)[#text(size: 10pt, fill: gray_700)[─ 4 m →]],
      diagram_box([J0], [Nút nối]),
      align(center)[#text(size: 10pt, fill: gray_700)[─ 3 m →]],
      diagram_box([JL], [Nút nối]),
      align(center)[#text(size: 10pt, fill: gray_700)[─ 3 m →]],
      diagram_box([LM01], [Ô xe máy]),
    )
    #v(0.16cm)
    #align(center)[
      #text(size: 9.5pt)[BFS chọn 1 cạnh = 20 m; Dijkstra chọn 3 cạnh = 10 m.]
    ]
  ],
  kind: "diagram",
  supplement: [Hình],
  caption: [Đồ thị minh họa: ít cạnh không đồng nghĩa tổng khoảng cách ngắn nhất],
) <fig:bfs-dijkstra>

Với Binary Min-Heap lazy, một đỉnh có thể được push sau mỗi lần relax thành
công. Heap có thể chứa $O(E)$ entry; do đó cận sử dụng trong báo cáo là
$O((V+E) log E)$ và bộ nhớ $O(V+E)$.

#pagebreak(weak: true)
== Levenshtein Distance và Fuzzy Search

Biển số được viết hoa và loại bỏ khoảng trắng, dấu chấm, dấu gạch. Với hai chuỗi
có độ dài $m$ và $ell$, điều kiện cơ sở là:

#math.equation(
  block: true,
  numbering: "(1)",
)[
  $d_(i,0) = i quad (0 <= i <= m), comma
  d_(0,j) = j quad (0 <= j <= ell).$
]

Với $1 <= i <= m$ và $1 <= j <= ell$, truy hồi quy hoạch động là:

#math.equation(
  block: true,
  numbering: "(1)",
)[
  $d_(i,j) = min(
    d_(i-1,j) + 1,
    d_(i,j-1) + 1,
    d_(i-1,j-1) + c_(i,j)
  ).$
]

trong đó $c_(i,j)=0$ nếu hai ký tự đang xét bằng nhau, ngược lại bằng 1. Công
thức tương ứng bài toán string-to-string correction của Wagner và Fischer
#link(<ref-wagner>)[[6]]. Hiện thực tại `src/FuzzySearch.cpp` chỉ giữ hai hàng:

#figure(
  ```cpp
  for (std::size_t row = 1; row <= right.size(); ++row) {
      current[0] = static_cast<int>(row);
      for (std::size_t col = 1; col <= left.size(); ++col) {
          const int ins = current[col - 1] + 1;
          const int del = previous[col] + 1;
          const int sub = previous[col - 1]
                        + (left[col - 1] == right[row - 1] ? 0 : 1);
          current[col] = std::min({ins, del, sub});
      }
      previous.swap(current);
  }
  ```,
  kind: "code",
  supplement: [Mã nguồn],
  caption: [Levenshtein Distance với hai hàng bộ nhớ],
) <code:levenshtein>

Mỗi session được chấm edit distance; kết quả được Merge Sort theo bộ khóa
`(distance, plate, cardId)` rồi lấy ba phần tử đầu. Chi phí toàn bộ là
$O(N m ell + N log N)$, bộ nhớ phụ $O(N + min(m, ell))$. Merge Sort tự cài
nằm tại `include/ds/MergeSort.hpp`.

== Chọn overflow reachable

Heap overflow cho thứ tự toàn cục nhưng root có thể nằm ở thành phần đồ thị mà
cổng hiện tại không đi tới. Hệ thống tạm pop entry hợp lệ nhưng unreachable vào
buffer, chọn entry reachable đầu tiên mà không pop ứng viên, rồi push toàn bộ
buffer trở lại. Entry stale sau checkout hoặc di dời mới bị loại vĩnh viễn.

#figure(
  align(center)[
    #grid(
      columns: (2.7cm, 0.5cm, 3.0cm, 0.5cm, 3.0cm, 0.5cm, 2.7cm),
      align: center + horizon,
      diagram_box([HEAP TOP], [Kiểm tra session/slot]),
      text(size: 14pt, fill: gray_700)[→],
      diagram_box([REACHABILITY], [Dijkstra từ đúng gate]),
      text(size: 14pt, fill: gray_700)[→],
      diagram_box([CANDIDATE], [Peek; chưa pop], fill: navy_light, stroke: navy),
      text(size: 14pt, fill: gray_700)[→],
      diagram_box([RESTORE], [Push lại buffer]),
    )
  ],
  kind: "diagram",
  supplement: [Hình],
  caption: [Chọn xe overflow sớm nhất trong tập có thể tiếp cận],
) <fig:overflow>

Trường hợp xấu phải kiểm tra $o$ entry; mỗi entry chạy Dijkstra và được pop/push
lại. Cận thời gian là $O(o(V+E) log E + o log o)$.

== Stack cho dãy hẹp

Xe mục tiêu ở `LM03` bị `LM01`, `LM02` chắn. Hệ thống sắp removal order theo
độ sâu, push `LM01`, rồi `LM02`; sau khi đưa xe mục tiêu ra, pop `LM02`, rồi
`LM01` để hoàn nguyên đúng thứ tự. Relocation chỉ commit sau xác nhận.
`entryTime` của Giảng viên bắt đầu khi được cấp ô, không phải lúc xem kế hoạch.
Nếu một ô khác trống trước xác nhận, pending request được cấp trực tiếp và plan
bị hủy; ứng viên overflow vẫn còn trong Heap.

= Cài đặt và tổ chức mã nguồn

== Cấu trúc project

#figure(
  ```text
  DoAn11_SmartCampusParking/
  ├── app/main.cpp
  ├── data/                         # bản đồ, phí, camera log
  ├── docs/                         # phân tích, ma trận, kiểm thử
  ├── include/
  │   ├── domain/
  │   ├── ds/                       # Hash, Queue, Stack, Heap, Merge Sort
  │   ├── graph/
  │   ├── io/
  │   └── services/
  ├── src/
  ├── tests/test_main.cpp
  ├── CMakeLists.txt
  └── Makefile
  ```,
  kind: "code",
  supplement: [Mã nguồn],
  caption: [Cấu trúc thư mục của gói dự án],
) <code:project-tree>

Mã được chia thành header và source; các cấu trúc dữ liệu template nằm trong
`include/ds`. Build chọn chuẩn C++17 theo ISO/IEC 14882:2017
#link(<ref-cpp17>)[[5]] và bật các cờ
`-Wall -Wextra -Wpedantic -Wconversion -Wshadow`.

== Đọc dữ liệu transactional

`DataLoader` đọc node và edge vào một Graph tạm. Chỉ khi toàn bộ dữ liệu hợp lệ
mới thay Graph cũ; vì vậy lỗi CSV không làm hệ thống mất bản đồ đang dùng. Cạnh
phải có khoảng cách dương; ID phải tồn tại và không trùng.

#pagebreak(weak: true)
== Cấu hình phí

Phí được đọc từ `data/fees.cfg`, làm tròn lên theo mỗi giờ bắt đầu và tối thiểu
một giờ. Bảng dưới đây mô tả đúng cấu hình đã dùng trong lần kiểm thử, không
tuyên bố đây là mức phí chính thức của giảng viên.

Gọi $Delta t=t_("out")-t_("in") >= 0$ là thời lượng theo giây,
$r_("type")$ là mức phí mỗi giờ bắt đầu. Hiện thực trong
`src/FeeCalculator.cpp` làm tròn trước theo phút, sau đó theo giờ:

#math.equation(
  block: true,
  numbering: "(1)",
)[
  $M = ceil(Delta t / 60), quad
  H = max(1, ceil(M / 60)) = max(1, ceil(Delta t / 3600)), quad
  F = r_("type") dot H.$
] <eq:fee>

Vì vậy 59 và 60 phút đều tính một giờ, còn 61 phút tính hai giờ. Hiện thực còn
kiểm tra thời gian ra không sớm hơn thời gian vào, mức phí dương và tràn số
64-bit trước khi nhân.

#figure(
  readable_table[
    #table(
      columns: (4.0cm, 4.3cm, 6.7cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (center, center, left),
      fill: table_fill,
      table.header(head([Loại xe]), head([Mức/giờ bắt đầu]), head([Ví dụ kiểm thử])),
      cell([Xe máy]), cell([5.000 VND]), cell([61 phút → 10.000 VND]),
      cell([Ô tô]), cell([15.000 VND]), cell([61 phút → 30.000 VND]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Cấu hình phí mẫu được sử dụng trong lần kiểm thử],
) <tbl:fees>

#note_block(
  [Điểm cấu hình cần xác nhận],
  [Nếu đề bài hoặc giảng viên quy định mức phí khác, nhóm chỉ thay
   `data/fees.cfg`, chạy lại toàn bộ test và cập nhật bảng bằng chứng. Cấu hình
   mẫu hiện tại không được trình bày như một quy định học phần.],
)

= Invariant, kiểm thử và kết quả

== Invariant trọng tâm

Sau mỗi chuỗi mutation, `validateInvariants()` kiểm tra:

1. Hai Hash Table có cùng số phần tử và trỏ qua lại đúng.
2. Mỗi ô occupied có đúng một session `Parked`; session trỏ ngược cùng ô.
3. Ô occupied phải enabled và tương thích loại xe.
4. Session `Holding` không có slot và xuất hiện đúng một lần trong Queue khu tạm.
5. Queue chờ không chứa xe đã có session và giữ đúng vai trò/loại xe.
6. Pending lecturer chưa có session, không nằm Queue và target vẫn là sinh viên
   overflow hợp lệ, cùng loại xe.
7. `activeSessions = occupiedSlots + holdingSessions`.

Invariant không thay thế test, nhưng phát hiện sớm trạng thái trung gian bị lệch
sau check-in, check-out, relocation và rollback. Hàm kiểm tra nằm tại
`src/ParkingSystem.cpp`; test runner trong `tests/test_main.cpp` gọi lại
invariant sau các chuỗi tích hợp.

Nếu $S$ là tập session đang hoạt động, $P$ là chỉ mục biển số, $O$ là tập ô đang
được chiếm và $H$ là tập session trong khu tạm, điều kiện đếm cốt lõi là:

#math.equation(
  block: true,
  numbering: "(1)",
)[
  $|S| = |P| = |O| + |H|.$
] <eq:session-count>

Phương trình #ref(<eq:session-count>) là điều kiện cần nhưng chưa đủ; các kiểm
tra liên kết hai chiều và tính tương thích trong danh sách trên mới loại được
trạng thái “đúng số lượng nhưng sai ô”.

== Chiến lược kiểm thử

#figure(
  readable_table[
    #table(
      columns: (3.0cm, 5.9cm, 6.1cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (left, left, left),
      fill: table_fill,
      table.header(head([Nhóm test]), head([Phạm vi]), head([Bằng chứng tiêu biểu])),
      cell([Unit]), cell([Hash, Queue, Stack, Heap, Merge Sort, Levenshtein]), cell([Collision/rehash; FIFO/LIFO; ổn định; DP]),
      cell([Boundary]), cell([59/60/61 phút; input sai; cạnh âm; unreachable]), cell([Tính phí và validation]),
      cell([Integration]), cell([Check-in/out; overflow; relocation; rehome; history]), cell([Invariant sau chuỗi thao tác]),
      cell([Stress]), cell([10.000 khóa Hash Table]), cell([Rehash và tra cứu sau tăng tải]),
      cell([Dynamic]), cell([ASan + UBSan]), cell([Không phát hiện lỗi trong ca đã chạy]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Chiến lược kiểm thử nhiều lớp],
) <tbl:test-strategy>

== Kết quả xác nhận

Hồ sơ kiểm thử đầy đủ được lưu ngày 24/07/2026 trên macOS Apple Silicon,
Apple Clang 17 và C++17. Ngày 26/07/2026, source tiếp tục vượt qua bước
compile-only với cùng bộ cờ, còn demo hiện có chạy thành công và tái hiện các
kết quả BFS 20 m, Dijkstra 10 m, thứ tự Queue/Heap/Stack, fuzzy distance 1 và
`Invariant: PASS`. Test thường và sanitizer 24/24 bên dưới là kết quả đã lưu
ngày 24/07/2026; chúng cần được chạy lại nếu source hoặc dữ liệu thay đổi.

#grid(
  columns: (1fr, 1fr, 1fr),
  column-gutter: 0.22cm,
  metric([24 / 24], [NHÓM TEST PASS], note: [0 test fail]),
  metric([0], [CẢNH BÁO BUILD], note: [bộ cờ nghiêm ngặt]),
  metric([PASS], [INVARIANT DEMO], note: [trạng thái cuối]),
)

#v(0.3cm)

#figure(
  readable_table[
    #table(
      columns: (3.6cm, 4.7cm, 6.7cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (left, left, left),
      fill: table_fill,
      table.header(head([Lệnh]), head([Kết quả]), head([Kết luận được phép])),
      cell([`make -B all`]), cell([Thành công; 0 warning]), cell([Mã C++17 biên dịch với các cờ đã cấu hình]),
      cell([`make test`]), cell([24 PASS; 0 FAIL]), cell([Các nhóm unit, boundary, integration, stress đều đạt]),
      cell([`make demo`]), cell([`Invariant: PASS`]), cell([Luồng demo xác định kết thúc nhất quán]),
      cell([`make sanitize`]), cell([24 PASS; không có report]), cell([ASan/UBSan không phát hiện lỗi trong ca thực thi]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Bằng chứng thực thi ngày 24/07/2026],
) <tbl:results>

#note_block(
  [Giới hạn của bằng chứng],
  [Kết quả sanitizer chỉ chứng minh công cụ không phát hiện lỗi trong các ca đã
   chạy. Nó không phải là chứng minh toán học rằng mọi đầu vào đều không có lỗi
   bộ nhớ hoặc hành vi không xác định.],
)

== Dấu vân tay tệp thực thi

Để phân biệt đúng binary đã được kiểm tra với một bản build về sau, nhóm ghi lại
SHA-256 của ba tệp thực thi tại thời điểm rà soát:

#figure(
  readable_table[
    #table(
      columns: (4.1cm, 10.9cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (left, left),
      fill: table_fill,
      table.header(head([Tệp]), head([SHA-256])),
      cell([`smart_parking`]),
      cell([#text(font: "DejaVu Sans Mono", size: 7.2pt)[6b75ac18da598ca34b2a9de0a4c83f36c98ca3ff9d050441d5f4715c09edcba6]]),
      cell([`smart_parking_tests`]),
      cell([#text(font: "DejaVu Sans Mono", size: 7.2pt)[ab3f1166647370717071519e77011db10e2601d164eda522e3e24f2d13323e0e]]),
      cell([`smart_parking_tests_san`]),
      cell([#text(font: "DejaVu Sans Mono", size: 7.2pt)[d70dd780a627af0cdc97256102c321fbf41762733fe9b322acae0c3757a513c1]]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [SHA-256 của các binary dùng làm bằng chứng],
) <tbl:binary-hashes>

== Quan hệ giữa 25 yêu cầu và 24 nhóm test

Ma trận đặc tả đánh số 25 yêu cầu nguyên tử R01–R25 để truy vết đầy đủ chức năng
và ràng buộc. Chương trình kiểm thử dùng 24 nhóm executable; một số nhóm tích
hợp kiểm tra đồng thời nhiều yêu cầu liên quan. Vì vậy báo cáo dùng hai phát
biểu riêng:

- “R01–R25 đều có hiện thực và bằng chứng tương ứng” nói về độ phủ đặc tả.
- “24/24 nhóm test PASS” nói về kết quả của executable test runner.

= Phân tích độ phức tạp

#figure(
  readable_table[
    #table(
      columns: (4.7cm, 5.1cm, 5.3cm),
      stroke: table_rule,
      inset: (x: 5pt, y: 4pt),
      align: (left, center, center),
      fill: table_fill,
      table.header(head([Thao tác]), head([Thời gian]), head([Bộ nhớ])),
      cell([Hash find/insert/erase]), cell([TB $O(1)$; xấu $O(n)$]), cell([$O(n+B)$ tổng]),
      cell([Queue enqueue/dequeue]), cell([$O(1)$]), cell([$O(n)$ tổng]),
      cell([Stack push/pop]), cell([$O(1)$]), cell([$O(k)$ tổng]),
      cell([Heap top/push/pop]), cell([$O(1)$ / $O(log n)$]), cell([$O(n)$]),
      cell([BFS]), cell([$O(V+E)$]), cell([$O(V)$]),
      cell([Dijkstra lazy heap]), cell([$O((V+E) log E)$]), cell([$O(V+E)$]),
      cell([Levenshtein một cặp]), cell([$O(m ell)$]), cell([$O(min(m, ell))$]),
      cell([Fuzzy trên $N$ xe]), cell([$O(N m ell + N log N)$]), cell([$O(N + min(m, ell))$]),
      cell([Merge Sort]), cell([$O(n log n)$]), cell([$O(n)$]),
      cell([Overflow reachable]), cell([$O(o(V+E)log E + o log o)$]), cell([$O(V+E+o)$]),
      cell([Kế hoạch dãy hẹp]), cell([$O(S+k log k)$]), cell([$O(k)$]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Độ phức tạp của các thao tác chính],
) <tbl:complexity>

Các ký hiệu trong #ref(<tbl:complexity>) tuân theo #ref(<tbl:notation>): $B$ là
số bucket, còn $k$ là số xe chắn; hai đại lượng không dùng chung ký hiệu. Cận
Dijkstra phản ánh đúng biến thể lazy entry đang cài đặt, không dùng cận của
biến thể có decrease-key.

= Đánh giá và hướng phát triển

== Điểm đạt được

- Cả yêu cầu cơ bản và nâng cao được tích hợp trong một chương trình C++17.
- CTDL cốt lõi được tự cài, tái sử dụng có chủ đích và gắn với nghiệp vụ.
- Phản ví dụ BFS 20 m so với Dijkstra 10 m giúp kiểm chứng lý do chọn thuật toán.
- Relocation xử lý reachability, pending priority, rollback và thời điểm tính phí.
- Invariant nối các cấu trúc rời rạc thành một trạng thái hệ thống nhất quán.
- Ma trận R01–R25 liên kết yêu cầu, hiện thực và bằng chứng.

== Giới hạn

- Chưa xử lý nhiều check-in đồng thời hoặc persistence qua lần khởi động.
- Camera và cổng vào mới là mô phỏng; chưa tích hợp phần cứng.
- Fuzzy Search chưa dùng trọng số riêng cho cặp lỗi OCR như `0/O`, `5/S`.
- Nhiều overflow unreachable có thể khiến hệ thống chạy Dijkstra lặp lại.
- Cấu hình phí đang là dữ liệu kiểm thử, chờ xác nhận nếu học phần có mức cố định.
- Kết quả sanitizer chỉ áp dụng cho các ca đã chạy.

== Hướng phát triển

1. Lưu snapshot hoặc database nhưng giữ ADT làm chỉ mục trong bộ nhớ.
2. Cache reachability hoặc kết quả Dijkstra theo cổng khi bản đồ lớn.
3. Thêm trọng số OCR được hiệu chỉnh từ dữ liệu camera thật.
4. Thêm khóa giao dịch hoặc event queue nếu mô phỏng nhiều cổng đồng thời.
5. Xây GUI như lớp trình bày, không cho phép bỏ qua `ParkingSystem`.

= Kết luận

Hệ thống đã hiện thực đầy đủ luồng check-in, check-out, Queue khi bãi đầy, dẫn
đường có trọng số, tìm biển số gần đúng và relocation hai pha trong một chương
trình C++17. Hai Hash Table duy trì chỉ mục card/plate; Graph và Binary Min-Heap
thực hiện Dijkstra; Queue, Heap và Stack đảm nhận ba quy tắc thứ tự khác nhau;
`ParkingSystem` kiểm soát mutation và `validateInvariants()` kiểm tra trạng thái
sau các chuỗi tích hợp.

Hồ sơ ngày #meta.evidence_date ghi 24/24 nhóm test PASS và sanitizer không phát
hiện lỗi trong các ca đã chạy. Demo tái lập BFS 20 m, Dijkstra 10 m, fuzzy
distance 1, relocation theo Heap/Stack và kết thúc bằng `Invariant: PASS`. Các
kết quả này chứng minh phạm vi đã kiểm thử; chúng không loại bỏ những giới hạn
đã nêu về dữ liệu mô phỏng, xử lý đồng thời, persistence và cấu hình phí.

// ===========================================================================
// TÀI LIỆU THAM KHẢO
// ===========================================================================

#pagebreak()
#heading(numbering: none)[Tài liệu tham khảo]
#set par(first-line-indent: 0pt, hanging-indent: 0.7cm, spacing: 0.36em)

[1] Bộ môn Máy tính và Hệ thống nhúng, “Danh sách liên kết, Stack và Queue,”
bài giảng học phần Cấu trúc dữ liệu và giải thuật, 2026. <ref-lecture-1>

[2] Bộ môn Máy tính và Hệ thống nhúng, “Min-Heap, Heap Sort và Shell Sort,”
bài giảng học phần Cấu trúc dữ liệu và giải thuật, 2026. <ref-lecture-2>

[3] Bộ môn Máy tính và Hệ thống nhúng, “Merge Sort, Radix Sort, tìm kiếm và sắp
xếp,” bài giảng học phần Cấu trúc dữ liệu và giải thuật, 2026. <ref-lecture-3>

[4] T. H. Cormen, C. E. Leiserson, R. L. Rivest, and C. Stein,
_Introduction to Algorithms_, 4th ed. Cambridge, MA, USA: MIT Press, 2022.
<ref-clrs>

[5] ISO/IEC 14882:2017, _Programming Languages — C++_, 5th ed.,
International Organization for Standardization, Dec. 2017.
#link("https://www.iso.org/standard/68564.html")[https://www.iso.org/standard/68564.html].
<ref-cpp17>

[6] R. A. Wagner and M. J. Fischer, “The String-to-String Correction
Problem,” _Journal of the ACM_, vol. 21, no. 1, pp. 168–173, Jan. 1974,
doi: #link("https://doi.org/10.1145/321796.321811")[10.1145/321796.321811].
<ref-wagner>

// ===========================================================================
// PHỤ LỤC
// ===========================================================================

#pagebreak()
#heading(numbering: none)[Phụ lục A — Hướng dẫn tái lập]

#set par(first-line-indent: 0pt)

Tại thư mục gốc của source code, chạy lần lượt:

```bash
make -B all
make test
make demo
make sanitize
```

Kết quả mong đợi đối với phiên bản đã xác nhận:

- build hoàn tất và không có warning;
- test runner in `24 PASS, 0 FAIL`;
- dòng cuối demo là `Invariant: PASS`;
- sanitizer không in báo cáo lỗi trong các ca đã thực thi.

Trích đoạn dưới đây lấy từ demo chạy lại ngày 26/07/2026; dấu ba chấm chỉ phần
nghiệp vụ trung gian đã lược để giữ phụ lục ngắn:

#figure(
  ```text
  $ ./build/smart_parking --demo
  BFS_SO_CANH: GATE_A -> LM01 (20 m)
  DIJKSTRA: GATE_A -> J0 -> JL -> LM01 (10 m)
  ...
  Camera: 59-X1.123.4S -> chuan hoa: 59X11234S
  1. 59X112345 | distance=1 | similarity=88.9%
  ...
  Invariant: PASS
  ```,
  kind: "evidence",
  supplement: [Log],
  caption: [Trích kết quả demo tái lập trên binary đã kiểm tra],
) <log:demo>

Trên Windows, dùng CMake theo hướng dẫn trong `README.md`. Khi khác compiler
hoặc khác hệ điều hành, cần ghi lại môi trường và không mặc định kết quả phải
giống hoàn toàn nếu chưa chạy.

#pagebreak()
#heading(numbering: none)[Phụ lục B — Ma trận yêu cầu rút gọn]

#figure(
  readable_table[
    #table(
      columns: (1.4cm, 4.8cm, 4.6cm, 4.4cm),
      stroke: table_rule,
      inset: (x: 4pt, y: 3.5pt),
      align: (center, left, left, left),
      fill: table_fill,
      table.header(head([ID]), head([Yêu cầu]), head([Hiện thực]), head([Bằng chứng])),
      cell([R01–02]), cell([Đọc bản đồ và log camera]), cell([`src/DataLoader.cpp`]), cell([CSV quote; lỗi không hỏng Graph]),
      cell([R03–07]), cell([Hash, check-in/out, phí]), cell([`HashTable.hpp`; `ParkingSystem.cpp`; `FeeCalculator.cpp`]), cell([Collision; trùng; 59/60/61 phút]),
      cell([R08–09]), cell([Bãi đầy và tách loại xe]), cell([`LinkedQueue.hpp`; `ParkingSystem.cpp`]), cell([FIFO; không head-of-line blocking]),
      cell([R10–13]), cell([Graph, BFS, Dijkstra, tie-break]), cell([`ParkingGraph.cpp`; `BinaryMinHeap.hpp`]), cell([20 m/10 m; tie-break]),
      cell([R14–15]), cell([Camera nhiễu và top-3]), cell([`FuzzySearch.cpp`; `MergeSort.hpp`]), cell([Distance 1; tie-break]),
      cell([R16–20]), cell([Overflow và relocation]), cell([`ParkingSystem.cpp`; `LinkedStack.hpp`]), cell([Reachable; rollback; ưu tiên]),
      cell([R21–25]), cell([History, invariant, sanitizer, stress]), cell([`ParkingSystem.cpp`; `test_main.cpp`]), cell([24 PASS; 10.000 khóa]),
    )
  ],
  kind: table,
  supplement: [Bảng],
  caption: [Ánh xạ 25 yêu cầu sang tệp hiện thực và nhóm bằng chứng],
) <tbl:traceability>

#v(0.35cm)
#note_block(
  [Cách đọc ma trận],
  [Các dải R01–R25 biểu diễn 25 yêu cầu đặc tả. Một executable test có thể bao
   phủ nhiều yêu cầu, nên tổng số nhóm test là 24 và không bắt buộc bằng 25.],
)
