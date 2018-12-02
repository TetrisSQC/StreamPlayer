

const
 MAX_REORDER_DELAY =16;

 AVFMT_TS_DISCONT  =$0200; //**< Format allows timestamp discontinuities. Note, muxers always require valid (monotone) timestamps */

  AVSEEK_FLAG_BACKWARD = 1; ///< seek backward
  AVSEEK_FLAG_BYTE     = 2; ///< seeking based on position in bytes
  AVSEEK_FLAG_ANY      = 4; ///< seek to any frame, even non key-frames
  AVSEEK_FLAG_FRAME    = 8; ///< seeking based on frame number


type

  PAVPacketList = ^TAVPacketList;
  TAVPacketList = record
    pkt: TAVPacket;
    next: PAVPacketList;
  end;


  PAVStreamInternal = ^TAVStreamInternal;
  TAVStreamInternal = record
    // need {$ALIGN 8}
  end;

(**
 * The duration of a video can be estimated through various ways, and this enum can be used
 * to know how the duration was estimated.
 *)
 TAVDurationEstimationMethod = (
    AVFMT_DURATION_FROM_PTS,    ///< Duration accurately estimated from PTSes
    AVFMT_DURATION_FROM_STREAM, ///< Duration estimated from a stream with a known duration
    AVFMT_DURATION_FROM_BITRATE ///< Duration estimated from bitrate (less accurate)
 );

 TAVStreamParseType = (
    AVSTREAM_PARSE_NONE,
    AVSTREAM_PARSE_FULL,       (**< full parsing and repack *)
    AVSTREAM_PARSE_HEADERS,    (**< Only parse headers, do not repack. *)
    AVSTREAM_PARSE_TIMESTAMPS, (**< full parsing and interpolation of timestamps for frames not starting on a packet boundary *)
    AVSTREAM_PARSE_FULL_ONCE,  (**< full parsing and repack of the first frame only, only implemented for H.264 currently *)
//    AVSTREAM_PARSE_FULL_RAW=MKTAG(0,'R','A','W'),
    (**< full parsing and repack with timestamp and position generation by parser for raw
     this assumes that each packet in the file contains no demuxer level headers and
     just codec level data, otherwise position generation would fail *)
    AVSTREAM_PARSE_FULL_RAW=(0 or (Ord('R') shl 8) or (Ord('A') shl 16) or (Ord('W') shl 24))
);

 PAVIndexEntry = ^TAVIndexEntry;
 TAVIndexEntry =record
    pos: int64;
    timestamp:int64;        (**<
                               * Timestamp in AVStream.time_base units, preferably the time from which on correctly decoded frames are available
                               * when seeking to this entry. That means preferable PTS on keyframe based formats.
                               * But demuxers can choose to store a different timestamp, if it is more convenient for the implementation or nothing better
                               * is known
                               *)
//#define AVINDEX_KEYFRAME 0x0001
//#define AVINDEX_DISCARD_FRAME  0x0002
                                   (**
                                          * Flag is used to indicate which frame should be discarded after decoding.
                                    *)
//    int flags:2;
//    int size:30; //Yeah, trying to keep the size of this small to reduce memory requirements (it is 24 vs. 32 bytes due to possible 8-byte alignment).
    flags_size:integer;
    min_distance:integer;         (**< Minimum distance between this and the previous keyframe, used to avoid unneeded searching. *)
 end; // AVIndexEntry;


(**
 * This structure contains the data a format has to probe a file.
 *)

  PAVProbeData = ^TAVProbeData;
  TAVProbeData = record
    filename: PAnsiChar;
    buf: PByte;      (**< Buffer must have AVPROBE_PADDING_SIZE of extra allocated bytes filled with zero. *)
    buf_size: Integer;   (**< Size of buf except extra allocated bytes *)
    mime_type: PAnsiChar;  (**< mime_type, when known. *)
  end;




{$ifdef FF_API_LAVF_FRAC}
(*************************************************)
(* fractional numbers for exact pts handling *)

(**
 * The exact value of the fractional number is: 'val + num / den'.
 * num is assumed to be 0 <= num < den.
 *)

 TAVFrac =record
    val,num,den:int64;
 end; // AVFrac;
{$endif}



  PPAVCodec = ^PAVCodec;
  PAVCodec = Pointer;

  PAVFormatInternal= Pointer;

  PAVDeviceInfoList = Pointer;  // defined in libavdevice/avdevice.h
  PAVDeviceCapabilitiesQuery = Pointer; // defined in libavdevice/avdevice.h


  PPAVCodecTag = ^PAVCodecTag;
  PAVCodecTag = Pointer;

  PPAVStream=^PAVStream;
  PAVStream=^TAVStream;

(**
 * Stream structure.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVStream) must not be used outside libav*.
 *)
  TAVStream = record
    index:integer;    (**< stream index in AVFormatContext *)
    (**
     * Format-specific stream ID.
     * decoding: set by libavformat
     * encoding: set by the user, replaced by libavformat if left unset
     *)
    id: integer;
{$ifdef FF_API_LAVF_AVCTX}
    (**
     * @deprecated use the codecpar struct instead
     *)
//    attribute_deprecated
    codec:PAVCodecContext;
{$endif}
    priv_data:Pointer;

{$ifdef FF_API_LAVF_FRAC}
    (**
     * @deprecated this field is unused
     *)
//    attribute_deprecated
    pts:TAVFrac;
{$endif}

    (**
     * This is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented.
     *
     * decoding: set by libavformat
     * encoding: May be set by the caller before avformat_write_header() to
     *           provide a hint to the muxer about the desired timebase. In
     *           avformat_write_header(), the muxer will overwrite this field
     *           with the timebase that will actually be used for the timestamps
     *           written into the file (which may or may not be related to the
     *           user-provided one, depending on the format).
     *)
    time_base:TAVRational;

    (**
     * Decoding: pts of the first frame of the stream in presentation order, in stream time base.
     * Only set this if you are absolutely 100% sure that the value you set
     * it to really is the pts of the first frame.
     * This may be undefined (AV_NOPTS_VALUE).
     * @note The ASF header does NOT contain a correct start_time the ASF
     * demuxer must NOT set this.
     *)
    start_time: Int64;

    (**
     * Decoding: duration of the stream, in stream time base.
     * If a source file does not specify a duration, but does specify
     * a bitrate, this value will be estimated from bitrate and file size.
     *
     * Encoding: May be set by the caller before avformat_write_header() to
     * provide a hint to the muxer about the estimated duration.
     *)
    duration: Int64;

    nb_frames: Int64;                 ///< number of frames in this stream if known or 0

    disposition: Integer; (**< AV_DISPOSITION_* bit field *)

    discard: TAVDiscard; ///< Selects which packets can be discarded at will and do not need to be demuxed.

    (**
     * sample aspect ratio (0 if unknown)
     * - encoding: Set by user.
     * - decoding: Set by libavformat.
     *)
    sample_aspect_ratio:TAVRational;

    metadata:PAVDictionary;

    (**
     * Average framerate
     *
     * - demuxing: May be set by libavformat when creating the stream or in
     *             avformat_find_stream_info().
     * - muxing: May be set by the caller before avformat_write_header().
     *)
    avg_frame_rate: TAVRational;

    (**
     * For streams with AV_DISPOSITION_ATTACHED_PIC disposition, this packet
     * will contain the attached picture.
     *
     * decoding: set by libavformat, must not be modified by the caller.
     * encoding: unused
     *)
    attached_pic: TAVPacket;

    (**
     * An array of side data that applies to the whole stream (i.e. the
     * container does not allow it to change between packets).
     *
     * There may be no overlap between the side data in this array and side data
     * in the packets. I.e. a given side data is either exported by the muxer
     * (demuxing) / set by the caller (muxing) in this array, then it never
     * appears in the packets, or the side data is exported / sent through
     * the packets (always in the first packet where the value becomes known or
     * changes), then it does not appear in this array.
     *
     * - demuxing: Set by libavformat when the stream is created.
     * - muxing: May be set by the caller before avformat_write_header().
     *
     * Freed by libavformat in avformat_free_context().
     *
     * @see av_format_inject_global_side_data()
     *)
    side_data:PAVPacketSideData;
    (**
     * The number of elements in the AVStream.side_data array.
     *)
    nb_side_data: Integer;

    (**
     * Flags for the user to detect events happening on the stream. Flags must
     * be cleared by the user once the event has been handled.
     * A combination of AVSTREAM_EVENT_FLAG_*.
     *)
    event_flags:integer;
