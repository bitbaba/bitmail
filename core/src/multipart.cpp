/* asn_mime.c */
/*
 * Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project.
 */
/* ====================================================================
 * Copyright (c) 1999-2008 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
# include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <string>
#include <sstream>
#include <vector>

/*
 * Generalised MIME like utilities for streaming ASN1. Although many have a
 * PKCS7/CMS like flavour others are more general purpose.
 */

/*
 * MIME format structures Note that all are translated to lower case apart
 * from parameter values. Quotes are stripped off
 */

typedef struct {
    char *param_name;           /* Param name e.g. "micalg" */
    char *param_value;          /* Param value e.g. "sha1" */
} MIME_PARAM;

DECLARE_STACK_OF(MIME_PARAM)
IMPLEMENT_STACK_OF(MIME_PARAM)



typedef struct {
    char *name;                 /* Name of line e.g. "content-type" */
    char *value;                /* Value of line e.g. "text/plain" */
    STACK_OF(MIME_PARAM) *params; /* Zero or more parameters */
} MIME_HEADER;

DECLARE_STACK_OF(MIME_HEADER)
IMPLEMENT_STACK_OF(MIME_HEADER)



static char * strip_ends(char *name);

static char * strip_start(char *name);

static char * strip_end(char *name);

static int    strip_eol(char *linebuf, int *plen);




static MIME_HEADER *           mime_hdr_new(char *name, char *value);

static STACK_OF(MIME_HEADER) * mime_parse_hdr(BIO *bio);

static MIME_HEADER *           mime_hdr_find(STACK_OF(MIME_HEADER) *hdrs, const char *name);

static MIME_PARAM *            mime_param_find(MIME_HEADER *hdr, const char *name);

static int  mime_hdr_addparam(MIME_HEADER *mhdr, char *name, char *value);

static int  mime_hdr_cmp(const MIME_HEADER *const *a, const MIME_HEADER *const *b);
						
static int  mime_param_cmp(const MIME_PARAM *const *a, const MIME_PARAM *const *b);
						  
static void mime_param_free(MIME_PARAM *param);

static void mime_hdr_free(MIME_HEADER *hdr);

static int  mime_bound_check(char *line, int linelen, char *bound, int blen);

static int  multi_split(BIO *bio, char *bound, STACK_OF(BIO) **ret);




#define MAX_SMLEN 1024
#define mime_debug(x)           /* x */

int multipart_split(const std::string & in, std::vector<std::string> & out)
{
    STACK_OF(MIME_HEADER) *headers = NULL;
    STACK_OF(BIO) *parts = NULL;
    MIME_HEADER *hdr;
    MIME_PARAM *prm;
	int ret;
	
	BIO * bio = BIO_new_mem_buf((void*)in.data(), in.length());
	
	if (!(headers = mime_parse_hdr(bio))) {
	    BIO_free(bio);
        return 1;
    }

    if (!(hdr = mime_hdr_find(headers, "content-type")) || !hdr->value) {
        sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
		BIO_free(bio);
        return 1;
    }

    /** Fix: match multipart/mixed, mulitpart/alternative, etc**/
    if (!strncmp(hdr->value, "multipart/", 10)) {
        /* Split into multiple parts */
        prm = mime_param_find(hdr, "boundary");
        if (!prm || !prm->param_value) {
            sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
			BIO_free(bio);
            return 1;
        }
        
		ret = multi_split(bio, prm->param_value, &parts);
		
        if (!ret || (sk_BIO_num(parts) == 0)) {
            sk_BIO_pop_free(parts, BIO_vfree);
			BIO_free(bio);
            return 1;
        }

        /* Parse the signature piece */
		for (int i = 0; i < sk_BIO_num(parts); i ++){
			BIO * piece = sk_BIO_value(parts, i);
			BUF_MEM * bufptr = NULL;
			BIO_get_mem_ptr(piece, &bufptr);
			std::string s = "";
			s.append((char *)bufptr->data, bufptr->length);
			out.push_back(s);
		}

		sk_BIO_pop_free(parts, BIO_vfree);
    }
	
	sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
	BIO_free(bio);
	
	return 0;
}

std::string multipart_partType(const std::string & in)
{
	STACK_OF(MIME_HEADER) *headers = NULL;
    STACK_OF(BIO) *parts = NULL;
    MIME_HEADER *hdr;
	
	BIO * bio = BIO_new_mem_buf((void*)in.data(), in.length());
	
	if (!(headers = mime_parse_hdr(bio))) {
	    BIO_free(bio);
        return "";
    }

    if (!(hdr = mime_hdr_find(headers, "content-type")) || !hdr->value) {
        sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
		BIO_free(bio);
        return "";
    }

	std::string sType = hdr->value;
	sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
	BIO_free(bio);
		
	return sType;
}

