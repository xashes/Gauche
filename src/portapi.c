/*
 * portapi.c - port common API
 *
 *  Copyright(C) 2002 by Shiro Kawai (shiro@acm.org)
 *
 *  Permission to use, copy, modify, distribute this software and
 *  accompanying documentation for any purpose is hereby granted,
 *  provided that existing copyright notices are retained in all
 *  copies and that this notice is included verbatim in all
 *  distributions.
 *  This software is provided as is, without express or implied
 *  warranty.  In no circumstances the author(s) shall be liable
 *  for any damages arising out of the use of this software.
 *
 *  $Id: portapi.c,v 1.1 2002-07-05 21:10:49 uid50821 Exp $
 */

/* This file is included twice by port.c to define safe- and unsafe-
 * variant of port common APIs.
 */

#ifdef SAFE_PORT_OP
#define VMDECL        ScmVM *vm = Scm_VM()
#define LOCK(p)       PORT_LOCK(p, vm)
#define UNLOCK(p)     PORT_UNLOCK(p)
#define SAFE_CALL(p, exp) PORT_SAFE_CALL(p, exp)
#else
#define VMDECL        /*none*/
#define LOCK(p)       /*none*/
#define UNLOCK(p)     /*none*/
#define SAFE_CALL(p, exp) (exp)
#endif

/* Convenience macro */

#define CLOSE_CHECK(port)                                               \
    do {                                                                \
        if (SCM_PORT_CLOSED_P(port)) {                                  \
            UNLOCK(p);                                                  \
            Scm_Error("I/O attempted on closed port: %S", (port));      \
        }                                                               \
    } while (0)

/*=================================================================
 * Putb
 */

#ifdef SAFE_PORT_OP
void Scm_Putb(ScmByte b, ScmPort *p)
#else
void Scm_PutbUnsafe(ScmByte b, ScmPort *p)
#endif
{
    int err = FALSE;
    VMDECL;

    LOCK(p);
    CLOSE_CHECK(p);

    switch (SCM_PORT_TYPE(p)) {
    case SCM_PORT_FILE:
        if (p->src.buf.current >= p->src.buf.end) {
            SAFE_CALL(p, bufport_flush(p, 1));
        }
        SCM_ASSERT(p->src.buf.current < p->src.buf.end);
        *p->src.buf.current++ = b;
        if (p->src.buf.mode == SCM_PORT_BUFFER_NONE) {
            SAFE_CALL(p, bufport_flush(p, 1));
        }
        UNLOCK(p);
        break;
    case SCM_PORT_OSTR:
        SCM_DSTRING_PUTB(&p->src.ostr, b);
        UNLOCK(p);
        break;
    case SCM_PORT_PROC:
        SAFE_CALL(p, p->src.vt.Putb(b, p));
        UNLOCK(p);
        break;
    default:
        UNLOCK(p);
        Scm_Error("bad port type for output: %S", p);
    }
}

/*=================================================================
 * Putc
 */

#ifdef SAFE_PORT_OP
void Scm_Putc(ScmChar c, ScmPort *p)
#else
void Scm_PutcUnsafe(ScmChar c, ScmPort *p)
#endif
{
    int nb;
    VMDECL;
    
    LOCK(p);
    CLOSE_CHECK(p);
    
    switch (SCM_PORT_TYPE(p)) {
    case SCM_PORT_FILE:
        nb = SCM_CHAR_NBYTES(c);
        if (p->src.buf.current+nb > p->src.buf.end) {
            SAFE_CALL(p, bufport_flush(p, nb));
        }
        SCM_ASSERT(p->src.buf.current+nb <= p->src.buf.end);
        SCM_CHAR_PUT(p->src.buf.current, c);
        p->src.buf.current += nb;
        if (p->src.buf.mode == SCM_PORT_BUFFER_LINE) {
            if (c == '\n') {
                SAFE_CALL(p, bufport_flush(p, nb));
            }
        } else if (p->src.buf.mode == SCM_PORT_BUFFER_NONE) {
            SAFE_CALL(p, bufport_flush(p, nb));
        }
        UNLOCK(p);
        break;
    case SCM_PORT_OSTR:
        SCM_DSTRING_PUTC(&p->src.ostr, c);
        UNLOCK(p);
        break;
    case SCM_PORT_PROC:
        PORT_SAFE_CALL(p, p->src.vt.Putc(c, p));
        UNLOCK(p);
        break;
    default:
        UNLOCK(p);
        Scm_Error("bad port type for output: %S", p);
    }
}