//#define AVSTREAM_EVENT_FLAG_METADATA_UPDATED 0x0001 ///< The call resulted in updated metadata.

    (*****************************************************************
     * All fields below this line are not part of the public API. They
     * may not be used outside of libavformat and can be changed and
     * removed at will.
     * Internal note: be aware that physically removing these fields
     * will break ABI. Replace removed fields with dummy fields, and
     * add new fields to AVStreamInternal.
     *****************************************************************
     *)

    (**
     * Stream information used internally by avformat_find_stream_info()
     *)
//#define MAX_STD_TIMEBASES (30*12+30+3+6)
    info: Pointer;

//    struct {
//        int64_t last_dts;
//        int64_t duration_gcd;
//        int duration_count;
//        int64_t rfps_duration_sum;
//        double (*duration_error)[2][MAX_STD_TIMEBASES];
//        int64_t codec_info_duration;
//        int64_t codec_info_duration_fields;

        (**
         * 0  -> decoder has not been searched for yet.
         * >0 -> decoder found
         * <0 -> decoder with codec_id == -found_decoder has not been found
         *)
//        int found_decoder;

//        int64_t last_duration;

        (**
         * Those are used for average framerate estimation.
         *)
//        int64_t fps_first_dts;
//        int     fps_first_dts_idx;
//        int64_t fps_last_dts;
//        int     fps_last_dts_idx;

//    } *info;

    pts_wrap_bits:integer; (**< number of bits in pts (used for wrapping control) *)

    // Timestamp generation support:
    (**
     * Timestamp corresponding to the last dts sync point.
     *
     * Initialized when AVCodecParserContext.dts_sync_point >= 0 and
     * a DTS is received from the underlying container. Otherwise set to
     * AV_NOPTS_VALUE by default.
     *)
    first_dts: Int64;
    cur_dts: Int64;
    last_IP_pts: Int64;
    last_IP_duration: Integer;

    (**
     * Number of packets to buffer for codec probing
     *)
    probe_packets:integer;

    (**
     * Number of frames that have been demuxed during avformat_find_stream_info()
     *)
    codec_info_nb_frames: Integer;

    (* av_read_frame() support *)
//    enum AVStreamParseType need_parsing;
    need_parsing: TAVStreamParseType;

    parser: PAVCodecParserContext;

    (**
     * last packet in packet_buffer for this stream when muxing.
     *)
//     struct AVPacketList *last_in_packet_buffer;
    last_in_packet_buffer: Pointer;

    probe_data: TAVProbeData;

// #define MAX_REORDER_DELAY 16
//    int64_t pts_buffer[MAX_REORDER_DELAY+1];
    pts_buffer:array[0..MAX_REORDER_DELAY] of int64;

    index_entries: PAVIndexEntry; (**< Only used if the format does not
                                    support seeking natively. *)

    nb_index_entries:integer;
    index_entries_allocated_size: Cardinal;

    (**
     * Real base framerate of the stream.
     * This is the lowest framerate with which all timestamps can be
     * represented accurately (it is the least common multiple of all
     * framerates in the stream). Note, this value is just a guess!
     * For example, if the time base is 1/90000 and all frames have either
     * approximately 3600 or 1800 timer ticks, then r_frame_rate will be 50/1.
     *
     * Code outside avformat should access this field using:
     * av_stream_get/set_r_frame_rate(stream)
     *)
    r_frame_rate:TAVRational;

    (**
     * Stream Identifier
     * This is the MPEG-TS stream identifier +1
     * 0 means unknown
     *)
    stream_identifier: Integer;

    interleaver_chunk_size: Int64;
    interleaver_chunk_duration: Int64;

    (**
     * stream probing state
     * -1   -> probing finished
     *  0   -> no probing requested
     * rest -> perform probing with request_probe being the minimum score to accept.
     * NOT PART OF PUBLIC API
     *)
    request_probe: Integer;
    (**
     * Indicates that everything up to the next keyframe
     * should be discarded.
     *)
    skip_to_keyframe: Integer;

    (**
     * Number of samples to skip at the start of the frame decoded from the next packet.
     *)
    skip_samples: Integer;

    (**
     * If not 0, the number of samples that should be skipped from the start of
     * the stream (the samples are removed from packets with pts==0, which also
     * assumes negative timestamps do not happen).
     * Intended for use with formats such as mp3 with ad-hoc gapless audio
     * support.
     *)
    start_skip_samples: Int64;

    (**
     * If not 0, the first audio sample that should be discarded from the stream.
     * This is broken by design (needs global sample count), but can't be
     * avoided for broken by design formats such as mp3 with ad-hoc gapless
     * audio support.
     *)
    first_discard_sample: Int64;

    (**
     * The sample after last sample that is intended to be discarded after
     * first_discard_sample. Works on frame boundaries only. Used to prevent
     * early EOF if the gapless info is broken (considered concatenated mp3s).
     *)
    last_discard_sample: Int64;

    (**
     * Number of internally decoded frames, used internally in libavformat, do not access
     * its lifetime differs from info which is why it is not in that structure.
     *)
    nb_decoded_frames: Integer;

    (**
     * Timestamp offset added to timestamps before muxing
     * NOT PART OF PUBLIC API
     *)
    mux_ts_offset: Int64;

    (**
     * Internal data to check for wrapping of the time stamp
     *)
    pts_wrap_reference: Int64;

    (**
     * Options for behavior, when a wrap is detected.
     *
     * Defined by AV_PTS_WRAP_ values.
     *
     * If correction is enabled, there are two possibilities:
     * If the first time stamp is near the wrap point, the wrap offset
     * will be subtracted, which will create negative time stamps.
     * Otherwise the offset will be added.
     *)
    pts_wrap_behavior: Integer;

    (**
     * Internal data to prevent doing update_initial_durations() twice
     *)
    update_initial_durations_done: Integer;

    (**
     * Internal data to generate dts from pts
     *)
    pts_reorder_error: array[0..MAX_REORDER_DELAY] of Int64;
    pts_reorder_error_count: array[0..MAX_REORDER_DELAY] of Byte;


    (**
     * Internal data to analyze DTS and detect faulty mpeg streams
     *)
    last_dts_for_order_check: Int64;
    dts_ordered: Byte;
    dts_misordered: Byte;

    (**
     * Internal data to inject global side data
     *)
    inject_global_side_data: Integer;

    (*****************************************************************
     * All fields above this line are not part of the public API.
     * Fields below are part of the public API and ABI again.
     *****************************************************************
     *)

    (**
     * String containing paris of key and values describing recommended encoder configuration.
     * Paris are separated by ','.
     * Keys are separated from values by '='.
     *)
    recommended_encoder_configuration: PAnsiChar;

    (**
     * display aspect ratio (0 if unknown)
     * - encoding: unused
     * - decoding: Set by libavformat to calculate sample_aspect_ratio internally
     *)
    display_aspect_ratio: TAVRational;

