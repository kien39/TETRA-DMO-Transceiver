# DMOdecoder
Fork from tetra-kit decoder to include option to decode DMO tetra signal from file 

Sửa đổi chương trình decoder để giải mã tín hiệu tetra DMO từ file bitstream nhận từ dòng lệnh.

## Chạy chương trình với command line option -D để chạy DMO mode.
./decoder -D -d n -i /path/filname
options:  -d n với n=1, 4 để các level debug khác nhau (sẽ output dữ liệu ra màn hình)

compile bằng lệnh make với makefile default đã có sẵn.

Các file dữ liệu mẫu để test:
RxDMO_pluto_390mhz_s36kBit.uint8
plutoDMO_bits.uint8
rtlsdrTMO_bits.uint8

khi chạy decoder, speech audio data sẽ được ghi ra file .out với tên file được generate tự động, và lưu vào subfolder /audio. 
File .out này là các tetra audio frame (tương ứng với 30ms), cần chạy Tetra speech decoder để chuyển đổi sang file .wav bằng chương trình out2wave.sh trong tetra-kit/recorder

## Các việc đã làm
- main.cc sỬa đổi code để thêm option -D
- decoder.cc sửa đổi code khắc phục lỗi khi tín hiệu lose sync
- mac.cc viết code bổ sung thêm các case cho DMO trong
    + rxSymbol: thay đổi function detect frame bằng cách kiểm tra tổng số bit lỗi < mức ngưỡng, thay vì match 100%, phân loại burst DMO
      thành 3 loại DSB, DNB và DNB-SF 
    + serviceLowerMac và serviceUpperMac, bổ sung code xử lý dữ liệu cho DMO
- uPlane.cc bổ sung case xử lý dữ liệu DMO traffic (ngầm hiểu là voice traffic), lấy ra các speech frame và viết ra file .out

## Các việc tồn tại 
- Chưa clean up các đoạn code debug, testing trong quá trình viết code
- chưa xử lý hết các dữ liệu Control Plane ở upperMac và Llc Layers...
- Xử lý dữ liệu Tetra speech trong uPlane đang bỏ sót tình huống DNB-SF burst có bkn1 là signaling, nhưng bkn2 là speech.
- khi chạy decoder từ folder khác không phải current folder, xuất hiện lỗi segmentation fault - khả năng do lỗi của 1 pointer variable nào đó chưa được initialize, nhiều khả năng trong phần code uPlane, nhưng chưa phát hiện ra.

## Mục tiêu tiếp theo 
- Tích hợp phần giải điều chế pi4 dqpsk và phần xử lý âm thanh ra loa vào cùng 1 phần mềm.
- Hoàn thiện thiết kế, cấu trúc chương trình với các object class dữ liệu để làm platform chuẩn  
- Xử lý giải mã cả tín hiệu TMO và DMO 

