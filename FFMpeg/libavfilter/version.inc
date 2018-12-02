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

{$ifndef AVFILTER_VERSION_H}
{$define AVFILTER_VERSION_H}

(**
 * @file
 * @ingroup lavfi
 * Libavfilter version macros
 *)

//#include "libavutil/version.h"

const
 LIBAVFILTER_VERSION_MAJOR    =   6;
 LIBAVFILTER_VERSION_MAJOR_S  =  '6';

 LIBAVFILTER_VERSION_MINOR    =   107;
 LIBAVFILTER_VERSION_MINOR_S  =  '107';

 LIBAVFILTER_VERSION_MICRO    =  100;
 LIBAVFILTER_VERSION_MICRO_S  = '100';

 LIBAVFILTER_VERSION_INT      = (LIBAVFILTER_VERSION_MAJOR shl 16) or
                              (LIBAVFILTER_VERSION_MINOR shl 8) or
                               LIBAVFILTER_VERSION_MICRO;


 LIBAVFILTER_VERSION          = LIBAVFILTER_VERSION_MAJOR_S + '.' +
                              LIBAVFILTER_VERSION_MINOR_S + '.' +
                              LIBAVFILTER_VERSION_MICRO_S;

 LIBAVFILTER_BUILD            = LIBAVFILTER_VERSION_INT;

 LIBAVFILTER_IDENT            = 'Lavfi'+LIBAVFILTER_VERSION;

{$IFDEF MSWINDOWS}
 LIBAVFILTER_LIBNAME          = 'avfilter-'+ LIBAVFILTER_VERSION_MAJOR_S + '.dll';
{$ENDIF}

{$IFDEF UNIX}
 LIBAVFILTER_LIBNAME          = 'libavfilter.so';
{$ENDIF}
(**
 * FF_API_* defines may be placed below to indicate public API that will be
 * dropped at a future version bump. The defines themselves are not part of
 * the public API and may change, break or disappear at any time.
 *)

{$IFNDEF FF_API_OLD_FILTER_OPTS}
  {$IF LIBAVFILTER_VERSION_MAJOR < 7}
    {$DEFINE FF_API_OLD_FILTER_OPTS}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_OLD_FILTER_OPTS_ERROR}
  {$IF LIBAVFILTER_VERSION_MAJOR < 7}
    {$DEFINE FF_API_OLD_FILTER_OPTS_ERROR}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_AVFILTER_OPEN}
  {$IF LIBAVFILTER_VERSION_MAJOR < 7}
    {$DEFINE FF_API_AVFILTER_OPEN}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_AVFILTER_INIT_FILTER}
  {$IF LIBAVFILTER_VERSION_MAJOR < 7}
    {$DEFINE FF_API_AVFILTER_INIT_FILTER}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_OLD_FILTER_REGISTER}
  {$IF LIBAVFILTER_VERSION_MAJOR < 7}
    {$DEFINE FF_API_OLD_FILTER_REGISTER}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_NOCONST_GET_NAME}
  {$IF LIBAVFILTER_VERSION_MAJOR < 7}
    {$DEFINE FF_API_NOCONST_GET_NAME}
  {$IFEND}
{$ENDIF}

{$IFNDEF FF_API_LAVR_OPTS}
  {$IF LIBAVFILTER_VERSION_MAJOR < 7}
    {$DEFINE FF_API_LAVR_OPTS}
  {$IFEND}
{$ENDIF}

{$endif} (* AVFILTER_VERSION_H *)
