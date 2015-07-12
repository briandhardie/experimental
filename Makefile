all: hdhomerun_control hdhr_record

hdhomerun_control: libhdhomerun hdhomerun_control.cpp
	g++ hdhomerun_control.cpp -I./libhdhomerun/ -lhdhomerun -o hdhomerun_control  

hdhr_record: libhdhomerun hdhr_record.cpp
	g++ hdhr_record.cpp -I./libhdhomerun/ -lhdhomerun -o hdhr_record

.PHONY: libhdhomerun
libhdhomerun:
	$(MAKE) -C libhdhomerun

.PHONY: clean
clean:
	rm -f hdhomerun_control hdhr_record

