const
(* values for the flags, the stuff on the command line is different *)
 SWS_FAST_BILINEAR     =1;
 SWS_BILINEAR          =2;
 SWS_BICUBIC           =4;
 SWS_X                 =8;
 SWS_POINT          =$10;
 SWS_AREA           =$20;
 SWS_BICUBLIN       =$40;
 SWS_GAUSS          =$80;
 SWS_SINC          =$100;
 SWS_LANCZOS       =$200;
 SWS_SPLINE        =$400;

 SWS_SRC_V_CHR_DROP_MASK     =$30000;
 SWS_SRC_V_CHR_DROP_SHIFT    =16;

 SWS_PARAM_DEFAULT           =123456;

 SWS_PRINT_INFO              =$1000;

//the following 3 flags are not completely implemented
//internal chrominance subsampling info
 SWS_FULL_CHR_H_INT    =$2000;
//input subsampling info
 SWS_FULL_CHR_H_INP    =$4000;
 SWS_DIRECT_BGR        =$8000;
 SWS_ACCURATE_RND      =$40000;
 SWS_BITEXACT          =$80000;
 SWS_ERROR_DIFFUSION  =$800000;

 SWS_MAX_REDUCE_CUTOFF =0.002;

 SWS_CS_ITU709         =1;
 SWS_CS_FCC            =4;
 SWS_CS_ITU601         =5;
 SWS_CS_ITU624         =5;
 SWS_CS_SMPTE170M      =5;
 SWS_CS_SMPTE240M      =7;
 SWS_CS_DEFAULT        =5;
 SWS_CS_BT2020         =9;


type

// when used for filters they must have an odd number of elements
// coeffs cannot be shared between vectors

  PSwsVector = ^TSwsVector;
  TSwsVector = record
    coeff: PDouble;             ///< pointer to the list of coefficients
    length: Integer;            ///< number of coefficients in the vector
  end;


// vectors can be shared
  PSwsFilter = ^TSwsFilter;
  TSwsFilter = record
    lumH: PSwsVector;
    lumV: PSwsVector;
    chrH: PSwsVector;
    chrV: PSwsVector;
  end;


 PSwsContext=Pointer;


(**
 * Free the swscaler context swsContext.
 * If swsContext is NULL, then does nothing.
 *)
procedure sws_freeContext(swsContext: PSwsContext); cdecl;
     external LIBSWSCALE_LIBNAME name 'sws_freeContext';

(**
 * Allocate and return an SwsContext. You need it to perform
 * scaling/conversion operations using sws_scale().
 *
 * @param srcW the width of the source image
 * @param srcH the height of the source image
 * @param srcFormat the source image format
 * @param dstW the width of the destination image
 * @param dstH the height of the destination image
 * @param dstFormat the destination image format
 * @param flags specify which algorithm and options to use for rescaling
 * @param param extra parameters to tune the used scaler
 *              For SWS_BICUBIC param[0] and [1] tune the shape of the basis
 *              function, param[0] tunes f(1) and param[1] fÂ´(1)
 *              For SWS_GAUSS param[0] tunes the exponent and thus cutoff
 *              frequency
 *              For SWS_LANCZOS param[0] tunes the width of the window function
 * @return a pointer to an allocated context, or NULL in case of error
 * @note this function is to be removed after a saner alternative is
 *       written
 *)

function sws_getContext(srcW, srcH: Integer; srcFormat: TAVPixelFormat;
                                  dstW, dstH: Integer; dstFormat: TAVPixelFormat;
                                  flags: Integer; srcFilter, dstFilter: PSwsFilter;
                                  param: PDouble): PSwsContext; cdecl;
         external LIBSWSCALE_LIBNAME name 'sws_getContext';


(**
 * Check if context can be reused, otherwise reallocate a new one.
 *
 * If context is NULL, just calls sws_getContext() to get a new
 * context. Otherwise, checks if the parameters are the ones already
 * saved in context. If that is the case, returns the current
 * context. Otherwise, frees context and gets a new context with
 * the new parameters.
 *
 * Be warned that srcFilter and dstFilter are not checked, they
 * are assumed to remain the same.
 *)
{
struct SwsContext *sws_getCachedContext(struct SwsContext *context,
                                        int srcW, int srcH, enum AVPixelFormat srcFormat,
                                        int dstW, int dstH, enum AVPixelFormat dstFormat,
                                        int flags, SwsFilter *srcFilter,
                                        SwsFilter *dstFilter, const double *param);
}
function sws_getCachedContext(context: PSwsContext;
                                        srcW, srcH: integer; srcFormat: TAVPixelFormat;
                                        dstW, dstH: integer; dstFormat: TAVPixelFormat;
                                        flags: Integer; srcFilter, dstFilter: PSwsFilter;
                                        const param: PDouble): PSwsContext; cdecl;
         external LIBSWSCALE_LIBNAME name 'sws_getCachedContext';


(**
 * Scale the image slice in srcSlice and put the resulting scaled
 * slice in the image in dst. A slice is a sequence of consecutive
 * rows in an image.
 *
 * Slices have to be provided in sequential order, either in
 * top-bottom or bottom-top order. If slices are provided in
 * non-sequential order the behavior of the function is undefined.
 *
 * @param c         the scaling context previously created with
 *                  sws_getContext()
 * @param srcSlice  the array containing the pointers to the planes of
 *                  the source slice
 * @param srcStride the array containing the strides for each plane of
 *                  the source image
 * @param srcSliceY the position in the source image of the slice to
 *                  process, that is the number (counted starting from
 *                  zero) in the image of the first row of the slice
 * @param srcSliceH the height of the source slice, that is the number
 *                  of rows in the slice
 * @param dst       the array containing the pointers to the planes of
 *                  the destination image
 * @param dstStride the array containing the strides for each plane of
 *                  the destination image
 * @return          the height of the output slice
 *)
//int sws_scale(struct SwsContext *c, const uint8_t *const srcSlice[],
//              const int srcStride[], int srcSliceY, int srcSliceH,
//              uint8_t *const dst[], const int dstStride[]);

function sws_scale(c: PSwsContext; const srcSlice: PPByte;
                    const srcStride: PInteger; srcSliceY, srcSliceH: Integer;
                    const dst: PPByte; const dstStride: PInteger): Integer; cdecl;
              external LIBSWSCALE_LIBNAME name 'sws_scale';

