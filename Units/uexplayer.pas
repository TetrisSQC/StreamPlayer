unit uexplayer;

{$HINTS OFF}
{
Written by R.Sombrowsky info@somby.de
}
{.$define log}// if you want to log the queued frames
interface

uses
 {$IFDEF MSWINDOWS}   shlobj, {$ENDIF}
 {$IFDEF UNIX} gtk2, gdk2x, {$ENDIF}
  Classes, SysUtils, Controls, Graphics, ExtCtrls,
  ffutils, libavformat, libavutil, libavcodec, libswscale, libswresample,
  libavdevice,
  sdl2, sdl2_ttf, syncobjs;

type

  TCustomEXPlayerControl = class;

  TEXPlayerQueueRec = record
    FValid: boolean;
    FFrame: PAVFrame;
  end;

  { TEXPlayerQueue }
  TEXPlayerQueue = class
  private
    //  FType:integer;  // nur zum test
    FPlayer: TCustomEXPlayerControl;
    FPutEvent,               // Event will set after a valid Put
    FGetEvent: TEvent;       // Event will set after a valid Get
    FPutIndex, FGetIndex: integer;
    FQueue: array of TEXPlayerQueueRec;
    FUnblockGet: boolean;
    FUnblockPut: boolean;
    procedure IncIndex(var ix: integer);
  protected
  public
    constructor Create(player: TCustomEXPlayerControl; maxframes: integer); virtual;
    destructor Destroy; override;
    function Put(fr: PAVFrame; wait: boolean = True): boolean;

    function Get(var fr: PAVFrame; wait: boolean = True): boolean;
    // Get the next frame if exists if the frame is valid you must free the frame self

    function Peek(var fr: PAVFrame): boolean;
    procedure Del; // invalidate the current getindex but the frame dont free
    procedure WakeUpPut; // if its waiting
    procedure WakeUpGet; // if its waiting
    procedure Clear;

    property UnblockPut: boolean read FUnblockPut write FUnblockPut;

    property UnblockGet: boolean read FUnblockGet write FUnblockGet;



  end;

  { TReadThread }
  TReadThread = class(TThread)
  protected
    FParent: TCustomEXPlayerControl;
    procedure Execute; override;
  public
    constructor Create(const AParent: TCustomExplayerControl);
    destructor Destroy; override;
  end;

  { TCustomEXPlayerControl }
  TCustomEXPlayerControl = class(TCustomControl)
  private
    FShowTimeHeight: double;
    FTimerId: TSDL_TimerID;
    FBadFrameTolerance: double;
    FSeeking: boolean;
    FDisableAudio: boolean;
    FDisableVideo: boolean;
    FLastErrorString: string;
    FPausing, FPlaying: boolean;
    FMute: boolean;
    FShowTimes: boolean;
    FTimerInterval: integer;
    FUrl: string;
    FWindow: PSDL_Window;
    FRenderer: PSDL_Renderer;
    FTexture: PSDL_Texture;
    FTextureValid: boolean;
    FSDLAudioBufferSize: word;
    FSDLPixelFormat: cardinal;
    FAVPixelFormat: TAVPixelFormat;
    FFont: PTTF_Font;
    FReadThread: TReadThread;

    // To the URL
    FFormatCtx: PAVFormatContext;


    // Video
    FVideoStreamIndex: integer;
    FVideoTimeBase: double;
    FVideoCodec: PAVCodec;
    FVideoCtx: PAVCodecContext;
    FVideoSwsCtx: PSwsContext;
    FVideoDecodeFrame: PAVFrame;
    FVideoDecodeFrameBuffer: PByte;
    FVideoQueue: TEXPlayerQueue;
    FVideoSynchPts,              // Synchronize-frame-pts
    FVideoSynchTime,             // Synchronize-real-time
    FVideoPts: int64;             // pts of the last shown videoframe
    FVideoWidth, FVideoHeight: integer;
    //   FVideoDuration:int64;
    FVideoShowNextFrame: boolean;
    // will be set to show the first frame if the state is paused
    FVideoSar: TAVRational;       // aspect-ratio of the last shown frame



    // Audio
    FAudioStreamIndex: integer;
    FAudioTimeBase: double;
    FAudioCodec: PAVCodec;
    FAudioCtx: PAVCodecContext;
    FAudioSwrCtx: PSwrContext;
    FAudioBuffer: PByte;          // here are the konvertioned Audiodata kumulativ
    FAudioBufSizeMax, FAudioBufSize, FAudioBufIndex: integer;
    FAudioQueue: TEXPlayerQueue;
    FAudioPts: int64;

    FReadThreadSleep: boolean;    // if the  ReadThread schould sleeping
    FReadThreadSleeping: boolean; // if the  Readthread is sleeping

    FEof: boolean;               // if the readthread has reached eof
    FLastTimeString: string;

    FDelayAudioQueueing: boolean;
    FMaxAudioFrames, FMaxVideoFrames: integer;

    function GetAudioAvailable: boolean;
    function GetDuration: int64;
    function GetPosition: int64;
    function GetVideoAvailable: boolean;
    function GetVideoTimeCheck: integer;
    procedure SetMaxAudioFrames(AValue: integer);
    procedure SetMaxVideoFrames(AValue: integer);
    procedure SetVideoTimeCheck(AValue: integer);
  protected
    procedure CreateWindowAndRenderer; virtual;
    procedure DestroyWindowAndRenderer; virtual;

    function LoadText(s: string; co: TSDL_Color; var w, h: integer): PSDL_Texture; virtual;
    procedure DoOnResize; override;

    procedure DoTimer; virtual;
    procedure DoInternalOnResize; virtual;
    procedure DoOnEof; virtual;
    procedure DoUpdateProgress; virtual;

    procedure StartTimer; virtual;
    procedure StopTimer; virtual;

    function GetRendererFlags: longword; virtual;
  public
    constructor Create(AOwner: TComponent); override;
    procedure DestroyWnd; override;
    destructor Destroy; override;
    procedure SetPixelFormat(SDLPixelFormat: cardinal;
      AVPixelFormat: TAVPixelFormat); virtual;
    // set the SDL-renderformat and the compatible FFMPEG-decoderformat

    procedure PaintOverlays(renderer: PSDL_Renderer; var present: boolean); virtual;
    // We can show own Overlays as Images etc.
    // Do only set present to true if it is necessary never to false !!!

    procedure PaintTimes(renderer: PSDL_Renderer; var present: boolean); virtual;
    // Part of PaintOverlays

    procedure PaintImage; virtual; // shows the last image in the texture
    procedure Paint; override;    // Main showing

    function Play: boolean; virtual;  // Staring the play
    procedure Pause; virtual;        // Pause the play
    procedure Resume; virtual;       // Resume a pausing play
    procedure Stop; virtual;         // Stops the play

    procedure Seek(ms: int64); virtual; // Seeking only if there is a duration and
    // the time in ms is greater 0 or lower as duration

    property VideoTimeCheck: integer read GetVideoTimeCheck write SetVideoTimeCheck;
    // Timeinterval for sampling rate for video-frames

    property LastErrorString: string read FLastErrorString write FLastErrorString;
    // Last errorstring etc. after play is false

    property Url: string read FUrl write FUrl;
    // url or filename to play

    property Mute: boolean read FMute write FMute;
    // Mute the sound

    property DisableAudio: boolean read FDisableAudio write FDisableAudio;
    // disable then sound (must be set before play)
    property DisableVideo: boolean read FDisableVideo write FDisableVideo;
    // disable the video (must be set before play)
    property MaxAudioFrames: integer read FMaxAudioFrames write SetMaxAudioFrames;
    // set the max capacity of audioqueue

    property MaxVideoFrames: integer read FMaxVideoFrames write SetMaxVideoFrames;
    // set the max capacity of audioqueue

    property BadFrameTolerance: double read FBadFrameTolerance write FBadFrameTolerance;
    // in s.
    // after check i found that on livestreams it could be that one or more frames
    // had a pts far away from the last frame-pts.
    // This frames are deleted because these freezing the video.


    property Playing: boolean read FPlaying;  // The media is playing
    property Pausing: boolean read FPausing;  // The media is pausing

    property ShowTimes: boolean read FShowTimes write FShowTimes;
    // Here you can switch on or off to show the times.
    // If on and the media is playing:
    // If the media has a duration then the current position and the duration
    // are shown in the right down corner.
    // If the media has no duration then the current time is showing


    property Duration: int64 read GetDuration;
    // Duration in ms, bei Livestreams -1

    property Position: int64 read GetPosition;
    // Current Position in ms, bei Livestreams -1

    property ShowTimeHeight: double read FShowTimeHeight write FShowTimeHeight;
    // Height of Showtime in % to ControlHeight

    property Renderer: PSDL_Renderer read FRenderer;
    property SDLFont: PTTF_Font read FFont;

    property AudioAvailable: boolean read GetAudioAvailable;
    // Only when Playing

    property VideoAvailable: boolean read GetVideoAvailable;
    // Only when Playing

  end;

  { TEXPlayerControl }
  TEXPlayerControlEof = procedure(Sender: TObject; var handled: boolean) of object;

  TEXPlayerControl = class(TCustomEXPlayerControl)
  private
    FOnEof: TEXPlayerControlEof;
    FOnProgress: TNotifyEvent;
  protected
    procedure DoOnEof; override;
    procedure DoUpdateProgress; override;
  published
    property Action;
    property Align;
    property Anchors;
    property BadFrameTolerance;
    property BorderSpacing;
    property Constraints;
    property DisableAudio;
    property DisableVideo;
    property Duration;
    property Enabled;
    property Font;
    property Hint;
    property MaxAudioFrames;
    property MaxVideoFrames;
    property Mute;
    property ParentBidiMode;
    property ParentFont;
    property ParentShowHint;
    property PopupMenu;
    property Position;
    property ShowHint;
    property ShowTimes;
    property Url;
    property VideoTimeCheck;
    property Visible;

    property OnChangeBounds;
    property OnClick;
    property OnDblClick;
    property OnEof: TEXPlayerControlEof read FOnEof write FOnEof;
    property OnMouseDown;
    property OnMouseEnter;
    property OnMouseLeave;
    property OnMouseMove;
    property OnMouseUp;
    property OnMouseWheel;
    property OnMouseWheelDown;
    property OnMouseWheelUp;
    property OnProgress: TNotifyEvent read FOnProgress write FOnProgress;
    property OnResize;

  end;

