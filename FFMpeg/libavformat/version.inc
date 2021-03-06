(*
 * Version macros.
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *)

{$ifndef AVFORMAT_VERSION_H}
{$define AVFORMAT_VERSION_H}

(**
 * @file
 * @ingroup libavf
 * Libavformat version macros
 *)

//#include "libavutil/version.h"

// Major bumping may affect Ticket5467, 5421, 5451(compatibility with Chromium)
// Also please add any ticket numbers that you believe might be affected here
const
 LIBAVFORMAT_VERSION_MAJOR    =   57;
 LIBAVFORMAT_VERSION_MAJOR_S  =  '57';

 LIBAVFORMAT_VERSION_MINOR    =   83;
 LIBAVFORMAT_VERSION_MINOR_S  =  '83';

 LIBAVFORMAT_VERSION_MICRO    =  100;
 LIBAVFORMAT_VERSION_MICRO_S  = '100';

 LIBAVFORMAT_VERSION_INT      = (LIBAVFORMAT_VERSION_MAJOR shl 16) or
                                (LIBAVFORMAT_VERSION_MINOR shl 8) or
                                 LIBAVFORMAT_VERSION_MICRO;


 LIBAVFORMAT_VERSION          = LIBAVFORMAT_VERSION_MAJOR_S + '.' +
                                LIBAVFORMAT_VERSION_MINOR_S + '.' +
                                LIBAVFORMAT_VERSION_MICRO_S;

 LIBAVFORMAT_BUILD            = LIBAVFORMAT_VERSION_INT;

 LIBAVFORMAT_IDENT            = 'Lavf'+LIBAVFORMAT_VERSION;

{$IFDEF MSWINDOWS}
 LIBAVFORMAT_LIBNAME          = 'avformat-'+ LIBAVFORMAT_VERSION_MAJOR_S + '.dll';
{$ENDIF}
{$IFDEF UNIX}
 LIBAVFORMAT_LIBNAME          = 'libavformat.so';
{$ENDIF}

(**
 * FF_API_* defines may be placed below to indicate public API that will be
 * dropped at a future version bump. The defines themselves are not part of
 * the public API and may change, break or disappear at any time.
 *
 * @note, when bumping the major version it is recommended to manually
 * disable each FF_API_* in its own commit instead of disabling them all
 * at once through the bump. This improves the git bisect-ability of the change.
 *
 *)
{$IFNDEF FF_API_LAVF_BITEXACT}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
   {$DEFINE FF_API_LAVF_BITEXACT}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_LAVF_FRAC}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
   {$DEFINE FF_API_LAVF_FRAC}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_LAVF_CODEC_TB}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
   {$DEFINE FF_API_LAVF_CODEC_TB}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_URL_FEOF}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_URL_FEOF}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_LAVF_FMT_RAWPICTURE}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_LAVF_FMT_RAWPICTURE}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_COMPUTE_PKT_FIELDS2}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_COMPUTE_PKT_FIELDS2}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_OLD_OPEN_CALLBACKS}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_OLD_OPEN_CALLBACKS}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_LAVF_AVCTX}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_LAVF_AVCTX}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_NOCONST_GET_SIDE_DATA}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_NOCONST_GET_SIDE_DATA}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_HTTP_USER_AGENT}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_HTTP_USER_AGENT}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_HLS_WRAP}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_HLS_WRAP}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_LAVF_MERGE_SD}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_LAVF_MERGE_SD}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_LAVF_KEEPSIDE_FLAG}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_LAVF_KEEPSIDE_FLAG}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_OLD_ROTATE_API}
  {$IF LIBAVFORMAT_VERSION_MAJOR < 58}
    {$DEFINE FF_API_OLD_ROTATE_API}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_R_FRAME_RATE}
  {$DEFINE FF_API_R_FRAME_RATE}
{$ENDIF}

{$endif} (* AVFORMAT_VERSION_H *)


