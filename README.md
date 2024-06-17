## A. Giới thiệu
  Đây là 1 project nhỏ thiết kế một thiết bị nhúng để đo nhịp tim và nồng độ oxi trong máu.
  - Sử dụng cảm biến MAX30102 để lấy được dữ liệu về nhịp tim và chỉ số SpO2.
  - Dữ liệu sẽ được hiển thị ra màn hình OLED cho người dùng.
  - Thiết bị sẽ thông báo bằng buzzer nếu chỉ số nhịp tim và SpO2 khác thường.

## B.  Hướng dẫn sử dụng
### 1. Hình ảnh minh hoạ sản phẩm
Khi bật thiết bị:

![20240617_221204](https://github.com/ztrevah/SpO2/assets/93901738/d4fc3bec-ab6d-437a-b707-f327816767ec)

Đặt ngón tay lên cảm biến và quan sát chỉ số nhịp tim và nồng độ oxi trên màn hình OLED:

![20240617_221210](https://github.com/ztrevah/SpO2/assets/93901738/d6362bab-261a-44dd-aa6d-983cf6b58289)

Khi nhấc ngón tay khỏi sensor:

![20240617_221212](https://github.com/ztrevah/SpO2/assets/93901738/fb83a006-aeb7-49b5-8f69-8f9ff298a503)

### 2. Hướng dẫn sử dụng
- Cấp nguồn 3.3V cho ESP8266 (có thể cấp nguồn bằng cách cắm vào máy tính).
- Đặt ngón tay lên cảm biến và duy trì áp lực để chỉ số hiển thị ra ổn định.
- Quan sát chỉ số BPM và SpO2 được hiển thị trên màn hình OLED.

#### Để có cái nhìn rõ hơn về sản phẩm này, hãy xem video demo tại [đây](https://drive.google.com/file/d/1ZJ9Q8RwgMdIQ0EB2SPxngzKBdT-qQ4i_/view?usp=drive_link)

## C. Danh sách linh kiện (Bill of Material)
| Linh kiện | Số lượng | Link mua hàng | 
| --- | --- | ---|
| ESP8266 NodeMCU 1.0 (ESP12-E Module) | 1 | [Linh kiện 3M](https://chotroihn.vn/module-iot-esp8266-esp-12e-cp2102-k1b7-6-8g) |
| Cảm biến nhịp tim MAX30102 | 1 | [Shopee](https://shopee.vn/Module-c%E1%BA%A3m-bi%E1%BA%BFn-nh%E1%BB%8Bp-tim-v%C3%A0-n%E1%BB%93ng-%C4%91%E1%BB%99-oxy-trong-m%C3%A1u-MAX30102-1.8-3.3V-5V-i.70782946.7316453605?sp_atk=435a61c9-8578-41a7-a56f-f4a583c9aab2&xptdk=435a61c9-8578-41a7-a56f-f4a583c9aab2) |
| Màn hình OLED 0.96inch | 1 | [Shopee](https://shopee.vn/M%C3%A0n-h%C3%ACnh-hi%E1%BB%83n-th%E1%BB%8B-128x64-Oled-0.96-Inch-giao-Ti%E1%BA%BFp-I2C-chuy%C3%AAn-d%E1%BB%A5ng-SSD1315-SSD1306-i.16504852.12103032615?sp_atk=41724e15-7898-42fd-bf72-608a1e33ee42&xptdk=41724e15-7898-42fd-bf72-608a1e33ee42) |
| Buzzer | 1 | [Linh kiện 3M](https://chotroihn.vn/coi-chip-9-5x12mm) |
| Breadboard | 1 | [Linh kiện 3M](https://chotroihn.vn/board-test-mb-102-16-5x5-5) |
| Dây nối |  |  |

## D. Sơ đồ nguyên lý - Hardware Schematic
![image](https://github.com/ztrevah/SpO2/assets/93901738/0c9eb88e-fda1-45c0-95fc-70ebf8d9ecd0)
- Nối chân GND và VCC/VIN của cảm biến MAX30102 và màn hình OLED với chân GND và 3.3V của ESP8266 để cấp điện áp cho chúng hoạt động
- Các chân SCL và SDA của cảm biến MAX30102 và màn hình OLED được nối với lần lượt với chân D1 và D2 của ESP8266 - 2 chân được cấu hình mặc định cho giao tiếp I2C của ESP8266
- Còi buzzer được nối 1 chân với chân 3.3V của ESP8266, chân còn lại nối với chân D5 của ESP8266

## E.  Thiết kế phần mềm - Software Concept
- Đo nồng độ oxy trong máu: 
    + Hai đèn LED của cảm biến MAX30102(Đỏ và IR) phát ra ánh sáng ở hai khoảng bước sóng khác nhau ( ~660nm cho LED đỏ, ~880nm cho LED hồng ngoại).
    + Hemoglobin oxy hóa hấp thụ nhiều ánh sáng hồng ngoại và phản xạ lại ánh sáng đỏ, trong khi hemoglobin khử oxy thì ngược lại.
    + Cảm biến MAX30102 từ đó có thể đọc được các mức hấp thụ hai ánh sáng này khác nhau thông qua ánh sáng được phản xạ để tìm nồng độ oxy trong máu.
- Đo nhịp tim:
    + Mỗi nhịp tim bơm máu qua các mạch máu, làm thay đổi thể tích máu trong mô.
    + Cảm biến MAX30102 phát hiện các dao động này bằng cách đo sự thay đổi trong cường độ ánh sáng hồng ngoại phản xạ từ mô.
    + Thời gian giữa các đỉnh của tín hiệu ánh sáng phản xạ tương ứng với nhịp tim.
## F. Tác giả
   Nguyễn Quang Chiến - 20215533 - [@ztrevah](https://github.com/ztrevah/SpO2)
   
   Võ Anh Dũng - 20215548 - 
