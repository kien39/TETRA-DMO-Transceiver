# tetraDMO-Transmitter

1. USER_INTERFACE
Thiết kế trên GNURadio, đầu vào là mic. 
Sử dụng QT GUI Push Button làm PTT button
Cho phép chọn TalkGroup

2. AUDIO_SOURCE:
Có thể dùng push_button variable để điều khiển audio_source lúc này mới bắt đầu hoạt động ? Nếu không thì cũng sẽ điều khiển để source encoder chỉ hoạt động khi có push_button ON.

3. PlutoTx_SINK
Tương tự như vậy, có thể dùng push_button để điều khiển pluto_sink lúc này mới bắt đầu hoạt động ? PlutoTx chỉ transmit khi có các burst dữ liệu được gửi đến, theo tín hiệu BURST ACTIVE hay là sẽ để phát liên tục, nếu input = zero thì tự động không phát. Còn module IQ Encoder / IQ Mapping sẽ phải chịu trách nhiệm việc điều chế tín hiệu theo các bursts.

4. WORKFLOW
Khi nút PTT được bấm:
start Frame Counter
 gửi đi DMAC_call_setup bursts, và
 gửi đi các speech bursts ở TN1
 Nếu là các burst 6, 12 thì gửi thêm DSB ở TN3
 Nếu là burst 18 thì gửi Dmac-Sync

## DMO GroupCall setup sequence 

## DMO GroupCall terminate sequence

## Block Diagram  
![image](https://github.com/user-attachments/assets/4baf6ebf-0444-4bd7-bf9d-24bff403059f)

pi4DQPSK Modulator:
Đã triển khai ok trên python block.
- Qua kiểm tra thực tế, máy thu Tetra chỉ quan tâm decode tín hiệu mà không quan tâm đến khe thời gian hay tín hiệu nhiễu gì cả. Nếu để điều chế IQ liên tục, và phát đi cả các inactive burst (như tín hiệu TMO) thì máy bộ đàm tetra vẫn thu bình thường. Như vậy máy thu Tetra sau khi đồng bộ được thời gian với nguồn phát, thì chỉ quan tâm đến khe thời gian của mình và mặc kệ các đoạn tín hiệu bên ngoài có nhiễu hay không nhiễu
- các tín hiệu guard symbols có thể để hoặc cắt đi cũng không ảnh hưởng đến khả năng thu
- Có thể điều chế IQ theo từng burst, hoặc điều chế IQ cho cả chuỗi tín hiệu (cả inactive burst), và zerorize các inactive burst, để phát không gây nhiễu.

