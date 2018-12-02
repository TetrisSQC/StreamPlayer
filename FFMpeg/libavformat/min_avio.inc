type
(**
 * Callback for checking whether to abort blocking functions.
 * AVERROR_EXIT is returned in this case by the interrupted
 * function. During blocking operations, callback is called with
 * opaque as parameter. If the callback returns 1, the
 * blocking operation will be aborted.
 *
 * No members can be added to this struct without a major bump, if
 * new elements have been added after this struct in AVFormatContext
 * or AVIOContext.
 *)
 PAVIOInterruptCB = ^TAVIOInterruptCB;
 TAVIOInterruptCB =record
    callback: function(opaque: Pointer): Integer; cdecl;
    opaque: Pointer;
 end;


(**
 * Different data types that can be returned via the AVIO
 * write_data_type callback.
 *)
 TAVIODataMarkerType= (
    (**
     * Header data; this needs to be present for the stream to be decodeable.
     *)
    AVIO_DATA_MARKER_HEADER,
    (**
     * A point in the output bytestream where a decoder can start decoding
     * (i.e. a keyframe). A demuxer/decoder given the data flagged with
     * AVIO_DATA_MARKER_HEADER, followed by any AVIO_DATA_MARKER_SYNC_POINT,
     * should give decodeable results.
     *)
    AVIO_DATA_MARKER_SYNC_POINT,
    (**
     * A point in the output bytestream where a demuxer can start parsing
     * (for non self synchronizing bytestream formats). That is, any
     * non-keyframe packet start point.
     *)
    AVIO_DATA_MARKER_BOUNDARY_POINT,
    (**
     * This is any, unlabelled data. It can either be a muxer not marking
     * any positions at all, it can be an actual boundary/sync point
     * that the muxer chooses not to mark, or a later part of a packet/fragment
     * that is cut into multiple write callbacks due to limited IO buffer size.
     *)
    AVIO_DATA_MARKER_UNKNOWN,
    (**
     * Trailer data, which doesn't contain actual content, but only for
     * finalizing the output file.
     *)
    AVIO_DATA_MARKER_TRAILER,
    (**
     * A point in the output bytestream where the underlying AVIOContext might
     * flush the buffer depending on latency or buffering requirements. Typically
     * means the end of a packet.
     *)
    AVIO_DATA_MARKER_FLUSH_POINT
);




(**
 * Bytestream IO Context.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVIOContext) must not be used outside libav*.
 *
 * @note None of the function pointers in AVIOContext should be called
 *       directly, they should only be set by the client application
 *       when implementing custom I/O. Normally these are set to the
 *       function pointers specified in avio_alloc_context()
 *)
 PPAVIOContext = ^PAVIOContext;
 PAVIOContext = ^TAVIOContext;
 TAVIOContext =record
    (**
     * A class for private options.
     *
     * If this AVIOContext is created by avio_open2(), av_class is set and
     * passes the options down to protocols.
     *
     * If this AVIOContext is manually allocated, then av_class may be set by
     * the caller.
     *
     * warning -- this field can be NULL, be sure to not pass this AVIOContext
     * to any av_opt_* functions in that case.
     *)
 //   const AVClass *av_class;
    av_class:Pointer;
    (*
     * The following shows the relationship between buffer, buf_ptr,
     * buf_ptr_max, buf_end, buf_size, and pos, when reading and when writing
     * (since AVIOContext is used for both):
     *
     **********************************************************************************
     *                                   READING
     **********************************************************************************
     *
     *                            |              buffer_size              |
     *                            |---------------------------------------|
     *                            |                                       |
     *
     *                         buffer          buf_ptr       buf_end
     *                            +---------------+-----------------------+
     *                            |/ / / / / / / /|/ / / / / / /|         |
     *  read buffer:              |/ / consumed / | to be read /|         |
     *                            |/ / / / / / / /|/ / / / / / /|         |
     *                            +---------------+-----------------------+
     *
     *                                                         pos
     *              +-------------------------------------------+-----------------+
     *  input file: |                                           |                 |
     *              +-------------------------------------------+-----------------+
     *
     *
     **********************************************************************************
     *                                   WRITING
     **********************************************************************************
     *
     *                             |          buffer_size                 |
     *                             |--------------------------------------|
     *                             |                                      |
     *
     *                                                buf_ptr_max
     *                          buffer                 (buf_ptr)       buf_end
     *                             +-----------------------+--------------+
     *                             |/ / / / / / / / / / / /|              |
     *  write buffer:              | / / to be flushed / / |              |
     *                             |/ / / / / / / / / / / /|              |
     *                             +-----------------------+--------------+
     *                               buf_ptr can be in this
     *                               due to a backward seek
     *
     *                            pos
     *               +-------------+----------------------------------------------+
     *  output file: |             |                                              |
     *               +-------------+----------------------------------------------+
     *
     *)

    buffer: PByte;        (**< Start of the buffer. *)
    buffer_size: Integer; (**< Maximum buffer size *)
    buf_ptr: PByte;       (**< Current position in the buffer *)
    buf_end: PByte;       (**< End of the data, may be less than
                               buffer+buffer_size if the read function returned
                               less data than requested, e.g. for streams where
                               no more data has been received yet. *)
    opaque: Pointer;      (**< A private pointer, passed to the read/write/seek/...
                               functions. *)
    read_packet: function(opaque: Pointer; buf: PByte; buf_size: Integer): Integer; cdecl;
    write_packet: function(opaque: Pointer; buf: PByte; buf_size: Integer): Integer; cdecl;
    seek: function(opaque: Pointer; offset: Int64; whence: Integer): Int64; cdecl;

    pos: Int64;           (**< position in the file of the current buffer *)
    must_flush: Integer;  (**< true if the next seek should flush *)
    eof_reached: Integer; (**< true if eof reached *)
    write_flag: Integer;  (**< true if open for writing *)
    max_packet_size: Integer;
    checksum: Cardinal;
    checksum_ptr: PByte;

    update_checksum: function(checksum: Cardinal; const buf: PByte; size: Cardinal): Cardinal; cdecl;
    error: Integer;       (**< contains the error code or 0 if no error happened *)

    (**
     * Pause or resume playback for network streaming protocols - e.g. MMS.
     *)
