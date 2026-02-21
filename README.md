Simple video to ascii player, written in C

    Requirements:
        - Unix system
        - libavcodec
        - libavformat 
        - libavutil 
        - libswscale


    Usage:
        ./video-to-ascii <path to video> <text factor>
        text factor ranges from -9 to 9, 9 (or -9) has little text in the video, 1 (or -1) has lots of text.
        negtive text factors will play the video without colour

Original video
![Original Video](./assets/example-normal-video.webm)

Ascii video (watch in fullscreen to see text)
![Ascii video](./assets/example-ascii-video.mp4)
