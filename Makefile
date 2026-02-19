video-to-ascii: main.c
	gcc main.c -lm -o main `pkg-config --cflags --libs libavformat libavcodec libavutil libswscale`