/*=================================================================
 * Puts
 */

#ifdef SAFE_PORT_OP
void Scm_Puts(ScmString *s, ScmPort *p)
#else
void Scm_PutsUnsafe(ScmString *s, ScmPort *p)
#endif
{
    VMDECL;
    LOCK(p);
    CLOSE_CHECK(p);
    
    switch (SCM_PORT_TYPE(p)) {
    case SCM_PORT_FILE:
        SAFE_CALL(p, bufport_write(p, SCM_STRING_START(s), SCM_STRING_SIZE(s)));
        
        if (p->src.buf.mode == SCM_PORT_BUFFER_LINE) {
            const char *cp = p->src.buf.current;
            while (cp-- > p->src.buf.buffer) {
                if (*cp == '\n') {
                    SAFE_CALL(p, bufport_flush(p, (int)(cp - p->src.buf.current)));
                    break;
                }
            }
        } else if (p->src.buf.mode == SCM_PORT_BUFFER_NONE) {
            SAFE_CALL(p, bufport_flush(p, 0));
        }
        UNLOCK(p);
        break;
    case SCM_PORT_OSTR:
        Scm_DStringAdd(&p->src.ostr, s);
        UNLOCK(p);
        break;
    case SCM_PORT_PROC:
        SAFE_CALL(p, p->src.vt.Puts(s, p));
        UNLOCK(p);
        break;
    default:
        UNLOCK(p);
        Scm_Error("bad port type for output: %S", p);
    }
}

/*=================================================================
 * Putz
 */

#ifdef SAFE_PORT_OP
void Scm_Putz(const char *s, int siz, ScmPort *p)
#else
void Scm_PutzUnsafe(const char *s, int siz, ScmPort *p)
#endif
{
    VMDECL;
    LOCK(p);
    CLOSE_CHECK(p);
    if (siz < 0) siz = strlen(s);
    switch (SCM_PORT_TYPE(p)) {
    case SCM_PORT_FILE:
        SAFE_CALL(p, bufport_write(p, s, siz));
        if (p->src.buf.mode == SCM_PORT_BUFFER_LINE) {
            const char *cp = p->src.buf.current;
            while (cp-- > p->src.buf.buffer) {
                if (*cp == '\n') {
                    SAFE_CALL(p, bufport_flush(p, (int)(cp - p->src.buf.current)));
                    break;
                }
            }
        } else if (p->src.buf.mode == SCM_PORT_BUFFER_NONE) {
            SAFE_CALL(p, bufport_flush(p, 0));
        }
        UNLOCK(p);
        break;
    case SCM_PORT_OSTR:
        Scm_DStringPutz(&p->src.ostr, s, siz);
        UNLOCK(p);
        break;
    case SCM_PORT_PROC:
        SAFE_CALL(p, p->src.vt.Putz(s, siz, p));
        UNLOCK(p);
        break;
    default:
        UNLOCK(p);
        Scm_Error("bad port type for output: %S", p);
    }
}

/*=================================================================
 * Flush
 */

#ifdef SAFE_PORT_OP
void Scm_Flush(ScmPort *p)
#else
void Scm_FlushUnsafe(ScmPort *p)
#endif
{
    VMDECL;
    LOCK(p);
    CLOSE_CHECK(p);
    switch (SCM_PORT_TYPE(p)) {
    case SCM_PORT_FILE:
        SAFE_CALL(p, bufport_flush(p, 0));
        UNLOCK(p);
        break;
    case SCM_PORT_OSTR:
        UNLOCK(p);
        break;
    case SCM_PORT_PROC:
        SAFE_CALL(p, p->src.vt.Flush(p));
        UNLOCK(p);
        break;
    default:
        UNLOCK(p);
        Scm_Error("bad port type for output: %S", p);
    }
}

