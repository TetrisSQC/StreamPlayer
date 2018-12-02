
type
(**
 * Structure describes basic parameters of the device.
 *)
  PPAVDeviceInfo = ^PAVDeviceInfo;
  PAVDeviceInfo = ^TAVDeviceInfo;
  TAVDeviceInfo = record
    device_name: PAnsiChar;                   (**< device name, format depends on device *)
    device_description: PAnsiChar;            (**< human friendly name *)
  end;

(**
 * List of devices.
 *)
  PPAVDeviceInfoList = ^PAVDeviceInfoList;
  PAVDeviceInfoList = ^TAVDeviceInfoList;
  TAVDeviceInfoList = record
    devices: PPAVDeviceInfo;                  (**< list of autodetected devices *)
    nb_devices: Integer;                      (**< number of autodetected devices *)
    default_device: Integer;                  (**< index of default device or -1 if no default *)
  end;


(**
 * Structure describes device capabilities.
 *
 * It is used by devices in conjunction with av_device_capabilities AVOption table
 * to implement capabilities probing API based on AVOption API. Should not be used directly.
 *)
 PPAVDeviceCapabilitiesQuery = ^PAVDeviceCapabilitiesQuery;
 PAVDeviceCapabilitiesQuery = ^TAVDeviceCapabilitiesQuery;
 TAVDeviceCapabilitiesQuery =record
//    const AVClass *av_class;
    av_class:Pointer;
    device_context: PAVFormatContext;
    codec: TAVCodecID;
    sample_format: TAVSampleFormat;
    pixel_format: TAVPixelFormat;
    sample_rate: Integer;
    channels: Integer;
    channel_layout: Int64;
    window_width: Integer;
    window_height: Integer;
    frame_width: Integer;
    frame_height: Integer;
    fps: TAVRational;

 end;

(**
 * Initialize libavdevice and register all the input and output devices.
 *)
procedure avdevice_register_all; cdecl;
    external LIBAVDEVICE_LIBNAME name 'avdevice_register_all';



