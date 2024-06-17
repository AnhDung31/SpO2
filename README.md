## A. Giới thiệu
  Đây là 1 project nhỏ thiết kế một thiết bị nhúng để đo nhịp tim và nồng độ oxi trong máu.
  - Sử dụng cảm biến MAX30102 để lấy được dữ liệu về nhịp tim và chỉ số SpO2.
  - Dữ liệu sẽ được hiển thị ra màn hình OLED cho người dùng.
  - Thiết bị sẽ thông báo bằng buzzer néu chỉ số nhịp tim và SpO2 khác thường.
## B.  Hướng dẫn sử dụng
### Hình ảnh minh hoạ sản phẩm
![sketch_img](https://github.com/ztrevah/SpO2/assets/93901738/473c8b42-57cc-43ff-8469-9798860ff3f1)
### Hướng dẫn sử dụng
- Cấp nguồn 3.3V cho ESP8266 (qua máy tính).
- Đặt ngón tay lên cảm biến và duy trì áp lực để chỉ số hiển thị ra ổn định.
- Quan sát chỉ số BPM và SpO2 được hiển thị trên màn hình OLED.
## C. Danh sách linh kiện (Bill of Material)
| Linh kiện | Số lượng | Link mua hàng | 
| --- | --- | ---|
| NodeMCU 1.0 (ESP12-E Module) | 1 | |
| Cảm biến nhịp tim MAX30102 | 1 | |
| Màn hình OLED 0.96inch | 1 | |
## D. Sơ đồ nguyên lý - Hardware Schematic
![image](https://github.com/ztrevah/SpO2/assets/93901738/0c9eb88e-fda1-45c0-95fc-70ebf8d9ecd0)
- Nối chân GND và VCC/VIN của cảm biến MAX30102 và màn hình OLED với chân GND và 3.3V của ESP8266 để cấp điện áp cho chúng hoạt động
- Các chân SCL và SDA của cảm biến MAX30102 và màn hình OLED được nối với lần lượt với chân D1 và D2 của ESP8266
- Còi buzzer được nối 1 chân với chân 3.3V của ESP8266, chân còn lại nối với chân D5 của ESP8266
## E.  Thiết kế phần mềm - Software Concept
   
## F. Tác giả
   Nguyễn Quang Chiến - 20215533 - [@ztrevah](https://github.com/ztrevah/SpO2)
   
   Võ Anh Dũng - 20215548
