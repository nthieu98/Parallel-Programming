* Yêu cầu:
- Hệ điều hành Ubuntu
- GCC, MPI
- Python 3

* Cài đặt
- Cài đặt MPI: sudo apt install mpich
- Cài đặt python 3: sudo apt-get install python3
- Cài đặt pip3: sudo apt-get install -y python3-pip
- Cài đặt các gói cho python 3: pip3 install -r requirements.txt

* Cầu hình khởi tạo cho chương trình
- Chương trình được nhập cấu hình đầu vào từ: config.txt
- Cấu trúc file config.txt:
	+ Gồm 4 dòng
	+ Dòng 1 là số nguyên n - kích thước của lưới (n chia hết cho số luồng)
	+ Dòng 2 là số nguyên numIter - số vòng lặp
	+ Dòng 3 là số thực eta - hệ số phát triển (eta có 2 chữ số thập phân)
	+ Dòng 4 là số thực omega - hệ số trộn của SOR (omega có 2 chữ số thập phân)
	+ Dòng 5 là số thực tolerance - điều kiện dừng của chương trình
	
* Biên dịch, thực thi
- File chương trình: DLA.c
	+ Lệnh biên dịch file: mpicc DLA.c -lm
	+ Lệnh thực thi: mpirun -np 4 ./a.out
	+ Log của chương trình sẽ lưu lại sau mỗi 10 vòng lặp và được đặt tại thư mục ./log
- File mô phỏng: visual.py
	+ Lệnh thực thi: python3 visual.py
	+ Hình ảnh sẽ được tạo ra và lưu trư trong thư mục ./image
 	+ Video sẽ được tạo ra và lưu trữ tại ./video
Hoặc:
- Sử dụng file run.sh bao gồm các bước trên
- Lệnh thực thi: ./run.sh
 