//    struct FFFrac *priv_pts;
    priv_pts: Pointer;

    (**
     * An opaque field for libavformat internal usage.
     * Must not be accessed in any way by callers.
     *)
    internal: PAVStreamInternal;

    (*
     * Codec parameters associated with this stream. Allocated and freed by
     * libavformat in avformat_new_stream() and avformat_free_context()
     * respectively.
     *
     * - demuxing: filled by libavformat on stream creation or in
     *             avformat_find_stream_info()
     * - muxing: filled by the caller before avformat_write_header()
     *)
    codecpar:PAVCodecParameters;
 end;




  PPAVProgram=^PAVProgram;
  PAVProgram=Pointer;

  PPAVChapter=^PAVChapter;
  PAVChapter=Pointer;

  PPAVFormatContext = ^PAVFormatContext;
  PAVFormatContext = ^TAVFormatContext;

(**
 * Callback used by devices to communicate with application.
 *)
//typedef int (*av_format_control_message)(struct AVFormatContext *s, int type,
//                                         void *data, size_t data_size);

  Tav_format_control_message=function(s:PAVFormatContext;type_:integer;
                                     data:Pointer;data_size:Cardinal):integer;cdecl;


  PPAVInputFormat = ^PAVInputFormat;
  PAVInputFormat = ^TAVInputFormat;
  TAVInputFormat = record
    (**
     * A comma separated list of short names for the format. New names
     * may be appended with a minor bump.
     *)
    name: PAnsiChar;

    (**
     * Descriptive name for the format, meant to be more human-readable
     * than name. You should use the NULL_IF_CONFIG_SMALL() macro
     * to define it.
     *)
    long_name: PAnsiChar;

    (**
     * Can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER, AVFMT_SHOW_IDS,
     * AVFMT_GENERIC_INDEX, AVFMT_TS_DISCONT, AVFMT_NOBINSEARCH,
     * AVFMT_NOGENSEARCH, AVFMT_NO_BYTE_SEEK, AVFMT_SEEK_TO_PTS.
     *)
    flags: Integer;

    (**
     * If extensions are defined, then no probe is done. You should
     * usually not use extension format guessing because it is not
     * reliable enough
     *)
    extensions: PAnsiChar;

//    const struct AVCodecTag * const *codec_tag;
    codec_tag: PPAVCodecTag;

//    const AVClass *priv_class; ///< AVClass for the private context
    priv_class: Pointer;

    (**
     * Comma-separated list of mime types.
     * It is used check for matching mime types while probing.
     * @see av_probe_input_format2
     *)
//    const char *mime_type;
    mime_type:PAnsiChar;

    (*****************************************************************
     * No fields below this line are part of the public API. They
     * may not be used outside of libavformat and can be changed and
     * removed at will.
     * New public fields should be added right above.
     *****************************************************************
     *)
//    struct AVInputFormat *next;
     next:PAVInputFormat;

    (**
     * Raw demuxers store their codec ID here.
     *)
//    int raw_codec_id;
     raw_codec_id:integer;
    (**
     * Size of private data so that it can be allocated in the wrapper.
     *)
//    int priv_data_size;
    priv_data_size:integer;
    (**
     * Tell if a given file has a chance of being parsed as this format.
     * The buffer provided is guaranteed to be AVPROBE_PADDING_SIZE bytes
     * big so you do not have to check for that unless you need more.
     *)
//    int (*read_probe)(AVProbeData *);
    read_probe: function (p:Pointer):integer;cdecl;
    (**
     * Read the format header and initialize the AVFormatContext
     * structure. Return 0 if OK. 'avformat_new_stream' should be
     * called to create new streams.
     *)
//    int (*read_header)(struct AVFormatContext *);
    read_header: function (p:PAVFormatContext):integer;cdecl;

    (**
     * Read one packet and put it in 'pkt'. pts and flags are also
     * set. 'avformat_new_stream' can be called only if the flag
     * AVFMTCTX_NOHEADER is used and only in the calling thread (not in a
     * background thread).
     * @return 0 on success, < 0 on error.
     *         When returning an error, pkt must not have been allocated
     *         or must be freed before returning
     *)
//    int (*read_packet)(struct AVFormatContext *, AVPacket *pkt);
     read_packet: function (p:PAVFormatContext;pkt:Pointer):integer;cdecl;
    (**
     * Close the stream. The AVFormatContext and AVStreams are not
     * freed by this function
     *)
//    int (*read_close)(struct AVFormatContext *);
    read_close: function (p:PAVFormatContext):integer;cdecl;

    (**
     * Seek to a given timestamp relative to the frames in
     * stream component stream_index.
     * @param stream_index Must not be -1.
     * @param flags Selects which direction should be preferred if no exact
     *              match is available.
     * @return >= 0 on success (but not necessarily the new offset)
     *)
//    int (*read_seek)(struct AVFormatContext *,
//                     int stream_index, int64_t timestamp, int flags);
     read_seek: function (p:PAVFormatContext;
                       stream_index:integer;timestamp:int64;flags:integer):integer;cdecl;

    (**
     * Get the next timestamp in stream[stream_index].time_base units.
     * @return the timestamp or AV_NOPTS_VALUE if an error occurred
     *)
//    int64_t (*read_timestamp)(struct AVFormatContext *s, int stream_index,
//                              int64_t *pos, int64_t pos_limit);
     read_timestamp:function(s:PAVFormatContext;stream_index:integer;
                             pos:PInt64;pos_limit:int64):int64;cdecl;
    (**
     * Start/resume playing - only meaningful if using a network-based format
     * (RTSP).
     *)
//    int (*read_play)(struct AVFormatContext *);
    read_play: function (p:PAVFormatContext):integer;cdecl;

    (**
     * Pause playing - only meaningful if using a network-based format
     * (RTSP).
     *)
//    int (*read_pause)(struct AVFormatContext *);
    read_pause: function (p:PAVFormatContext):integer;cdecl;

    (**
     * Seek to timestamp ts.
     * Seeking will be done so that the point from which all active streams
     * can be presented successfully will be closest to ts and within min/max_ts.
     * Active streams are all streams that have AVStream.discard < AVDISCARD_ALL.
     *)
//    int (*read_seek2)(struct AVFormatContext *s, int stream_index, int64_t min_ts,
//           int64_t ts, int64_t max_ts, int flags);
     read_seek2: function (s:PAVFormatContext;stream_index:integer;min_ts:int64;
                 ts:int64; max_ts:int64;flags:integer):integer;cdecl;

    (**
     * Returns device list with it properties.
     * @see avdevice_list_devices() for more details.
     *)
//    int (*get_device_list)(struct AVFormatContext *s, struct AVDeviceInfoList *device_list);
     get_device_list: function (s:PAVFormatContext; device_list:Pointer):integer;cdecl;
    (**
     * Initialize device capabilities submodule.
     * @see avdevice_capabilities_create() for more details.
     *)
//    int (*create_device_capabilities)(struct AVFormatContext *s, struct AVDeviceCapabilitiesQuery *caps);
     create_device_capabilities: function (s:PAVFormatContext; caps:Pointer):integer;cdecl;

    (**
     * Free device capabilities submodule.
     * @see avdevice_capabilities_free() for more details.
     *)
//    int (*free_device_capabilities)(struct AVFormatContext *s, struct AVDeviceCapabilitiesQuery *caps);
     free_device_capabilities: function (s:PAVFormatContext; caps:Pointer):integer;cdecl;
