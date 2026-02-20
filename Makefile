video-to-ascii: main.c
	gcc main.c -lm -o video-to-ascii `pkg-config --cflags --libs libavformat libavcodec libavutil libswscale`
