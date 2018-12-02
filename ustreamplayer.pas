unit ustreamplayer;

{
Written by R.Sombrowsky info@somby.de
}

{$mode delphi}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ExtCtrls, Buttons, ActnList, LazUtf8,
  IniFiles,
  sdl2,
  uexplayer;

type

  TStreamPlayerForm = class;

  { TEXStreamPlayer }

  TEXStreamPlayer = class(TEXPlayerControl)
  private
    FParent: TStreamPlayerForm;
    FError, FSelecting: boolean;
    procedure SetError(AValue: boolean);
    procedure SetSelecting(AValue: boolean);

  protected
    procedure ShowItem(renderer: PSDL_Renderer; x, y, w, h, idx: integer;
      selected,                  // selected
      current: boolean); virtual;  // current
    procedure ShowItemName(error: boolean); virtual;

    function GetRendererFlags: longword; override;

  public
    procedure PaintOverlays(renderer: PSDL_Renderer; var present: boolean); override;
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    property Selecting: boolean read FSelecting write SetSelecting;
    property Error: boolean read FError write SetError;
  end;


  TStreamPlayerItem = class(TCollectionItem)
  public
    FName, FUrl: string;
  end;

  { TStreamPlayerForm }

  TStreamPlayerForm = class(TForm)
    aClose: TAction;
    aShowMax: TAction;
    aShowWin: TAction;
    aUp: TAction;
    aDown: TAction;
    aRight: TAction;
    aLeft: TAction;
    aSelect: TAction;
    aMute: TAction;
    aPauseResume: TAction;
    ActionList1: TActionList;
    tmrLoad: TTimer;
    procedure aCloseExecute(Sender: TObject);
    procedure aDownExecute(Sender: TObject);
    procedure aLeftExecute(Sender: TObject);
    procedure aShowMaxExecute(Sender: TObject);
    procedure aShowWinExecute(Sender: TObject);
    procedure aMuteExecute(Sender: TObject);
    procedure aPauseResumeExecute(Sender: TObject);
    procedure aRightExecute(Sender: TObject);
    procedure aSelectExecute(Sender: TObject);
    procedure aUpExecute(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure tmrLoadTimer(Sender: TObject);
  private
    FPlayer: TEXStreamPlayer;
    FSelCols, FSelRows: integer;
    FWinFormat: boolean;
    FStartIndex, FSelIndex,                // selectindex
    FItemIndex: integer;       // current shown index
    FItems: TCollection;

    procedure StartPlay;
  public
    procedure LoadItems;
    procedure AdjustStartIndex;
    procedure LoadParams;   // You can load some params at the begin
    procedure SaveParams;   // You can save some params at the end
    procedure SetWinFormat(win: boolean);
  end;

var
  StreamPlayerForm: TStreamPlayerForm;

implementation

{$R *.lfm}

resourcestring
{
 rsLiveStreamNotAvailable='Livestream nicht verfügbar';
 rsNoItems='Keine Einträge verfügbar';
}

  rsLiveStreamNotAvailable = 'Livestream not available';
  rsNoItems = 'No entries available';


{ TEXStreamPlayer }

procedure TEXStreamPlayer.SetSelecting(AValue: boolean);
begin
  if FSelecting = AValue then
    Exit;
  FSelecting := AValue;
  Invalidate;
end;

procedure TEXStreamPlayer.SetError(AValue: boolean);
begin
  if FError = AValue then
    Exit;
  FError := AValue;
  invalidate;
end;

procedure TEXStreamPlayer.ShowItem(renderer: PSDL_Renderer; x, y, w, h, idx: integer;
  selected, current: boolean);
var
  r, rt: TSDL_Rect;
  it: TStreamPlayerItem;
  tx: PSDL_Texture;
  co: TSDL_Color;
  ww, hh: integer;
begin
  r.x := x;
  r.y := y;
  r.w := w;
  r.h := h;
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  if selected then
  begin
    SDL_SetRenderDrawColor(renderer, 200, 200, 255, 200);
  end
  else
  if current then
  begin
    SDL_SetRenderDrawColor(renderer, 255, 200, 200, 200);
  end
  else
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);

  SDL_RenderFillRect(renderer, @r);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  SDL_RenderDrawLine(renderer, x, y, x + w - 1, y);
  SDL_RenderDrawLine(renderer, x, y + h - 1, x + w - 1, y + h - 1);

  SDL_RenderDrawLine(renderer, x, y, x, y + h - 1);
  SDL_RenderDrawLine(renderer, x + w - 1, y, x + w - 1, y + h - 1);



  if (idx >= 0) and (idx < FParent.FItems.Count) then
  begin
    it := TStreamPlayerItem(FParent.FItems.Items[idx]);
    if current then
      with co do
      begin
        a := 255;
        r := 255;
        g := 0;
        b := 0;
      end
    else
      with co do
      begin
        a := 255;
        r := 0;
        g := 0;
        b := 0;
      end;

    tx := LoadText(it.FName, co, ww, hh);

    rt.h := h div 2;
    rt.w := round(ww * rt.h / hh);
    rt.x := x + ((w - rt.w) div 2);
    rt.y := y + ((h - rt.h) div 2);

    SDL_RenderCopy(Renderer, tx, nil, @rt);
    SDL_DestroyTexture(tx);

  end;