end;

 PAVOutputFormat=^TAVOutputFormat;
 TAVOutputFormat =record
    name:PAnsiChar;
    (**
     * Descriptive name for the format, meant to be more human-readable
     * than name. You should use the NULL_IF_CONFIG_SMALL() macro
     * to define it.
     *)
    long_name: PAnsiChar;
    mime_type: PAnsiChar;
    extensions: PAnsiChar; (**< comma-separated filename extensions *)

    (* output support *)
    audio_codec: TAVCodecID;    (**< default audio codec *)
    video_codec: TAVCodecID;    (**< default video codec *)
    subtitle_codec: TAVCodecID; (**< default subtitle codec *)

    (**
     * can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER,
     * AVFMT_GLOBALHEADER, AVFMT_NOTIMESTAMPS, AVFMT_VARIABLE_FPS,
     * AVFMT_NODIMENSIONS, AVFMT_NOSTREAMS, AVFMT_ALLOW_FLUSH,
     * AVFMT_TS_NONSTRICT, AVFMT_TS_NEGATIVE
     *)
    flags:integer;

    (**
     * List of supported codec_id-codec_tag pairs, ordered by "better
     * choice first". The arrays are all terminated by AV_CODEC_ID_NONE.
     *)
//    const struct AVCodecTag * const *codec_tag;
//    codec_tag: PPAVCodecTag;
    codec_tag: Pointer;

//    const AVClass *priv_class; ///< AVClass for the private context
    priv_class: Pointer;
    (*****************************************************************
     * No fields below this line are part of the public API. They
     * may not be used outside of libavformat and can be changed and
     * removed at will.
     * New public fields should be added right above.
     *****************************************************************
     *)
//    struct AVOutputFormat *next;
     next: PAVOutputFormat;
    (**
     * size of private data so that it can be allocated in the wrapper
     *)
    priv_data_size:integer;

//    int (*write_header)(struct AVFormatContext *);
    write_header: function(s: PAVFormatContext): Integer; cdecl;
    (**
     * Write a packet. If AVFMT_ALLOW_FLUSH is set in flags,
     * pkt can be NULL in order to flush data buffered in the muxer.
     * When flushing, return 0 if there still is more data to flush,
     * or 1 if everything was flushed and there is no more buffered
     * data.
     *)
//    int (*write_packet)(struct AVFormatContext *, AVPacket *pkt);
//    int (*write_trailer)(struct AVFormatContext *);
    write_packet: function(s: PAVFormatContext; pkt: PAVPacket): Integer; cdecl;
    write_trailer: function(s: PAVFormatContext): Integer; cdecl;

    (**
     * Currently only used to set pixel format if not YUV420P.
     *)
//    int (*interleave_packet)(struct AVFormatContext *, AVPacket *out,
//                             AVPacket *in, int flush);
    interleave_packet: function(s: PAVFormatContext; pktout: PAVPacket;
                                pktin: PAVPacket; flush: Integer): Integer; cdecl;

    (**
     * Test if the given codec can be stored in this container.
     *
     * @return 1 if the codec is supported, 0 if it is not.
     *         A negative number if unknown.
     *         MKTAG('A', 'P', 'I', 'C') if the codec is only supported as AV_DISPOSITION_ATTACHED_PIC
     *)
//    int (*query_codec)(enum AVCodecID id, int std_compliance);
    query_codec: function(id: TAVCodecID; std_compliance: Integer): Integer; cdecl;

//    void (*get_output_timestamp)(struct AVFormatContext *s, int stream,
//                                 int64_t *dts, int64_t *wall);
    get_output_timestamp: procedure(s: PAVFormatContext; stream: Integer;
                                 dts, wall: PInt64); cdecl;

    (**
     * Allows sending messages from application to device.
     *)
//    int (*control_message)(struct AVFormatContext *s, int type,
//                           void *data, size_t data_size);
    control_message: function(s: PAVFormatContext; ttype: Integer;
                           data: Pointer; data_size: Cardinal): Integer; cdecl;

    (**
     * Write an uncoded AVFrame.
     *
     * See av_write_uncoded_frame() for details.
     *
     * The library will free *frame afterwards, but the muxer can prevent it
     * by setting the pointer to NULL.
     *)
//    int (*write_uncoded_frame)(struct AVFormatContext *, int stream_index,
//                               AVFrame **frame, unsigned flags);
    write_uncoded_frame: function(s: PAVFormatContext; stream_index: Integer;
                               frame: PPAVFrame; flags: Cardinal): Integer; cdecl;
    (**
     * Returns device list with it properties.
     * @see avdevice_list_devices() for more details.
     *)
//    int (*get_device_list)(struct AVFormatContext *s, struct AVDeviceInfoList *device_list);
    get_device_list: function(s: PAVFormatContext; device_list: PAVDeviceInfoList): Integer; cdecl;

    (**
     * Initialize device capabilities submodule.
     * @see avdevice_capabilities_create() for more details.
     *)
//    int (*create_device_capabilities)(struct AVFormatContext *s, struct AVDeviceCapabilitiesQuery *caps);
    create_device_capabilities: function(s: PAVFormatContext; caps: PAVDeviceCapabilitiesQuery): Integer; cdecl;

    (**
     * Free device capabilities submodule.
     * @see avdevice_capabilities_free() for more details.
     *)
//    int (*free_device_capabilities)(struct AVFormatContext *s, struct AVDeviceCapabilitiesQuery *caps);
    free_device_capabilities: function(s: PAVFormatContext; caps: PAVDeviceCapabilitiesQuery): Integer; cdecl;

//    enum AVCodecID data_codec; (**< default data codec *)
    data_codec: TAVCodecID; (**< default data codec *)
    (**
     * Initialize format. May allocate data here, and set any AVFormatContext or
     * AVStream parameters that need to be set before packets are sent.
     * This method must not write output.
     *
     * Return 0 if streams were fully configured, 1 if not, negative AVERROR on failure
     *
     * Any allocations made here must be freed in deinit().
     *)
//    int (*init)(struct AVFormatContext *);
    init: function(s: PAVFormatContext): Integer; cdecl;
    (**
     * Deinitialize format. If present, this is called whenever the muxer is being
     * destroyed, regardless of whether or not the header has been written.
     *
     * If a trailer is being written, this is called after write_trailer().
     *
     * This is called if init() fails as well.
     *)
//    void (*deinit)(struct AVFormatContext *);
    deinit: procedure(s: PAVFormatContext); cdecl;
    (**
     * Set up any necessary bitstream filtering and extract any extra data needed
     * for the global header.
     * Return 0 if more packets from this stream must be checked; 1 if not.
     *)
//    int (*check_bitstream)(struct AVFormatContext *, const AVPacket *pkt);
    check_bitstream: function(s: PAVFormatContext; const pkt: PAVPacket): Integer; cdecl;

 end;




(**
 * Format I/O context.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVFormatContext) must not be used outside libav*, use
 * avformat_alloc_context() to create an AVFormatContext.
 *
 * Fields can be accessed through AVOptions (av_opt* ),
 * the name string used matches the associated command line parameter name and
 * can be found in libavformat/options_table.h.
 * The AVOption/command line parameter names differ in some cases from the C
 * structure field names for historic reasons or brevity.
 *)
 TAVFormatContext = record
    (**
     * A class for logging and @ref avoptions. Set by avformat_alloc_context().
     * Exports (de)muxer private options if they exist.
     *)
