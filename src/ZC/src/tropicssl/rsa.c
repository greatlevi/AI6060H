/*
*	The RSA public-key cryptosystem
*
*	Based on XySSL: Copyright (C) 2006-2008	 Christophe Devine
*
*	Copyright (C) 2009	Paul Bakker <polarssl_maintainer at polarssl dot org>
*
*	All rights reserved.
*
*	Redistribution and use in source and binary forms, with or without
*	modification, are permitted provided that the following conditions
*	are met:
*
*	  * Redistributions of source code must retain the above copyright
*		notice, this list of conditions and the following disclaimer.
*	  * Redistributions in binary form must reproduce the above copyright
*		notice, this list of conditions and the following disclaimer in the
*		documentation and/or other materials provided with the distribution.
*	  * Neither the names of PolarSSL or XySSL nor the names of its contributors
*		may be used to endorse or promote products derived from this software
*		without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*	FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
*	TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
*	RSA was designed by Ron Rivest, Adi Shamir and Len Adleman.
*
*	http://theory.lcs.mit.edu/~rivest/rsapaper.pdf
*	http://www.cacr.math.uwaterloo.ca/hac/about/chap8.pdf
*/

#include "secconfig.h"

#if defined(TROPICSSL_RSA_C)

#include "rsa.h"

#include <zc_common.h>
/*
* Initialize an RSA context
*/
void rsa_init(rsa_context * ctx,
              int padding, int hash_id, int (*f_rng) (void *), void *p_rng)
{
    memset(ctx, 0, sizeof(rsa_context));

    ctx->padding = padding;
    ctx->hash_id = hash_id;

    ctx->f_rng = f_rng;
    ctx->p_rng = p_rng;
}

#ifdef ZC_OFF_LINETEST

/*
* Generate an RSA keypair
*/
int rsa_gen_key(rsa_context * ctx, int nbits, int exponent)
{
    int ret;
    mpi P1, Q1, H, G;

    if (ctx->f_rng == NULL || nbits < 128 || exponent < 3)
        return (TROPICSSL_ERR_RSA_BAD_INPUT_DATA);

    mpi_init(&P1); mpi_init(&Q1); mpi_init(&H); mpi_init(&G);

    /*
    * find primes P and Q with Q < P so that:
    * GCD( E, (P-1)*(Q-1) ) == 1
    */
    MPI_CHK(mpi_lset(&ctx->E, exponent));

    do {
        MPI_CHK(mpi_gen_prime(&ctx->P, (nbits + 1) >> 1, 0,
            ctx->f_rng, ctx->p_rng));

        MPI_CHK(mpi_gen_prime(&ctx->Q, (nbits + 1) >> 1, 0,
            ctx->f_rng, ctx->p_rng));

        if (mpi_cmp_mpi(&ctx->P, &ctx->Q) < 0)
            mpi_swap(&ctx->P, &ctx->Q);

        if (mpi_cmp_mpi(&ctx->P, &ctx->Q) == 0)
            continue;

        MPI_CHK(mpi_mul_mpi(&ctx->N, &ctx->P, &ctx->Q));
        if (mpi_msb(&ctx->N) != nbits)
            continue;

        MPI_CHK(mpi_sub_int(&P1, &ctx->P, 1));
        MPI_CHK(mpi_sub_int(&Q1, &ctx->Q, 1));
        MPI_CHK(mpi_mul_mpi(&H, &P1, &Q1));
        MPI_CHK(mpi_gcd(&G, &ctx->E, &H));
    } while (mpi_cmp_int(&G, 1) != 0);

    /*
    * D  = E^-1 mod ((P-1)*(Q-1))
    * DP = D mod (P - 1)
    * DQ = D mod (Q - 1)
    * QP = Q^-1 mod P
    */
    MPI_CHK(mpi_inv_mod(&ctx->D, &ctx->E, &H));
    MPI_CHK(mpi_mod_mpi(&ctx->DP, &ctx->D, &P1));
    MPI_CHK(mpi_mod_mpi(&ctx->DQ, &ctx->D, &Q1));
    MPI_CHK(mpi_inv_mod(&ctx->QP, &ctx->Q, &ctx->P));

    ctx->len = (mpi_msb(&ctx->N) + 7) >> 3;

cleanup:

    mpi_free(&G); mpi_free(&H); mpi_free(&Q1); mpi_free(&P1);

    if (ret != 0) {
        rsa_free(ctx);
        return (TROPICSSL_ERR_RSA_KEY_GEN_FAILED | ret);
    }

    return (0);
}

#endif

/*
* Do an RSA public key operation
*/
mpi T;
int rsa_public(rsa_context * ctx, const unsigned char *input, unsigned char *output)
{
    int ret, olen;
    mpi_init(&T);

    MPI_CHK(mpi_read_binary(&T, input, ctx->len));

    if (mpi_cmp_mpi(&T, &ctx->N) >= 0) {
        mpi_free(&T);
        return (TROPICSSL_ERR_RSA_BAD_INPUT_DATA);
    }

    olen = ctx->len;
    MPI_CHK(mpi_exp_mod(&T, &T, &ctx->E, &ctx->N, &ctx->RN));
    MPI_CHK(mpi_write_binary(&T, output, olen));

cleanup:

    mpi_free(&T);

    if (ret != 0)
        return (TROPICSSL_ERR_RSA_PUBLIC_FAILED | ret);

    return (0);
}