{$ifdef log}
var
  enablelog: boolean;
  logcount: integer;
// you can use it from other units
procedure wLog(s: string);
{$endif}
implementation

resourcestring
{
  rsNoStreamFound='Es wurde weder ein Video- noch ein Audiostream gefunden.';
  rsPlayerActive='Der Player wurde bereits gestartet.';
  rsVideoCtxNotAlloc='Video-Context kann nicht zugewiesen werden.';
  rsVideoSwsCtxNotAlloc='Video-SWS-Context kann nicht zugewiesen werden.';
  rsVideoDecodeFrameNotAlloc='Video-Dekodier-Frame kann nicht zugewiesen werden.';
  rsVideoFrameBufferNotAlloc='Video-Frame-Buffer kann nicht zugewiesen werden.';

  rsAudioCtxNotAlloc='Audio-Context kann nicht zugewiesen werden.';
  rsAudioSwrCtxNotAlloc='Audio-SWR-Context kann nicht zugewiesen werden.';
}

  rsNoStreamFound = 'There was no video- and audiostream found.';
  rsPlayerActive = 'The player already started.';
  rsVideoCtxNotAlloc = 'Video-context could not be allocated.';
  rsVideoSwsCtxNotAlloc = 'Video-SWS-context could not be allocated.';
  rsVideoDecodeFrameNotAlloc = 'Video-decode-frame could not be allocated.';
  rsVideoFrameBufferNotAlloc = 'Video-frame-buffer could not be allocated.';

  rsAudioCtxNotAlloc = 'Audio-context could not be allocated.';
  rsAudioSwrCtxNotAlloc = 'Audio-SWR-context could not be allocated.';




var
  InitSFOk: boolean;
  InitSFCount: integer;
  InitSFErrorString: string; // if initilization with error
{$ifdef log}
  log: TFileStream;
  logcs: TCriticalSection;

procedure wLog(s: string);
begin
  if not enablelog then
    exit;
  s := s + sLineBreak;
  logcs.Enter;
  try
    log.Write(s[1], length(s));
  finally
    logcs.Leave;
  end;
end;

{$endif}

function MulDiv(const a, b, c: Int32): Int32; inline;
begin
  Result := int64(a) * int64(b) div c;
end;

procedure InitSF;  // SDL und FFMPEG initialize
begin
  Inc(InitSFCount);
  if InitSFCount <> 1 then
    exit;   // already initialized

{$ifdef log}
  logcs := TCriticalSection.create;
  log := TFileStream.Create(IncludeTrailingPathDelimiter(GetTempDir)+'log.txt', fmcreate or fmShareDenyWrite);
{$endif}
  InitSFOk := False;
  av_register_all();
  avcodec_register_all();
  avdevice_register_all();
  avformat_network_init();

  if SDL_Init(SDL_INIT_AUDIO or SDL_INIT_VIDEO or SDL_INIT_TIMER) < 0 then
  begin
    InitSFErrorString := format('SDL_Init-Error: %s', [string(SDL_GetError)]);
    exit;
  end;

  if not SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, '1') then
  begin
    InitSFErrorString := 'SDL_SetHint LinearTexturRendering not set';
    exit;
  end;

  SDL_ShowCursor(0); // We switch off then SDL-Cursor because there are some
  // problems if the SDL-Cursor is on and the control-cursor
  // is off

  if TTF_Init < 0 then
  begin
    InitSFErrorString := format('TTF_Init-Error: %s', [string(TTF_GetError)]);
    exit;
  end;

  InitSFOk := True;
end;

procedure DoneSF;
begin
  if InitSFCount > 0 then
  begin
    Dec(InitSFCount);
  end
  else
    exit;
  if InitSFCount > 0 then
    exit;
{$ifdef log}
  log.Free;
  logcs.free;
{$endif}

  avformat_network_deinit();
  TTF_Quit();
  SDL_Quit();

end;



function GetTimeString(t: int64;    // Time in ms
  showms: boolean = False): string;
