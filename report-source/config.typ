#let meta = (
  faculty: "KHOA ĐIỆN TỬ - VIỄN THÔNG",
  department: "BỘ MÔN MÁY TÍNH VÀ HỆ THỐNG NHÚNG",
  course: "THỰC HÀNH CẤU TRÚC DỮ LIỆU VÀ GIẢI THUẬT",
  class_name: "24DTV_DKD2",
  lecturer: "ThS. Trần Tuấn Kiệt",
  project_no: "ĐỀ TÀI 11",
  project_name: "HỆ THỐNG BÃI GIỮ XE THÔNG MINH VÀ DẪN ĐƯỜNG",
  project_name_en: "SMART CAMPUS PARKING",
  language: "C++17",
  leader: "Lê Thanh Danh",
  leader_id: "24207086",
  university: "TRƯỜNG ĐẠI HỌC KHOA HỌC TỰ NHIÊN",
  university_parent: "ĐẠI HỌC QUỐC GIA THÀNH PHỐ HỒ CHÍ MINH",
  report_month: "07",
  report_year: "2026",
  evidence_date: "24/07/2026",
)

#let members = (
  (id: "24207086", name: "Lê Thanh Danh", role: "Nhóm trưởng"),
  (id: "24207108", name: "Nguyễn Hồng Tiến", role: "Thành viên"),
  (id: "24207033", name: "Phạm Minh Phát", role: "Thành viên"),
  (id: "24207067", name: "Lê Quốc Bảo", role: "Thành viên"),
  (id: "22207056", name: "Lương Hải Long", role: "Thành viên"),
  (id: "24207038", name: "Tống Duy Quyết", role: "Thành viên"),
  (id: "24207025", name: "Trần Ngọc Quang Minh", role: "Thành viên"),
  (id: "20207033", name: "Lê Ngọc Hiếu", role: "Thành viên"),
)

// Hệ màu chủ ý tối giản để phù hợp báo cáo học thuật.
#let navy = rgb("#17365D")
#let navy_light = rgb("#E9EEF3")
#let ink = rgb("#111111")
#let gray_700 = rgb("#4D5560")
#let gray_500 = rgb("#747C86")
#let gray_300 = rgb("#B8BEC6")
#let gray_200 = rgb("#D7DCE2")
#let gray_100 = rgb("#F2F5F8")
#let white = rgb("#FFFFFF")

#let body_size = 13pt
#let table_size = 10.5pt
#let code_size = 9pt

#let front_footer = context [
  #align(right)[
    #text(size: 10pt, fill: gray_700)[#counter(page).display("i")]
  ]
]

#let body_footer = context [
  #grid(
    columns: (1fr, auto),
    column-gutter: 0.6cm,
    align: (left + horizon, right + horizon),
    text(size: 9pt, fill: gray_500)[Đề tài 11 · Smart Campus Parking],
    text(size: 10pt, fill: gray_700)[#counter(page).display("1")],
  )
]

#let front_title(body) = block(
  width: 100%,
  above: 0.1cm,
  below: 0.45cm,
)[
  #align(center)[
    #text(size: 16pt, weight: "bold", fill: navy)[#upper(body)]
    #v(4pt)
    #line(length: 3.1cm, stroke: 0.8pt + navy)
  ]
]

#let info_label(body) = text(weight: "bold", fill: ink)[#body]

#let cell(body) = [
  #set par(
    first-line-indent: 0pt,
    justify: false,
    leading: 0.65em,
    spacing: 0.18em,
  )
  #body
]

#let head(body) = [
  #set par(first-line-indent: 0pt, justify: false)
  #text(weight: "bold", fill: ink)[#body]
]

#let readable_table(body, size: table_size) = block(
  width: 100%,
  breakable: true,
)[
  #set text(size: size)
  #set par(
    first-line-indent: 0pt,
    justify: false,
    leading: 0.65em,
    spacing: 0.16em,
  )
  #show raw: set text(font: "DejaVu Sans Mono", size: size - 0.4pt, fill: ink)
  #body
]

#let table_rule = 0.45pt + rgb("#7E8792")
#let table_fill(x, y) = if y == 0 { navy_light } else if calc.rem(y, 2) == 0 { gray_100 }

#let diagram_box(title, body, fill: white, stroke: gray_500) = block(
  width: 100%,
  fill: fill,
  stroke: 0.6pt + stroke,
  inset: (x: 7pt, y: 6pt),
  radius: 1.5pt,
  breakable: false,
)[
  #set par(first-line-indent: 0pt, justify: false, leading: 0.6em, spacing: 0.12em)
  #align(center)[
    #text(weight: "bold", size: 10.5pt, fill: navy)[#title]
    #v(2pt)
    #text(size: 9.3pt, fill: ink)[#body]
  ]
]

#let note_block(title, body) = block(
  width: 100%,
  fill: gray_100,
  stroke: (left: 2.2pt + navy),
  inset: (left: 9pt, right: 8pt, top: 6pt, bottom: 6pt),
  breakable: false,
)[
  #set par(first-line-indent: 0pt, justify: false, spacing: 0.18em)
  #text(weight: "bold", fill: navy)[#title]
  #linebreak()
  #body
]

#let metric(value, label, note: none) = block(
  width: 100%,
  inset: 8pt,
  fill: gray_100,
  stroke: 0.55pt + gray_300,
  radius: 1.5pt,
  breakable: false,
)[
  #set par(first-line-indent: 0pt, justify: false)
  #align(center)[
    #text(size: 17pt, weight: "bold", fill: navy)[#value]
    #v(2pt)
    #text(size: 9.2pt, weight: "bold", fill: ink)[#label]
    #if note != none [
      #v(1pt)
      #text(size: 8pt, fill: gray_700)[#note]
    ]
  ]
]

#let status_text(value, ok: true) = text(
  weight: "bold",
  fill: if ok { navy } else { gray_700 },
)[#value]