/*=================================================================
 * Ungetc
 */

#ifdef SAFE_PORT_OP
void Scm_Ungetc(ScmChar c, ScmPort *p)
#else
void Scm_UngetcUnsafe(ScmChar c, ScmPort *p)
#endif
{
    VMDECL;
    LOCK(p);
    SCM_UNGETC(c, p);
    UNLOCK(p);
}

/*=================================================================
 * Getb
 */

#ifndef SHIFT_SCRATCH  /* we need to define this only once */
/* shift scratch buffer content */
#define SHIFT_SCRATCH(p, off) \
   do { int i_; for (i_=0; i_ < (p)->scrcnt; i_++) (p)->scratch[i_]=(p)->scratch[i_+(off)]; } while (0)

/* handle the case that there's remaining data in the scratch buffer */
static int getb_scratch(ScmPort *p)
{
    int b = (unsigned char)p->scratch[0];
    p->scrcnt--;
    SHIFT_SCRATCH(p, 1);
    return b;
}

/* handle the case that there's an ungotten char */
static int getb_ungotten(ScmPort *p)
{
    SCM_CHAR_PUT(p->scratch, p->ungotten);
    p->scrcnt = SCM_CHAR_NBYTES(p->ungotten);
    p->ungotten = SCM_CHAR_INVALID;
    return getb_scratch(p);
}
#endif /*SHIFT_SCRATCH*/

/* Getb body */
#ifdef SAFE_PORT_OP
int Scm_Getb(ScmPort *p)
#else
int Scm_GetbUnsafe(ScmPort *p)
#endif
{
    int b = 0, r;
    VMDECL;

    LOCK(p);
    CLOSE_CHECK(p);

    /* check if there's "pushded back" stuff */
    if (p->scrcnt) {
        b = getb_scratch(p);
    } else if (p->ungotten != SCM_CHAR_INVALID) {
        b = getb_ungotten(p);
    } else {
        switch (SCM_PORT_TYPE(p)) {
        case SCM_PORT_FILE:
            if (p->src.buf.current >= p->src.buf.end) {
                SAFE_CALL(p, r = bufport_fill(p, 1, FALSE));
                if (r == 0) {
                    UNLOCK(p);
                    return EOF;
                }
            }
            b = (unsigned char)*p->src.buf.current++;
            break;
        case SCM_PORT_ISTR:
            if (p->src.istr.current >= p->src.istr.end) b = EOF;
            else b = (unsigned char)*p->src.istr.current++;
            break;
        case SCM_PORT_PROC:
            SAFE_CALL(p, b = p->src.vt.Getb(p));
            break;
        default:
            UNLOCK(p);
            Scm_Error("bad port type for output: %S", p);
        }
    }
    UNLOCK(p);
    return b;
}

/*=================================================================
 * Getc
 */

/* handle the case that there's data in scratch area */
#ifdef SAFE_PORT_OP
#define GETC_SCRATCH getc_scratch
static int getc_scratch(ScmPort *p)
#else
#define GETC_SCRATCH getc_scratch_unsafe
static int getc_scratch_unsafe(ScmPort *p)
#endif
{
    char tbuf[SCM_CHAR_MAX_BYTES];
    int nb = SCM_CHAR_NFOLLOWS(p->scratch[0]), ch, i, curr = p->scrcnt;
    int r;
    
    memcpy(tbuf, p->scratch, curr);
    p->scrcnt = 0;
    for (i=curr; i<=nb; i++) {
        SAFE_CALL(p, r = Scm_Getb(p));
        if (r == EOF) {
            UNLOCK(p);
            Scm_Error("encountered EOF in middle of a multibyte character from port %S", p);
        }
        tbuf[i] = (char)r;
    }
    SCM_CHAR_GET(tbuf, ch);
    return ch;
}