std::string multipart_partParam(const std::string & in, const std::string & header, const std::string & pName/*in lower case*/)
{
	STACK_OF(MIME_HEADER) *headers = NULL;
    STACK_OF(BIO) *parts = NULL;
    MIME_HEADER *hdr;
    MIME_PARAM *prm;
	
	BIO * bio = BIO_new_mem_buf((void*)in.data(), in.length());
	
	if (!(headers = mime_parse_hdr(bio))) {
	    BIO_free(bio);
        return "";
    }

    if (!(hdr = mime_hdr_find(headers, header.c_str())) || !hdr->value) {
        sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
		BIO_free(bio);
        return "";
    }

	prm = mime_param_find(hdr, pName.c_str());
	if (!prm || !prm->param_value) {
		sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
		BIO_free(bio);
		return "";
	}
	
        std::string pVal = prm->param_value;
	
	sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
	BIO_free(bio);
		
	return pVal;
}

std::string multipart_partContent(const std::string & in)
{
    char iobuf[4096];
    int len;
	STACK_OF(MIME_HEADER) *headers = NULL;
	
	BIO * bio = BIO_new_mem_buf((void*)in.data(), in.length());
	
	/* consume header*/
	if (!(headers = mime_parse_hdr(bio))) {
	    BIO_free(bio);
        return "";
    }	
	sk_MIME_HEADER_pop_free(headers, mime_hdr_free);

	/* eat body */
	std::string out;
	while ((len = BIO_read(bio, iobuf, sizeof(iobuf))) > 0){
		out.append(iobuf, len);
	}
	
	BIO_free(bio);
	
	return out;
}

std::string multipart_partEncoding(const std::string & in)
{
	STACK_OF(MIME_HEADER) *headers = NULL;
    STACK_OF(BIO) *parts = NULL;
    MIME_HEADER *hdr;
	
	BIO * bio = BIO_new_mem_buf((void*)in.data(), in.length());
	
	if (!(headers = mime_parse_hdr(bio))) {
	    BIO_free(bio);
        return "";
    }

    if (!(hdr = mime_hdr_find(headers, "content-transfer-encoding")) || !hdr->value) {
        sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
		BIO_free(bio);
        return "";
    }

	std::string sEncoding = hdr->value;
	
	sk_MIME_HEADER_pop_free(headers, mime_hdr_free);
	BIO_free(bio);
		
	return sEncoding;
}

/*
 * Split a multipart/XXX message body into component parts: result is
 * canonical parts in a STACK of bios
 */

static int multi_split(BIO *bio, char *bound, STACK_OF(BIO) **ret)
{
    char linebuf[MAX_SMLEN];
    int len, blen;
    int eol = 0, next_eol = 0;
    BIO *bpart = NULL;
    STACK_OF(BIO) *parts;
    char state, part, first;

    blen = strlen(bound);
    part = 0;
    state = 0;
    first = 1;
    parts = sk_BIO_new_null();
    *ret = parts;
    while ((len = BIO_gets(bio, linebuf, MAX_SMLEN)) > 0) {
        state = mime_bound_check(linebuf, len, bound, blen);
        if (state == 1) {
            first = 1;
            part++;
        } else if (state == 2) {
            sk_BIO_push(parts, bpart);
            return 1;
        } else if (part) {
            /* Strip CR+LF from linebuf */
            next_eol = strip_eol(linebuf, &len);
            if (first) {
                first = 0;
                if (bpart)
                    sk_BIO_push(parts, bpart);
                bpart = BIO_new(BIO_s_mem());
                BIO_set_mem_eof_return(bpart, 0);
            } else if (eol)
                BIO_write(bpart, "\r\n", 2);
            eol = next_eol;
            if (len)
                BIO_write(bpart, linebuf, len);
        }
    }
    return 0;
}

/* This is the big one: parse MIME header lines up to message body */

#define MIME_INVALID    0
#define MIME_START      1
#define MIME_TYPE       2
#define MIME_NAME       3
#define MIME_VALUE      4
#define MIME_QUOTE      5
#define MIME_COMMENT    6

