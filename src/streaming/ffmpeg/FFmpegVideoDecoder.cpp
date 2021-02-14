#include "FFmpegVideoDecoder.hpp"
#include "Settings.hpp"
#include "Logger.hpp"
#include "AVFrameHolder.hpp"

// Disables the deblocking filter at the cost of image quality
#define DISABLE_LOOP_FILTER 0x1
// Uses the low latency decode flag (disables multithreading)
#define LOW_LATENCY_DECODE 0x2

#define DECODER_BUFFER_SIZE 92 * 1024 * 2

#include <switch.h>
#include <unistd.h>

class ThreadLockWatcher {
public:
    void start() {
        std::lock_guard<std::mutex> lock(m_mutex);
        is_alive = true;
        prev = current = 0;
        pthread_create(&thread, NULL, entry, this);
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        is_alive = false;
    }
    
    void enter(int line) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        last_line = line;
        prev = current;
        current++;
    }
    
private:
    bool is_alive = false;
    u64 prev = 0;
    u64 current = 0;
    int last_line = 0;
    
    static void* entry(void* context) {
        while (true) {
            if (auto watcher = static_cast<ThreadLockWatcher *>(context)) {
                if (watcher->is_alive) {
                    if (watcher->current != 0) {
                        std::lock_guard<std::mutex> lock(watcher->m_mutex);
                        
                        if (watcher->prev > watcher->current) {
                            Logger::error("ThreadLockWatcher", "Thread Lock Detected on a line: %i", watcher->last_line);
                        } else {
                            watcher->prev++;
                        }
                    }
                    usleep(500'000);
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        }
        return NULL;
    }
    
    std::mutex m_mutex;
    pthread_t thread;
};

static ThreadLockWatcher watcher;
#define DBG() watcher.enter(__LINE__);

FFmpegVideoDecoder::FFmpegVideoDecoder() {
    //pthread_mutex_init(&m_mutex, NULL);
    watcher.start();
}

FFmpegVideoDecoder::~FFmpegVideoDecoder() {
    watcher.stop();
    //pthread_mutex_destroy(&m_mutex);
}

int FFmpegVideoDecoder::setup(int video_format, int width, int height, int redraw_rate, void *context, int dr_flags) {
    m_stream_fps = redraw_rate;
    
    Logger::info("FFmpeg", "Setup with format: %s, width: %i, height: %i, fps: %i", video_format == VIDEO_FORMAT_H264 ? "H264" : "HEVC", width, height, redraw_rate);
    
    av_log_set_level(AV_LOG_QUIET);
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58,10,100)
    avcodec_register_all();
#endif
    
    av_init_packet(&m_packet);
    
    int perf_lvl = LOW_LATENCY_DECODE;
    
    switch (video_format) {
        case VIDEO_FORMAT_H264:
            m_decoder = avcodec_find_decoder_by_name("h264");
            break;
        case VIDEO_FORMAT_H265:
            m_decoder = avcodec_find_decoder_by_name("hevc");
            break;
    }
    
    if (m_decoder == NULL) {
        Logger::error("FFmpeg", "Couldn't find decoder");
        return -1;
    }
    
    m_decoder_context = avcodec_alloc_context3(m_decoder);
    if (m_decoder_context == NULL) {
        Logger::error("FFmpeg", "Couldn't allocate context");
        return -1;
    }
    
    if (perf_lvl & DISABLE_LOOP_FILTER)
        // Skip the loop filter for performance reasons
        m_decoder_context->skip_loop_filter = AVDISCARD_ALL;
    
    if (perf_lvl & LOW_LATENCY_DECODE)
        // Use low delay single threaded encoding
        m_decoder_context->flags |= AV_CODEC_FLAG_LOW_DELAY;
    
    int decoder_threads = Settings::settings()->decoder_threads();
    
    if (decoder_threads == 0) {
        m_decoder_context->thread_type = FF_THREAD_FRAME;
    } else {
        m_decoder_context->thread_type = FF_THREAD_SLICE;
        m_decoder_context->thread_count = decoder_threads;
    }
    
    m_decoder_context->width = width;
    m_decoder_context->height = height;
    m_decoder_context->pix_fmt = AV_PIX_FMT_YUV420P;
    
    int err = avcodec_open2(m_decoder_context, m_decoder, NULL);
    if (err < 0) {
        Logger::error("FFmpeg", "Couldn't open codec");
        return err;
    }
    
    m_frames_count = 2;
    m_frames = (AVFrame**)malloc(m_frames_count * sizeof(AVFrame*));
    if (m_frames == NULL) {
        Logger::error("FFmpeg", "Couldn't allocate frames");
        return -1;
    }
    
    for (int i = 0; i < m_frames_count; i++) {
        m_frames[i] = av_frame_alloc();
        if (m_frames[i] == NULL) {
            Logger::error("FFmpeg", "Couldn't allocate frame");
            return -1;
        }
    }
    
    m_ffmpeg_buffer = (char*)malloc(DECODER_BUFFER_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
    if (m_ffmpeg_buffer == NULL) {
        Logger::error("FFmpeg", "Not enough memory");
        cleanup();
        return -1;
    }
    
    Logger::info("FFmpeg", "Setup done!");
    
    return DR_OK;
}

void FFmpegVideoDecoder::cleanup() {
    Logger::info("FFmpeg", "Cleanup...");
    
    if (m_decoder_context) {
        avcodec_close(m_decoder_context);
        av_free(m_decoder_context);
        m_decoder_context = NULL;
    }
    
    if (m_frames) {
        for (int i = 0; i < m_frames_count; i++) {
            if (m_frames[i])
                av_frame_free(&m_frames[i]);
        }
        
        free(m_frames);
        m_frames = nullptr;
    }
    
    if (m_ffmpeg_buffer) {
        free(m_ffmpeg_buffer);
        m_ffmpeg_buffer = nullptr;
    }
    
    Logger::info("FFmpeg", "Cleanup done!");
}

int FFmpegVideoDecoder::submit_decode_unit(PDECODE_UNIT decode_unit) {
    DBG();
    if (decode_unit->fullLength < DECODER_BUFFER_SIZE) {
        PLENTRY entry = decode_unit->bufferList;
        DBG();
        if (!m_last_frame) {
            m_video_decode_stats.measurement_start_timestamp = LiGetMillis();
            m_last_frame = decode_unit->frameNumber;
            DBG();
        }
        else {
            // Any frame number greater than m_LastFrameNumber + 1 represents a dropped frame
            m_video_decode_stats.network_dropped_frames += decode_unit->frameNumber - (m_last_frame + 1);
            m_video_decode_stats.total_frames += decode_unit->frameNumber - (m_last_frame + 1);
            m_last_frame = decode_unit->frameNumber;
            DBG();
        }
        
        m_video_decode_stats.received_frames++;
        m_video_decode_stats.total_frames++;
        DBG();
        
        int length = 0;
        while (entry != NULL) {
            if (length > DECODER_BUFFER_SIZE) {
                Logger::error("FFmpeg", "Big buffer to decode... !");
            }
            
            DBG();
            memcpy(m_ffmpeg_buffer + length, entry->data, entry->length);
            length += entry->length;
            entry = entry->next;
            DBG();
        }
        
        DBG();
        m_video_decode_stats.total_reassembly_time += LiGetMillis() - decode_unit->receiveTimeMs;
        
        m_frames_in++;
        
        uint64_t before_decode = LiGetMillis();
        
        if (length > DECODER_BUFFER_SIZE) {
            Logger::error("FFmpeg", "Big buffer to decode...");
        }
        
        DBG();
        if (decode(m_ffmpeg_buffer, length) == 0) {
            m_frames_out++;
            DBG();
            m_video_decode_stats.total_decode_time += LiGetMillis() - before_decode;
            
            // Also count the frame-to-frame delay if the decoder is delaying frames
            // until a subsequent frame is submitted.
            m_video_decode_stats.total_decode_time += (m_frames_in - m_frames_out) * (1000 / m_stream_fps);
            m_video_decode_stats.decoded_frames++;
            DBG();
            //if (pthread_mutex_lock(&m_mutex) == 0) {
                m_frame = get_frame(true);
                AVFrameHolder::holder()->push(m_frame);
            DBG();//
                // Push event!!
                //pthread_mutex_unlock(&m_mutex);
            //}
        }
    } else {
        Logger::error("FFmpeg", "Big buffer to decode... 2");
    }
    return DR_OK;
}

int FFmpegVideoDecoder::capabilities() const {
    return CAPABILITY_SLICES_PER_FRAME(4) | CAPABILITY_DIRECT_SUBMIT;
}

int FFmpegVideoDecoder::decode(char* indata, int inlen) {
    m_packet.data = (uint8_t *)indata;
    m_packet.size = inlen;
    DBG();
    int err = avcodec_send_packet(m_decoder_context, &m_packet);
    DBG();
    if (err != 0) {
        DBG();
        char error[512];
        av_strerror(err, error, sizeof(error));
        Logger::error("FFmpeg", "Decode failed - %s", error);
        DBG();
    }
    DBG();
    return err != 0 ? err : 0;
}

AVFrame* FFmpegVideoDecoder::get_frame(bool native_frame) {
    DBG();
    int err = avcodec_receive_frame(m_decoder_context, m_frames[m_next_frame]);
    DBG();
    if (err == 0) {
        DBG();
        m_current_frame = m_next_frame;
        m_next_frame = (m_current_frame + 1) % m_frames_count;
        DBG();
        if (/*ffmpeg_decoder == SOFTWARE ||*/ native_frame)
            return m_frames[m_current_frame];
    } else if (err != AVERROR(EAGAIN)) {
        DBG();
        char error[512];
        av_strerror(err, error, sizeof(error));
        DBG();
        Logger::error("FFmpeg", "Receive failed - %d/%s", err, error);
    }
    return NULL;
}

AVFrame* FFmpegVideoDecoder::frame() const {
    return m_frame;
}

VideoDecodeStats* FFmpegVideoDecoder::video_decode_stats() {
    uint64_t now = LiGetMillis();
    m_video_decode_stats.total_fps = (float)m_video_decode_stats.total_frames / ((float)(now - m_video_decode_stats.measurement_start_timestamp) / 1000);
    m_video_decode_stats.received_fps = (float)m_video_decode_stats.received_frames / ((float)(now - m_video_decode_stats.measurement_start_timestamp) / 1000);
    m_video_decode_stats.decoded_fps = (float)m_video_decode_stats.decoded_frames / ((float)(now - m_video_decode_stats.measurement_start_timestamp) / 1000);
    return (VideoDecodeStats*)&m_video_decode_stats;
}
