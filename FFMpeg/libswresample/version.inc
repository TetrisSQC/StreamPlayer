(*
 * Version macros.
 *
 * This file is part of libswresample
 *
 * libswresample is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libswresample is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libswresample; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *)

{$ifndef SWRESAMPLE_VERSION_H}
{$define SWRESAMPLE_VERSION_H}

(**
 * @file
 * Libswresample version macros
 *)

//#include "LIBSWRESAMPLE/avutil.h"


const
 LIBSWRESAMPLE_VERSION_MAJOR    =   2;
 LIBSWRESAMPLE_VERSION_MAJOR_S  =  '2';

 LIBSWRESAMPLE_VERSION_MINOR    =   9;
 LIBSWRESAMPLE_VERSION_MINOR_S  =  '9';

 LIBSWRESAMPLE_VERSION_MICRO    =  100;
 LIBSWRESAMPLE_VERSION_MICRO_S  = '100';

 LIBSWRESAMPLE_VERSION_INT      = (LIBSWRESAMPLE_VERSION_MAJOR shl 16) or
                              (LIBSWRESAMPLE_VERSION_MINOR shl 8) or
                               LIBSWRESAMPLE_VERSION_MICRO;


 LIBSWRESAMPLE_VERSION          = LIBSWRESAMPLE_VERSION_MAJOR_S + '.' +
                              LIBSWRESAMPLE_VERSION_MINOR_S + '.' +
                              LIBSWRESAMPLE_VERSION_MICRO_S;

 LIBSWRESAMPLE_BUILD            = LIBSWRESAMPLE_VERSION_INT;

 LIBSWRESAMPLE_IDENT            = 'SwR'+LIBSWRESAMPLE_VERSION;

{$IFDEF MSWINDOWS}
 LIBSWRESAMPLE_LIBNAME          = 'swresample-'+ LIBSWRESAMPLE_VERSION_MAJOR_S + '.dll';
{$ENDIF}
{$IFDEF UNIX}
 LIBSWRESAMPLE_LIBNAME          = 'libswresample.so';
{$ENDIF}

{$endif} (* SWRESAMPLE_VERSION_H *)