var
  ms, dd, hh, mm, ss: integer;
begin

  ms := t mod 1000;
  t := t div 1000;
  ss := t mod 60;
  t := t div 60;
  mm := t mod 60;
  t := t div 60;
  hh := t mod 60;
  dd := t div 24;
  if dd <> 0 then
    Result := format('%d:%.2d:%.2d:%.2d', [dd, hh, mm, ss])
  else
    Result := format('%.2d:%.2d:%.2d', [hh, mm, ss]);
  if showms then
    Result := Result + '.' + format('%.3d', [ms]);
end;


{ TReadThread }
constructor TReadThread.Create(const AParent: TCustomEXPlayerControl);
begin
  inherited Create(False);
  FParent := AParent;
end;

destructor TReadThread.Destroy;
begin
  Terminate;
  WaitFor;
  inherited;
end;

procedure TReadThread.Execute;
// Read all packets decode it an put it in then Queues if there is space
var
  rc: integer;
  EnableAudioQueue: boolean;
  pa: PAVPacket;
{$ifdef log}
  ca, cv: integer;

{$endif}

  procedure QueueFrame(ct: PAVCodecContext; q: TEXPlayerQueue;
    pp: PAVPacket; video: boolean);
  var
    rc: integer;
    ts: int64;
    pq: boolean;
{$ifdef log}
    c: integer;
{$endif}
    fr: PAVFrame;
  begin
    rc := avcodec_send_packet(ct, pp);
    if (rc < 0) then
    begin
      exit;
    end;
{$ifdef log}
    c := 0;
{$endif}
    repeat
      if FParent.FReadThreadSleep then
        break;
      fr := av_frame_alloc();
      if fr = nil then
        break;
      rc := avcodec_receive_frame(ct, fr);
      if (rc = 0) then
      begin
        ts := av_frame_get_best_effort_timestamp(fr);
        // we Queded only the frames with timestamp>=0
        //            wLog(format('  V:%d ts:%d',[integer(video),ts]));
        pq := False;
        if (not FParent.FReadThreadSleep) and (ts >= 0) then
        begin
          if video then
          begin
            // we enable the queuing of audioframes only
            // if we have the first videoframe.
            // We do it so because we could see a dark image for a longer time
            EnableAudioQueue := True;
            pq := True;
          end
          else
          begin
            if EnableAudioQueue then
              pq := True;
          end;
        end;

        if pq then
        begin
{$ifdef log}
          Inc(c);
          wLog(format('  PutFrame: %.5d be:%.9d dts:%.9d pts:%.9d rp:%d',
            [c, av_frame_get_best_effort_timestamp(fr), fr^.pkt_dts,
            fr^.pts, fr^.repeat_pict]));
{$endif}
          if not q.Put(fr) then
          begin
            // not FPlaying or Unblock
            av_frame_free(@fr);
            break;
          end;
        end
        else
        begin
          av_frame_free(@fr);
        end;
        ;
      end
      else
      begin
        av_frame_free(@fr);
        break;
      end;
    until False;

  end;

begin
{$ifdef log}
  ca := 0;
  cv := 0;
  wLog('ReadThread started');
{$endif}
  pa := av_packet_alloc;
  with FParent do
  begin
    EnableAudioQueue := FVideoStreamIndex < 0;
    // If we want to show video we wait at the beginning with queueing of audioframes
    // so long if we have the first videoframe
    while FPlaying and not Terminated do
    begin
      if FReadThreadSleep or FEof then
      begin
        // In the sleeping-mode the Thread can be suspended because it is not blocked
        FReadThreadSleeping := True;
        Sleep(20);
        Continue;
      end;
      if FDelayAudioQueueing then
      begin
        if FVideoStreamIndex >= 0 then
          EnableAudioQueue := False; // we wait until to the first videoframe
        FDelayAudioQueueing := False;
      end;
      rc := av_read_frame(FFormatCtx, pa);
      if (rc < 0) then
      begin
        av_packet_unref(pa);
        if rc = AVERROR_EOF then
        begin
          FEof := True;
          Continue;
        end
        else
        begin
          break;
        end;
      end;
      if (FAudioStreamIndex >= 0) and (pa^.stream_index = FAudioStreamIndex) then
      begin
{$ifdef log}
        Inc(ca);
        wLog(format('AudioPacket: %.5d dts:%.9d pts:%.9d', [ca, pa^.dts, pa^.pts]));
{$endif}
        QueueFrame(FAudioCtx, FAudioQueue, pa, False);
      end
      else
      if (FVideoStreamIndex >= 0) and (pa^.stream_index = FVideoStreamIndex) then
      begin
{$ifdef log}
        Inc(cv);
        wLog(format('VideoPacket: %.5d dts:%.9d pts:%.9d', [cv, pa^.dts, pa^.pts]));
{$endif}
        QueueFrame(FVideoCtx, FVideoQueue, pa, True);
      end;
      av_packet_unref(pa);
    end;
  end;
  av_packet_free(@pa);
{$ifdef log}
  wLog('ReadThread ended');
{$endif}

end;


procedure AudioThread(userdata: Pointer; stream: PByte; len: integer); cdecl;
var
  si, len1: integer;
  b: PByte;
  fr: PAVFrame;
  da: PByte;
  ts: int64;
begin
  with TCustomEXPlayerControl(userdata) do
  begin
    da := nil;
    fr := nil;
    while (len > 0) and FPlaying do
    begin
      if FAudioBufIndex >= FAudioBufSize then
      begin
        // We have sended all audiodata to sdl. Now we get the next audiodata
        // from the queue and decode it.
        // If we have no frames or we must wait for then next frames
        // we dont blocking we give sdl only 0-data (no sound).
        //              fr:=FAudioQueue.Get(true,false); // no blocking
        //              if fr=nil then begin
        if not FAudioQueue.Get(fr, False) then
        begin
          if (not FPlaying) then
            break; // end
          // clear buffer
          fillchar(stream^, len, 0);   // silence
          if FEof then
            TThread.Synchronize(nil, @DoOnEof);
          break;
        end;
        si := av_samples_get_buffer_size(nil, FAudioCtx^.channels,
          fr^.nb_samples,
          AV_SAMPLE_FMT_FLT, 1);
        ReAllocMem(da, si);
        if FMute or FVideoShowNextFrame then
        begin
          fillchar(FAudioBuffer^, si, 0);
        end
        else
        begin
          swr_convert(FAudioSwrCtx, @da,
            fr^.nb_samples,
            @fr^.Data[0], fr^.nb_samples);
          Move(da^, FAudioBuffer^, si);
        end;
        FAudioBufIndex := 0;
        FAudioBufSize := si;
        ts := av_frame_get_best_effort_timestamp(fr);
        if ts >= 0 then
        begin
          FAudioPts := ts;
          TThread.Synchronize(nil, @DoUpdateProgress);
        end;
        av_frame_free(@fr);
      end;
      len1 := FAudioBufSize - FAudioBufIndex; // What is to send from then audiobuffer
      if (len1 > len) then
        len1 := len;   // First we send only a part
      b := FAudioBuffer;
      Inc(b, FAudioBufIndex);
      Move(b^, stream^, len1);
      Dec(len, len1);
      Inc(stream, len1);
      Inc(FAudioBufIndex, len1);
    end;
    ReAllocMem(da, 0);
  end;

