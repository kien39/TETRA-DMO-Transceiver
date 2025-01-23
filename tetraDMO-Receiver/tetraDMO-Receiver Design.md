# Thiết kế chương trình tetraDMO-Receiver

## 1. Mục tiêu: 
Một chương trình duy nhất thu tín hiệu từ thiết bị SDR (rtlsdr hoặc pluto sdr) và thực hiện đầy đủ các chức năng giải điều chế pi4dqpsk, decode tín hiệu tetraDMO và đẩy tín hiệu audio ra loa (đồng thời xử lý chỉ bật loa khi nhận được cuộc gọi và tắt loa khi cuộc gọi kết thúc. 

Chỉ thực hiện nhận cuộc gọi nhóm, và hiển thị Talkgroup ID, Caller ID khi nhận cuộc gọi, đồng thời với việc đẩy audio ra loa.

## 2. Lựa chọn ngôn ngữ lập trình và công cụ: 
Sau khi xem xét các lựa chọn tự viết chương trình trên C++, Python, và sử dụng GNURadio, quyết định lựa chọn PA sử dụng GNURadio kết hợp với viết các module OOT để xử lý các chức năng tetra decoder, speech codec decoder và đưa audio ra loa, với các lý do sau đây:
- GNURadio sử dụng flowchart với các block thực hiện các chức năng, có cấu trúc rõ ràng, mang tính trực quan
- Trên GNUradio đã có sẵn các khối pi4 demodulation với signal source cho rtlsdr và plutoSdr, cũng như AudioSink đẩy audio ra loa.
- Phát triển các OOT module trên GNURadio để nâng cao khả năng khai thác, sử dụng GNURadio
- Mặc dù GNURadio chạy python wrapper, nhưng các module viết trên c++, nên đảm bảo về tốc độ xử lý nhanh.

## 3. Thiết kế chương trình:

![image](https://github.com/user-attachments/assets/5c631208-7ae3-4c13-b656-f565dc518c70)

Giao tiếp giữa các module:  
(1) IQ signal stream of complex numbers at sampling rate  
(2) demoded s36kbit stream, each byte is 01 bit, at rate 36k samples per second
    Asynchronous: không có liên kết tỷ lệ giữa tín hiệu input và output 
(3) Tetra ACELP encoded speech frames (216 bits each): vector of 432 byte length, each byte represent 01 bit  
(4) raw speech data stream at rate 8ksps, each data sample is a int16 word  

![image](https://github.com/user-attachments/assets/cf868bfb-4885-44e1-af11-936a3be15f99)

## 4. Triển khai 
tetraDMO-Decoder_v0 triển khai 02 block:
- DmoBurstDetect: kiểm tra phát hiện các DmoBurst, và chuyển các burst này ra khối tiếp theo, với phân loại burst DSB, DNB, DNB_SF, gửi đi trên 520 burst data (510 data bytes + 10 header bytes). 
- serviceLowerMac/UpperMac: thực hiện giải mã và lọc ra các u-plane speech frame để đưa sang speech decoder.
- Các khối TetraACELP Channel Decoder và TetraACELP Speech Decoder là dummy. Dự kiến là các sync blocks.
- Button Push_to_view để đưa dữ liệu ra theo từng burst, phục vụ việc kiểm tra. Khi mỗi burst được xử lý, thông số loại burst được hiển thị trong cửa số dòng lệnh GR. Có thể thử chạy với file dữ liệu s36kBit đầu ra của điều chế pi4dqpsk.
 
![image](https://github.com/user-attachments/assets/cd6743c3-7b71-4c6a-bae9-42495960ee41)

tetraDMO-Decoder_v1 triển khai đầy đủ cả 04 khối, đầu vào là file s36kBit và đầu ra là tín hiệu audio được giải mã và đẩy ra loa máy tính.

![image](https://github.com/user-attachments/assets/f829d29e-be18-44f3-ad71-301e5fca82b4)

Để gửi signaling data giữa các block, extra bits được sử dụng.
- DmoBurstDetect dùng 2 byte cuối (thực chất chỉ dùng 1 byte) để xác định loại burst 
- serviceMac dùng 1 bit cuối để xác định burst có Frame Stealing hay không.

Cần kiểm tra xem việc xử lý frame stealing của tín hiệu audio đã đúng chưa (khi frame stealing của 1/2 burst, còn 1/2 burst còn lại là voice traffic, thì vocice này có được đưa sang speech decoder để decode không ? 
