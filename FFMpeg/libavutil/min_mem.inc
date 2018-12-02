
(**
 * Allocate a memory block with alignment suitable for all memory accesses
 * (including vectors if available on the CPU).
 *
 * @param size Size in bytes for the memory block to be allocated
 * @return Pointer to the allocated block, or `NULL` if the block cannot
 *         be allocated
 * @see av_mallocz()
 *)
//void *av_malloc(size_t size) av_malloc_attrib av_alloc_size(1);
function av_malloc(size: Cardinal): Pointer; cdecl;
     external LIBAVUTIL_LIBNAME name 'av_malloc';


(**
 * Allocate a memory block with alignment suitable for all memory accesses
 * (including vectors if available on the CPU) and zero all the bytes of the
 * block.
 *
 * @param size Size in bytes for the memory block to be allocated
 * @return Pointer to the allocated block, or `NULL` if it cannot be allocated
 * @see av_malloc()
 *)
// void *av_mallocz(size_t size) av_malloc_attrib av_alloc_size(1);
function av_mallocz(size: Cardinal): Pointer; cdecl;
        external LIBAVUTIL_LIBNAME name 'av_mallocz';

(**
 * Duplicate a string.
 *
 * @param s String to be duplicated
 * @return Pointer to a newly-allocated string containing a
 *         copy of `s` or `NULL` if the string cannot be allocated
 * @see av_strndup()
 *)
//char *av_strdup(const char *s) av_malloc_attrib;
function av_strdup(const s: PAnsiChar): PAnsiChar; cdecl;
      external LIBAVUTIL_LIBNAME name 'av_strdup';


(**
 * Free a memory block which has been allocated with a function of av_malloc()
 * or av_realloc() family, and set the pointer pointing to it to `NULL`.
 *
 * @code{.c}
 * uint8_t *buf = av_malloc(16);
 * av_free(buf);
 * // buf now contains a dangling pointer to freed memory, and accidental
 * // dereference of buf will result in a use-after-free, which may be a
 * // security risk.
 *
 * uint8_t *buf = av_malloc(16);
 * av_freep(&buf);
 * // buf is now NULL, and accidental dereference will only result in a
 * // NULL-pointer dereference.
 * @endcode
 *
 * @param ptr Pointer to the pointer to the memory block which should be freed
 * @note `*ptr = NULL` is safe and leads to no action.
 * @see av_free()
 *)
procedure av_freep(ptr: Pointer); cdecl;
      external LIBAVUTIL_LIBNAME name 'av_freep';

(**
 * Free a memory block which has been allocated with a function of av_malloc()
 * or av_realloc() family.
 *
 * @param ptr Pointer to the memory block which should be freed.
 *
 * @note `ptr = NULL` is explicitly allowed.
 * @note It is recommended that you use av_freep() instead, to prevent leaving
 *       behind dangling pointers.
 * @see av_freep()
 *)
procedure av_free(ptr: Pointer); cdecl;
      external LIBAVUTIL_LIBNAME name 'av_free';


function av_mallocz_array(nmemb:Cardinal; size:Cardinal):Pointer; inline;




