inpired by this [tweet](https://x.com/gen_intuition/status/1978823244338659498)

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

![Original Video](./assets/wake-the-fuck-up.gif)


Ascii video with colour

![Ascii video](./assets/we-got-a-city-to-burn.gif)


Works best if your terminal emulator is fast (eg alacritty)

    Known issues:
        Text being the wrong in colour in kitty terminal 

Personal project to learn ffmpeg's libav
