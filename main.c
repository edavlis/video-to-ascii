#include <bits/time.h>
#include <libavcodec/avcodec.h> // For AVCodecContext, decoding
#include <libavcodec/codec.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h> // For AVFormatContext, opening files
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h> // For image utilities (allocations, etc)
#include <libavutil/mem.h>
#include <libswscale/swscale.h>
#include <libswscale/version.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// get time or something like that
long current_time_milliseconds() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

void sleep_ms(long milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

int main(int argc, char *argv[]) {

  // different character sets
  char *charset;
  char *charset1 = "`^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCL";
  char *charset2 =
      " `^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
  char *charset3 =
      "     `^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
  char *charset4 =
      "          "
      "`^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
  char *charset5 =
      "                      "
      "`^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
  char *charset6 =
      "                                "
      "`^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
  char *charset7 =
      "                                                         "
      "`^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
  char *charset8 =
      "                                                                        "
      "         "
      "`^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
  char *charset9 = "                                                           "
                   "                                 ";
  int charset_length = strlen(charset) - 1;
  int ascii_factor = atoi(argv[2]);

  switch (abs(atoi(argv[2]))) {
  case 1:
    charset = charset1;
    break;
  case 2:
    charset = charset2;
    break;
  case 3:
    charset = charset3;
    break;
  case 4:
    charset = charset4;
    break;
  case 5:
    charset = charset5;
    break;
  case 6:
    charset = charset6;
    break;
  case 7:
    charset = charset7;
    break;
  case 8:
    charset = charset8;
    break;
  case 9:
    charset = charset9;
    break;
  }

  // termios stuff
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int terminal_height = w.ws_row - 1;
  int terminal_width = w.ws_col;

  // ffmpeg initialisation stuff
  AVFormatContext *fmt_ctx = NULL;
  AVCodecContext *cdc_ctx = NULL;
  const AVCodec *codec = NULL;

  AVPacket *packet = NULL;
  AVFrame *frame = NULL;

  int video_stream_index;

  avformat_open_input(&fmt_ctx, argv[1], NULL, NULL);
  avformat_find_stream_info(fmt_ctx, NULL);

  for (int i = 0; i < fmt_ctx->nb_streams; i++) {
    if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_index = i;
    }
  }
	float frames_per_second = (float)(fmt_ctx->streams[video_stream_index]->avg_frame_rate.num) /  (float)(fmt_ctx->streams[video_stream_index]->avg_frame_rate.den);
	int milliseconds_per_frame = (float)(1000 / frames_per_second); // int becuase fractions of a milisecond dont matter
  codec = avcodec_find_decoder(
      fmt_ctx->streams[video_stream_index]->codecpar->codec_id);

  cdc_ctx = avcodec_alloc_context3(codec);
  avcodec_parameters_to_context(cdc_ctx,
                                fmt_ctx->streams[video_stream_index]->codecpar);

  avcodec_open2(cdc_ctx, codec, NULL);

  packet = av_packet_alloc();
  frame = av_frame_alloc();

  int frame_width;
  int frame_height;
  size_t frame_character_buffer_size = terminal_height * terminal_width * 12;
  char *frame_character_buffer = malloc(frame_character_buffer_size);
  char *ptr = frame_character_buffer;

  // frame  -> ascii
  while (av_read_frame(fmt_ctx, packet) >= 0) {

    if (packet->stream_index == video_stream_index) {
      avcodec_send_packet(cdc_ctx, packet);

      while (avcodec_receive_frame(cdc_ctx, frame) == 0) {

        // resizing to terminal size
        frame_width = frame->width;
        frame_height = frame->height;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int terminal_height = w.ws_row - 1;
        int terminal_width = w.ws_col;
        // if frame is too tall resize
        if (frame_height > terminal_height) {
          long start_time = current_time_milliseconds();
          int resized_terminal_width = (int)floor(((float)terminal_height / frame_height) * frame_width);
          struct SwsContext *sws_ctx = sws_getContext( frame_width, frame_height, frame->format, resized_terminal_width, terminal_height, AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
          AVFrame *resized_Frame = av_frame_alloc();
          resized_Frame->format = AV_PIX_FMT_RGB24;
          resized_Frame->height = terminal_height;
          resized_Frame->width = resized_terminal_width;
          av_frame_get_buffer(resized_Frame, 32);
          sws_scale(sws_ctx, (const uint8_t *const *)frame->data, frame->linesize, 0, frame_height, resized_Frame->data, resized_Frame->linesize);

          // magically read RGB values or wtvr

          uint8_t *data = resized_Frame->data[0];
          int linesize = resized_Frame->linesize[0];

          ptr = frame_character_buffer;

          for (int z = 0; z < resized_Frame->height; z++) {
            uint8_t *row = data + z * linesize;
            for (int x = 0; x < resized_Frame->width; x++) {
              uint8_t r = row[x * 3 + 0];
              uint8_t g = row[x * 3 + 1];
              uint8_t b = row[x * 3 + 2];
              int average_brightness = (float)(r + g + b) / 3;
              int character_index =
                  round((float)((float)average_brightness / 256.0) *
                        (charset_length));
              if (ascii_factor < 0) {
                *ptr++ = charset[character_index];
              } else {
                ptr += sprintf(ptr, "\033[48;2;%d;%d;%dm%c", r, g, b,
                               charset[character_index]);
              }
            }
            ptr += sprintf(ptr, "\033[0m");
            *ptr++ = '\n'; // new line of the frame
          }
          *ptr = '\0';
          write(STDOUT_FILENO, frame_character_buffer,
                ptr - frame_character_buffer);
          fflush(stdout);
          long end_time = current_time_milliseconds();
					if ((end_time - start_time) < milliseconds_per_frame) {
						sleep_ms(milliseconds_per_frame - (end_time - start_time));
					}
          printf("\033[H"); // new frame

          sws_freeContext(sws_ctx);
          av_frame_free(&resized_Frame);
        }
      }
    }
    av_packet_unref(packet);
  }
  av_frame_free(&frame);
  av_packet_free(&packet);
  avcodec_free_context(&cdc_ctx);
  avformat_close_input(&fmt_ctx);
  return 0;
}
