unit libavutil;
{$ALIGN 8}
{$MINENUMSIZE 4}
interface

{$i libavutil\version.inc}
{$i libavutil\min_error.inc}
{$i libavutil\min_rational.inc}
{$i libavutil\min_samplefmt.inc}
{$i libavutil\min_mem.inc}
{$i libavutil\min_pixfmt.inc}
{$i libavutil\min_dict.inc}
{$i libavutil\min_time.inc}
{$i libavutil\min_common.inc}
{$i libavutil\min_log.inc}
{$i libavutil\min_opt.inc}
{$i libavutil\min_buffer.inc}
{$i libavutil\min_avutil.inc}
{$i libavutil\min_frame.inc}
{$i libavutil\min_channel_layout.inc}
{$i libavutil\min_imgutils.inc}


implementation
{$i libavutil\min_common_impl.inc}
{$i libavutil\min_mem_impl.inc}
{$i libavutil\min_rational_impl.inc}

end.
