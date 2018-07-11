/*
 * butterfly.h
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: sadko
 */

// This is the SSE implementation of the scrambling functions for self data

namespace lsp
{
    namespace sse
    {
        static inline void FFT_SCRAMBLE_SELF_DIRECT_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Calculate number of items
            size_t items    = (1 << rank) - 1;

            move(dst_re, src_re, items);
            move(dst_im, src_im, items);

            for (size_t i = 1; i < items; ++i)
            {
                size_t j = reverse_bits(FFT_TYPE(i), rank);    /* Reverse the order of the bits */
                if (i >= j)
                    continue;

                /* Copy the values from the reversed position */
                __asm__ __volatile
                (
                    __ASM_EMIT("movss (%0, %2, 4), %%xmm0")
                    __ASM_EMIT("movss (%1, %2, 4), %%xmm1")
                    __ASM_EMIT("movss (%0, %3, 4), %%xmm2")
                    __ASM_EMIT("movss (%1, %3, 4), %%xmm3")
                    __ASM_EMIT("movss %%xmm2, (%0, %2, 4)")
                    __ASM_EMIT("movss %%xmm3, (%1, %2, 4)")
                    __ASM_EMIT("movss %%xmm0, (%0, %3, 4)")
                    __ASM_EMIT("movss %%xmm1, (%1, %3, 4)")
                    :
                    : "r"(dst_re), "r"(dst_im), "r"(i), "r"(j)
                    : "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3"
                );
            }

            // Perform butterfly 4x
            items = 1 << (rank - 2);

            // Perform 4-element butterflies
            __asm__ __volatile__
            (
                /* Prefetch data */
                __ASM_EMIT("prefetchnta (%0)")
                __ASM_EMIT("prefetchnta (%1)")
                /* Loop */
                __ASM_EMIT("1:")
                /* Prefetch data */
                __ASM_EMIT("prefetchnta 0x10(%0)")
                __ASM_EMIT("prefetchnta 0x10(%1)")

                /* Load data to registers */
                __ASM_EMIT(LS_RE " (%0), %%xmm0")          /* xmm0 = r0 r1 r2 r3 */
                __ASM_EMIT(LS_RE " (%1), %%xmm1")          /* xmm1 = i0 i1 i2 i3 */

                /* Shuffle 1 */
                __ASM_EMIT("movaps  %%xmm0, %%xmm2")        /* xmm2 = r0 r1 r2 r3 */
                __ASM_EMIT("shufps $0x88, %%xmm1, %%xmm2")  /* xmm2 = r0 r2 i0 i2 */
                __ASM_EMIT("shufps $0xdd, %%xmm1, %%xmm0")  /* xmm0 = r1 r3 i1 i3 */
                __ASM_EMIT("movaps  %%xmm2, %%xmm1")        /* xmm1 = r0 r2 i0 i2 */

                /* Transform 1 */
                __ASM_EMIT("addps   %%xmm0, %%xmm1")        /* xmm1 = sr0 sr2 si0 si2 = r0+r1 r2+r3 i0+i1 i2+i3 */
                __ASM_EMIT("subps   %%xmm0, %%xmm2")        /* xmm2 = sr1 sr3 si1 si3 = r0-r1 r2-r3 i0-i1 i2-i3 */

                /* Shuffle 2 */
                __ASM_EMIT("movaps  %%xmm1, %%xmm0")        /* xmm0 = sr0 sr2 si0 si2 */
                __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = sr0 si0 sr1 si1 */
                __ASM_EMIT("shufps $0x7d, %%xmm2, %%xmm1")  /* xmm1 = sr2 si2 si3 sr3 */
                __ASM_EMIT("movaps %%xmm0, %%xmm2")         /* xmm2 = sr0 si0 sr1 si1 */

                /* Transform 2 */
                __ASM_EMIT("addps   %%xmm1, %%xmm0")        /* xmm0 = dr0 di0 dr1 di3 = sr0+sr2 si0+si2 sr1+si1 si1+sr3 */
                __ASM_EMIT("subps   %%xmm1, %%xmm2")        /* xmm2 = dr2 di2 dr3 di1 = sr0-sr2 si0-si2 sr1-si3 si1-sr3 */

                /* Collect final values */
                __ASM_EMIT("movaps  %%xmm0, %%xmm1")        /* xmm1 = dr0 di0 dr1 di3 */
                __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = dr0 dr1 dr2 dr3 */
                __ASM_EMIT("shufps $0xdd, %%xmm2, %%xmm1")  /* xmm1 = di0 di3 di2 di1 */
                __ASM_EMIT("shufps $0x6c, %%xmm1, %%xmm1")  /* xmm1 = di0 di1 di2 di3 */

                /* Store values */
                __ASM_EMIT(LS_RE " %%xmm0, (%0)")
                __ASM_EMIT(LS_RE " %%xmm1, (%1)")

                /* Move pointers */
                __ASM_EMIT("add     $0x10, %0")
                __ASM_EMIT("add     $0x10, %1")

                /* Repeat cycle */
                __ASM_EMIT("dec     %2")
                __ASM_EMIT("jnz     1b")
                __ASM_EMIT("2:")
                : "+r"(dst_re), "+r"(dst_im), "+r"(items)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2"
            );
        }