static STACK_OF(MIME_HEADER) *mime_parse_hdr(BIO *bio)
{
    char *p, *q, c;
    char *ntmp;
    char linebuf[MAX_SMLEN];
    MIME_HEADER *mhdr = NULL;
    STACK_OF(MIME_HEADER) *headers;
    int len, state, save_state = 0;

    headers = sk_MIME_HEADER_new(mime_hdr_cmp);
    if (!headers)
        return NULL;
    while ((len = BIO_gets(bio, linebuf, MAX_SMLEN)) > 0) {
        /* If whitespace at line start then continuation line */
        if (mhdr && isspace((unsigned char)linebuf[0]))
            state = MIME_NAME;
        else
            state = MIME_START;
        ntmp = NULL;
        /* Go through all characters */
        for (p = linebuf, q = linebuf; (c = *p) && (c != '\r') && (c != '\n');
             p++) {

            /*
             * State machine to handle MIME headers if this looks horrible
             * that's because it *is*
             */

            switch (state) {
            case MIME_START:
                if (c == ':') {
                    state = MIME_TYPE;
                    *p = 0;
                    ntmp = strip_ends(q);
                    q = p + 1;
                }
                break;

            case MIME_TYPE:
                if (c == ';') {
                    mime_debug("Found End Value\n");
                    *p = 0;
                    mhdr = mime_hdr_new(ntmp, strip_ends(q));
                    sk_MIME_HEADER_push(headers, mhdr);
                    ntmp = NULL;
                    q = p + 1;
                    state = MIME_NAME;
                } else if (c == '(') {
                    save_state = state;
                    state = MIME_COMMENT;
                }
                break;

            case MIME_COMMENT:
                if (c == ')') {
                    state = save_state;
                }
                break;

            case MIME_NAME:
                if (c == '=') {
                    state = MIME_VALUE;
                    *p = 0;
                    ntmp = strip_ends(q);
                    q = p + 1;
                }
                break;

            case MIME_VALUE:
                if (c == ';') {
                    state = MIME_NAME;
                    *p = 0;
                    mime_hdr_addparam(mhdr, ntmp, strip_ends(q));
                    ntmp = NULL;
                    q = p + 1;
                } else if (c == '"') {
                    mime_debug("Found Quote\n");
                    state = MIME_QUOTE;
                } else if (c == '(') {
                    save_state = state;
                    state = MIME_COMMENT;
                }
                break;

            case MIME_QUOTE:
                if (c == '"') {
                    mime_debug("Found Match Quote\n");
                    state = MIME_VALUE;
                }
                break;
            }
        }

        if (state == MIME_TYPE) {
            mhdr = mime_hdr_new(ntmp, strip_ends(q));
            sk_MIME_HEADER_push(headers, mhdr);
        } else if (state == MIME_VALUE)
            mime_hdr_addparam(mhdr, ntmp, strip_ends(q));
        if (p == linebuf)
            break;              /* Blank line means end of headers */
    }

    return headers;

}

static char *strip_ends(char *name)
{
    return strip_end(strip_start(name));
}

/* Strip a parameter of whitespace from start of param */
static char *strip_start(char *name)
{
    char *p, c;
    /* Look for first non white space or quote */
    for (p = name; (c = *p); p++) {
        if (c == '"') {
            /* Next char is start of string if non null */
            if (p[1])
                return p + 1;
            /* Else null string */
            return NULL;
        }
        if (!isspace((unsigned char)c))
            return p;
    }
    return NULL;
}

/* As above but strip from end of string : maybe should handle brackets? */
static char *strip_end(char *name)
{
    char *p, c;
    if (!name)
        return NULL;
    /* Look for first non white space or quote */
    for (p = name + strlen(name) - 1; p >= name; p--) {
        c = *p;
        if (c == '"') {
            if (p - 1 == name)
                return NULL;
            *p = 0;
            return name;
        }
        if (isspace((unsigned char)c))
            *p = 0;
        else
            return name;
    }
    return NULL;
}

static MIME_HEADER *mime_hdr_new(char *name, char *value)
{
    MIME_HEADER *mhdr;
    char *tmpname, *tmpval, *p;
    int c;
    if (name) {
        if (!(tmpname = BUF_strdup(name)))
            return NULL;
        for (p = tmpname; *p; p++) {
            c = (unsigned char)*p;
            if (isupper(c)) {
                c = tolower(c);
                *p = c;
            }
        }
    } else
        tmpname = NULL;
    if (value) {
        if (!(tmpval = BUF_strdup(value)))
            return NULL;
        for (p = tmpval; *p; p++) {
            c = (unsigned char)*p;
            if (isupper(c)) {
                c = tolower(c);
                *p = c;
            }
        }
    } else
        tmpval = NULL;
    mhdr = (MIME_HEADER *)OPENSSL_malloc(sizeof(MIME_HEADER));
    if (!mhdr)
        return NULL;
    mhdr->name = tmpname;
    mhdr->value = tmpval;
    if (!(mhdr->params = sk_MIME_PARAM_new(mime_param_cmp)))
        return NULL;
    return mhdr;
}

