type
 PRDFTContext=Pointer;
 PFFTSample=Pointer;

procedure av_rdft_end(s:PRDFTContext); cdecl;
         external LIBAVCODEC_LIBNAME name 'av_rdft_end';


