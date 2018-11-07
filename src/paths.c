/*
 * paths.c - get 'known' pathnames, such as the system's library directory.
 *
 *   Copyright (c) 2005-2018  Shiro Kawai  <shiro@acm.org>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* This file is used by both libgauche (included from libeval.scm) and
 * gauche-config (included from gauche-config.c).  The latter
 * doesn't use ScmObj, so the function works on bare C strings.
 */

#define LIBGAUCHE_BODY
#include "gauche.h"

#if !defined(PATH_ALLOC)
#define PATH_ALLOC(n)  SCM_MALLOC_ATOMIC(n)
#endif

#if defined(GAUCHE_WINDOWS)
#include "getdir_win.c"
#elif defined(GAUCHE_MACOSX_FRAMEWORK)
#include "getdir_darwin.c"
#else
#include "getdir_dummy.c"
#endif

#include "substitute_all.c"


/* The configure-generated path may have '@' in the pathnames.  We replace
   it with the installation directory. 

   NB: This is a static function, but called from gauche-config.c (it includes
   paths.c).
*/
static const char *replace_install_dir(const char *orig,
                                       void (*errfn)(const char *, ...))
{
    if (strstr(orig, "@") == NULL) return orig; /* no replace */
    return substitute_all(orig, "@", get_install_dir(errfn));
}