//    int (*read_pause)(void *opaque, int pause);
    read_pause: function(opaque: Pointer; pause: Integer): Integer; cdecl;
    (**
     * Seek to a given timestamp in stream with the specified stream_index.
     * Needed for some network streaming protocols which don't support seeking
     * to byte position.
     *)
//    int64_t (*read_seek)(void *opaque, int stream_index,
//                         int64_t timestamp, int flags);
    read_seek: function(opaque: Pointer; stream_index: Integer;
                        timestamp: Int64; flags: Integer): Int64; cdecl;
    (**
     * A combination of AVIO_SEEKABLE_ flags or 0 when the stream is not seekable.
     *)
    seekable:integer;

    (**
     * max filesize, used to limit allocations
     * This field is internal to libavformat and access from outside is not allowed.
     *)
    maxsize:int64;

    (**
     * avio_read and avio_write should if possible be satisfied directly
     * instead of going through a buffer, and avio_seek will always
     * call the underlying seek function directly.
     *)
    direct:integer;

    (**
     * Bytes read statistic
     * This field is internal to libavformat and access from outside is not allowed.
     *)
    bytes_read:int64;

    (**
     * seek statistic
     * This field is internal to libavformat and access from outside is not allowed.
     *)
    seek_count:integer;

    (**
     * writeout statistic
     * This field is internal to libavformat and access from outside is not allowed.
     *)
    writeout_count:integer;

    (**
     * Original buffer size
     * used internally after probing and ensure seekback to reset the buffer size
     * This field is internal to libavformat and access from outside is not allowed.
     *)
    orig_buffer_size:integer;

    (**
     * Threshold to favor readahead over seek.
     * This is current internal only, do not use from outside.
     *)
    short_seek_threshold:integer;

    (**
     * ',' separated list of allowed protocols.
     *)
    protocol_whitelist: PAnsiChar;

    (**
     * ',' separated list of disallowed protocols.
     *)
    protocol_blacklist: PAnsiChar;

    (**
     * A callback that is used instead of write_packet.
     *)
//    int (*write_data_type)(void *opaque, uint8_t *buf, int buf_size,
//                           enum AVIODataMarkerType type, int64_t time);
    write_data_type: function(opaque: Pointer; buf: PByte; buf_size: Integer;
                              type_: TAVIODataMarkerType; time: Int64): Integer; cdecl;
    (**
     * If set, don't call write_data_type separately for AVIO_DATA_MARKER_BOUNDARY_POINT,
     * but ignore them and treat them as AVIO_DATA_MARKER_UNKNOWN (to avoid needlessly
     * small chunks of data returned from the callback).
     *)
    ignore_boundary_point:integer;

    (**
     * Internal, not meant to be used from outside of AVIOContext.
     *)
    current_type: TAVIODataMarkerType;
    last_time: Int64;

    (**
     * A callback that is used instead of short_seek_threshold.
     * This is current internal only, do not use from outside.
     *)
//    int (*short_seek_get)(void *opaque);
    short_seek_get: function(opaque: Pointer):integer;cdecl;
    written:int64;

    (**
     * Maximum reached position before a backward seek in the write buffer,
     * used keeping track of already written data for a later flush.
     *)
    buf_ptr_max:PByte;
    (**
     * Try to buffer at least this amount of data before flushing it
     *)
    min_packet_size:integer;
 end; // AVIOContext;