//    const AVClass *av_class;
    av_class: Pointer;
    (**
     * The input container format.
     *
     * Demuxing only, set by avformat_open_input().
     *)
    iformat: PAVInputFormat;

    (**
     * The output container format.
     *
     * Muxing only, must be set by the caller before avformat_write_header().
     *)
    oformat: PAVOutputFormat;

    (**
     * Format private data. This is an AVOptions-enabled struct
     * if and only if iformat/oformat.priv_class is not NULL.
     *
     * - muxing: set by avformat_write_header()
     * - demuxing: set by avformat_open_input()
     *)
    priv_data:Pointer;

    (**
     * I/O context.
     *
     * - demuxing: either set by the user before avformat_open_input() (then
     *             the user must close it manually) or set by avformat_open_input().
     * - muxing: set by the user before avformat_write_header(). The caller must
     *           take care of closing / freeing the IO context.
     *
     * Do NOT set this field if AVFMT_NOFILE flag is set in
     * iformat/oformat.flags. In such a case, the (de)muxer will handle
     * I/O in some other way and this field will be NULL.
     *)
    pb: PAVIOContext;

    (* stream info *)
    (**
     * Flags signalling stream properties. A combination of AVFMTCTX_*.
     * Set by libavformat.
     *)
    ctx_flags:integer;

    (**
     * Number of elements in AVFormatContext.streams.
     *
     * Set by avformat_new_stream(), must not be modified by any other code.
     *)
    nb_streams:cardinal;
    (**
     * A list of all streams in the file. New streams are created with
     * avformat_new_stream().
     *
     * - demuxing: streams are created by libavformat in avformat_open_input().
     *             If AVFMTCTX_NOHEADER is set in ctx_flags, then new streams may also
     *             appear in av_read_frame().
     * - muxing: streams are created by the user before avformat_write_header().
     *
     * Freed by libavformat in avformat_free_context().
     *)
    streams: PPAVStream;

    (**
     * input or output filename
     *
     * - demuxing: set by avformat_open_input()
     * - muxing: may be set by the caller before avformat_write_header()
     *)
//    char filename[1024];
    filename: array[0..1024-1] of AnsiChar;

    (**
     * Position of the first frame of the component, in
     * AV_TIME_BASE fractional seconds. NEVER set this value directly:
     * It is deduced from the AVStream values.
     *
     * Demuxing only, set by libavformat.
     *)
    start_time:int64;

    (**
     * Duration of the stream, in AV_TIME_BASE fractional
     * seconds. Only set this value if you know none of the individual stream
     * durations and also do not set any of them. This is deduced from the
     * AVStream values if not set.
     *
     * Demuxing only, set by libavformat.
     *)
    duration:int64;

    (**
     * Total stream bitrate in bit/s, 0 if not
     * available. Never set it directly if the file_size and the
     * duration are known as FFmpeg can compute it automatically.
     *)
    bit_rate:int64;

    packet_size:cardinal;
    max_delay: Integer;

    (**
     * Flags modifying the (de)muxer behaviour. A combination of AVFMT_FLAG_*.
     * Set by the user before avformat_open_input() / avformat_write_header().
     *)
    flags:integer;
{
#define AVFMT_FLAG_GENPTS       0x0001 ///< Generate missing pts even if it requires parsing future frames.
#define AVFMT_FLAG_IGNIDX       0x0002 ///< Ignore index.
#define AVFMT_FLAG_NONBLOCK     0x0004 ///< Do not block when reading packets from input.
#define AVFMT_FLAG_IGNDTS       0x0008 ///< Ignore DTS on frames that contain both DTS & PTS
#define AVFMT_FLAG_NOFILLIN     0x0010 ///< Do not infer any values from other values, just return what is stored in the container
#define AVFMT_FLAG_NOPARSE      0x0020 ///< Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled
#define AVFMT_FLAG_NOBUFFER     0x0040 ///< Do not buffer frames when possible
#define AVFMT_FLAG_CUSTOM_IO    0x0080 ///< The caller has supplied a custom AVIOContext, don't avio_close() it.
#define AVFMT_FLAG_DISCARD_CORRUPT  0x0100 ///< Discard frames marked corrupted
#define AVFMT_FLAG_FLUSH_PACKETS    0x0200 ///< Flush the AVIOContext every packet.
}
(**
 * When muxing, try to avoid writing any random/volatile data to the output.
 * This includes any random IDs, real-time timestamps/dates, muxer version, etc.
 *
 * This flag is mainly intended for testing.
 *)
{
#define AVFMT_FLAG_BITEXACT         0x0400
#define AVFMT_FLAG_MP4A_LATM    0x8000 ///< Enable RTP MP4A-LATM payload
#define AVFMT_FLAG_SORT_DTS    0x10000 ///< try to interleave outputted packets by dts (using this flag can slow demuxing down)
#define AVFMT_FLAG_PRIV_OPT    0x20000 ///< Enable use of private options by delaying codec open (this could be made default once all code is converted)
#if FF_API_LAVF_KEEPSIDE_FLAG
#define AVFMT_FLAG_KEEP_SIDE_DATA 0x40000 ///< Don't merge side data but keep it separate. Deprecated, will be the default.
#endif
#define AVFMT_FLAG_FAST_SEEK   0x80000 ///< Enable fast, but inaccurate seeks for some formats
#define AVFMT_FLAG_SHORTEST   0x100000 ///< Stop muxing when the shortest stream stops.
#define AVFMT_FLAG_AUTO_BSF   0x200000 ///< Wait for packet data before writing a header, and add bitstream filters as requested by the muxer
}
    (**
     * Maximum size of the data read from input for determining
     * the input container format.
     * Demuxing only, set by the caller before avformat_open_input().
     *)
    probesize:int64;

    (**
     * Maximum duration (in AV_TIME_BASE units) of the data read
     * from input in avformat_find_stream_info().
     * Demuxing only, set by the caller before avformat_find_stream_info().
     * Can be set to 0 to let avformat choose using a heuristic.
     *)

    max_analyze_duration: Int64;

    key: PByte;
    keylen: Integer;

    nb_programs: Cardinal;
    programs: PPAVProgram;


    (**
     * Forced video codec_id.
     * Demuxing: Set by user.
     *)
    video_codec_id: TAVCodecID;

    (**
     * Forced audio codec_id.
     * Demuxing: Set by user.
     *)
    audio_codec_id: TAVCodecID;

    (**
     * Forced subtitle codec_id.
     * Demuxing: Set by user.
     *)
    subtitle_codec_id: TAVCodecID;

    (**
     * Maximum amount of memory in bytes to use for the index of each stream.
     * If the index exceeds this size, entries will be discarded as
     * needed to maintain a smaller size. This can lead to slower or less
     * accurate seeking (depends on demuxer).
     * Demuxers for which a full in-memory index is mandatory will ignore
     * this.
     * - muxing: unused
     * - demuxing: set by user
     *)
    max_index_size: Cardinal;

    (**
     * Maximum amount of memory in bytes to use for buffering frames
     * obtained from realtime capture devices.
     *)
    max_picture_buffer: Cardinal;

    (**
     * Number of chapters in AVChapter array.
     * When muxing, chapters are normally written in the file header,
     * so nb_chapters should normally be initialized before write_header
     * is called. Some muxers (e.g. mov and mkv) can also write chapters
     * in the trailer.  To write chapters in the trailer, nb_chapters
     * must be zero when write_header is called and non-zero when
     * write_trailer is called.
     * - muxing: set by user
     * - demuxing: set by libavformat
     *)
    nb_chapters: Cardinal;
    chapters: PPAVChapter;

    (**
     * Metadata that applies to the whole file.
     *
     * - demuxing: set by libavformat in avformat_open_input()
     * - muxing: may be set by the caller before avformat_write_header()
     *
     * Freed by libavformat in avformat_free_context().
     *)
    metadata: PAVDictionary;

    (**
     * Start time of the stream in real world time, in microseconds
     * since the Unix epoch (00:00 1st January 1970). That is, pts=0 in the
     * stream was captured at this real world time.
     * - muxing: Set by the caller before avformat_write_header(). If set to
     *           either 0 or AV_NOPTS_VALUE, then the current wall-time will
     *           be used.
     * - demuxing: Set by libavformat. AV_NOPTS_VALUE if unknown. Note that
     *             the value may become known after some number of frames
     *             have been received.
     *)
    start_time_realtime: Int64;

    (**
     * The number of frames used for determining the framerate in
     * avformat_find_stream_info().
     * Demuxing only, set by the caller before avformat_find_stream_info().
     *)
    fps_probe_size: Integer;

    (**
     * Error recognition; higher values will detect more errors but may
     * misdetect some more or less valid parts as errors.
     * Demuxing only, set by the caller before avformat_open_input().
     *)
    error_recognition: Integer;

    (**
     * Custom interrupt callbacks for the I/O layer.
     *
     * demuxing: set by the user before avformat_open_input().
     * muxing: set by the user before avformat_write_header()
     * (mainly useful for AVFMT_NOFILE formats). The callback
     * should also be passed to avio_open2() if it's used to
     * open the file.
     *)
    interrupt_callback: TAVIOInterruptCB;

    (**
     * Flags to enable debugging.
     *)
    debug: Integer;
