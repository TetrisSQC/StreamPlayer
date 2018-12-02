
const
  ff_log2_tab: array[0..255] of Byte = (  // libavutil/intmath.h
        0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
  );

(**
 * Clip a signed integer value into the amin-amax range.
 * @param a value to clip
 * @param amin minimum value of the clip range
 * @param amax maximum value of the clip range
 * @return clipped value
 *)

function av_clip(a, amin, amax: Integer): Integer;
begin
//#if defined(HAVE_AV_CONFIG_H) && defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
//  if (amin > amax) abort();
//#endif
  if a < amin then
    Result := amin
  else if a > amax then
    Result := amax
  else
    Result := a;
end;

function av_log2(v: Cardinal):integer;  // libavutil/intmath.h
 begin
    result:=0;
    if (v and $ffff0000)<>0 then begin
        v:=v shr 16;
        inc(Result,16);
    end;
    if (v and $ff00)<>0 then begin
        v :=v shr 8;
        inc(Result,8);
    end;
//    n += ff_log2_tab[v];
    inc(result,ff_log2_tab[v]);
 end;



