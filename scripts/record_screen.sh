ffmpeg \
	-video_size 1920x1080 \
	-framerate 30 \
	-f x11grab \
	-i :0.0 \
	-f pulse \
	-i bluez_output.98_47_44_93_A0_67.1.monitor \
	-c:v libx264 \
	-preset veryfast \
	-qp 10 \
	-c:a aac \
	-b:a 128k \
	recording.mp4
