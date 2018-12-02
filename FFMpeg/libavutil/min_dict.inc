
const
 AV_DICT_MATCH_CASE      =1;   //**< Only get an entry with exact-case key match. Only relevant in av_dict_get(). */
 AV_DICT_IGNORE_SUFFIX   =2;   //**< Return first entry in a dictionary whose first part corresponds to the search key,
                               //           ignoring the suffix of the found key string. Only relevant in av_dict_get(). */
 AV_DICT_DONT_STRDUP_KEY =4;   //**< Take ownership of a key that's been
                               //          allocated with av_malloc() or another memory allocation function. */
 AV_DICT_DONT_STRDUP_VAL =8;   //**< Take ownership of a value that's been
                               //          allocated with av_malloc() or another memory allocation function. */
 AV_DICT_DONT_OVERWRITE =16;   ///< Don't overwrite existing entries.
 AV_DICT_APPEND         =32;   //**< If the entry already exists, append to it.  Note that no
                               //       delimiter is added, the strings are simply concatenated. */
 AV_DICT_MULTIKEY       =64;   //**< Allow to store several equal keys in the dictionary */

type
 PAVDictionaryEntry=^TAVDictionaryEntry;
 TAVDictionaryEntry =record
    key: PAnsiChar;
    value: PAnsiChar;
 end; //AVDictionaryEntry;


 PPAVDictionary=^PAVDictionary;
 PAVDictionary=Pointer;


(**
 * Get a dictionary entry with matching key.
 *
 * The returned entry key or value must not be changed, or it will
 * cause undefined behavior.
 *
 * To iterate through all the dictionary entries, you can set the matching key
 * to the null string "" and set the AV_DICT_IGNORE_SUFFIX flag.
 *
 * @param prev Set to the previous matching element to find the next.
 *             If set to NULL the first matching element is returned.
 * @param key matching key
 * @param flags a collection of AV_DICT_* flags controlling how the entry is retrieved
 * @return found entry or NULL in case no matching entry was found in the dictionary
 *)
function av_dict_get(const m: PAVDictionary; const key: PAnsiChar;
         const prev: PAVDictionaryEntry; flags:integer):PAVDictionaryEntry; cdecl;
      external LIBAVUTIL_LIBNAME name 'av_dict_get';


(**
 * Set the given entry in *pm, overwriting an existing entry.
 *
 * Note: If AV_DICT_DONT_STRDUP_KEY or AV_DICT_DONT_STRDUP_VAL is set,
 * these arguments will be freed on error.
 *
 * Warning: Adding a new entry to a dictionary invalidates all existing entries
 * previously returned with av_dict_get.
 *
 * @param pm pointer to a pointer to a dictionary struct. If *pm is NULL
 * a dictionary struct is allocated and put in *pm.
 * @param key entry key to add to *pm (will either be av_strduped or added as a new key depending on flags)
 * @param value entry value to add to *pm (will be av_strduped or added as a new key depending on flags).
 *        Passing a NULL value will cause an existing entry to be deleted.
 * @return >= 0 on success otherwise an error code <0
 *)

function av_dict_set(pm: PPAVDictionary; const key, value: PAnsiChar; flags: Integer): Integer; cdecl;
      external LIBAVUTIL_LIBNAME name 'av_dict_set';


(**
 * Free all the memory allocated for an AVDictionary struct
 * and all keys and values.
 *)
procedure av_dict_free(m: PPAVDictionary);cdecl;
      external LIBAVUTIL_LIBNAME name 'av_dict_free';


(**
 * Convenience wrapper for av_dict_set that converts the value to a string
 * and stores it.
 *
 * Note: If AV_DICT_DONT_STRDUP_KEY is set, key will be freed on error.
 *)
function av_dict_set_int(pm: PPAVDictionary; const key: PAnsiChar; value: Int64; flags: Integer): Integer; cdecl;
      external LIBAVUTIL_LIBNAME name 'av_dict_set_int';


