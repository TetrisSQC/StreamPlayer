type

(**
 * A reference counted buffer type. It is opaque and is meant to be used through
 * references (AVBufferRef).
 *)
 PAVBuffer = Pointer;

(**
 * A reference to a data buffer.
 *
 * The size of this struct is not a part of the public ABI and it is not meant
 * to be allocated directly.
 *)
 PPAVBufferRef=^PAVBufferRef;
 PAVBufferRef=^TAVBufferRef;
 TAVBufferRef =record
    buffer: PAVBuffer;

    (**
     * The data buffer. It is considered writable if and only if
     * this is the only reference to the buffer, in which case
     * av_buffer_is_writable() returns 1.
     *)
    data: PByte;
    (**
     * Size of data in bytes.
     *)
    size: Integer;
 end; // AVBufferRef;