end;

procedure TEXStreamPlayer.ShowItemName(error: boolean);
var
  co: TSDL_Color;
  tx: PSDL_Texture;
  rt: TSDL_Rect;
  it: TStreamPlayerItem;
  ww, hh: integer;
  s: string;
begin
  if error then
    with co do
    begin
      a := 255;
      r := 255;
      g := 0;
      b := 0;
    end
  else
    with co do
    begin
      a := 255;
      r := 0;
      g := 255;
      b := 0;
    end;
  it := nil;
  if (FParent.FItemIndex >= 0) and (FParent.FItemIndex < FParent.FItems.Count) then
  begin
    it := TStreamPlayerItem(FParent.FItems.Items[FParent.FItemIndex]);
    tx := LoadText(it.FName, co, ww, hh);
    rt.h := round(Height * 10 / 100);   // 8% of height
    rt.w := round(ww * rt.h / hh);
    rt.x := (Width - rt.w) div 2;
    rt.y := (Height div 2) - int64(rt.h) - round(Height * 0.5 / 100);
    SDL_RenderCopy(Renderer, tx, nil, @rt);
    SDL_DestroyTexture(tx);
  end;

  if error then
  begin
    if it = nil then
      s := rsNoItems
    else
      s := rsLiveStreamNotAvailable;
    tx := LoadText(s, co, ww, hh);
    rt.h := round(Height * 6 / 100);
    rt.w := round(ww * rt.h / hh);
    rt.x := (Width - rt.w) div 2;
    rt.y := (Height div 2) + round(Height * 0.5 / 100);
    SDL_RenderCopy(Renderer, tx, nil, @rt);
    SDL_DestroyTexture(tx);
  end;
end;

function TEXStreamPlayer.GetRendererFlags: longword;
begin
  Result := inherited GetRendererFlags;
  // result:=SDL_RENDERER_ACCELERATED;

end;

procedure TEXStreamPlayer.PaintOverlays(renderer: PSDL_Renderer; var present: boolean);
var
  rt: TSDL_Rect;
  row, col, idx, x, y, iw, ih, ww, hh: integer;
  tx: PSDL_Texture;
  co: TSDL_Color;

