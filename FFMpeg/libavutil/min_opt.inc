const
 AV_OPT_FLAG_ENCODING_PARAM = 1;   ///< a generic parameter which can be set by the user for muxing or encoding
 AV_OPT_FLAG_DECODING_PARAM = 2;   ///< a generic parameter which can be set by the user for demuxing or decoding


 AV_OPT_FLAG_AUDIO_PARAM     =8;
 AV_OPT_FLAG_VIDEO_PARAM     =16;
 AV_OPT_FLAG_SUBTITLE_PARAM  =32;

type
  PPAVOption = ^PAVOption;
  PAVOption = Pointer;
const
 AV_OPT_SEARCH_CHILDREN  = (1 shl 0); //**< Search in possible children of the
                                      //   given object first. */
(**
 *  The obj passed to av_opt_find() is fake -- only a double pointer to AVClass
 *  instead of a required pointer to a struct containing AVClass. This is
 *  useful for searching for options without needing to allocate the corresponding
 *  object.
 *)
 AV_OPT_SEARCH_FAKE_OBJ  = (1 shl 1);

(**
 *  In av_opt_get, return NULL if the option has a pointer type and is set to NULL,
 *  rather than returning an empty string.
 *)
 AV_OPT_ALLOW_NULL =(1 shl 2);

(**
 *  Allows av_opt_query_ranges and av_opt_query_ranges_default to return more than
 *  one component for certain option types.
 *  @see AVOptionRanges for details.
 *)
 AV_OPT_MULTI_COMPONENT_RANGE =(1 shl 12);


(**
 * Look for an option in an object. Consider only options which
 * have all the specified flags set.
 *
 * @param[in] obj A pointer to a struct whose first element is a
 *                pointer to an AVClass.
 *                Alternatively a double pointer to an AVClass, if
 *                AV_OPT_SEARCH_FAKE_OBJ search flag is set.
 * @param[in] name The name of the option to look for.
 * @param[in] unit When searching for named constants, name of the unit
 *                 it belongs to.
 * @param opt_flags Find only options with all the specified flags set (AV_OPT_FLAG).
 * @param search_flags A combination of AV_OPT_SEARCH_*.
 *
 * @return A pointer to the option found, or NULL if no option
 *         was found.
 *
 * @note Options found with AV_OPT_SEARCH_CHILDREN flag may not be settable
 * directly with av_opt_set(). Use special calls which take an options
 * AVDictionary (e.g. avformat_open_input()) to set options found with this
 * flag.
 *)

function av_opt_find(obj: Pointer; const name, unit_: PAnsiChar;
                     opt_flags, search_flags: Integer): PAVOption; cdecl;
         external LIBAVUTIL_LIBNAME name 'av_opt_find';


(**
 * @defgroup opt_set_funcs Option setting functions
 * @{
 * Those functions set the field of obj with the given name to value.
 *
 * @param[in] obj A struct whose first element is a pointer to an AVClass.
 * @param[in] name the name of the field to set
 * @param[in] val The value to set. In case of av_opt_set() if the field is not
 * of a string type, then the given string is parsed.
 * SI postfixes and some named scalars are supported.
 * If the field is of a numeric type, it has to be a numeric or named
 * scalar. Behavior with more than one scalar and +- infix operators
 * is undefined.
 * If the field is of a flags type, it has to be a sequence of numeric
 * scalars or named flags separated by '+' or '-'. Prefixing a flag
 * with '+' causes it to be set without affecting the other flags;
 * similarly, '-' unsets a flag.
 * @param search_flags flags passed to av_opt_find2. I.e. if AV_OPT_SEARCH_CHILDREN
 * is passed here, then the option may be set on a child of obj.
 *
 * @return 0 if the value has been set, or an AVERROR code in case of
 * error:
 * AVERROR_OPTION_NOT_FOUND if no matching option exists
 * AVERROR(ERANGE) if the value is out of range
 * AVERROR(EINVAL) if the value is not valid
 *)
function av_opt_set(obj: Pointer; const name: PAnsiChar; val: PAnsiChar; search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set';

function av_opt_set_int(obj: Pointer; const name: PAnsiChar; val: Int64; search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set_int';

function av_opt_set_double(obj: Pointer; const name: PAnsiChar; val: Double; search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set_double';

function av_opt_set_q(obj: Pointer; const name: PAnsiChar; val: TAVRational; search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set_q';

function av_opt_set_bin(obj: Pointer; const name: PAnsiChar; val: PByte; size, search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set_bin';

function av_opt_set_image_size(obj: Pointer; const name: PAnsiChar; w, h, search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set_image_size';

function av_opt_set_pixel_fmt(obj: Pointer; const name: PAnsiChar; fmt: TAVPixelFormat; search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set_pixel_fmt';

function av_opt_set_sample_fmt(obj: Pointer; const name: PAnsiChar; fmt: TAVSampleFormat; search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set_sample_fmt';

function av_opt_set_video_rate(obj: Pointer; const name: PAnsiChar; val: TAVRational; search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set_video_rate';

function av_opt_set_channel_layout(obj: Pointer; const name: PAnsiChar; ch_layout: Int64; search_flags: Integer): Integer; cdecl;
    external LIBAVUTIL_LIBNAME name 'av_opt_set_channel_layout';



