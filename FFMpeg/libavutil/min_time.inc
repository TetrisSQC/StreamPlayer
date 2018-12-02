

(**
 * Get the current time in microseconds.
 *)
function av_gettime: Int64; cdecl;
     external LIBAVUTIL_LIBNAME name 'av_gettime';



(**
 * Get the current time in microseconds since some unspecified starting point.
 * On platforms that support it, the time comes from a monotonic clock
 * This property makes this time source ideal for measuring relative time.
 * The returned values may not be monotonic on platforms where a monotonic
 * clock is not available.
 *)
function av_gettime_relative():int64; cdecl;
      external LIBAVUTIL_LIBNAME name 'av_gettime_relative';

(**
 * Sleep for a period of time.  Although the duration is expressed in
 * microseconds, the actual delay may be rounded to the precision of the
 * system timer.
 *
 * @param  usec Number of microseconds to sleep.
 * @return zero on success or (negative) error code.
 *)
function av_usleep(usec: Cardinal): Integer; cdecl;
      external LIBAVUTIL_LIBNAME name 'av_usleep';