begin
  if FError or (not Playing) or Pausing then
  begin
    PaintImage;
    present := True;
  end;
  if FError then
  begin
    ShowItemName(True);
  end
  else
  if not VideoAvailable and AudioAvailable then
  begin
    ShowItemName(False);
  end;
  if FSelecting then
  begin
    iw := ((5 * Width) div 6) div FParent.FSelCols;  // Itemwidth
    ih := ((5 * Height) div 6) div FParent.FSelRows; // Itemheight
    x := (Width - FParent.FSelCols * iw) div 2;
    y := (Height - FParent.FSelRows * ih) div 2;
    idx := FParent.FStartIndex;
    for col := 0 to FParent.FSelCols - 1 do
    begin
      for row := 0 to FParent.FSelRows - 1 do
      begin
        ShowItem(renderer, x + col * iw, y + row * ih, iw, ih, idx, idx =
          FParent.FSelIndex, idx = FParent.FItemIndex);
        Inc(idx);
      end;
    end;
    with co do
    begin
      a := 255;
      r := 0;
      g := 0;
      b := 255;
    end;
    if FParent.FStartIndex > 0 then
    begin
      // set arrow left
      tx := LoadText('<', co, ww, hh);
      rt.h := ih div 2;
      rt.w := round(ww * rt.h / hh);
      rt.x := x - rt.w;
      rt.y := y + FParent.FSelRows * ih - rt.h - 1;
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
      SDL_RenderFillRect(renderer, @rt);
      SDL_RenderCopy(Renderer, tx, nil, @rt);
      SDL_DestroyTexture(tx);
    end;
    if FParent.FStartIndex < (((FParent.FItems.Count - 1) div
      FParent.FSelRows) // last column
      - FParent.FSelCols + 1) * FParent.FSelRows then
    begin
      // set arrow right
      tx := LoadText('>', co, ww, hh);
      rt.h := ih div 2;
      rt.w := round(ww * rt.h / hh);
      rt.x := x + FParent.FSelCols * iw;
      rt.y := y + FParent.FSelRows * ih - rt.h - 1;
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
      SDL_RenderFillRect(renderer, @rt);
      SDL_RenderCopy(Renderer, tx, nil, @rt);
      SDL_DestroyTexture(tx);
    end;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  end;

end;

constructor TEXStreamPlayer.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
end;

destructor TEXStreamPlayer.Destroy;
begin
  inherited Destroy;
end;

{ TStreamPlayerForm }

procedure TStreamPlayerForm.FormCreate(Sender: TObject);
begin
  Color := clBlack;
  FWinFormat := True;
  FSelCols := 2;
  FSelRows := 10;

  FItemIndex := -1;
  FItems := TCollection.Create(TStreamPlayerItem);
  LoadItems;
  LoadParams;
  FPlayer := TEXStreamPlayer.Create(self);
  with FPlayer do
  begin
    Cursor := crNone;
    FParent := self;
    Align := alClient;
    Parent := self;
  end;
  SetWinFormat(FWinFormat);

end;

procedure TStreamPlayerForm.FormDestroy(Sender: TObject);
begin
  SaveParams;
  FItems.Free;
end;

procedure TStreamPlayerForm.FormShow(Sender: TObject);
var
  it: TStreamPlayerItem;
begin
  if FItems.Count > 0 then
  begin
    it := TStreamPlayerItem(FItems.Items[FItemIndex]);
    FPlayer.Url := it.FUrl;
  end;
  tmrLoad.enabled := true;
end;

procedure TStreamPlayerForm.tmrLoadTimer(Sender: TObject);
begin
 tmrLoad.enabled := false;
 StartPlay;
end;

procedure TStreamPlayerForm.StartPlay;
begin
  FPlayer.Repaint; // first black
  if not FPlayer.Play then
  begin
    FPlayer.Error := True;
  end
  else
    FPlayer.Error := False;
end;


procedure TStreamPlayerForm.LoadItems;
var
  ini: TIniFile;
  s: string;
  sl: TStringList;
  i: integer;
  it: TStreamPlayerItem;
begin
  FItems.Clear;
  sl := nil;
  ini := nil;
  try
    sl := TStringList.Create;
    ini := TIniFile.Create(ChangeFileExt(ParamStr(0), '.ini'));
    ini.ReadSections(sl);
    for i := 0 to sl.Count - 1 do
    begin
      s := ini.ReadString(sl[i], 'url', '');
      if s <> '' then
      begin
        it := TStreamPlayerItem(FItems.Add);
        it.FName := WinCPToUTF8(sl[i]);
        it.FUrl := WinCPToUTF8(s);
      end;
    end;

  finally
    ini.Free;
    sl.Free;
  end;

end;

procedure TStreamPlayerForm.AdjustStartIndex;
var
  l: integer;
begin
  if FSelIndex < FStartIndex then
  begin
    FStartIndex := (FSelIndex div FSelRows) * FSelRows;
  end
  else
  if FSelIndex >= FStartIndex + FSelCols * FSelRows then
  begin
    FStartIndex := (FSelIndex div FSelRows) * FSelRows - (FSelCols - 1) * FSelRows;
    l := (((FItems.Count - 1) div FSelRows) // last column
      - FSelCols + 1) * FSelRows;
    if l < 0 then
      l := 0;
    if FStartIndex > l then
      FStartIndex := l;
  end;