        static inline void FFT_SCRAMBLE_COPY_DIRECT_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            size_t regs     = 1 << rank;

            for (size_t i=0; i<regs; ++i)
            {
                size_t index    = reverse_bits(FFT_TYPE(i), rank);

                __asm__ __volatile__
                (
                    /* Load scalar values */
                    __ASM_EMIT("movss   (%3, %2, 4), %%xmm1") /* xmm1 = r0 x x x */
                    __ASM_EMIT("movss   (%4, %2, 4), %%xmm2") /* xmm2 = i0 x x x */
                    __ASM_EMIT("add     %5, %2")

                    __ASM_EMIT("movss   (%3, %2, 4), %%xmm5") /* xmm5 = r2 x x x */
                    __ASM_EMIT("movss   (%4, %2, 4), %%xmm6") /* xmm4 = i2 x x x */
                    __ASM_EMIT("add     %5, %2")

                    __ASM_EMIT("movss   (%3, %2, 4), %%xmm0") /* xmm3 = r1 x x x */
                    __ASM_EMIT("movss   (%4, %2, 4), %%xmm3") /* xmm2 = i1 x x x */
                    __ASM_EMIT("add     %5, %2")

                    __ASM_EMIT("movss   (%3, %2, 4), %%xmm4") /* xmm7 = r3 x x x */
                    __ASM_EMIT("movss   (%4, %2, 4), %%xmm7") /* xmm6 = i3 x x x */

                    /* Perform 4-element butterfly */
                    /* Shuffle 1 */

                    __ASM_EMIT("movlhps %%xmm5, %%xmm1")        /* xmm1 = r0 x r2 x */
                    __ASM_EMIT("movlhps %%xmm4, %%xmm0")        /* xmm0 = r1 x r3 x */
                    __ASM_EMIT("movlhps %%xmm6, %%xmm2")        /* xmm2 = i0 x i2 x */
                    __ASM_EMIT("movlhps %%xmm7, %%xmm3")        /* xmm3 = i1 x i3 x */

                    __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm1")  /* xmm1 = r0 r2 i0 i2 */
                    __ASM_EMIT("shufps $0x88, %%xmm3, %%xmm0")  /* xmm0 = r1 r3 i1 i3 */
                    __ASM_EMIT("movaps %%xmm1, %%xmm2")         /* xmm2 = r0 r2 i0 i2 */

                    /* Transform 1 */
                    __ASM_EMIT("addps   %%xmm0, %%xmm1")        /* xmm1 = sr0 sr2 si0 si2 = r0+r1 r2+r3 i0+i1 i2+i3 */
                    __ASM_EMIT("subps   %%xmm0, %%xmm2")        /* xmm2 = sr1 sr3 si1 si3 = r0-r1 r2-r3 i0-i1 i2-i3 */

                    /* Shuffle 2 */
                    __ASM_EMIT("movaps  %%xmm1, %%xmm0")        /* xmm0 = sr0 sr2 si0 si2 */
                    __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = sr0 si0 sr1 si1 */
                    __ASM_EMIT("shufps $0x7d, %%xmm2, %%xmm1")  /* xmm1 = sr2 si2 si3 sr3 */
                    __ASM_EMIT("movaps %%xmm0, %%xmm2")         /* xmm2 = sr0 si0 sr1 si1 */

                    /* Transform 2 */
                    __ASM_EMIT("addps   %%xmm1, %%xmm0")        /* xmm0 = dr0 di0 dr1 di3 = sr0+sr2 si0+si2 sr1+si3 si1+sr3 */
                    __ASM_EMIT("subps   %%xmm1, %%xmm2")        /* xmm2 = dr2 di2 dr3 di1 = sr0-sr2 si0-si2 sr1-si3 si1-sr3 */

                    /* Collect final values */
                    __ASM_EMIT("movaps  %%xmm0, %%xmm1")        /* xmm1 = dr0 di0 dr1 di3 */
                    __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = dr0 dr1 dr2 dr3 */
                    __ASM_EMIT("shufps $0xdd, %%xmm2, %%xmm1")  /* xmm1 = di0 di3 di2 di1 */
                    __ASM_EMIT("shufps $0x6c, %%xmm1, %%xmm1")  /* xmm1 = di0 di1 di2 di3 */

                    /* Store values */
                    __ASM_EMIT(LS_RE " %%xmm0, (%0)")
                    __ASM_EMIT(LS_IM " %%xmm1, (%1)")

                    /* Update pointers */
                    __ASM_EMIT("add     $0x10, %0")
                    __ASM_EMIT("add     $0x10, %1")

                    : "+r" (dst_re), "+r"(dst_im), "+r"(index)
                    : "r"(src_re), "r"(src_im), "r"(regs)
                    : "cc", "memory",
                        "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                        "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                );
            }
        }

        static inline void FFT_SCRAMBLE_SELF_REVERSE_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Calculate number of items
            size_t items    = (1 << rank) - 1;

            dsp::move(dst_re, src_re, items);
            dsp::move(dst_im, src_im, items);

            for (size_t i = 1; i < items; ++i)
            {
                size_t j = reverse_bits(FFT_TYPE(i), rank);    /* Reverse the order of the bits */
                if (i >= j)
                    continue;

                /* Copy the values from the reversed position */
                __asm__ __volatile
                (
                    __ASM_EMIT("movss (%0, %2, 4), %%xmm0")
                    __ASM_EMIT("movss (%1, %2, 4), %%xmm1")
                    __ASM_EMIT("movss (%0, %3, 4), %%xmm2")
                    __ASM_EMIT("movss (%1, %3, 4), %%xmm3")
                    __ASM_EMIT("movss %%xmm2, (%0, %2, 4)")
                    __ASM_EMIT("movss %%xmm3, (%1, %2, 4)")
                    __ASM_EMIT("movss %%xmm0, (%0, %3, 4)")
                    __ASM_EMIT("movss %%xmm1, (%1, %3, 4)")
                    :
                    : "r"(dst_re), "r"(dst_im), "r"(i), "r"(j)
                    : "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3"
                );
            }

            // Perform butterfly 4x
            items = 1 << (rank - 2);

            // Perform 4-element butterflies
            __asm__ __volatile__
            (
                /* Prefetch data */
                __ASM_EMIT("prefetchnta (%0)")
                __ASM_EMIT("prefetchnta (%1)")
                /* Loop */
                __ASM_EMIT("1:")
                /* Prefetch data */
                __ASM_EMIT("prefetchnta 0x10(%0)")
                __ASM_EMIT("prefetchnta 0x10(%1)")

                /* Load data to registers */
                __ASM_EMIT(LS_RE " (%0), %%xmm0")          /* xmm0 = r0 r1 r2 r3 */
                __ASM_EMIT(LS_RE " (%1), %%xmm1")          /* xmm1 = i0 i1 i2 i3 */

                /* Shuffle 1 */
                __ASM_EMIT("movaps  %%xmm0, %%xmm2")        /* xmm2 = r0 r1 r2 r3 */
                __ASM_EMIT("shufps $0x88, %%xmm1, %%xmm2")  /* xmm2 = r0 r2 i0 i2 */
                __ASM_EMIT("shufps $0xdd, %%xmm1, %%xmm0")  /* xmm0 = r1 r3 i1 i3 */
                __ASM_EMIT("movaps  %%xmm2, %%xmm1")        /* xmm1 = r0 r2 i0 i2 */

                /* Transform 1 */
                __ASM_EMIT("addps   %%xmm0, %%xmm1")        /* xmm1 = sr0 sr2 si0 si2 = r0+r1 r2+r3 i0+i1 i2+i3 */
                __ASM_EMIT("subps   %%xmm0, %%xmm2")        /* xmm2 = sr1 sr3 si1 si3 = r0-r1 r2-r3 i0-i1 i2-i3 */

                /* Shuffle 2 */
                __ASM_EMIT("movaps  %%xmm1, %%xmm0")        /* xmm0 = sr0 sr2 si0 si2 */
                __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = sr0 si0 sr1 si1 */
                __ASM_EMIT("shufps $0x7d, %%xmm2, %%xmm1")  /* xmm1 = sr2 si2 si3 sr3 */
                __ASM_EMIT("movaps %%xmm0, %%xmm2")         /* xmm2 = sr0 si0 sr1 si1 */

                /* Transform 2 */
                __ASM_EMIT("addps   %%xmm1, %%xmm0")        /* xmm0 = dr0 di0 dr3 di1 = sr0+sr2 si0+si2 sr1+si3 si1+sr3 */
                __ASM_EMIT("subps   %%xmm1, %%xmm2")        /* xmm2 = dr2 di2 dr1 di3 = sr0-sr2 si0-si2 sr1-si3 si1-sr3 */

                /* Collect final values */
                __ASM_EMIT("movaps  %%xmm0, %%xmm1")        /* xmm1 = dr0 di0 dr3 di1 */
                __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = dr0 dr3 dr2 dr1 */
                __ASM_EMIT("shufps $0xdd, %%xmm2, %%xmm1")  /* xmm1 = di0 di1 di2 di3 */
                __ASM_EMIT("shufps $0x6c, %%xmm0, %%xmm0")  /* xmm0 = dr0 dr1 dr2 dr3 */

                /* Store values */
                __ASM_EMIT(LS_RE " %%xmm0, (%0)")
                __ASM_EMIT(LS_RE " %%xmm1, (%1)")

                /* Move pointers */
                __ASM_EMIT("add     $0x10, %0")
                __ASM_EMIT("add     $0x10, %1")

                /* Repeat cycle */
                __ASM_EMIT("dec     %2")
                __ASM_EMIT("jnz     1b")
                __ASM_EMIT("2:")
                : "+r"(dst_re), "+r"(dst_im), "+r"(items)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2"
            );
        }

        static inline void FFT_SCRAMBLE_COPY_REVERSE_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            size_t regs     = 1 << rank;

            for (size_t i=0; i<regs; ++i)
            {
                size_t index    = reverse_bits(FFT_TYPE(i), rank);

                __asm__ __volatile__
                (
                    /* Load scalar values */
                    __ASM_EMIT("movss   (%3, %2, 4), %%xmm1") /* xmm1 = r0 x x x */
                    __ASM_EMIT("movss   (%4, %2, 4), %%xmm2") /* xmm2 = i0 x x x */
                    __ASM_EMIT("add     %5, %2")

                    __ASM_EMIT("movss   (%3, %2, 4), %%xmm5") /* xmm5 = r2 x x x */
                    __ASM_EMIT("movss   (%4, %2, 4), %%xmm6") /* xmm4 = i2 x x x */
                    __ASM_EMIT("add     %5, %2")

                    __ASM_EMIT("movss   (%3, %2, 4), %%xmm0") /* xmm3 = r1 x x x */
                    __ASM_EMIT("movss   (%4, %2, 4), %%xmm3") /* xmm2 = i1 x x x */
                    __ASM_EMIT("add     %5, %2")

                    __ASM_EMIT("movss   (%3, %2, 4), %%xmm4") /* xmm7 = r3 x x x */
                    __ASM_EMIT("movss   (%4, %2, 4), %%xmm7") /* xmm6 = i3 x x x */

                    /* Perform 4-element butterfly */
                    /* Shuffle 1 */

                    __ASM_EMIT("movlhps %%xmm5, %%xmm1")        /* xmm1 = r0 x r2 x */
                    __ASM_EMIT("movlhps %%xmm4, %%xmm0")        /* xmm0 = r1 x r3 x */
                    __ASM_EMIT("movlhps %%xmm6, %%xmm2")        /* xmm2 = i0 x i2 x */
                    __ASM_EMIT("movlhps %%xmm7, %%xmm3")        /* xmm3 = i1 x i3 x */

                    __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm1")  /* xmm1 = r0 r2 i0 i2 */
                    __ASM_EMIT("shufps $0x88, %%xmm3, %%xmm0")  /* xmm0 = r1 r3 i1 i3 */
                    __ASM_EMIT("movaps %%xmm1, %%xmm2")         /* xmm2 = r0 r2 i0 i2 */

                    /* Transform 1 */
                    __ASM_EMIT("addps   %%xmm0, %%xmm1")        /* xmm1 = sr0 sr2 si0 si2 = r0+r1 r2+r3 i0+i1 i2+i3 */
                    __ASM_EMIT("subps   %%xmm0, %%xmm2")        /* xmm2 = sr1 sr3 si1 si3 = r0-r1 r2-r3 i0-i1 i2-i3 */

                    /* Shuffle 2 */
                    __ASM_EMIT("movaps  %%xmm1, %%xmm0")        /* xmm0 = sr0 sr2 si0 si2 */
                    __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = sr0 si0 sr1 si1 */
                    __ASM_EMIT("shufps $0x7d, %%xmm2, %%xmm1")  /* xmm1 = sr2 si2 si3 sr3 */
                    __ASM_EMIT("movaps %%xmm0, %%xmm2")         /* xmm2 = sr0 si0 sr1 si1 */

                    /* Transform 2 */
                    __ASM_EMIT("addps   %%xmm1, %%xmm0")        /* xmm0 = dr0 di0 dr3 di1 = sr0+sr2 si0+si2 sr1+si3 si1+sr3 */
                    __ASM_EMIT("subps   %%xmm1, %%xmm2")        /* xmm2 = dr2 di2 dr1 di3 = sr0-sr2 si0-si2 sr1-si3 si1-sr3 */

                    /* Collect final values */
                    __ASM_EMIT("movaps  %%xmm0, %%xmm1")        /* xmm1 = dr0 di0 dr3 di1 */
                    __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = dr0 dr3 dr2 dr1 */
                    __ASM_EMIT("shufps $0xdd, %%xmm2, %%xmm1")  /* xmm1 = di0 di1 di2 di3 */
                    __ASM_EMIT("shufps $0x6c, %%xmm0, %%xmm0")  /* xmm0 = dr0 dr1 dr2 dr3 */

                    /* Store values */
                    __ASM_EMIT(LS_RE " %%xmm0, (%0)")
                    __ASM_EMIT(LS_IM " %%xmm1, (%1)")

                    /* Update pointers */
                    __ASM_EMIT("add     $0x10, %0")
                    __ASM_EMIT("add     $0x10, %1")

                    : "+r" (dst_re), "+r"(dst_im), "+r"(index)
                    : "r"(src_re), "r"(src_im), "r"(regs)
                    : "cc", "memory",
                        "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                        "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                );
            }
        }
    }
} // namespace lsp

#undef FFT_SCRAMBLE_SELF_DIRECT_NAME
#undef FFT_SCRAMBLE_SELF_REVERSE_NAME
#undef FFT_SCRAMBLE_COPY_DIRECT_NAME
#undef FFT_SCRAMBLE_COPY_REVERSE_NAME
#undef FFT_TYPE
#undef LS_RE
#undef LS_IM