end;



function TimerThread(interval: UInt32; param: Pointer): UInt32; cdecl;
begin
  TThread.Synchronize(nil, @TCustomEXPlayerControl(param).DoTimer);
  Result := interval;
end;

{ TEXPlayerQueue }

procedure TEXPlayerQueue.IncIndex(var ix: integer);
begin
  Inc(ix);
  if ix >= Length(FQueue) then
    ix := 0;
end;

constructor TEXPlayerQueue.Create(player: TCustomEXPlayerControl; maxframes: integer);
var
  i: integer;
begin
  FPlayer := player;
  FGetEvent := TEvent.Create(nil, False, False, '');
  FPutEvent := TEvent.Create(nil, False, False, '');

  SetLength(FQueue, maxframes);
  for i := 0 to Length(FQueue) - 1 do
    with FQueue[i] do
    begin
      FValid := False;
      FFrame := nil;
    end;

end;

destructor TEXPlayerQueue.Destroy;
begin
  Clear;
  SetLength(FQueue, 0);
  FGetEvent.Free;
  FPutEvent.Free;
  inherited Destroy;
end;

function TEXPlayerQueue.Put(fr: PAVFrame; wait: boolean): boolean;
begin
  Result := False;
  if fr = nil then
    exit;
  if (not FPlayer.FPlaying) or FUnblockPut then
    exit;
  with FQueue[FPutIndex] do
  begin
    while FValid do
    begin
      // Current index valid yet
      //         wLog(format('PutMsg CheckIndex:%d valid',[FPutIndex]));
      if (not FPlayer.FPlaying) or FUnblockPut then
        exit;
      if not wait then
        exit;
      FGetEvent.WaitFor(INFINITE); // Warten, bis abgeholt wird
    end;
    // Element ist leer und kann gefüllt werden
    FFrame := fr;
    FValid := True;
  end;
  IncIndex(FPutIndex);
  FPutEvent.SetEvent;
  //  wLog(format('PutMsg:%s NewIndex:%d',[msg,FPutIndex]));
  Result := True;

end;

function TEXPlayerQueue.Get(var fr: PAVFrame; wait: boolean): boolean;
begin
  Result := False;
  if (not FPlayer.FPlaying) or FUnblockGet then
    exit;
  //  wLog(format('GetMsg CheckIndex:%d',[FGetIndex]));
  with FQueue[FGetIndex] do
  begin
    while not FValid do
    begin
      if (not FPlayer.FPlaying) or FUnblockGet then
        exit;
      if not wait then
        exit;
      FPutEvent.WaitFor(INFINITE); // Warten, bis etwas da ist
    end;
    // Element ist voll und kann geleert werden
    fr := FFrame;
    FFrame := nil;
    FValid := False;  // the position can be used to put
  end;
  IncIndex(FGetIndex);
  FGetEvent.SetEvent;
  //  wLog(format('GetMsg:%s NextIndex:%d',[msg,FGetIndex]));
  Result := True;

end;

function TEXPlayerQueue.Peek(var fr: PAVFrame): boolean;
begin
  with FQueue[FGetIndex] do
  begin
    fr := FFrame;
    Result := FValid;
  end;
end;

procedure TEXPlayerQueue.Del;
begin
  with FQueue[FGetIndex] do
  begin
    if not FValid then
      exit;
    FFrame := nil;
    FValid := False;
  end;
  IncIndex(FGetIndex);
  FGetEvent.SetEvent;
end;


procedure TEXPlayerQueue.WakeUpPut;
begin
  FGetEvent.SetEvent;   // get-event must be wake up because put could be waiting for it
end;

procedure TEXPlayerQueue.WakeUpGet;
begin
  FPutEvent.SetEvent;   // put-event must be wake up because get could be waiting for it
end;

procedure TEXPlayerQueue.Clear;
var
  i: integer;
begin
  for i := 0 to Length(FQueue) - 1 do
    with FQueue[i] do
    begin
      FValid := False;
      if FFrame <> nil then
        av_frame_free(@FFrame);
    end;
  FPutIndex := 0;
  FGetIndex := 0;

end;

{ TEXPlayerControl }

procedure TEXPlayerControl.DoOnEof;
var
  handled: boolean;
begin
  handled := False;
  if Assigned(FOnEof) then
    FOnEof(self, handled);
  if not handled then
    inherited DoOnEof;
end;

procedure TEXPlayerControl.DoUpdateProgress;
begin
  inherited DoUpdateProgress;
  if Assigned(FOnProgress) then
    FOnProgress(self);
end;


function TCustomEXPlayerControl.GetVideoTimeCheck: integer;
begin
  Result := FTimerInterval;
end;

function TCustomEXPlayerControl.GetDuration: int64;
begin
  Result := -1;
  if FFormatCtx = nil then
    exit;
  Result := FFormatCtx^.duration div 1000;
end;

function TCustomEXPlayerControl.GetAudioAvailable: boolean;
begin
  Result := FAudioStreamIndex >= 0;
end;



function TCustomEXPlayerControl.GetPosition: int64;
begin
  Result := -1;
  if (FAudioStreamIndex >= 0) then
  begin
    if (FAudioPts >= 0) then
    begin
      Result := round(FAudioPts * FAudioTimebase * 1000);
      if FFormatCtx^.start_time <> AV_NOPTS_VALUE then
        Result := Result - round(FFormatCtx^.start_time / 1000);
    end;
  end;
  if Result >= 0 then
    exit;
  if (FVideoStreamIndex >= 0) then
  begin
    if (FVideoPts >= 0) then
    begin
      Result := round(FVideoPts * FVideoTimebase * 1000);
      if FFormatCtx^.start_time <> AV_NOPTS_VALUE then
        Result := Result - round(FFormatCtx^.start_time / 1000);
    end;
  end;
end;


function TCustomEXPlayerControl.GetVideoAvailable: boolean;
begin
  Result := FVideoStreamIndex >= 0;
end;


procedure TCustomEXPlayerControl.SetMaxAudioFrames(AValue: integer);
begin
  if (AValue <= 0) then
    exit;
  FMaxAudioFrames := AValue;
end;

procedure TCustomEXPlayerControl.SetMaxVideoFrames(AValue: integer);
begin
  if (AValue <= 0) then
    exit;
  FMaxVideoFrames := AValue;
end;

procedure TCustomEXPlayerControl.SetVideoTimeCheck(AValue: integer);
begin
  FTimerInterval := AValue;
end;

procedure TCustomEXPlayerControl.DoTimer;
begin
  if (not FPlaying) or (FPausing and not FVideoShowNextFrame) then
    exit;
  Invalidate;
  // Repaint;   // not use it blocks the messages
end;

procedure TCustomEXPlayerControl.CreateWindowAndRenderer;
var
  ok: boolean;
