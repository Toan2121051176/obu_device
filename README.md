# Thiết bị OBU

## Mục Tiêu
Xây dựng thiết bị thu phí nội đô dựa trên GPS.

## Ngôn Ngữ Sử Dụng
- Python
- C++
- Node.js
- JavaScript

## Các Tính Năng Đã Phát Triển
- **Đọc tín hiệu GPS và gửi tới Module SIM 4G qua DMA:**
  - Mỗi giây gửi tín hiệu GPS 1 lần.
- **Webserver hiển thị thông tin GPS:**
  - Nhận thông tin GPS và hiển thị trên bản đồ giao diện.
  - Hiệu chỉnh GPS để vẽ đúng quãng đường di chuyển của thiết bị.
- **Phát hiện và thông báo lỗi:**
  - Các lỗi bao gồm mất GPS, mất 4G, tọa độ bất thường, ...
- **Tính phí dựa trên quãng đường di chuyển:**
  - Server tính phí và gửi lại cho thiết bị.
  - Hiển thị cước phí trên màn hình LCD.
- **Demo bản thiết kế mạch:**
  - Cung cấp thiết kế mạch cho thiết bị phần cứng.

## Các Tính Năng Sẽ Phát Triển Thêm
- **Quản lý nhiều thiết bị cùng một lúc:**
  - Webserver quản lý các thiết bị bằng cách gắn ID cho từng thiết bị.
  - Xây dựng cơ sở dữ liệu để quản lý dữ liệu thu được.
- **Hoàn thiện thiết kế mạch phần cứng:**
  - Tích hợp JLink debugger vào bản thiết kế.
- **Cải thiện màn hình LCD:**
  - Sửa lỗi màn hình LCD hoạt động thiếu ổn định.

## Hướng Dẫn Sử Dụng
1. **Cài đặt môi trường:**
   - Cài đặt Python, Node.js, và các thư viện cần thiết.
   - Cấu hình module SIM 4G và thiết bị GPS.
2. **Chạy ứng dụng:**
   - Chạy server Node.js để nhận và xử lý dữ liệu GPS.
   - Kết nối thiết bị OBU và kiểm tra tín hiệu trên giao diện bản đồ.
3. **Theo dõi và kiểm tra:**
   - Kiểm tra thông báo lỗi (nếu có) trên giao diện web.
   - Theo dõi cước phí hiển thị trên màn hình LCD.




"# obu_device" 
