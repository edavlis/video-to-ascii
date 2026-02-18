#include <libavutil/mem.h> 
#include <libswscale/swscale.h>
#include <libswscale/version.h>
#include <stdint.h>
#include <termios.h>
#include <libavcodec/codec.h>
#include <sys/ioctl.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>  // For AVFormatContext, opening files
#include <libavcodec/avcodec.h>    // For AVCodecContext, decoding
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>    // For image utilities (allocations, etc)
#include <math.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int terminal_height = w.ws_row;
	int terminal_width = w.ws_col;

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

	codec = avcodec_find_decoder(fmt_ctx->streams[video_stream_index]->codecpar->codec_id);

	cdc_ctx = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(cdc_ctx, fmt_ctx->streams[video_stream_index]->codecpar);

	avcodec_open2(cdc_ctx, codec, NULL);

	packet = av_packet_alloc();
	frame = av_frame_alloc();

	int frame_width;
	int frame_height;


	while (av_read_frame(fmt_ctx, packet) >= 0) {
	
			if (packet->stream_index == video_stream_index) {
				avcodec_send_packet(cdc_ctx, packet);
					
					while (avcodec_receive_frame(cdc_ctx, frame ) == 0)  {

					/////////////////////// alll the logic and shit goes HERE nigga!!!!!!

					// resizing to terminal size
						frame_width = frame->width;
						frame_height = frame->height;
						ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
						int terminal_height = w.ws_row;
						int terminal_width = w.ws_col;
						
						// if frame is too tall resize
						if (frame_height > terminal_height) {
							
							int resized_terminal_width = (int) floor(((float) terminal_height / frame_height) * frame_width);
							struct SwsContext *sws_ctx = sws_getContext(frame_width,frame_height,frame->format,resized_terminal_width,terminal_height,AV_PIX_FMT_RGB24,SWS_FAST_BILINEAR, NULL, NULL, NULL);
							AVFrame *resized_Frame = av_frame_alloc();
							resized_Frame->format = AV_PIX_FMT_RGB24;
							resized_Frame->height = terminal_height;
							resized_Frame->width = resized_terminal_width;
							av_frame_get_buffer(resized_Frame,32);
							sws_scale(sws_ctx, (const uint8_t *const *)frame->data,frame->linesize,0,frame_height,resized_Frame->data,resized_Frame->linesize);

							printf("Decoded format: %s\n", av_get_pix_fmt_name(frame->format));
							printf("(PASS 1) %dx%d\n",  resized_Frame->width,resized_Frame->height);
							

							// showtime, image processing
							

							

							// magically read RGB values or wtvr

								uint8_t *data = resized_Frame->data[0];
								int linesize = resized_Frame->linesize[0];

								for (int z = 0; z < resized_Frame->height; z++) {
									uint8_t *row = data + z * linesize;
									for (int x = 0; x < resized_Frame->width; x++) {
										uint8_t r = row[x * 3 + 0];
										uint8_t g = row[x * 3 + 1];
										uint8_t b = row[x * 3 + 2];
										printf("\033[48;2;%d;%d;%dm \033[0m",r,g,b);
								}
									printf("\n");
//									printf("\033[H");

						}
						
							sws_freeContext(sws_ctx);
							av_frame_free(&resized_Frame);
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
}