begin
  if FWindow <> nil then
    exit;     // already created
  ok := False;
  try
   // FWindow := SDL_CreateWindow('Player', 0, 0, 800,600,SDL_WINDOW_OPENGL or SDL_WINDOW_SHOWN or SDL_WINDOW_RESIZABLE);
{$IFDEF MSWINDOWS}
    FWindow := SDL_CreateWindowFrom(Pointer(Handle));
{$ENDIF}
{$IFDEF UNIX}
    FWindow := SDL_CreateWindowFrom(Pointer(GDK_WINDOW_XWINDOW(PGtkWidget(PtrUInt(Handle))^.window)));
{$ENDIF}
    if FWindow = nil then
    begin
      FlastErrorString := format('SDL_CreateWindowFrom-Error: %s', [string(SDL_GetError)]);
      exit;
    end;

    FRenderer := SDL_CreateRenderer(FWindow, -1, GetRendererFlags);
    if FRenderer = nil then
    begin
      FLastErrorString := format('SDL_CreateRenderer: %s', [string(SDL_GetError)]);
      exit;
    end;

    if SDL_SetRenderDrawColor(FRenderer, 0, 0, 0, 255) < 0 then
    begin
      FLastErrorString := format('SDL_SetRenderDrawColor: %s', [string(SDL_GetError)]);
      exit;
    end;
    ok := True;

  finally
    if not ok then
      DestroyWindowAndRenderer;
  end;
end;

procedure TCustomEXPlayerControl.DestroyWindowAndRenderer;
begin
  if (FRenderer <> nil) then
  begin
    SDL_DestroyRenderer(FRenderer);
    FRenderer := nil;
  end;

  if (FWindow <> nil) then
  begin
    SDL_DestroyWindow(FWindow);
    FWindow := nil;
  end;

end;

function TCustomEXPlayerControl.LoadText(s: string; co: TSDL_Color;
  var w, h: integer): PSDL_Texture;
var
  su: PSDL_Surface;