static int mime_hdr_addparam(MIME_HEADER *mhdr, char *name, char *value)
{
    char *tmpname, *tmpval, *p;
    int c;
    MIME_PARAM *mparam;
    if (name) {
        tmpname = BUF_strdup(name);
        if (!tmpname)
            return 0;
        for (p = tmpname; *p; p++) {
            c = (unsigned char)*p;
            if (isupper(c)) {
                c = tolower(c);
                *p = c;
            }
        }
    } else
        tmpname = NULL;
    if (value) {
        tmpval = BUF_strdup(value);
        if (!tmpval)
            return 0;
    } else
        tmpval = NULL;
    /* Parameter values are case sensitive so leave as is */
    mparam = (MIME_PARAM *)OPENSSL_malloc(sizeof(MIME_PARAM));
    if (!mparam)
        return 0;
    mparam->param_name = tmpname;
    mparam->param_value = tmpval;
    sk_MIME_PARAM_push(mhdr->params, mparam);
    return 1;
}

static int mime_hdr_cmp(const MIME_HEADER *const *a,
                        const MIME_HEADER *const *b)
{
    if (!(*a)->name || !(*b)->name)
        return ! !(*a)->name - ! !(*b)->name;

    return (strcmp((*a)->name, (*b)->name));
}

static int mime_param_cmp(const MIME_PARAM *const *a,
                          const MIME_PARAM *const *b)
{
    if (!(*a)->param_name || !(*b)->param_name)
        return ! !(*a)->param_name - ! !(*b)->param_name;
    return (strcmp((*a)->param_name, (*b)->param_name));
}

/* Find a header with a given name (if possible) */

static MIME_HEADER *mime_hdr_find(STACK_OF(MIME_HEADER) *hdrs, const char *name)
{
    MIME_HEADER htmp;
    int idx;
    htmp.name = (char *)name;
    idx = sk_MIME_HEADER_find(hdrs, &htmp);
    if (idx < 0)
        return NULL;
    return sk_MIME_HEADER_value(hdrs, idx);
}

static MIME_PARAM *mime_param_find(MIME_HEADER *hdr, const char *name)
{
    MIME_PARAM param;
    int idx;
    param.param_name = (char *)name;
    idx = sk_MIME_PARAM_find(hdr->params, &param);
    if (idx < 0)
        return NULL;
    return sk_MIME_PARAM_value(hdr->params, idx);
}

static void mime_hdr_free(MIME_HEADER *hdr)
{
    if (hdr->name)
        OPENSSL_free(hdr->name);
    if (hdr->value)
        OPENSSL_free(hdr->value);
    if (hdr->params)
        sk_MIME_PARAM_pop_free(hdr->params, mime_param_free);
    OPENSSL_free(hdr);
}

static void mime_param_free(MIME_PARAM *param)
{
    if (param->param_name)
        OPENSSL_free(param->param_name);
    if (param->param_value)
        OPENSSL_free(param->param_value);
    OPENSSL_free(param);
}

/*-
 * Check for a multipart boundary. Returns:
 * 0 : no boundary
 * 1 : part boundary
 * 2 : final boundary
 */
static int mime_bound_check(char *line, int linelen, char *bound, int blen)
{
    if (linelen == -1)
        linelen = strlen(line);
    if (blen == -1)
        blen = strlen(bound);
    /* Quickly eliminate if line length too short */
    if (blen + 2 > linelen)
        return 0;
    /* Check for part boundary */
    if (!strncmp(line, "--", 2) && !strncmp(line + 2, bound, blen)) {
        if (!strncmp(line + blen + 2, "--", 2))
            return 2;
        else
            return 1;
    }
    return 0;
}

static int strip_eol(char *linebuf, int *plen)
{
    int len = *plen;
    char *p, c;
    int is_eol = 0;
    p = linebuf + len - 1;
    for (p = linebuf + len - 1; len > 0; len--, p--) {
        c = *p;
        if (c == '\n')
            is_eol = 1;
        else if (c != '\r')
            break;
    }
    *plen = len;
    return is_eol;
}

std::string toMimeTextPlain(const std::string &s)
{
    std::string mime = "Content-Type: text/plain\r\n";
    mime += "Content-Transfer-Encoding: 7bit\r\n";
    mime += "\r\n";
    mime += (s);
    return mime;
}

std::string fromMimeTextPlain(const std::string & part)
{
    std::string qsEncoding = multipart_partEncoding(part);

    std::string content = multipart_partContent(part);

    if (qsEncoding == "7bit"){ // latin1
        return content;
    }
    return "";
}