// #define FF_FDEBUG_TS        0x0001

    (**
     * Maximum buffering duration for interleaving.
     *
     * To ensure all the streams are interleaved correctly,
     * av_interleaved_write_frame() will wait until it has at least one packet
     * for each stream before actually writing any packets to the output file.
     * When some streams are "sparse" (i.e. there are large gaps between
     * successive packets), this can result in excessive buffering.
     *
     * This field specifies the maximum difference between the timestamps of the
     * first and the last packet in the muxing queue, above which libavformat
     * will output a packet regardless of whether it has queued a packet for all
     * the streams.
     *
     * Muxing only, set by the caller before avformat_write_header().
     *)
    max_interleave_delta: Int64;

    (**
     * Allow non-standard and experimental extension
     * @see AVCodecContext.strict_std_compliance
     *)
    strict_std_compliance: Integer;

    (**
     * Flags for the user to detect events happening on the file. Flags must
     * be cleared by the user once the event has been handled.
     * A combination of AVFMT_EVENT_FLAG_*.
     *)
    event_flags: Integer;
//#define AVFMT_EVENT_FLAG_METADATA_UPDATED 0x0001 ///< The call resulted in updated metadata.

    (**
     * Maximum number of packets to read while waiting for the first timestamp.
     * Decoding only.
     *)
    max_ts_probe: Integer;

    (**
     * Avoid negative timestamps during muxing.
     * Any value of the AVFMT_AVOID_NEG_TS_* constants.
     * Note, this only works when using av_interleaved_write_frame. (interleave_packet_per_dts is in use)
     * - muxing: Set by user
     * - demuxing: unused
     *)
    avoid_negative_ts: integer;
//#define AVFMT_AVOID_NEG_TS_AUTO             -1 ///< Enabled when required by target format
//#define AVFMT_AVOID_NEG_TS_MAKE_NON_NEGATIVE 1 ///< Shift timestamps so they are non negative
//#define AVFMT_AVOID_NEG_TS_MAKE_ZERO         2 ///< Shift timestamps so that they start at 0

    (**
     * Transport stream id.
     * This will be moved into demuxer private options. Thus no API/ABI compatibility
     *)
    ts_id: integer;

    (**
     * Audio preload in microseconds.
     * Note, not all formats support this and unpredictable things may happen if it is used when not supported.
     * - encoding: Set by user
     * - decoding: unused
     *)
    audio_preload: integer;

    (**
     * Max chunk time in microseconds.
     * Note, not all formats support this and unpredictable things may happen if it is used when not supported.
     * - encoding: Set by user
     * - decoding: unused
     *)
    max_chunk_duration: Integer;

    (**
     * Max chunk size in bytes
     * Note, not all formats support this and unpredictable things may happen if it is used when not supported.
     * - encoding: Set by user
     * - decoding: unused
     *)
    max_chunk_size: Integer;

    (**
     * forces the use of wallclock timestamps as pts/dts of packets
     * This has undefined results in the presence of B frames.
     * - encoding: unused
     * - decoding: Set by user
     *)
    use_wallclock_as_timestamps: Integer;

    (**
     * avio flags, used to force AVIO_FLAG_DIRECT.
     * - encoding: unused
     * - decoding: Set by user
     *)
    avio_flags: Integer;

    (**
     * The duration field can be estimated through various ways, and this field can be used
     * to know how the duration was estimated.
     * - encoding: unused
     * - decoding: Read by user
     *)
//    enum AVDurationEstimationMethod duration_estimation_method;
    duration_estimation_method: TAVDurationEstimationMethod;

    (**
     * Skip initial bytes when opening stream
     * - encoding: unused
     * - decoding: Set by user
     *)
    skip_initial_bytes: Int64;

    (**
     * Correct single timestamp overflows
     * - encoding: unused
     * - decoding: Set by user
     *)
    correct_ts_overflow: Cardinal;

    (**
     * Force seeking to any (also non key) frames.
     * - encoding: unused
     * - decoding: Set by user
     *)
    seek2any: Integer;

    (**
     * Flush the I/O context after each packet.
     * - encoding: Set by user
     * - decoding: unused
     *)
    flush_packets: Integer;

    (**
     * format probing score.
     * The maximal score is AVPROBE_SCORE_MAX, its set when the demuxer probes
     * the format.
     * - encoding: unused
     * - decoding: set by avformat, read by user
     *)
    probe_score: Integer;

    (**
     * number of bytes to read maximally to identify format.
     * - encoding: unused
     * - decoding: set by user
     *)
    format_probesize: Integer;

    (**
     * ',' separated list of allowed decoders.
     * If NULL then all are allowed
     * - encoding: unused
     * - decoding: set by user
     *)
    codec_whitelist: PAnsiChar;

    (**
     * ',' separated list of allowed demuxers.
     * If NULL then all are allowed
     * - encoding: unused
     * - decoding: set by user
     *)
    format_whitelist: PAnsiChar;

    (**
     * An opaque field for libavformat internal usage.
     * Must not be accessed in any way by callers.
     *)
    internal: PAVFormatInternal;

    (**
     * IO repositioned flag.
     * This is set by avformat when the underlaying IO context read pointer
     * is repositioned, for example when doing byte based seeking.
     * Demuxers can use the flag to detect such changes.
     *)
    io_repositioned:integer;

    (**
     * Forced video codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     *)
    video_codec:PAVCodec;

    (**
     * Forced audio codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     *)
    audio_codec: PAVCodec;

    (**
     * Forced subtitle codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     *)
    subtitle_codec: PAVCodec;

    (**
     * Forced data codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     *)
    data_codec: PAVCodec;

    (**
     * Number of bytes to be written as padding in a metadata header.
     * Demuxing: Unused.
     * Muxing: Set by user via av_format_set_metadata_header_padding.
     *)
    metadata_header_padding: integer;

    (**
     * User data.
     * This is a place for some private data of the user.
     *)
    opaque: Pointer;

    (**
     * Callback used by devices to communicate with application.
     *)
//    av_format_control_message control_message_cb;
    control_message_cb: Tav_format_control_message;

    (**
     * Output timestamp offset, in microseconds.
     * Muxing: set by user
     *)
    output_ts_offset: int64;

    (**
     * dump format separator.
     * can be ", " or "\n      " or anything else
     * - muxing: Set by user.
     * - demuxing: Set by user.
     *)
    dump_separator: PByte;

    (**
     * Forced Data codec_id.
     * Demuxing: Set by user.
     *)
    data_codec_id: TAVCodecID;

