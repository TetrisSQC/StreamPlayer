(**
 * Return the size in bytes of the amount of data required to store an
 * image with the given parameters.
 *
 * @param pix_fmt  the pixel format of the image
 * @param width    the width of the image in pixels
 * @param height   the height of the image in pixels
 * @param align    the assumed linesize alignment
 * @return the buffer size in bytes, a negative error code in case of failure
 *)
function av_image_get_buffer_size(pix_fmt: TAVPixelFormat; width, height, align: Integer): Integer; cdecl;
     external LIBAVUTIL_LIBNAME name 'av_image_get_buffer_size';


(**
 * Setup the data pointers and linesizes based on the specified image
 * parameters and the provided array.
 *
 * The fields of the given image are filled in by using the src
 * address which points to the image data buffer. Depending on the
 * specified pixel format, one or multiple image data pointers and
 * line sizes will be set.  If a planar format is specified, several
 * pointers will be set pointing to the different picture planes and
 * the line sizes of the different planes will be stored in the
 * lines_sizes array. Call with src == NULL to get the required
 * size for the src buffer.
 *
 * To allocate the buffer and fill in the dst_data and dst_linesize in
 * one call, use av_image_alloc().
 *
 * @param dst_data      data pointers to be filled in
 * @param dst_linesize  linesizes for the image in dst_data to be filled in
 * @param src           buffer which will contain or contains the actual image data, can be NULL
 * @param pix_fmt       the pixel format of the image
 * @param width         the width of the image in pixels
 * @param height        the height of the image in pixels
 * @param align         the value used in src for linesize alignment
 * @return the size in bytes required for src, a negative error code
 * in case of failure
 *)
// int av_image_fill_arrays(uint8_t *dst_data[4], int dst_linesize[4],
//                         const uint8_t *src,
//                         enum AVPixelFormat pix_fmt, int width, int height, int align);
function av_image_fill_arrays(dst_data: PPByte{array[0..3] of PByte}; dst_linesize: PInteger{array[0..3] of Integer};
                          const src: PByte;
                          pix_fmt: TAVPixelFormat; width, height, align: Integer): Integer; cdecl;
          external LIBAVUTIL_LIBNAME name 'av_image_fill_arrays';


