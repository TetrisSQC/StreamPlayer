type
(**
 * Rational number (pair of numerator and denominator).
 *)
  PAVRational = ^TAVRational;
  TAVRational = record
    num: Integer; ///< numerator
    den: Integer; ///< denominator
  end;

(**
 * Convert an AVRational to a `double`.
 * @param a AVRational to convert
 * @return `a` in floating-point form
 * @see av_d2q()
 *)
// static inline double
function av_q2d(a: TAVRational):double; inline;
{
    return a.num / (double) a.den;
}

