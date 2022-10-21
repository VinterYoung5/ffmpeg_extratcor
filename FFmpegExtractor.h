/*
 * Copyright 2012 Michael Chen <omxcodec@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FFMPEG_EXTRACTOR_H_

#define FFMPEG_EXTRACTOR_H_

#include <media/stagefright/foundation/ABase.h>
#include <media/stagefright/MediaExtractor.h>
#include <utils/threads.h>
#include <utils/KeyedVector.h>

#ifdef __cplusplus
extern "C" {
#endif

//#include "config.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

#include "libavutil/avstring.h"
//#include "libavutil/colorspace.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavformat/avformat.h"
//#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
//#include "libavutil/internal.h"
#include "libavcodec/avfft.h"
//#include "libavcodec/xiph.h"
#include "libswresample/swresample.h"
#include "libavcodec/avcodec.h"

#ifdef __cplusplus
}
#endif


const char *MEDIA_MIMETYPE_CONTAINER_FFMPEG = "container/ffmpeg";
const char *MEDIA_MIMETYPE_VIDEO_FFMPEG = "video/ffmpeg";
const char *MEDIA_MIMETYPE_AUDIO_FFMPEG = "audio/ffmpeg";

namespace android {

typedef struct PacketQueue {
    AVPacket flush_pkt;
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    int abort_request;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} PacketQueue;
void packet_queue_init(PacketQueue *q);
void packet_queue_destroy(PacketQueue *q);
void packet_queue_flush(PacketQueue *q);
void packet_queue_end(PacketQueue *q);
void packet_queue_abort(PacketQueue *q);
int packet_queue_put(PacketQueue *q, AVPacket *pkt);
int packet_queue_put_nullpacket(PacketQueue *q, int stream_index);
int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block);
media_status_t initFFmpeg();
void deInitFFmpeg();

struct ABuffer;
struct AMessage;
struct FFmpegSource;

struct FFmpegExtractor : public MediaExtractorPluginHelper {
    explicit FFmpegExtractor(DataSourceHelper *source);

    virtual size_t countTracks();
    virtual MediaTrackHelper *getTrack(size_t index);
    virtual media_status_t getTrackMetaData(AMediaFormat *meta, size_t index, uint32_t flags);

    virtual media_status_t getMetaData(AMediaFormat *meta);

    virtual uint32_t flags() const;
    //virtual const char * name() { return "FFmpegExtractor"; }

protected:
    virtual ~FFmpegExtractor();

private:
    friend struct FFmpegSource;

    struct Track {
        Track *next;
        AMediaFormat *meta;
        uint32_t trackId; //stream index
        uint32_t timescale;
    //yangwen add
    AVStream *mStream;
    PacketQueue *mQueue;
    //yangwen add end
    };

    Track *mFirstTrack, *mLastTrack;
    DataSourceHelper *mDataSource;
    AMediaFormat *mFileMetaData;

//yangwen add
bool mEOF;
AVFormatContext *mFormatCtx;
bool mFFmpegInited;
AVStream *mVideoStream;
AVStream *mAudioStream;
PacketQueue mAudioQ;
PacketQueue mVideoQ;
char mFilename[PATH_MAX];
status_t mInitCheck;
int mAbortRequest;
int64_t mDuration;
int mSeekByBytes;

int initStreams();
void deInitStreams();
static int decode_interrupt_cb(void *ctx);
void initFFmpegDefaultOpts();
int stream_component_open(int stream_index);

//yangwen add endd

    FFmpegExtractor(const FFmpegExtractor &);
    FFmpegExtractor &operator=(const FFmpegExtractor &);

};


struct FFmpegSource : public MediaTrackHelper {
    FFmpegSource(AMediaFormat *format, FFmpegExtractor *extractor, uint32_t trackId, int32_t timeScale);

    virtual media_status_t start();
    virtual media_status_t stop();
    virtual media_status_t getFormat(AMediaFormat *meta);
    virtual media_status_t read(
            MediaBufferHelper **buffer, const ReadOptions *options);

protected:
    virtual ~FFmpegSource();

private:
    friend struct FFmpegExtractor;
    int32_t mTimescale;


    FFmpegExtractor *mExtractor;
    uint32_t mTrackId;

    FFmpegSource(const FFmpegSource &);
    FFmpegSource &operator=(const FFmpegSource &);

};




}  

#endif  // SUPER_EXTRACTOR_H_