begin
  Result := nil;
  su := TTF_RenderUTF8_Blended(FFont, PAnsiChar(s), co);
  if su = nil then
  begin
    exit;
  end;
  try
    Result := SDL_CreateTextureFromSurface(FRenderer, su);
    if Result = nil then
    begin
      exit;
    end;
    SDL_SetTextureBlendMode(Result, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(Result, co.a);

    if SDL_QueryTexture(Result, nil, nil, @w, @h) <> 0 then
    begin
      SDL_DestroyTexture(Result);
      Result := nil;
    end;
  finally
    if su <> nil then
    begin
      SDL_FreeSurface(su);
    end;
  end;

end;

procedure TCustomEXPlayerControl.DoInternalOnResize;
var
  pr: boolean;
begin
  if FRenderer = nil then
    exit;
  PaintImage;
  pr := True;
  PaintOverLays(FRenderer, pr);
  SDL_RenderPresent(FRenderer);
end;

procedure TCustomEXPlayerControl.DoOnResize;
begin
  inherited DoOnResize;

  TThread.Synchronize(nil, @DoInternalOnResize);
end;

procedure TCustomEXPlayerControl.DoOnEof;
begin
  Stop;
end;

procedure TCustomEXPlayerControl.DoUpdateProgress;
begin

end;

procedure TCustomEXPlayerControl.StartTimer;
begin
  if FTimerId <> 0 then
    exit;
  FTimerId := SDL_AddTimer(FTimerInterval, @TimerThread, self);
end;

procedure TCustomEXPlayerControl.StopTimer;
begin
  if FTimerId <> 0 then
  begin
    SDL_RemoveTimer(FTimerId);
    FTimerId := 0;
  end;
end;

function TCustomEXPlayerControl.GetRendererFlags: longword;
begin
  Result := SDL_RENDERER_ACCELERATED or SDL_RENDERER_PRESENTVSYNC;
  // result:=SDL_RENDERER_ACCELERATED;
end;

procedure TCustomEXPlayerControl.Paint;
var
  nfr, fr: PAVFrame;
  its, ts: int64;
  tr, tv: double;
  pr: boolean;
begin
  if (not InitSFOk) // or (FWindow=nil)
  then
    with canvas do
    begin
      // if we integrate the control as part of Lazarus we only use a black window
      Brush.Color := self.Color;
      fillrect(ClientRect);
      exit;
    end;
  CreateWindowAndRenderer;   // if not yet done
  pr := False;
  nfr := nil;
  fr := nil;
  its := -1;
  repeat
    if (not FPlaying) or (FSeeking) or (FTexture = nil) then
    begin
      SDL_RenderClear(FRenderer);
      pr := True;    // we must present it
      break;
    end;
    if FPausing and not FVideoShowNextFrame then
      break;
    if not FVideoQueue.Peek(fr) then
    begin
      // There is no frame we wait for the next invalidate be timer-tick
      if FEof then
        TThread.Synchronize(nil, @DoOnEof);
      break;
    end;
    // valid frame
    ts := av_frame_get_best_effort_timestamp(fr);
    if ts < 0 then
    begin  // could be a B-Frame
      FVideoQueue.Del;
      av_frame_free(@fr);
      Continue;  // next
    end;
    if FVideoSynchPts < 0 then
    begin
      // We set the sysnchonize if we have no audio
      FVideoSynchPts := ts;
      FVideoSynchTime := av_gettime_relative;
    end;
    tv := ts * FVideoTimebase;
    if (FAudioStreamIndex >= 0) then
    begin
      tr := FAudioPts * FAudioTimeBase;
    end
    else
    begin
      tr := (av_gettime_relative - FVideoSynchTime) / 1000000.0 +
        FVideoSynchPts * FVideoTimebase;
    end;

    if (tv > tr + FBadFrameTolerance) then
    begin  // if the frame is out of time
      FVideoQueue.Del;
      av_frame_free(@fr);
      // next
    end
    else
    if (tr >= 0) and                     // if there is a audiopacket
      (tv <= tr) then
    begin
      if nfr <> nil then
        av_frame_free(@nfr);  // old frame not to render
      FVideoQueue.Del;
      nfr := fr;         // mark next frame to render, at the and it will only
      // render the latest frame
      fr := nil;
      its := ts;
    end
    else
    begin
      // The pts of current frame should be shown later,
      // thats why we hold the frame in the queue (not deleting)
      // for the next check.
      // We don't need to presend the renderer
      break;
    end;
  until False;

  if nfr <> nil then
  begin
    if FVideoShowNextFrame then
    begin
      if FPausing then
      begin
        // Stop the Timer
        StopTimer;
        if FAudioStreamIndex >= 0 then
        begin
          SDL_PauseAudio(1);
        end;
      end;
      FVideoShowNextFrame := False;
    end;

    // decodeframe
    sws_scale(FVideoSwsCtx,
      nfr^.Data,
      nfr^.linesize,
      0,
      FVideoCtx^.Height,
      FVideoDecodeFrame^.Data,
      FVideoDecodeFrame^.linesize);

    SDL_UpdateTexture(FTexture, nil, FVideoDecodeFrame^.Data[0],
      FVideoDecodeFrame^.linesize[0]); // langsam
    FTextureValid := True;
    FVideoSar := av_guess_sample_aspect_ratio(FFormatCtx, PPtrIdx(
      FFormatCtx^.streams, FVideoStreamIndex), nfr);
    FVideoPts := its;   // pts of last shown image
    PaintImage;       // Show the Image-Texture
    pr := True;
    if (FAudioStreamIndex < 0) then
      TThread.Synchronize(nil, @DoUpdateProgress); // UpdateProgress
    av_frame_free(@nfr);
  end;
  PaintOverLays(FRenderer, pr);  // We can show own overlays (images etc.) or times
  if pr then
  begin
    SDL_RenderPresent(FRenderer);
  end;
end;

procedure TCustomEXPlayerControl.SetPixelFormat(SDLPixelFormat: cardinal;
  AVPixelFormat: TAVPixelFormat);
begin
  // Is a very good selection for speed
  FSDLPixelFormat := SDLPixelFormat;
  FAVPixelFormat := AVPixelFormat;
end;

procedure TCustomEXPlayerControl.PaintOverlays(renderer: PSDL_Renderer;
  var present: boolean);
begin
  PaintTimes(renderer, present);
end;

procedure TCustomEXPlayerControl.PaintTimes(renderer: PSDL_Renderer; var present: boolean);
var
  tx: PSDL_Texture;
  co: TSDL_Color;
  tis: string;
  ww, hh: integer;
  rd: TSDL_Rect;
  d, p: int64;
begin
  //    result:=false;
  if not FShowTimes then
    exit;
  if not FPlaying then
    exit;
  with co do
  begin
    r := 255;
    g := 0;
    b := 0;
    a := 255;
  end;
  hh := 0;
  ww := 0;
  d := Duration;
  p := Position;

  if (d >= 0) and (p >= 0) then
  begin
    tis := GetTimeString(p) + '   ' + GetTimeString(d);
  end
  else
  begin
    // We take the local-time
    tis := formatdatetime('hh:nn:ss', now);  // Excuse this is the german format
  end;
  if tis <> FLastTimeString then
  begin
    if not present then
      PaintImage;
    present := True;

  end;

  FLastTimeString := tis;
  tx := LoadText(tis, co, ww, hh);
{
    with rd do begin
       x:=Self.Width-ww-10;
       y:=Self.Height-hh-10;
       w:=ww;
       h:=hh;
    end;
}

  with rd do
  begin
    h := round(self.Height * FShowTimeHeight / 100);
    w := round(ww * h / hh);
    x := Self.Width - w - 10;
    y := Self.Height - h - 10;
  end;

  SDL_RenderCopy(FRenderer, tx, nil, @rd);
  SDL_DestroyTexture(tx);
end;

procedure TCustomEXPlayerControl.PaintImage;
var
  ww, hh: integer;
  rd: TSDL_Rect;

begin
  if (FWindow = nil) or (FRenderer = nil) or (FTexture = nil) or
    (FVideoWidth = 0) or (FVideoHeight = 0) then
    exit;
  ww := FVideoWidth;
  hh := FVideoHeight;
  if (FVideoSar.num > 0) and (FVideoSar.den > 0) then
  begin
    ww := round(ww * av_q2d(FVideoSar));
  end;
  rd.w := self.Width;
  rd.h := MulDiv(rd.w, hh, ww);
  if rd.h > self.Height then
  begin
    rd.h := self.Height;
    rd.w := MulDiv(rd.h, ww, hh);
  end;
  rd.x := (self.Width - rd.w) div 2;
  rd.y := (self.Height - rd.h) div 2;

  SDL_RenderClear(FRenderer);
  if FTextureValid then
    SDL_RenderCopy(FRenderer, FTexture, nil, @rd);
end;

{ TCustomEXPlayerControl }
constructor TCustomEXPlayerControl.Create(AOwner: TComponent);
var
  a: array[0..MAX_PATH] of AnsiChar;
  fp: ansistring;

begin

  inherited Create(AOwner);

  if not (csDesigning in ComponentState) then
    InitSF;

  Color := clBlack;
  FTimerInterval := 15;
  FShowTimeHeight := 5;
  if InitSFOk then
  begin
    // Font for Time
    fillchar(a, sizeof(a), 0);
    {$IFDEF MSWINDOWS}
    SHGetSpecialFolderPath(0, @a, CSIDL_FONTS, False);
    fp := string(IncludeTrailingPathDelimiter(a)) + 'Tahoma.ttf';
    {$ELSE}
     fp := '/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf';
    {$ENDIF}
    FFont := TTF_OpenFont(PAnsiChar(fp), 28);
    TTF_SetFontStyle(FFont, TTF_STYLE_BOLD);
  end;
  FSDLAudioBufferSize := 1024;
  FAudioBufSizeMax := (192000 * 3) div 2;
{
 FVideoQueue:=TEXPlayerQueue.Create(self,false);
 FVideoQueue.FMaxFrames:=50;
 FAudioQueue:=TEXPlayerQueue.Create(self,true);
 FAudioQueue.FMaxFrames:=250;
}
  FMaxVideoFrames := 50;
  FMaxAudioFrames := 250;
  FAudioStreamIndex := -1;
  FVideoStreamIndex := -1;
  FBadFrameTolerance := 20.0;
  SetPixelFormat(SDL_PIXELFORMAT_IYUV, AV_PIX_FMT_YUV420P);

end;


procedure TCustomEXPlayerControl.DestroyWnd;
begin
  Stop;
  DestroyWindowAndRenderer;

  inherited DestroyWnd;
end;

destructor TCustomEXPlayerControl.Destroy;
begin
  Stop;

  if FFont <> nil then
  begin
    TTF_CloseFont(FFont);
  end;
  DestroyWindowAndRenderer;

  if not (csDesigning in ComponentState) then
    DoneSF;
  inherited Destroy;
end;


function TCustomEXPlayerControl.Play: boolean;
var
  rc, numbytes: integer;
  wantedSpec: TSDL_AudioSpec;

begin
  Result := False;
  if FPlaying then
  begin
    FLastErrorString := rsPlayerActive;
    exit;
  end;
  try
    if (not InitSFOk) then
    begin
      FLastErrorString := InitSFErrorString;
      exit;
    end;
    if (FWindow = nil) // or (FRenderer=nil)
    then
      exit;

    fillchar(wantedSpec, sizeof(wantedSpec), 0);

    rc := avformat_open_input(@FFormatCtx, PAnsiChar(FUrl), nil, nil);
    if (rc < 0) then
    begin
      FLastErrorString := format('avformat_open_input: %d', [rc]);
      exit;
    end;

    rc := avformat_find_stream_info(FFormatCtx, nil);
    // Load of stream-infos
    if (rc < 0) then
    begin
      FLastErrorString := format('avformat_find_stream_info: %d', [rc]);
      exit;
    end;
    FVideoStreamIndex := -1;
    if not FDisableVideo then
    begin
      rc := av_find_best_stream(FFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, @FVideoCodec, 0);
      if (rc >= 0) then
        FVideoStreamIndex := rc;
    end;
    FAudioStreamIndex := -1;
    if not FDisableAudio then
    begin
      rc := av_find_best_stream(FFormatCtx, AVMEDIA_TYPE_AUDIO, -1,
        FVideoStreamIndex, @FAudioCodec, 0);
      if (rc >= 0) then
        FAudioStreamIndex := rc;
    end;
    if (FVideoStreamIndex < 0) and (FAudioStreamIndex < 0) then
    begin
      FLastErrorString := rsNoStreamFound;
      exit;
    end;
{$ifdef log}
    wLog(format('VideoStreamIndex:%d AudioStreamIndex:%d',
      [FVideoStreamIndex, FAudioStreamIndex]));
{$endif}
    FVideoWidth := 0;
    FVideoHeight := 0;
    if (FVideoStreamIndex >= 0) then
    begin
      FVideoQueue := TEXPlayerQueue.Create(self, FMaxVideoFrames);

      FVideoTimeBase := av_q2d(PPtrIdx(FFormatCtx^.streams, FVideoStreamIndex)^.time_base);
      FVideoCtx := avcodec_alloc_context3(FVideoCodec);
      if (FVideoCtx = nil) then
      begin
        FLastErrorString := rsVideoCtxNotAlloc;
        exit;
      end;
      rc := avcodec_parameters_to_context(FVideoCtx,
        PPtrIdx(FFormatCtx^.streams, FVideoStreamIndex)^.codecpar);
      if (rc < 0) then
      begin
        FLastErrorString := format('avcodec_parameters_to_context (video): %d', [rc]);
        exit;
      end;
      av_codec_set_pkt_timebase(FVideoCtx, PPtrIdx(
        FFormatCtx^.streams, FVideoStreamIndex)^.time_base);
      rc := avcodec_open2(FVideoCtx, FVideoCodec, nil);
      if (rc < 0) then
      begin
        FLastErrorString := format('avcodec_open2 (video): %d', [rc]);
        exit;
      end;

      FVideoSwsCtx := sws_getContext(FVideoCtx^.Width,
        FVideoCtx^.Height, FVideoCtx^.pix_fmt,
        FVideoCtx^.Width, FVideoCtx^.Height, FAVpixelformat,
        //            SWS_BICUBIC,
        SWS_BILINEAR, nil, nil, nil);
      if (FVideoSwsCtx = nil) then
      begin
        FLastErrorString := rsVideoSwsCtxNotAlloc;
        exit;
      end;

      FVideoWidth := PPtrIdx(FFormatCtx^.streams, FVideoStreamIndex)^.codecpar^.Width;
      // Videowidth

      FVideoHeight := PPtrIdx(FFormatCtx^.streams, FVideoStreamIndex)^.codecpar^.Height;
      // Videoheight

      numbytes := av_image_get_buffer_size(FAVPixelFormat, FVideoWidth, FVideoHeight, 8);
      if (numbytes < 1) then
      begin
        FLastErrorString := format('av_image_get_buffer_size (video): %d', [numbytes]);
        exit;
      end;
      FVideoDecodeFrame := av_frame_alloc();
      if (FVideoDecodeFrame = nil) then
      begin
        FLastErrorString := rsVideoDecodeFrameNotAlloc;
        exit;
      end;

      FVideoDecodeFrameBuffer := av_malloc(numBytes);
      if FVideoDecodeFrameBuffer = nil then
      begin
        FLastErrorString := rsVideoFrameBufferNotAlloc;
        exit;
      end;

      rc := av_image_fill_arrays(@FVideoDecodeFrame^.Data[0],
        @FVideoDecodeFrame^.linesize[0],
        FVideoDecodeFrameBuffer, FAVPixelFormat,
        FVideoWidth, FVideoHeight, 1);
      if (rc < 0) then
      begin
        FLastErrorString := format('av_image_fill_arrays: %d', [rc]);
        exit;
      end;

      CreateWindowAndRenderer;  // if there isnt done
      if FRenderer = nil then
        exit;

      FTexture := SDL_CreateTexture(FRenderer,
        FSDLPixelFormat, SDL_TEXTUREACCESS_STREAMING,
        FVideoWidth, FVideoHeight);

      if FTexture = nil then
      begin
        FLastErrorString := format('SDL_CreateTexture: %s', [string(SDL_GetError)]);
        exit;
      end;

      SDL_RenderClear(FRenderer);
      SDL_RenderPresent(FRenderer);
    end;

    // Audio
    if (FAudioStreamIndex >= 0) then
    begin
      FAudioQueue := TEXPlayerQueue.Create(self, FmaxAudioFrames);
      //      FAudioQueue.FType:=1;
      FAudioTimeBase := av_q2d(PPtrIdx(FFormatCtx^.streams, FAudioStreamIndex)^.time_base);
      FAudioCtx := avcodec_alloc_context3(FAudioCodec);
      if (FAudioCtx = nil) then
      begin
        FLastErrorString := rsAudioCtxNotAlloc;
        exit;
      end;
      rc := avcodec_parameters_to_context(FAudioCtx,
        PPtrIdx(FFormatCtx^.streams, FAudioStreamIndex)^.codecpar);
      if (rc < 0) then
      begin
        FLastErrorString := format('avcodec_parameters_to_context (audio): %d', [rc]);
        exit;
      end;

      av_codec_set_pkt_timebase(FAudioCtx, PPtrIdx(
        FFormatCtx^.streams, FAudioStreamIndex)^.time_base);
      rc := avcodec_open2(FAudioCtx, FAudioCodec, nil);
      if (rc <> 0) then
      begin
        FLastErrorString := format('avcodec_open2 (audio): %d', [rc]);
        exit;
      end;
      FAudioSwrCtx := swr_alloc();
      if (FAudioSwrCtx = nil) then
      begin
        FLastErrorString := rsAudioSwrCtxNotAlloc;
        exit;
      end;
      if (FAudioCtx^.channel_layout <> 0) then
      begin
        av_opt_set_channel_layout(FAudioSwrCtx, 'in_channel_layout',
          FAudioCtx^.channel_layout, 0);
        av_opt_set_channel_layout(FAudioSwrCtx, 'out_channel_layout',
          FAudioCtx^.channel_layout, 0);
      end
      else
      if (FAudioCtx^.channels = 2) then
      begin
        av_opt_set_channel_layout(FAudioSwrCtx,
          'in_channel_layout', AV_CH_LAYOUT_STEREO, 0);
        av_opt_set_channel_layout(FAudioSwrCtx, 'out_channel_layout',
          AV_CH_LAYOUT_STEREO, 0);
      end
      else
      if (FAudioCtx^.channels = 1) then
      begin
        av_opt_set_channel_layout(FAudioSwrCtx,
          'in_channel_layout', AV_CH_LAYOUT_MONO, 0);
        av_opt_set_channel_layout(FAudioSwrCtx, 'out_channel_layout',
          AV_CH_LAYOUT_MONO, 0);
      end
      else
      begin
        av_opt_set_channel_layout(FAudioSwrCtx,
          'in_channel_layout', AV_CH_LAYOUT_NATIVE, 0);
        av_opt_set_channel_layout(FAudioSwrCtx, 'out_channel_layout',
          AV_CH_LAYOUT_NATIVE, 0);
      end;

      av_opt_set_int(FAudioSwrCtx, 'in_sample_rate', FAudioCtx^.sample_rate, 0);
      av_opt_set_int(FAudioSwrCtx, 'out_sample_rate', FAudioCtx^.sample_rate, 0);
      av_opt_set_sample_fmt(FAudioSwrCtx, 'in_sample_fmt', FAudioCtx^.sample_fmt, 0);
      av_opt_set_sample_fmt(FAudioSwrCtx, 'out_sample_fmt', AV_SAMPLE_FMT_FLT, 0);
      rc := swr_init(FAudioSwrCtx);
      if (rc < 0) then
      begin
        FLastErrorString := format('swr_init: %d', [rc]);
        exit;
      end;
      ReAllocMem(FAudioBuffer, FAudioBufSizeMax);

      wantedSpec.channels := FAudioCtx^.channels;
      wantedSpec.freq := FAudioCtx^.sample_rate;
      wantedSpec.format := AUDIO_F32;
      wantedSpec.silence := 0;
      wantedSpec.samples := FSDLAudioBufferSize;
      wantedSpec.userdata := self;
      wantedSpec.callback := @AudioThread;

      // the second params nil !
      if (SDL_OpenAudio(@wantedSpec, nil) < 0) then
      begin
        FLastErrorString := format('SDL_OpenAudio: %s', [string(SDL_GetError())]);
        exit;
      end;
    end;
    FEof := False;
    FAudioBufSize := 0;
    FAudioBufIndex := 0;
    //   FImagePts:=-1;
    FAudioPts := -1;
    FVideoPts := -1;
    FVideoSynchPts := -1;
    FVideoSynchTime := -1;           // SynchRealtime

    FReadThread := TReadThread.Create(self);
    if FAudioStreamIndex >= 0 then
    begin
      SDL_PauseAudio(0);
    end;
    StartTimer;

    FPlaying := True;
    FPausing := False;
    FLastTimeString := '';
    Result := True;
  finally
    if not Result then
      Stop;
  end;
end;

procedure TCustomEXPlayerControl.Pause;
begin
  if not FPlaying then
    exit;
  FPausing := True;
  StopTimer;
  if FAudioStreamIndex >= 0 then
  begin
    SDL_PauseAudio(1);
  end;

end;

procedure TCustomEXPlayerControl.Resume;
begin
  if (not FPlaying) or (not FPausing) then
    exit;
  // Set SynchTime new
  FVideoSynchPts := -1;
  FVideoSynchTime := -1;

  FPausing := False;
  StartTimer;
  if FAudioStreamIndex >= 0 then
  begin
    SDL_PauseAudio(0);
  end;

end;

procedure TCustomEXPlayerControl.Stop;

begin
  if FPlaying then
  begin
    StopTimer;
    FPlaying := False;
  end;
  if FVideoQueue <> nil then
    with FVideoQueue do
    begin
      WakeUpGet;
      WakeUpPut;
    end;
  if FAudioQueue <> nil then
    with FAudioQueue do
    begin
      WakeUpGet;
      WakeUpPut;
    end;

  if FAudioCodec <> nil then
  begin
    SDL_CloseAudio;
  end;
  FreeAndNil(FReadThread);
  FreeAndNil(FVideoQueue);
  FreeAndNil(FAudioQueue);

  if (FTexture <> nil) then
  begin
    SDL_DestroyTexture(FTexture);
    FTexture := nil;
  end;
  FTextureValid := False;
  ReAllocMem(FAudioBuffer, 0);
  if (FAudioSwrCtx <> nil) then
  begin
    swr_free(@FAudioSwrCtx);
  end;
  if (FAudioCtx <> nil) then
  begin
    avcodec_free_context(@FAudioCtx);
  end;

  if (FVideoDecodeFrameBuffer <> nil) then
  begin
    av_free(FVideoDecodeFrameBuffer);
    FVideoDecodeFrameBuffer := nil;
  end;

  if (FVideoDecodeFrame <> nil) then
  begin
    av_frame_free(@FVideoDecodeFrame);
  end;
  if (FVideoSwsCtx <> nil) then
  begin
    sws_freeContext(FVideoSwsCtx);
    FVideoSwsCtx := nil;
  end;

  if (FVideoCtx <> nil) then
  begin
    avcodec_free_context(@FVideoCtx);
  end;

  FAudioStreamIndex := -1;
  FAudioTimeBase := 0;
  FVideoTimeBase := 0;

  FVideoStreamIndex := -1;
  FAudioCodec := nil;
  FVideoCodec := nil;
  if (FFormatCtx <> nil) then
  begin
    avformat_close_input(@FFormatCtx);
  end;
  FVideoWidth := 0;
  FVideoHeight := 0;
  FVideoSar.den := 0;
  FVideoSar.num := 0;
  DoUpdateProgress;

  Invalidate;
end;

procedure TCustomEXPlayerControl.Seek(ms: int64);
var
  rc, si, flags: integer;
  ts: int64;
begin
  if not FPlaying then
    exit;
  if (ms < 0) or (Duration <= 0) or (ms > Duration) then
    exit;
  if FSeeking then
    exit;
  FSeeking := True;
  try
    StopTimer;
    // we stop the AudioThread if it is not in pause-mode
    if (FAudioStreamIndex >= 0) and not FPausing then
    begin
      SDL_PauseAudio(1);
    end;
    FReadThreadSleeping := False;
    FReadThreadSleep := True;

    FAudioQueue.FUnblockPut := True;
    FAudioQueue.WakeUpPut;
    FVideoQueue.FUnblockPut := True;
    FVideoQueue.WakeUpPut;

    // wait for sleeping readthread
    while not FReadThreadSleeping do
      sleep(10);

    FAudioQueue.FUnblockPut := False;
    FVideoQueue.FUnblockPut := False;

    si := -1;
    ts := ms * int64(1000);
    if FFormatCtx^.start_time <> AV_NOPTS_VALUE then
      ts := ts + FFormatCtx^.start_time;
  {
   if FAudioStreamIndex>=0 then begin
      si:=FAudioStreamIndex;
      ts:=round(ms /(1000*FAudioTimeBase));
   end else
   if FVideoStreamIndex>=0 then begin
      si:=FVideoStreamIndex;
      ts:=round(ms /(1000* FVideoTimeBase));
   end;
  }
  {
    logcount:=0;
    enablelog:=true;
    wLog('Seek >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>');
  }
    //   avformat_flush(FFormatCtx);
    flags := AVSEEK_FLAG_ANY;
    //   flags:=0;
    rc := avformat_seek_file(FFormatCtx, si, int64.MinValue, ts, int64.MaxValue, flags);
    //   rc:=av_seek_frame(FFormatCtx,si,ts,flags);
    if rc >= 0 then
    begin
      // we clear all queue and buffers
      FAudioQueue.Clear;
      FVideoQueue.Clear;
      FAudioPts := -1;
      FVideoPts := -1;
      FVideoSynchPts := -1;
      FVideoSynchTime := -1;
      FAudioBufSize := 0;
      FAudioBufIndex := 0;
      avformat_flush(FFormatCtx);
      if FVideoCtx <> nil then
        avcodec_flush_buffers(FVideoCtx);
      if FAudioCtx <> nil then
        avcodec_flush_buffers(FAudioCtx);
      //      avformat_flush(FFormatCtx);
    end;

    FEof := False;
    FDelayAudioQueueing := True;  // audioqueing after the first video-frame
    FReadThreadSleep := False;    // ReadThread can work again
    if (FVideoStreamIndex >= 0) and Pausing then
      FVideoShowNextFrame := True;  // we show in pausing the next frame
    StartTimer;
    if (FAudioStreamIndex >= 0) then
    begin
      SDL_PauseAudio(0);
    end;
    Invalidate;
  finally
    FSeeking := False;
  end;
end;



end.