{$ifdef FF_API_OLD_OPEN_CALLBACKS}
    (**
     * Called to open further IO contexts when needed for demuxing.
     *
     * This can be set by the user application to perform security checks on
     * the URLs before opening them.
     * The function should behave like avio_open2(), AVFormatContext is provided
     * as contextual information and to reach AVFormatContext.opaque.
     *
     * If NULL then some simple checks are used together with avio_open2().
     *
     * Must not be accessed directly from outside avformat.
     * @See av_format_set_open_cb()
     *
     * Demuxing: Set by user.
     *
     * @deprecated Use io_open and io_close.
     *)
//    attribute_deprecated
//    int (*open_cb)(struct AVFormatContext *s, AVIOContext **p, const char *url,
//          int flags, const AVIOInterruptCB *int_cb, AVDictionary **options);
    open_cb: function(s:PAVFormatContext;p:PPAVIOContext;const url:PAnsiChar;
            flags:integer; int_cb:PAVIOInterruptCB;options:PPAVDictionary):integer;cdecl;

{$endif}

    (**
     * ',' separated list of allowed protocols.
     * - encoding: unused
     * - decoding: set by user
     *)
    protocol_whitelist: PAnsiChar;

    (*
     * A callback for opening new IO streams.
     *
     * Whenever a muxer or a demuxer needs to open an IO stream (typically from
     * avformat_open_input() for demuxers, but for certain formats can happen at
     * other times as well), it will call this callback to obtain an IO context.
     *
     * @param s the format context
     * @param pb on success, the newly opened IO context should be returned here
     * @param url the url to open
     * @param flags a combination of AVIO_FLAG_*
     * @param options a dictionary of additional options, with the same
     *                semantics as in avio_open2()
     * @return 0 on success, a negative AVERROR code on failure
     *
     * @note Certain muxers and demuxers do nesting, i.e. they open one or more
     * additional internal format contexts. Thus the AVFormatContext pointer
     * passed to this callback may be different from the one facing the caller.
     * It will, however, have the same 'opaque' field.
     *)
//    int (*io_open)(struct AVFormatContext *s, AVIOContext **pb, const char *url,
//                   int flags, AVDictionary **options);
    io_open: function(s: PAVFormatContext; pb: PPAVIOContext; const url: PAnsiChar;
                   flags: Integer; options: PPAVDictionary): Integer; cdecl;

    (**
     * A callback for closing the streams opened with AVFormatContext.io_open().
     *)
//    void (*io_close)(struct AVFormatContext *s, AVIOContext *pb);
    io_close: procedure(s: PAVFormatContext; pb: PAVIOContext); cdecl;

    (**
     * ',' separated list of disallowed protocols.
     * - encoding: unused
     * - decoding: set by user
     *)
    protocol_blacklist: PAnsiChar;

    (**
     * The maximum number of streams.
     * - encoding: unused
     * - decoding: set by user
     *)
    max_streams:integer;
 end; // AVFormatContext

const
  AVFMT_FLAG_GENPTS       = $0001; ///< Generate missing pts even if it requires parsing future frames.




procedure av_register_all; cdecl;
          external LIBAVFORMAT_LIBNAME name 'av_register_all';

function avformat_network_init: Integer; cdecl;
          external LIBAVFORMAT_LIBNAME name 'avformat_network_init';

function avformat_network_deinit: Integer; cdecl;
          external LIBAVFORMAT_LIBNAME name 'avformat_network_deinit';

(**
 * Close an opened input AVFormatContext. Free it and all its contents
 * and set *s to NULL.
 *)
procedure avformat_close_input(s: PPAVFormatContext); cdecl;
          external LIBAVFORMAT_LIBNAME name 'avformat_close_input';

(**
 * Allocate an AVFormatContext.
 * avformat_free_context() can be used to free the context and everything
 * allocated by the framework within it.
 *)
 function avformat_alloc_context():PAVFormatContext;cdecl;
          external LIBAVFORMAT_LIBNAME name 'avformat_alloc_context';


(**
 * Open an input stream and read the header. The codecs are not opened.
 * The stream must be closed with avformat_close_input().
 *
 * @param ps Pointer to user-supplied AVFormatContext (allocated by avformat_alloc_context).
 *           May be a pointer to NULL, in which case an AVFormatContext is allocated by this
 *           function and written into ps.
 *           Note that a user-supplied AVFormatContext will be freed on failure.
 * @param url URL of the stream to open.
 * @param fmt If non-NULL, this parameter forces a specific input format.
 *            Otherwise the format is autodetected.
 * @param options  A dictionary filled with AVFormatContext and demuxer-private options.
 *                 On return this parameter will be destroyed and replaced with a dict containing
 *                 options that were not found. May be NULL.
 *
 * @return 0 on success, a negative AVERROR on failure.
 *
 * @note If you want to use custom IO, preallocate the format context and set its pb field.
 *)
//  int avformat_open_input(AVFormatContext **ps, const char *url, AVInputFormat *fmt,
// AVDictionary **options);

function avformat_open_input(ps: PPAVFormatContext; const filename: PAnsiChar; fmt: PAVInputFormat;
          options: PPAVDictionary): Integer; cdecl;
          external LIBAVFORMAT_LIBNAME name 'avformat_open_input';


(**
 * This function will cause global side data to be injected in the next packet
 * of each stream as well as after any subsequent seek.
 *)
procedure av_format_inject_global_side_data(s: PAVFormatContext); cdecl;
          external LIBAVFORMAT_LIBNAME name 'av_format_inject_global_side_data';

(**
 * Check if the stream st contained in s is matched by the stream specifier
 * spec.
 *
 * See the "stream specifiers" chapter in the documentation for the syntax
 * of spec.
 *
 * @return  >0 if st is matched by spec;
 *          0  if st is not matched by spec;
 *          AVERROR code if spec is invalid
 *
 * @note  A stream specifier can match several streams in the format.
 *)
//int avformat_match_stream_specifier(AVFormatContext *s, AVStream *st,
//                                    const char *spec);
function avformat_match_stream_specifier(s:PAVFormatContext; st: PAVStream;
                                    const spec: PAnsiChar):integer;cdecl;
          external LIBAVFORMAT_LIBNAME name 'avformat_match_stream_specifier';


(**
 * Read packets of a media file to get stream information. This
 * is useful for file formats with no headers such as MPEG. This
 * function also computes the real framerate in case of MPEG-2 repeat
 * frame mode.
 * The logical file position is not changed by this function;
 * examined packets may be buffered for later processing.
 *
 * @param ic media file handle
 * @param options  If non-NULL, an ic.nb_streams long array of pointers to
 *                 dictionaries, where i-th member contains options for
 *                 codec corresponding to i-th stream.
 *                 On return each dictionary will be filled with options that were not found.
 * @return >=0 if OK, AVERROR_xxx on error
 *
 * @note this function isn't guaranteed to open all the codecs, so
 *       options being non-empty at return is a perfectly normal behavior.
 *
 * @todo Let the user decide somehow what information is needed so that
 *       we do not waste time getting stuff the user does not need.
 *)
function avformat_find_stream_info(ic: PAVFormatContext; options: PPAVDictionary): Integer; cdecl;
        external LIBAVFORMAT_LIBNAME name 'avformat_find_stream_info';