/* Getc body */
#ifdef SAFE_PORT_OP
int Scm_Getc(ScmPort *p)
#else
int Scm_GetcUnsafe(ScmPort *p)
#endif
{
    int first, nb, c = 0, r;
    VMDECL;

    LOCK(p);
    CLOSE_CHECK(p);
    if (p->scrcnt > 0) {
        r = GETC_SCRATCH(p);
        UNLOCK(p);
        return r;
    }
    if (p->ungotten != SCM_CHAR_INVALID) {
        c = p->ungotten;
        p->ungotten = SCM_CHAR_INVALID;
        UNLOCK(p);
        return c;
    }

    switch (SCM_PORT_TYPE(p)) {
    case SCM_PORT_FILE:
        if (p->src.buf.current >= p->src.buf.end) {
            SAFE_CALL(p, r = bufport_fill(p, 1, FALSE));
            if (r == 0) {
                UNLOCK(p);
                return EOF;
            }
        }
        first = (unsigned char)*p->src.buf.current++;
        nb = SCM_CHAR_NFOLLOWS(first);
        if (nb > 0) {
            if (p->src.buf.current + nb > p->src.buf.end) {
                /* The buffer doesn't have enough bytes to consist a char.
                   move the incomplete char to the scratch buffer and try
                   to fetch the rest of the char. */
                int rest, filled = 0; 
                p->scrcnt = (unsigned char)(p->src.buf.end - p->src.buf.current + 1);
                memcpy(p->scratch, p->src.buf.current-1, p->scrcnt);
                p->src.buf.current = p->src.buf.end;
                rest = nb + 1 - p->scrcnt;
                for (;;) {
                    SAFE_CALL(p, filled = bufport_fill(p, rest, FALSE));
                    if (filled <= 0) {
                        /* TODO: make this behavior customizable */
                        UNLOCK(p);
                        Scm_Error("encountered EOF in middle of a multibyte character from port %S", p);
                    }
                    if (filled >= rest) {
                        memcpy(p->scratch+p->scrcnt, p->src.buf.current, rest);
                        p->scrcnt += rest;
                        p->src.buf.current += rest;
                        break;
                    } else {
                        memcpy(p->scratch+p->scrcnt, p->src.buf.current, filled);
                        p->scrcnt += filled;
                        p->src.buf.current = p->src.buf.end;
                        rest -= filled;
                    }
                }
                SCM_CHAR_GET(p->scratch, c);
                p->scrcnt = 0;
            } else {
                SCM_CHAR_GET(p->src.buf.current-1, c);
                p->src.buf.current += nb;
            }
        } else {
            c = first;
            if (c == '\n') p->src.buf.line++;
        }
        UNLOCK(p);
        return c;
    case SCM_PORT_ISTR:
        if (p->src.istr.current >= p->src.istr.end) {
            UNLOCK(p);
            return EOF;
        }
        first = (unsigned char)*p->src.istr.current++;
        nb = SCM_CHAR_NFOLLOWS(first);
        if (nb > 0) {
            if (p->src.istr.current + nb > p->src.istr.end) {
                /* TODO: make this behavior customizable */
                UNLOCK(p);
                Scm_Error("encountered EOF in middle of a multibyte character from port %S", p);
            }
            SCM_CHAR_GET(p->src.istr.current-1, c);
            p->src.istr.current += nb;
        } else {
            c = first;
        }
        UNLOCK(p);
        return c;
    case SCM_PORT_PROC:
        SAFE_CALL(p, c = p->src.vt.Getc(p));
        UNLOCK(p);
        return c;
    default:
        UNLOCK(p);
        Scm_Error("bad port type for output: %S", p);
    }
    return 0;/*dummy*/
}

#undef GETC_SCRATCH

/*=================================================================
 * Getz - block read.
 *   If the buffering mode is BUFFER_FULL, this reads BUFLEN bytes
 *   unless it reaches EOF.  Otherwise, this reads less than BUFLEN
 *   if the data is not immediately available.
 */