/*
* Do an RSA private key operation
*/
mpi T, T1, T2;

int rsa_private(rsa_context * ctx, const unsigned char *input, unsigned char *output)
{
    int ret, olen;

    mpi_init(&T); mpi_init(&T1); mpi_init(&T2);

    MPI_CHK(mpi_read_binary(&T, input, ctx->len));

    if (mpi_cmp_mpi(&T, &ctx->N) >= 0) {
        mpi_free(&T);
        return (TROPICSSL_ERR_RSA_BAD_INPUT_DATA);
    }
#if 0
    MPI_CHK(mpi_exp_mod(&T, &T, &ctx->D, &ctx->N, &ctx->RN));
#else
    /*
    * faster decryption using the CRT
    *
    * T1 = input ^ dP mod P
    * T2 = input ^ dQ mod Q
    */
    MPI_CHK(mpi_exp_mod(&T1, &T, &ctx->DP, &ctx->P, &ctx->RP));
    MPI_CHK(mpi_exp_mod(&T2, &T, &ctx->DQ, &ctx->Q, &ctx->RQ));

    /*
    * T = (T1 - T2) * (Q^-1 mod P) mod P
    */
    MPI_CHK(mpi_sub_mpi(&T, &T1, &T2));
    MPI_CHK(mpi_mul_mpi(&T1, &T, &ctx->QP));
    MPI_CHK(mpi_mod_mpi(&T, &T1, &ctx->P));

    /*
    * output = T2 + T * Q
    */
    MPI_CHK(mpi_mul_mpi(&T1, &T, &ctx->Q));
    MPI_CHK(mpi_add_mpi(&T, &T2, &T1));
#endif

    olen = ctx->len;
    MPI_CHK(mpi_write_binary(&T, output, olen));

cleanup:

    mpi_free(&T); mpi_free(&T1); mpi_free(&T2);

    if (ret != 0)
        return (TROPICSSL_ERR_RSA_PRIVATE_FAILED | ret);

    return (0);
}

/*
* Add the message padding, then do an RSA operation
*/
int rsa_pkcs1_encrypt(rsa_context * ctx,
                      int mode, int ilen,
                      const unsigned char *input,
                      unsigned char *output)
{
    int nb_pad, olen;
    unsigned char *p = output;

    olen = ctx->len;

    switch (ctx->padding) {
    case RSA_PKCS_V15:

        if (ilen < 0 || olen < ilen + 11)
            return (TROPICSSL_ERR_RSA_BAD_INPUT_DATA);

        nb_pad = olen - 3 - ilen;

        *p++ = 0;
        *p++ = RSA_CRYPT;

        while (nb_pad-- > 0) {
            do {
                *p = (unsigned char)rand();
            } while (*p == 0);
            p++;
        }
        *p++ = 0;
        memcpy(p, input, ilen);
        break;

    default:

        return (TROPICSSL_ERR_RSA_INVALID_PADDING);
    }
    return ((mode == RSA_PUBLIC)
        ? rsa_public(ctx, output, output)
        : rsa_private(ctx, output, output));
}

/*
* Do an RSA operation, then remove the message padding
*/
unsigned char buf[64];

int rsa_pkcs1_decrypt(rsa_context * ctx,
                      int mode, int *olen,
                      const unsigned char *input,
                      unsigned char *output,
                      int output_max_len)
{
    int ret, ilen;
    unsigned char *p;

    ilen = ctx->len;

    if (ilen < 16 || ilen > (int)sizeof(buf))
        return (TROPICSSL_ERR_RSA_BAD_INPUT_DATA);

    ret = (mode == RSA_PUBLIC)
        ? rsa_public(ctx, input, buf)
        : rsa_private(ctx, input, buf);

    if (ret != 0)
        return (ret);

    p = buf;

    switch (ctx->padding) {
    case RSA_PKCS_V15:

        if (*p++ != 0 || *p++ != RSA_CRYPT)
            return (TROPICSSL_ERR_RSA_INVALID_PADDING);

        while (*p != 0) {
            if (p >= buf + ilen - 1)
                return (TROPICSSL_ERR_RSA_INVALID_PADDING);
            p++;
        }
        p++;
        break;

    default:

        return (TROPICSSL_ERR_RSA_INVALID_PADDING);
    }

    if (ilen - (int)(p - buf) > output_max_len)
        return (TROPICSSL_ERR_RSA_OUTPUT_TO_LARGE);
    
    *olen = ilen - (int)(p - buf);
    memcpy(output, p, *olen);

    return (0);
}


/*
* Free the components of an RSA key
*/
void rsa_free(rsa_context * ctx)
{
    mpi_free(&ctx->RQ); mpi_free(&ctx->RP); mpi_free(&ctx->RN);
    mpi_free(&ctx->QP); mpi_free(&ctx->DQ); mpi_free(&ctx->DP);
    mpi_free(&ctx->Q); mpi_free(&ctx->P); mpi_free(&ctx->D);
    mpi_free(&ctx->E); mpi_free(&ctx->N);
}


#endif