(**
 * Seek to timestamp ts.
 * Seeking will be done so that the point from which all active streams
 * can be presented successfully will be closest to ts and within min/max_ts.
 * Active streams are all streams that have AVStream.discard < AVDISCARD_ALL.
 *
 * If flags contain AVSEEK_FLAG_BYTE, then all timestamps are in bytes and
 * are the file position (this may not be supported by all demuxers).
 * If flags contain AVSEEK_FLAG_FRAME, then all timestamps are in frames
 * in the stream with stream_index (this may not be supported by all demuxers).
 * Otherwise all timestamps are in units of the stream selected by stream_index
 * or if stream_index is -1, in AV_TIME_BASE units.
 * If flags contain AVSEEK_FLAG_ANY, then non-keyframes are treated as
 * keyframes (this may not be supported by all demuxers).
 * If flags contain AVSEEK_FLAG_BACKWARD, it is ignored.
 *
 * @param s media file handle
 * @param stream_index index of the stream which is used as time base reference
 * @param min_ts smallest acceptable timestamp
 * @param ts target timestamp
 * @param max_ts largest acceptable timestamp
 * @param flags flags
 * @return >=0 on success, error code otherwise
 *
 * @note This is part of the new seek API which is still under construction.
 *       Thus do not use this yet. It may change at any time, do not expect
 *       ABI compatibility yet!
 *)
function avformat_seek_file(s: PAVFormatContext; stream_index: Integer; min_ts, ts, max_ts: Int64; flags: Integer): Integer; cdecl;
       external LIBAVFORMAT_LIBNAME name 'avformat_seek_file';


(**
 * Seek to the keyframe at timestamp.
 * 'timestamp' in 'stream_index'.
 *
 * @param s media file handle
 * @param stream_index If stream_index is (-1), a default
 * stream is selected, and timestamp is automatically converted
 * from AV_TIME_BASE units to the stream specific time_base.
 * @param timestamp Timestamp in AVStream.time_base units
 *        or, if no stream is specified, in AV_TIME_BASE units.
 * @param flags flags which select direction and seeking mode
 * @return >= 0 on success
 *)
function av_seek_frame(s: PAVFormatContext; stream_index: Integer; timestamp: Int64;
                       flags: Integer): Integer; cdecl;
    external LIBAVFORMAT_LIBNAME name 'av_seek_frame';


(**
 * Discard all internally buffered data. This can be useful when dealing with
 * discontinuities in the byte stream. Generally works only with formats that
 * can resync. This includes headerless formats like MPEG-TS/TS but should also
 * work with NUT, Ogg and in a limited way AVI for example.
 *
 * The set of streams, the detected duration, stream parameters and codecs do
 * not change when calling this function. If you want a complete reset, it's
 * better to open a new AVFormatContext.
 *
 * This does not flush the AVIOContext (s->pb). If necessary, call
 * avio_flush(s->pb) before calling this function.
 *
 * @param s media file handle
 * @return >=0 on success, error code otherwise
 *)
function avformat_flush(s: PAVFormatContext): Integer; cdecl;
     external LIBAVFORMAT_LIBNAME name 'avformat_flush';


(**
 * Find the "best" stream in the file.
 * The best stream is determined according to various heuristics as the most
 * likely to be what the user expects.
 * If the decoder parameter is non-NULL, av_find_best_stream will find the
 * default decoder for the stream's codec; streams for which no decoder can
 * be found are ignored.
 *
 * @param ic                media file handle
 * @param type              stream type: video, audio, subtitles, etc.
 * @param wanted_stream_nb  user-requested stream number,
 *                          or -1 for automatic selection
 * @param related_stream    try to find a stream related (eg. in the same
 *                          program) to this one, or -1 if none
 * @param decoder_ret       if non-NULL, returns the decoder for the
 *                          selected stream
 * @param flags             flags; none are currently defined
 * @return  the non-negative stream number in case of success,
 *          AVERROR_STREAM_NOT_FOUND if no stream with the requested type
 *          could be found,
 *          AVERROR_DECODER_NOT_FOUND if streams were found but no decoder
 * @note  If av_find_best_stream returns successfully and decoder_ret is not
 *        NULL, then *decoder_ret is guaranteed to be set to a valid AVCodec.
 *)
function av_find_best_stream(ic: PAVFormatContext;
                        type_: TAVMediaType;
                        wanted_stream_nb,
                        related_stream: Integer;
                        decoder_ret: PPAVCodec;
                        flags: Integer): Integer; cdecl;
         external LIBAVFORMAT_LIBNAME name 'av_find_best_stream';

(**
 * Return the next frame of a stream.
 * This function returns what is stored in the file, and does not validate
 * that what is there are valid frames for the decoder. It will split what is
 * stored in the file into frames and return one for each call. It will not
 * omit invalid data between valid frames so as to give the decoder the maximum
 * information possible for decoding.
 *
 * If pkt->buf is NULL, then the packet is valid until the next
 * av_read_frame() or until avformat_close_input(). Otherwise the packet
 * is valid indefinitely. In both cases the packet must be freed with
 * av_packet_unref when it is no longer needed. For video, the packet contains
 * exactly one frame. For audio, it contains an integer number of frames if each
 * frame has a known fixed size (e.g. PCM or ADPCM data). If the audio frames
 * have a variable size (e.g. MPEG audio), then it contains one frame.
 *
 * pkt->pts, pkt->dts and pkt->duration are always set to correct
 * values in AVStream.time_base units (and guessed if the format cannot
 * provide them). pkt->pts can be AV_NOPTS_VALUE if the video format
 * has B-frames, so it is better to rely on pkt->dts if you do not
 * decompress the payload.
 *
 * @return 0 if OK, < 0 on error or end of file
 *)
function av_read_frame(s: PAVFormatContext; pkt: PAVPacket): Integer; cdecl;
    external LIBAVFORMAT_LIBNAME name 'av_read_frame';


(**
 * Guess the sample aspect ratio of a frame, based on both the stream and the
 * frame aspect ratio.
 *
 * Since the frame aspect ratio is set by the codec but the stream aspect ratio
 * is set by the demuxer, these two may not be equal. This function tries to
 * return the value that you should use if you would like to display the frame.
 *
 * Basic logic is to use the stream aspect ratio if it is set to something sane
 * otherwise use the frame aspect ratio. This way a container setting, which is
 * usually easy to modify can override the coded value in the frames.
 *
 * @param format the format context which the stream is part of
 * @param stream the stream which the frame is part of
 * @param frame the frame with the aspect ratio to be determined
 * @return the guessed (valid) sample_aspect_ratio, 0/1 if no idea
 *)
function av_guess_sample_aspect_ratio(format: PAVFormatContext; stream: PAVStream; frame: PAVFrame): TAVRational; cdecl;
    external LIBAVFORMAT_LIBNAME name 'av_guess_sample_aspect_ratio';

(**
 * Guess the frame rate, based on both the container and codec information.
 *
 * @param ctx the format context which the stream is part of
 * @param stream the stream which the frame is part of
 * @param frame the frame for which the frame rate should be determined, may be NULL
 * @return the guessed (valid) frame rate, 0/1 if no idea
 *)
function av_guess_frame_rate(ctx: PAVFormatContext; stream: PAVStream; frame: PAVFrame): TAVRational; cdecl;
    external LIBAVFORMAT_LIBNAME name 'av_guess_frame_rate';

(**
 * Free an AVFormatContext and all its streams.
 * @param s context to free
 *)
procedure avformat_free_context(s: PAVFormatContext); cdecl;
    external LIBAVFORMAT_LIBNAME name 'avformat_free_context';


(**
 * Print detailed information about the input or output format, such as
 * duration, bitrate, streams, container, programs, metadata, side data,
 * codec and time base.
 *
 * @param ic        the context to analyze
 * @param index     index of the stream to dump information about
 * @param url       the URL to print, such as source or destination file
 * @param is_output Select whether the specified context is an input(0) or output(1)
 *)

procedure av_dump_format(ic: PAVFormatContext;
                    index: Integer;
                    const url: PAnsiChar;
                    is_output: Integer); cdecl;
    external LIBAVFORMAT_LIBNAME name 'av_dump_format';