end;

procedure TStreamPlayerForm.LoadParams;
begin
  if FItems.Count > 0 then
    FItemIndex := 0;
end;

procedure TStreamPlayerForm.SaveParams;
begin
end;

procedure TStreamPlayerForm.SetWinFormat(win: boolean);
var
  l, t, w, h: integer;
begin
  if win then
  begin
    BorderStyle := bsSizeable;
    WindowState := wsNormal;
    w := 800;
    h := 450;
    l := (Screen.Width - w) div 2;
    t := (Screen.Height - h) div 2;
    SetBounds(l, t, w, h);
    FPlayer.Cursor := crDefault;
  end
  else
  begin
    BorderStyle := bsNone;
    WindowState := wsMaximized;
    FPlayer.Cursor := crNone;
  end;

end;

procedure TStreamPlayerForm.aCloseExecute(Sender: TObject);
begin
  if FPlayer.Selecting then
  begin
    FPlayer.Selecting := False;
  end
  else
  begin
    FPlayer.Stop;
    Close;
  end;
end;

procedure TStreamPlayerForm.aDownExecute(Sender: TObject);
begin
  if not FPlayer.Selecting or (FItems.Count = 0) or (FSelIndex < 0) or
    (FSelIndex >= FItems.Count - 1) then
    exit;
  Inc(FSelIndex);
  if FSelIndex >= FItems.Count then
    FSelIndex := FItems.Count - 1;
  AdjustStartIndex;
  FPlayer.invalidate;
end;

procedure TStreamPlayerForm.aLeftExecute(Sender: TObject);
begin
  if not FPlayer.Selecting or (FItems.Count = 0) or (FSelIndex < 0) then
    exit;
  Dec(FSelIndex, FSelRows);
  if FSelIndex < 0 then
    FSelIndex := 0;
  AdjustStartIndex;
  FPlayer.invalidate;
end;

procedure TStreamPlayerForm.aShowMaxExecute(Sender: TObject);
begin
  if not FWinFormat then
    exit;
  FWinFormat := False;
  FPlayer.Stop;
  SetWinFormat(FWinFormat);

end;

procedure TStreamPlayerForm.aShowWinExecute(Sender: TObject);
begin
  if FWinFormat then
    exit;
  FWinFormat := True;
  FPlayer.Stop;
  SetWinFormat(FWinFormat);
end;

procedure TStreamPlayerForm.aMuteExecute(Sender: TObject);
begin
  FPlayer.Mute := not FPlayer.Mute;
end;

procedure TStreamPlayerForm.aPauseResumeExecute(Sender: TObject);
begin
  if FPlayer.Pausing then
    FPlayer.Resume
  else
    FPlayer.Pause;
end;

procedure TStreamPlayerForm.aRightExecute(Sender: TObject);
begin
  if (not FPlayer.Selecting) or (FItems.Count = 0) or (FSelIndex < 0) or
    (FSelIndex >= FItems.Count - 1) then
    exit;
  Inc(FSelIndex, FSelRows);
  if FSelIndex >= FItems.Count then
    FSelIndex := FItems.Count - 1;
  AdjustStartIndex;
  FPlayer.Invalidate;
end;

procedure TStreamPlayerForm.aSelectExecute(Sender: TObject);
begin
  if FPlayer.Selecting then
  begin
    if FItemIndex <> FSelIndex then
    begin
      FPlayer.Stop;
      FItemIndex := FSelIndex;
      FPlayer.Url := TStreamPlayerItem(FItems.Items[FItemIndex]).FUrl;
      if not FPlayer.Play then
      begin
        FPlayer.Error := True;
      end
      else
        FPlayer.Error := False;
    end;
    FPlayer.Selecting := False;
  end
  else
  if FItems.Count > 0 then
  begin
    FSelIndex := FItemIndex;
    AdjustStartIndex;
    FPlayer.Selecting := True;
  end;
end;

procedure TStreamPlayerForm.aUpExecute(Sender: TObject);
begin
  if not FPlayer.Selecting or (FItems.Count = 0) or (FSelIndex < 0) then
    exit;
  Dec(FSelIndex);
  if FSelIndex < 0 then
    FSelIndex := 0;
  AdjustStartIndex;
  FPlayer.Invalidate;
end;

end.