#ifdef SAFE_PORT_OP
#define GETZ_SCRATCH getz_scratch
static int getz_scratch(char *buf, int buflen, ScmPort *p)
#else
#define GETZ_SCRATCH getz_scratch_unsafe
static int getz_scratch_unsafe(char *buf, int buflen, ScmPort *p)
#endif
{
    int i, n;
    if (p->scrcnt >= buflen) {
        memcpy(buf, p->scratch, buflen);
        p->scrcnt -= buflen;
        SHIFT_SCRATCH(p, buflen);
        return buflen;
    } else {
        memcpy(buf, p->scratch, p->scrcnt);
        i = p->scrcnt;
        p->scrcnt = 0;
        SAFE_CALL(p, n = Scm_Getz(buf+i, buflen-i, p));
        return i + n;
    }
}

#ifdef SAFE_PORT_OP
int Scm_Getz(char *buf, int buflen, ScmPort *p)
#else
int Scm_GetzUnsafe(char *buf, int buflen, ScmPort *p)
#endif
{
    int siz, r;
    VMDECL;
    LOCK(p);
    CLOSE_CHECK(p);

    if (p->scrcnt) {
        r = GETZ_SCRATCH(buf, buflen, p);
        UNLOCK(p);
        return r;
    }
    if (p->ungotten != SCM_CHAR_INVALID) {
        p->scrcnt = SCM_CHAR_NBYTES(p->ungotten);
        SCM_CHAR_PUT(p->scratch, p->ungotten);
        p->ungotten = SCM_CHAR_INVALID;
        r = GETZ_SCRATCH(buf, buflen, p);
        UNLOCK(p);
        return r;
    }

    switch (SCM_PORT_TYPE(p)) {
    case SCM_PORT_FILE:
        SAFE_CALL(p, siz = bufport_read(p, buf, buflen));
        UNLOCK(p);
        if (siz == 0) return EOF;
        else return siz;
    case SCM_PORT_ISTR:
        if (p->src.istr.current + buflen >= p->src.istr.end) {
            if (p->src.istr.current >= p->src.istr.end) {
                UNLOCK(p);
                return EOF;
            }
            siz = (int)(p->src.istr.end - p->src.istr.current);
            memcpy(buf, p->src.istr.current, siz);
            p->src.istr.current = p->src.istr.end;
            UNLOCK(p);
            return siz;
        } else {
            memcpy(buf, p->src.istr.current, buflen);
            p->src.istr.current += buflen;
            UNLOCK(p);
            return buflen;
        }
    case SCM_PORT_PROC:
        SAFE_CALL(p, r = p->src.vt.Getz(buf, buflen, p));
        UNLOCK(p);
        return r;
    default:
        UNLOCK(p);
        Scm_Error("bad port type for output: %S", p);
    }
    return -1;                  /* dummy */
}

#undef GETZ_SCRATCH

/*=================================================================
 * ReadLine
 *   Reads up to EOL or EOF.  
 */

/* Readline can be optimized by directly scanning the buffer instead of
   reading one char at a time, bypassing mb->char->mb conversion and
   DString creation.   There was a some attempt to implement it; see
   port.c, v1.69 for details.  For now, I drop that. */

#ifdef SAFE_PORT_OP
ScmObj Scm_ReadLine(ScmPort *p)
#else
ScmObj Scm_ReadLineUnsafe(ScmPort *p)
#endif
{
    int c1, c2;
    ScmDString ds;
    VMDECL;

    Scm_DStringInit(&ds);
    LOCK(p);
    c1 = Scm_GetcUnsafe(p);
    if (c1 == EOF) {
        UNLOCK(p);
        return SCM_EOF;
    }
    for (;;) {
        if (c1 == EOF || c1 == '\n') break;
        if (c1 == '\r') {
            c2 = Scm_GetcUnsafe(p);
            if (c2 == EOF || c2 == '\n') break;
            Scm_UngetcUnsafe(c2, p);
            break;
        }
        SCM_DSTRING_PUTC(&ds, c1);
        c1 = Scm_GetcUnsafe(p);
    }
    UNLOCK(p);
    return Scm_DStringGet(&ds);
}



#undef VMDECL
#undef LOCK
#undef UNLOCK
#undef SAFE_CALL
#undef CLOSE_CHECK

