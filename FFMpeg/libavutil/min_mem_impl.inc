function av_mallocz_array(nmemb:Cardinal; size:Cardinal):Pointer; inline;
begin
 if (size = 0) or (nmemb >= (MaxInt div size)) then
    Result := nil
  else
    Result := av_mallocz(nmemb * size);
end;

