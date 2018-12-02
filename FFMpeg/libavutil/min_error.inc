const
//#define AVERROR_OPTION_NOT_FOUND   FFERRTAG(0xF8,'O','P','T') ///< Option not found
  AVERROR_OPTION_NOT_FOUND    = -( $F8  or (Ord('O') shl 8) or (Ord('P') shl 16) or (Ord('T') shl 24));

//#define AVERROR_EOF                FFERRTAG( 'E','O','F',' ') ///< End of file
  AVERROR_EOF                 = -(Ord('E') or (Ord('O') shl 8) or (Ord('F') shl 16) or (Ord(' ') shl 24));

(**
 * Put a description of the AVERROR code errnum in errbuf.
 * In case of failure the global variable errno is set to indicate the
 * error. Even in case of failure av_strerror() will print a generic
 * error message indicating the errnum provided to errbuf.
 *
 * @param errnum      error code to describe
 * @param errbuf      buffer to which description is written
 * @param errbuf_size the size in bytes of errbuf
 * @return 0 on success, a negative value if a description for errnum
 * cannot be found
 *)
function av_strerror(errnum: integer; errbuf: PAnsiChar; errbuf_size: Cardinal): Integer; cdecl;
  external LIBAVUTIL_LIBNAME name 'av_strerror';


