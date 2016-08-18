/************************************************************************/
// 创建时间: 081123
// 最后确定: 081123

#include "../AnyPlay.h"
#include "wmp3decoder.h"
#pragma warning(disable: 4244)
#pragma warning(disable: 4018)
#pragma warning(disable: 4146)

namespace AnyPlay
{
 
#define OPT_ACCURACY


#define FPM_DEFAULT

 
#define HAVE_ASSERT_H 0

 
#define HAVE_ERRNO_H 0

 
#define HAVE_FCNTL_H 0

 
#define HAVE_LIMITS_H 0

 
#define HAVE_MEMORY_H 0

 
#define HAVE_STDINT_H 0

 
#define HAVE_STDLIB_H 1

 
#define HAVE_STRINGS_H 0

 
#define HAVE_STRING_H 0

 
#define HAVE_SYS_STAT_H 0

 
#define HAVE_SYS_TYPES_H 0

 
#define SIZEOF_INT 4

 
#define SIZEOF_LONG 4

 
#define SIZEOF_LONG_LONG 8

 
#define STDC_HEADERS 1


 
#define inline __inline



# if defined(OPT_SPEED) && defined(OPT_ACCURACY)
#  error "cannot optimize for both speed and accuracy"
# endif

# if defined(OPT_SPEED) && !defined(OPT_SSO)
#  define OPT_SSO
# endif

# if defined(HAVE_UNISTD_H) && defined(HAVE_WAITPID) &&  \
    defined(HAVE_FCNTL) && defined(HAVE_PIPE) && defined(HAVE_FORK)
#  define USE_ASYNC
# endif


# if !defined(HAVE_ASSERT_H)
#  if defined(NDEBUG)
#   define assert(x)	 
#  else
#   define assert(x)	do { if (!(x)) abort(); } while (0)
#  endif
# endif



#if SIZEOF_INT >= 4
typedef   signed int mad_fixed_t;

typedef   signed int mad_fixed64hi_t;
typedef unsigned int mad_fixed64lo_t;
# else
typedef   signed long mad_fixed_t;

typedef   signed long mad_fixed64hi_t;
typedef unsigned long mad_fixed64lo_t;
# endif

# if defined(_MSC_VER)
#  define mad_fixed64_t  signed __int64
# elif 1 || defined(__GNUC__)
#  define mad_fixed64_t  signed long long
# endif

# if defined(FPM_FLOAT)
typedef double mad_sample_t;
# else
typedef mad_fixed_t mad_sample_t;
# endif

 



# define mad_f_intpart(x)	((x) >> MAD_F_FRACBITS)
# define mad_f_fracpart(x)	((x) & ((1L << MAD_F_FRACBITS) - 1))
				 

# define mad_f_fromint(x)	((x) << MAD_F_FRACBITS)



# if defined(FPM_FLOAT)
#  error "FPM_FLOAT not yet supported"

#  undef MAD_F
#  define MAD_F(x)		mad_f_todouble(x)

#define mad_f_mul(x, y)	((x) * (y))
#define mad_f_scale64

#  undef ASO_ZEROCHECK

# elif defined(FPM_64BIT)

 
#  if defined(OPT_ACCURACY)
#   define mad_f_mul(x, y)  \
    ((mad_fixed_t)  \
     ((((mad_fixed64_t) (x) * (y)) +  \
       (1L << (MAD_F_SCALEBITS - 1))) >> MAD_F_SCALEBITS))
#  else
#   define mad_f_mul(x, y)  \
    ((mad_fixed_t) (((mad_fixed64_t) (x) * (y)) >> MAD_F_SCALEBITS))
#  endif

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

 

# elif defined(FPM_INTEL)

#  if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4035)   
static __forceinline
mad_fixed_t mad_f_mul_inline(mad_fixed_t x, mad_fixed_t y)
{
  enum {
    fracbits = MAD_F_FRACBITS
  };

  __asm {
    mov eax, x
    imul y
    shrd eax, edx, fracbits
  }

   
}
#   pragma warning(pop)

#   define mad_f_mul		mad_f_mul_inline
#   define mad_f_scale64
#  else
 
#   define MAD_F_MLX(hi, lo, x, y)  \
    asm ("imull %3"  \
	 : "=a" (lo), "=d" (hi)  \
	 : "%a" (x), "rm" (y)  \
	 : "cc")

#   if defined(OPT_ACCURACY)
 
#    define MAD_F_MLA(hi, lo, x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       MAD_F_MLX(__hi, __lo, (x), (y));  \
       asm ("addl %2,%0\n\t"  \
	    "adcl %3,%1"  \
	    : "=rm" (lo), "=rm" (hi)  \
	    : "r" (__lo), "r" (__hi), "0" (lo), "1" (hi)  \
	    : "cc");  \
    })
#   endif   

#   if defined(OPT_ACCURACY)
 
#    define mad_f_scale64(hi, lo)  \
    ({ mad_fixed64hi_t __hi_;  \
       mad_fixed64lo_t __lo_;  \
       mad_fixed_t __result;  \
       asm ("addl %4,%2\n\t"  \
	    "adcl %5,%3"  \
	    : "=rm" (__lo_), "=rm" (__hi_)  \
	    : "0" (lo), "1" (hi),  \
	      "ir" (1L << (MAD_F_SCALEBITS - 1)), "ir" (0)  \
	    : "cc");  \
       asm ("shrdl %3,%2,%1"  \
	    : "=rm" (__result)  \
	    : "0" (__lo_), "r" (__hi_), "I" (MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })
#   elif defined(OPT_INTEL)
 
#    define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result;  \
       asm ("shrl %3,%1\n\t"  \
	    "shll %4,%2\n\t"  \
	    "orl %2,%1"  \
	    : "=rm" (__result)  \
	    : "0" (lo), "r" (hi),  \
	      "I" (MAD_F_SCALEBITS), "I" (32 - MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })
#   else
#    define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result;  \
       asm ("shrdl %3,%2,%1"  \
	    : "=rm" (__result)  \
	    : "0" (lo), "r" (hi), "I" (MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })
#   endif   

#   define MAD_F_SCALEBITS  MAD_F_FRACBITS
#  endif

 

# elif defined(FPM_ARM)

 
# if 1
 
#  define mad_f_mul(x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       mad_fixed_t __result;  \
       asm ("smull	%0, %1, %3, %4\n\t"  \
	    "movs	%0, %0, lsr %5\n\t"  \
	    "adc	%2, %0, %1, lsl %6"  \
	    : "=&r" (__lo), "=&r" (__hi), "=r" (__result)  \
	    : "%r" (x), "r" (y),  \
	      "M" (MAD_F_SCALEBITS), "M" (32 - MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })
# endif

#  define MAD_F_MLX(hi, lo, x, y)  \
    asm ("smull	%0, %1, %2, %3"  \
	 : "=&r" (lo), "=&r" (hi)  \
	 : "%r" (x), "r" (y))

#  define MAD_F_MLA(hi, lo, x, y)  \
    asm ("smlal	%0, %1, %2, %3"  \
	 : "+r" (lo), "+r" (hi)  \
	 : "%r" (x), "r" (y))

#  define MAD_F_MLN(hi, lo)  \
    asm ("rsbs	%0, %2, #0\n\t"  \
	 "rsc	%1, %3, #0"  \
	 : "=r" (lo), "=r" (hi)  \
	 : "0" (lo), "1" (hi)  \
	 : "cc")

#  define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result;  \
       asm ("movs	%0, %1, lsr %3\n\t"  \
	    "adc	%0, %0, %2, lsl %4"  \
	    : "=&r" (__result)  \
	    : "r" (lo), "r" (hi),  \
	      "M" (MAD_F_SCALEBITS), "M" (32 - MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

 

# elif defined(FPM_MIPS)

 
#  define MAD_F_MLX(hi, lo, x, y)  \
    asm ("mult	%2,%3"  \
	 : "=l" (lo), "=h" (hi)  \
	 : "%r" (x), "r" (y))

# if defined(HAVE_MADD_ASM)
#  define MAD_F_MLA(hi, lo, x, y)  \
    asm ("madd	%2,%3"  \
	 : "+l" (lo), "+h" (hi)  \
	 : "%r" (x), "r" (y))
# elif defined(HAVE_MADD16_ASM)
 
#  define MAD_F_ML0(hi, lo, x, y)  \
    asm ("mult	%2,%3"  \
	 : "=l" (lo), "=h" (hi)  \
	 : "%r" ((x) >> 12), "r" ((y) >> 16))
#  define MAD_F_MLA(hi, lo, x, y)  \
    asm ("madd16	%2,%3"  \
	 : "+l" (lo), "+h" (hi)  \
	 : "%r" ((x) >> 12), "r" ((y) >> 16))
#  define MAD_F_MLZ(hi, lo)  ((mad_fixed_t) (lo))
# endif

# if defined(OPT_SPEED)
#  define mad_f_scale64(hi, lo)  \
    ((mad_fixed_t) ((hi) << (32 - MAD_F_SCALEBITS)))
#  define MAD_F_SCALEBITS  MAD_F_FRACBITS
# endif

 

# elif defined(FPM_SPARC)

 
#  define MAD_F_MLX(hi, lo, x, y)  \
    asm ("smul %2, %3, %0\n\t"  \
	 "rd %%y, %1"  \
	 : "=r" (lo), "=r" (hi)  \
	 : "%r" (x), "rI" (y))

 

# elif defined(FPM_PPC)

 
#  define MAD_F_MLX(hi, lo, x, y)  \
    do {  \
      asm ("mullw %0,%1,%2"  \
	   : "=r" (lo)  \
	   : "%r" (x), "r" (y));  \
      asm ("mulhw %0,%1,%2"  \
	   : "=r" (hi)  \
	   : "%r" (x), "r" (y));  \
    }  \
    while (0)

#  if defined(OPT_ACCURACY)
 
#   define MAD_F_MLA(hi, lo, x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       MAD_F_MLX(__hi, __lo, (x), (y));  \
       asm ("addc %0,%2,%3\n\t"  \
	    "adde %1,%4,%5"  \
	    : "=r" (lo), "=r" (hi)  \
	    : "%r" (lo), "r" (__lo),  \
	      "%r" (hi), "r" (__hi)  \
	    : "xer");  \
    })
#  endif

#  if defined(OPT_ACCURACY)
 
#   define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result, __round;  \
       asm ("rotrwi %0,%1,%2"  \
	    : "=r" (__result)  \
	    : "r" (lo), "i" (MAD_F_SCALEBITS));  \
       asm ("extrwi %0,%1,1,0"  \
	    : "=r" (__round)  \
	    : "r" (__result));  \
       asm ("insrwi %0,%1,%2,0"  \
	    : "+r" (__result)  \
	    : "r" (hi), "i" (MAD_F_SCALEBITS));  \
       asm ("add %0,%1,%2"  \
	    : "=r" (__result)  \
	    : "%r" (__result), "r" (__round));  \
       __result;  \
    })
#  else
#   define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result;  \
       asm ("rotrwi %0,%1,%2"  \
	    : "=r" (__result)  \
	    : "r" (lo), "i" (MAD_F_SCALEBITS));  \
       asm ("insrwi %0,%1,%2,0"  \
	    : "+r" (__result)  \
	    : "r" (hi), "i" (MAD_F_SCALEBITS));  \
       __result;  \
    })
#  endif
 
#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

 

# elif defined(FPM_DEFAULT)

 
#  if defined(OPT_SPEED)
#   define mad_f_mul(x, y)	(((x) >> 12) * ((y) >> 16))
#  else
#   define mad_f_mul(x, y)	((((x) + (1L << 11)) >> 12) *  \
				 (((y) + (1L << 15)) >> 16))
#  endif

 

# else
#  error "no FPM selected"
# endif

 

# if !defined(mad_f_mul)
#  define mad_f_mul(x, y)  \
    ({ register mad_fixed64hi_t __hi;  \
       register mad_fixed64lo_t __lo;  \
       MAD_F_MLX(__hi, __lo, (x), (y));  \
       mad_f_scale64(__hi, __lo);  \
    })
# endif

# if !defined(MAD_F_MLA)
#  define MAD_F_ML0(hi, lo, x, y)	((lo)  = mad_f_mul((x), (y)))
#  define MAD_F_MLA(hi, lo, x, y)	((lo) += mad_f_mul((x), (y)))
#  define MAD_F_MLN(hi, lo)		((lo)  = -(lo))
#  define MAD_F_MLZ(hi, lo)		((void) (hi), (mad_fixed_t) (lo))
# endif

# if !defined(MAD_F_ML0)
#  define MAD_F_ML0(hi, lo, x, y)	MAD_F_MLX((hi), (lo), (x), (y))
# endif

# if !defined(MAD_F_MLN)
#  define MAD_F_MLN(hi, lo)		((hi) = ((lo) = -(lo)) ? ~(hi) : -(hi))
# endif

# if !defined(MAD_F_MLZ)
#  define MAD_F_MLZ(hi, lo)		mad_f_scale64((hi), (lo))
# endif

# if !defined(mad_f_scale64)
#  if defined(OPT_ACCURACY)
#   define mad_f_scale64(hi, lo)  \
    ((((mad_fixed_t)  \
       (((hi) << (32 - (MAD_F_SCALEBITS - 1))) |  \
	((lo) >> (MAD_F_SCALEBITS - 1)))) + 1) >> 1)
#  else
#   define mad_f_scale64(hi, lo)  \
    ((mad_fixed_t)  \
     (((hi) << (32 - MAD_F_SCALEBITS)) |  \
      ((lo) >> MAD_F_SCALEBITS)))
#  endif
#  define MAD_F_SCALEBITS  MAD_F_FRACBITS
# endif

 

mad_fixed_t mad_f_abs(mad_fixed_t);
mad_fixed_t mad_f_div(mad_fixed_t, mad_fixed_t);



 




 




















 

 


# define MAD_VERSION_MAJOR	0
# define MAD_VERSION_MINOR	15
# define MAD_VERSION_PATCH	1
# define MAD_VERSION_EXTRA	" (beta)"

# define MAD_VERSION_STRINGIZE(str)	#str
# define MAD_VERSION_STRING(num)	MAD_VERSION_STRINGIZE(num)

# define MAD_VERSION		MAD_VERSION_STRING(MAD_VERSION_MAJOR) "."  \
				MAD_VERSION_STRING(MAD_VERSION_MINOR) "."  \
				MAD_VERSION_STRING(MAD_VERSION_PATCH)  \
				MAD_VERSION_EXTRA

# define MAD_PUBLISHYEAR	"2000-2004"
# define MAD_AUTHOR		"Underbit Technologies, Inc."
# define MAD_EMAIL		"info@underbit.com"
  

 


 












# define MAD_NCHANNELS(header)		((header)->mode ? 2 : 1)






enum {
  MAD_PRIVATE_HEADER	= 0x0100,	 
  MAD_PRIVATE_III	= 0x001f	 
};






 

 
int mad_layer_I(struct mad_stream *, struct mad_frame *);
int mad_layer_II(struct mad_stream *, struct mad_frame *);

 

 




 
enum {
  MAD_PCM_CHANNEL_SINGLE = 0
};

 
enum {
  MAD_PCM_CHANNEL_DUAL_1 = 0,
  MAD_PCM_CHANNEL_DUAL_2 = 1
};

 
enum {
  MAD_PCM_CHANNEL_STEREO_LEFT  = 0,
  MAD_PCM_CHANNEL_STEREO_RIGHT = 1
};

 

 





 
union huffquad {
  struct {
    unsigned short final  :  1;
    unsigned short bits   :  3;
    unsigned short offset : 12;
  } ptr;
  struct {
    unsigned short final  :  1;
    unsigned short hlen   :  3;
    unsigned short v      :  1;
    unsigned short w      :  1;
    unsigned short x      :  1;
    unsigned short y      :  1;
  } value;
  unsigned short final    :  1;
};

union huffpair {
  struct {
    unsigned short final  :  1;
    unsigned short bits   :  3;
    unsigned short offset : 12;
  } ptr;
  struct {
    unsigned short final  :  1;
    unsigned short hlen   :  3;
    unsigned short x      :  4;
    unsigned short y      :  4;
  } value;
  unsigned short final    :  1;
};

struct hufftable {
  union huffpair const *table;
  unsigned short linbits;
  unsigned short startbits;
};

  


 

 
int mad_layer_III(struct mad_stream *, struct mad_frame *);






# if defined(_MSC_VER)
#  define mad_fixed64_t  signed __int64
# elif 1 || defined(__GNUC__)
#  define mad_fixed64_t  signed long long
# endif


# if defined(FPM_FLOAT)
typedef double mad_sample_t;
# else
typedef mad_fixed_t mad_sample_t;
# endif



# if MAD_F_FRACBITS == 28
#  define MAD_F(x)		((mad_fixed_t) (x##L))
# else
#  if MAD_F_FRACBITS < 28
#   warning "MAD_F_FRACBITS < 28"
#   define MAD_F(x)		((mad_fixed_t)  \
				 (((x##L) +  \
				   (1L << (28 - MAD_F_FRACBITS - 1))) >>  \
				  (28 - MAD_F_FRACBITS)))
#  elif MAD_F_FRACBITS > 28
#   error "MAD_F_FRACBITS > 28 not currently supported"
#   define MAD_F(x)		((mad_fixed_t)  \
				 ((x##L) << (MAD_F_FRACBITS - 28)))
#  endif
# endif

# define MAD_F_MIN		((mad_fixed_t) -0x80000000L)
# define MAD_F_MAX		((mad_fixed_t) +0x7fffffffL)


# define mad_f_tofixed(x)	((mad_fixed_t)  \
				 ((x) * (double) (1L << MAD_F_FRACBITS) + 0.5))
# define mad_f_todouble(x)	((double)  \
				 ((x) / (double) (1L << MAD_F_FRACBITS)))

# define mad_f_intpart(x)	((x) >> MAD_F_FRACBITS)
# define mad_f_fracpart(x)	((x) & ((1L << MAD_F_FRACBITS) - 1))
				 

# define mad_f_fromint(x)	((x) << MAD_F_FRACBITS)

# define mad_f_add(x, y)	((x) + (y))
# define mad_f_sub(x, y)	((x) - (y))

# if defined(FPM_FLOAT)
#  error "FPM_FLOAT not yet supported"

#  undef MAD_F
#  define MAD_F(x)		mad_f_todouble(x)

#  define mad_f_mul(x, y)	((x) * (y))
#  define mad_f_scale64

#  undef ASO_ZEROCHECK

# elif defined(FPM_64BIT)

 
 
#  if defined(OPT_ACCURACY)
#   define mad_f_mul(x, y)  \
    ((mad_fixed_t)  \
     ((((mad_fixed64_t) (x) * (y)) +  \
       (1L << (MAD_F_SCALEBITS - 1))) >> MAD_F_SCALEBITS))
#  else
#   define mad_f_mul(x, y)  \
    ((mad_fixed_t) (((mad_fixed64_t) (x) * (y)) >> MAD_F_SCALEBITS))
#  endif

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

 

# elif defined(FPM_INTEL)

#  if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4035)   


#   pragma warning(pop)

#   define mad_f_mul		mad_f_mul_inline
#   define mad_f_scale64
#  else
 
#   define MAD_F_MLX(hi, lo, x, y)  \
    asm ("imull %3"  \
	 : "=a" (lo), "=d" (hi)  \
	 : "%a" (x), "rm" (y)  \
	 : "cc")

#   if defined(OPT_ACCURACY)
 
#    define MAD_F_MLA(hi, lo, x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       MAD_F_MLX(__hi, __lo, (x), (y));  \
       asm ("addl %2,%0\n\t"  \
	    "adcl %3,%1"  \
	    : "=rm" (lo), "=rm" (hi)  \
	    : "r" (__lo), "r" (__hi), "0" (lo), "1" (hi)  \
	    : "cc");  \
    })
#   endif   

#   if defined(OPT_ACCURACY)
 
#    define mad_f_scale64(hi, lo)  \
    ({ mad_fixed64hi_t __hi_;  \
       mad_fixed64lo_t __lo_;  \
       mad_fixed_t __result;  \
       asm ("addl %4,%2\n\t"  \
	    "adcl %5,%3"  \
	    : "=rm" (__lo_), "=rm" (__hi_)  \
	    : "0" (lo), "1" (hi),  \
	      "ir" (1L << (MAD_F_SCALEBITS - 1)), "ir" (0)  \
	    : "cc");  \
       asm ("shrdl %3,%2,%1"  \
	    : "=rm" (__result)  \
	    : "0" (__lo_), "r" (__hi_), "I" (MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })
#    else
#    define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result;  \
       asm ("shrdl %3,%2,%1"  \
	    : "=rm" (__result)  \
	    : "0" (lo), "r" (hi), "I" (MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })
#   endif   

#   define MAD_F_SCALEBITS  MAD_F_FRACBITS
#  endif

 

# elif defined(FPM_ARM)

 
# if 1
 
#  define mad_f_mul(x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       mad_fixed_t __result;  \
       asm ("smull	%0, %1, %3, %4\n\t"  \
	    "movs	%0, %0, lsr %5\n\t"  \
	    "adc	%2, %0, %1, lsl %6"  \
	    : "=&r" (__lo), "=&r" (__hi), "=r" (__result)  \
	    : "%r" (x), "r" (y),  \
	      "M" (MAD_F_SCALEBITS), "M" (32 - MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })
# endif

#  define MAD_F_MLX(hi, lo, x, y)  \
    asm ("smull	%0, %1, %2, %3"  \
	 : "=&r" (lo), "=&r" (hi)  \
	 : "%r" (x), "r" (y))

#  define MAD_F_MLA(hi, lo, x, y)  \
    asm ("smlal	%0, %1, %2, %3"  \
	 : "+r" (lo), "+r" (hi)  \
	 : "%r" (x), "r" (y))

#  define MAD_F_MLN(hi, lo)  \
    asm ("rsbs	%0, %2, #0\n\t"  \
	 "rsc	%1, %3, #0"  \
	 : "=r" (lo), "=r" (hi)  \
	 : "0" (lo), "1" (hi)  \
	 : "cc")

#  define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result;  \
       asm ("movs	%0, %1, lsr %3\n\t"  \
	    "adc	%0, %0, %2, lsl %4"  \
	    : "=&r" (__result)  \
	    : "r" (lo), "r" (hi),  \
	      "M" (MAD_F_SCALEBITS), "M" (32 - MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

 

# elif defined(FPM_MIPS)

 
#  define MAD_F_MLX(hi, lo, x, y)  \
    asm ("mult	%2,%3"  \
	 : "=l" (lo), "=h" (hi)  \
	 : "%r" (x), "r" (y))

# if defined(HAVE_MADD_ASM)
#  define MAD_F_MLA(hi, lo, x, y)  \
    asm ("madd	%2,%3"  \
	 : "+l" (lo), "+h" (hi)  \
	 : "%r" (x), "r" (y))
# elif defined(HAVE_MADD16_ASM)
 
#  define MAD_F_ML0(hi, lo, x, y)  \
    asm ("mult	%2,%3"  \
	 : "=l" (lo), "=h" (hi)  \
	 : "%r" ((x) >> 12), "r" ((y) >> 16))
#  define MAD_F_MLA(hi, lo, x, y)  \
    asm ("madd16	%2,%3"  \
	 : "+l" (lo), "+h" (hi)  \
	 : "%r" ((x) >> 12), "r" ((y) >> 16))
#  define MAD_F_MLZ(hi, lo)  ((mad_fixed_t) (lo))
# endif

# if defined(OPT_SPEED)
#  define mad_f_scale64(hi, lo)  \
    ((mad_fixed_t) ((hi) << (32 - MAD_F_SCALEBITS)))
#  define MAD_F_SCALEBITS  MAD_F_FRACBITS
# endif

 

# elif defined(FPM_SPARC)

 
#  define MAD_F_MLX(hi, lo, x, y)  \
    asm ("smul %2, %3, %0\n\t"  \
	 "rd %%y, %1"  \
	 : "=r" (lo), "=r" (hi)  \
	 : "%r" (x), "rI" (y))

 

# elif defined(FPM_PPC)

 
#  define MAD_F_MLX(hi, lo, x, y)  \
    do {  \
      asm ("mullw %0,%1,%2"  \
	   : "=r" (lo)  \
	   : "%r" (x), "r" (y));  \
      asm ("mulhw %0,%1,%2"  \
	   : "=r" (hi)  \
	   : "%r" (x), "r" (y));  \
    }  \
    while (0)

#  if defined(OPT_ACCURACY)
 
#   define MAD_F_MLA(hi, lo, x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       MAD_F_MLX(__hi, __lo, (x), (y));  \
       asm ("addc %0,%2,%3\n\t"  \
	    "adde %1,%4,%5"  \
	    : "=r" (lo), "=r" (hi)  \
	    : "%r" (lo), "r" (__lo),  \
	      "%r" (hi), "r" (__hi)  \
	    : "xer");  \
    })
#  endif

#  if defined(OPT_ACCURACY)
 
#   define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result, __round;  \
       asm ("rotrwi %0,%1,%2"  \
	    : "=r" (__result)  \
	    : "r" (lo), "i" (MAD_F_SCALEBITS));  \
       asm ("extrwi %0,%1,1,0"  \
	    : "=r" (__round)  \
	    : "r" (__result));  \
       asm ("insrwi %0,%1,%2,0"  \
	    : "+r" (__result)  \
	    : "r" (hi), "i" (MAD_F_SCALEBITS));  \
       asm ("add %0,%1,%2"  \
	    : "=r" (__result)  \
	    : "%r" (__result), "r" (__round));  \
       __result;  \
    })
#  else
#   define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result;  \
       asm ("rotrwi %0,%1,%2"  \
	    : "=r" (__result)  \
	    : "r" (lo), "i" (MAD_F_SCALEBITS));  \
       asm ("insrwi %0,%1,%2,0"  \
	    : "+r" (__result)  \
	    : "r" (hi), "i" (MAD_F_SCALEBITS));  \
       __result;  \
    })
#  endif

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

 

# elif defined(FPM_DEFAULT)

 
#  if defined(OPT_SPEED)
#   define mad_f_mul(x, y)	(((x) >> 12) * ((y) >> 16))
#  else
#   define mad_f_mul(x, y)	((((x) + (1L << 11)) >> 12) *  \
				 (((y) + (1L << 15)) >> 16))
#  endif

 

# else
#  error "no FPM selected"
# endif

 

# if !defined(mad_f_mul)
#  define mad_f_mul(x, y)  \
    ({ register mad_fixed64hi_t __hi;  \
       register mad_fixed64lo_t __lo;  \
       MAD_F_MLX(__hi, __lo, (x), (y));  \
       mad_f_scale64(__hi, __lo);  \
    })
# endif

# if !defined(MAD_F_MLA)
#  define MAD_F_ML0(hi, lo, x, y)	((lo)  = mad_f_mul((x), (y)))
#  define MAD_F_MLA(hi, lo, x, y)	((lo) += mad_f_mul((x), (y)))
#  define MAD_F_MLN(hi, lo)		((lo)  = -(lo))
#  define MAD_F_MLZ(hi, lo)		((void) (hi), (mad_fixed_t) (lo))
# endif

# if !defined(MAD_F_ML0)
#  define MAD_F_ML0(hi, lo, x, y)	MAD_F_MLX((hi), (lo), (x), (y))
# endif

# if !defined(MAD_F_MLN)
#  define MAD_F_MLN(hi, lo)		((hi) = ((lo) = -(lo)) ? ~(hi) : -(hi))
# endif

# if !defined(MAD_F_MLZ)
#  define MAD_F_MLZ(hi, lo)		mad_f_scale64((hi), (lo))
# endif

# if !defined(mad_f_scale64)
#  if defined(OPT_ACCURACY)
#   define mad_f_scale64(hi, lo)  \
    ((((mad_fixed_t)  \
       (((hi) << (32 - (MAD_F_SCALEBITS - 1))) |  \
	((lo) >> (MAD_F_SCALEBITS - 1)))) + 1) >> 1)
#  else
#   define mad_f_scale64(hi, lo)  \
    ((mad_fixed_t)  \
     (((hi) << (32 - MAD_F_SCALEBITS)) |  \
      ((lo) >> MAD_F_SCALEBITS)))
#  endif
#  define MAD_F_SCALEBITS  MAD_F_FRACBITS
# endif


 

mad_fixed_t mad_f_abs(mad_fixed_t);
mad_fixed_t mad_f_div(mad_fixed_t, mad_fixed_t);







 

# ifndef LIBMAD_DECODER_H
# define LIBMAD_DECODER_H





int mad_decoder_finish(struct mad_decoder *);

# define mad_decoder_options(decoder, opts)  \
    ((void) ((decoder)->options = (opts)))


int mad_decoder_message(struct mad_decoder *, void *, unsigned int *);

# endif


 



 

# ifdef HAVE_LIMITS_H
#  include <limits.h>
# else
#  define CHAR_BIT  8
# endif



 
static
unsigned short const crc_table[256] = {
  0x0000, 0x8005, 0x800f, 0x000a, 0x801b, 0x001e, 0x0014, 0x8011,
  0x8033, 0x0036, 0x003c, 0x8039, 0x0028, 0x802d, 0x8027, 0x0022,
  0x8063, 0x0066, 0x006c, 0x8069, 0x0078, 0x807d, 0x8077, 0x0072,
  0x0050, 0x8055, 0x805f, 0x005a, 0x804b, 0x004e, 0x0044, 0x8041,
  0x80c3, 0x00c6, 0x00cc, 0x80c9, 0x00d8, 0x80dd, 0x80d7, 0x00d2,
  0x00f0, 0x80f5, 0x80ff, 0x00fa, 0x80eb, 0x00ee, 0x00e4, 0x80e1,
  0x00a0, 0x80a5, 0x80af, 0x00aa, 0x80bb, 0x00be, 0x00b4, 0x80b1,
  0x8093, 0x0096, 0x009c, 0x8099, 0x0088, 0x808d, 0x8087, 0x0082,

  0x8183, 0x0186, 0x018c, 0x8189, 0x0198, 0x819d, 0x8197, 0x0192,
  0x01b0, 0x81b5, 0x81bf, 0x01ba, 0x81ab, 0x01ae, 0x01a4, 0x81a1,
  0x01e0, 0x81e5, 0x81ef, 0x01ea, 0x81fb, 0x01fe, 0x01f4, 0x81f1,
  0x81d3, 0x01d6, 0x01dc, 0x81d9, 0x01c8, 0x81cd, 0x81c7, 0x01c2,
  0x0140, 0x8145, 0x814f, 0x014a, 0x815b, 0x015e, 0x0154, 0x8151,
  0x8173, 0x0176, 0x017c, 0x8179, 0x0168, 0x816d, 0x8167, 0x0162,
  0x8123, 0x0126, 0x012c, 0x8129, 0x0138, 0x813d, 0x8137, 0x0132,
  0x0110, 0x8115, 0x811f, 0x011a, 0x810b, 0x010e, 0x0104, 0x8101,

  0x8303, 0x0306, 0x030c, 0x8309, 0x0318, 0x831d, 0x8317, 0x0312,
  0x0330, 0x8335, 0x833f, 0x033a, 0x832b, 0x032e, 0x0324, 0x8321,
  0x0360, 0x8365, 0x836f, 0x036a, 0x837b, 0x037e, 0x0374, 0x8371,
  0x8353, 0x0356, 0x035c, 0x8359, 0x0348, 0x834d, 0x8347, 0x0342,
  0x03c0, 0x83c5, 0x83cf, 0x03ca, 0x83db, 0x03de, 0x03d4, 0x83d1,
  0x83f3, 0x03f6, 0x03fc, 0x83f9, 0x03e8, 0x83ed, 0x83e7, 0x03e2,
  0x83a3, 0x03a6, 0x03ac, 0x83a9, 0x03b8, 0x83bd, 0x83b7, 0x03b2,
  0x0390, 0x8395, 0x839f, 0x039a, 0x838b, 0x038e, 0x0384, 0x8381,

  0x0280, 0x8285, 0x828f, 0x028a, 0x829b, 0x029e, 0x0294, 0x8291,
  0x82b3, 0x02b6, 0x02bc, 0x82b9, 0x02a8, 0x82ad, 0x82a7, 0x02a2,
  0x82e3, 0x02e6, 0x02ec, 0x82e9, 0x02f8, 0x82fd, 0x82f7, 0x02f2,
  0x02d0, 0x82d5, 0x82df, 0x02da, 0x82cb, 0x02ce, 0x02c4, 0x82c1,
  0x8243, 0x0246, 0x024c, 0x8249, 0x0258, 0x825d, 0x8257, 0x0252,
  0x0270, 0x8275, 0x827f, 0x027a, 0x826b, 0x026e, 0x0264, 0x8261,
  0x0220, 0x8225, 0x822f, 0x022a, 0x823b, 0x023e, 0x0234, 0x8231,
  0x8213, 0x0216, 0x021c, 0x8219, 0x0208, 0x820d, 0x8207, 0x0202
};

# define CRC_POLY  0x8005

 
void mad_bit_init(struct mad_bitptr *bitptr, unsigned char const *byte)
{
  bitptr->byte  = byte;
  bitptr->cache = 0;
  bitptr->left  = CHAR_BIT;
}

 
unsigned int mad_bit_length(struct mad_bitptr const *begin,
			    struct mad_bitptr const *end)
{
  return begin->left +
    CHAR_BIT * (end->byte - (begin->byte + 1)) + (CHAR_BIT - end->left);
}

 
unsigned char const *mad_bit_nextbyte(struct mad_bitptr const *bitptr)
{
  return bitptr->left == CHAR_BIT ? bitptr->byte : bitptr->byte + 1;
}

 
void mad_bit_skip(struct mad_bitptr *bitptr, unsigned int len)
{
  bitptr->byte += len / CHAR_BIT;
  bitptr->left -= len % CHAR_BIT;

  if (bitptr->left > CHAR_BIT) {
    bitptr->byte++;
    bitptr->left += CHAR_BIT;
  }

  if (bitptr->left < CHAR_BIT)
    bitptr->cache = *bitptr->byte;
}

 
unsigned long mad_bit_read(struct mad_bitptr *bitptr, unsigned int len)
{
  register unsigned long value;

  if (bitptr->left == CHAR_BIT)
    bitptr->cache = *bitptr->byte;

  if (len < bitptr->left) {
    value = (bitptr->cache & ((1 << bitptr->left) - 1)) >>
      (bitptr->left - len);
    bitptr->left -= len;

    return value;
  }

   

  value = bitptr->cache & ((1 << bitptr->left) - 1);
  len  -= bitptr->left;

  bitptr->byte++;
  bitptr->left = CHAR_BIT;

   

  while (len >= CHAR_BIT) {
    value = (value << CHAR_BIT) | *bitptr->byte++;
    len  -= CHAR_BIT;
  }

  if (len > 0) {
    bitptr->cache = *bitptr->byte;

    value = (value << len) | (bitptr->cache >> (CHAR_BIT - len));
    bitptr->left -= len;
  }

  return value;
}

# if 0
 
void mad_bit_write(struct mad_bitptr *bitptr, unsigned int len,
		   unsigned long value)
{
  unsigned char *ptr;

  ptr = (unsigned char *) bitptr->byte;

   
}
# endif

 
unsigned short mad_bit_crc(struct mad_bitptr bitptr, unsigned int len,
			   unsigned short init)
{
  register unsigned int crc;

  for (crc = init; len >= 32; len -= 32) {
    register unsigned long data;

    data = mad_bit_read(&bitptr, 32);

    crc = (crc << 8) ^ crc_table[((crc >> 8) ^ (data >> 24)) & 0xff];
    crc = (crc << 8) ^ crc_table[((crc >> 8) ^ (data >> 16)) & 0xff];
    crc = (crc << 8) ^ crc_table[((crc >> 8) ^ (data >>  8)) & 0xff];
    crc = (crc << 8) ^ crc_table[((crc >> 8) ^ (data >>  0)) & 0xff];
  }

  switch (len / 8) {
  case 3: crc = (crc << 8) ^
	    crc_table[((crc >> 8) ^ mad_bit_read(&bitptr, 8)) & 0xff];
  case 2: crc = (crc << 8) ^
	    crc_table[((crc >> 8) ^ mad_bit_read(&bitptr, 8)) & 0xff];
  case 1: crc = (crc << 8) ^
	    crc_table[((crc >> 8) ^ mad_bit_read(&bitptr, 8)) & 0xff];

  len %= 8;

  case 0: break;
  }

  while (len--) {
    register unsigned int msb;

    msb = mad_bit_read(&bitptr, 1) ^ (crc >> 15);

    crc <<= 1;
    if (msb & 1)
      crc ^= CRC_POLY;
  }

  return crc & 0xffff;
}



 





 # if defined(__GNUC__) ||  \
    (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901)
 #  define PTR(offs, bits)	{ { 0, bits, offs } }
 #  if defined(WORDS_BIGENDIAN)
#   define V(v, w, x, y, hlen)	{ { 1, hlen, (v << 11) | (w << 10) |  \
                                             (x <<  9) | (y <<  8) } }
#  else
#   define V(v, w, x, y, hlen)	{ { 1, hlen, (v <<  0) | (w <<  1) |  \
                                             (x <<  2) | (y <<  3) } }
#  endif
# else
#  define PTR(offs, bits)	{ { 0, bits, offs } }
#  if defined(WORDS_BIGENDIAN)
#   define V(v, w, x, y, hlen)	{ { 1, hlen, (v << 11) | (w << 10) |  \
                                             (x <<  9) | (y <<  8) } }
#  else
#   define V(v, w, x, y, hlen)	{ { 1, hlen, (v <<  0) | (w <<  1) |  \
                                             (x <<  2) | (y <<  3) } }
#  endif
# endif

static
union huffquad const hufftabA[] = {
    PTR(16, 2),
    PTR(20, 2),
    PTR(24, 1),
    PTR(26, 1),
    V(0, 0, 1, 0, 4),
    V(0, 0, 0, 1, 4),
    V(0, 1, 0, 0, 4),
    V(1, 0, 0, 0, 4),
    V(0, 0, 0, 0, 1),
    V(0, 0, 0, 0, 1),
    V(0, 0, 0, 0, 1),
    V(0, 0, 0, 0, 1),
    V(0, 0, 0, 0, 1),
    V(0, 0, 0, 0, 1),
    V(0, 0, 0, 0, 1),
    V(0, 0, 0, 0, 1),

   
    V(1, 0, 1, 1, 2),	 
    V(1, 1, 1, 1, 2),
    V(1, 1, 0, 1, 2),
    V(1, 1, 1, 0, 2),

   
    V(0, 1, 1, 1, 2),	 
    V(0, 1, 0, 1, 2),
    V(1, 0, 0, 1, 1),
    V(1, 0, 0, 1, 1),

   
    V(0, 1, 1, 0, 1),	 
    V(0, 0, 1, 1, 1),

   
    V(1, 0, 1, 0, 1),	 
    V(1, 1, 0, 0, 1)
};

static
union huffquad const hufftabB[] = {
    V(1, 1, 1, 1, 4),
    V(1, 1, 1, 0, 4),
    V(1, 1, 0, 1, 4),
    V(1, 1, 0, 0, 4),
    V(1, 0, 1, 1, 4),
    V(1, 0, 1, 0, 4),
    V(1, 0, 0, 1, 4),
    V(1, 0, 0, 0, 4),
    V(0, 1, 1, 1, 4),
    V(0, 1, 1, 0, 4),
    V(0, 1, 0, 1, 4),
    V(0, 1, 0, 0, 4),
    V(0, 0, 1, 1, 4),
    V(0, 0, 1, 0, 4),
    V(0, 0, 0, 1, 4),
    V(0, 0, 0, 0, 4)
};

# undef V
# undef PTR

# if defined(__GNUC__) ||  \
    (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901)
 #  define PTR(offs, bits)	{  { 0, bits, offs } }
 #if defined(WORDS_BIGENDIAN)
#   define V(x, y, hlen)	{ { 1, hlen, (x << 8) | (y << 4) } }
#  else
#   define V(x, y, hlen)	{ { 1, hlen, (x << 0) | (y << 4) } }
#  endif
# else
#  define PTR(offs, bits)	{ { 0, bits, offs } }
#  if defined(WORDS_BIGENDIAN)
#   define V(x, y, hlen)	{ { 1, hlen, (x << 8) | (y << 4) } }
#  else
#   define V(x, y, hlen)	{ { 1, hlen, (x << 0) | (y << 4) } }
#  endif
# endif

static
union huffpair const hufftab0[] = {
    V(0, 0, 0)
};

static
union huffpair const hufftab1[] = {
    V(1, 1, 3),
    V(0, 1, 3),
    V(1, 0, 2),
    V(1, 0, 2),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1)
};

static
union huffpair const hufftab2[] = {
    PTR(8, 3),
    V(1, 1, 3),
    V(0, 1, 3),
    V(1, 0, 3),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),

   
    V(2, 2, 3),	 
    V(0, 2, 3),
    V(1, 2, 2),
    V(1, 2, 2),
    V(2, 1, 2),
    V(2, 1, 2),
    V(2, 0, 2),
    V(2, 0, 2)
};

static
union huffpair const hufftab3[] = {
    PTR(8, 3),
    V(1, 0, 3),
    V(1, 1, 2),
    V(1, 1, 2),
    V(0, 1, 2),
    V(0, 1, 2),
    V(0, 0, 2),
    V(0, 0, 2),

   
    V(2, 2, 3),	 
    V(0, 2, 3),
    V(1, 2, 2),
    V(1, 2, 2),
    V(2, 1, 2),
    V(2, 1, 2),
    V(2, 0, 2),
    V(2, 0, 2)
};

static
union huffpair const hufftab5[] = {
    PTR(8, 4),
    V(1, 1, 3),
    V(0, 1, 3),
    V(1, 0, 3),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),

   
    PTR(24, 1),	 
    V(3, 2, 4),
    V(3, 1, 3),
    V(3, 1, 3),
    V(1, 3, 4),
    V(0, 3, 4),
    V(3, 0, 4),
    V(2, 2, 4),
    V(1, 2, 3),
    V(1, 2, 3),
    V(2, 1, 3),
    V(2, 1, 3),
    V(0, 2, 3),
    V(0, 2, 3),
    V(2, 0, 3),
    V(2, 0, 3),

   
    V(3, 3, 1),	 
    V(2, 3, 1)
};

static
union huffpair const hufftab6[] = {
    PTR(16, 3),
    PTR(24, 1),
    PTR(26, 1),
    V(1, 2, 4),
    V(2, 1, 4),
    V(2, 0, 4),
    V(0, 1, 3),
    V(0, 1, 3),
    V(1, 1, 2),
    V(1, 1, 2),
    V(1, 1, 2),
    V(1, 1, 2),
    V(1, 0, 3),
    V(1, 0, 3),
    V(0, 0, 3),
    V(0, 0, 3),

   
    V(3, 3, 3),	 
    V(0, 3, 3),
    V(2, 3, 2),
    V(2, 3, 2),
    V(3, 2, 2),
    V(3, 2, 2),
    V(3, 0, 2),
    V(3, 0, 2),

   
    V(1, 3, 1),	 
    V(3, 1, 1),

   
    V(2, 2, 1),	 
    V(0, 2, 1)
};

static
union huffpair const hufftab7[] = {
    PTR(16, 4),
    PTR(32, 4),
    PTR(48, 2),
    V(1, 1, 4),
    V(0, 1, 3),
    V(0, 1, 3),
    V(1, 0, 3),
    V(1, 0, 3),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),

   
    PTR(52, 2),	 
    PTR(56, 1),
    PTR(58, 1),
    V(1, 5, 4),
    V(5, 1, 4),
    PTR(60, 1),
    V(5, 0, 4),
    PTR(62, 1),
    V(2, 4, 4),
    V(4, 2, 4),
    V(1, 4, 3),
    V(1, 4, 3),
    V(4, 1, 3),
    V(4, 1, 3),
    V(4, 0, 3),
    V(4, 0, 3),

   
    V(0, 4, 4),	 
    V(2, 3, 4),
    V(3, 2, 4),
    V(0, 3, 4),
    V(1, 3, 3),
    V(1, 3, 3),
    V(3, 1, 3),
    V(3, 1, 3),
    V(3, 0, 3),
    V(3, 0, 3),
    V(2, 2, 3),
    V(2, 2, 3),
    V(1, 2, 2),
    V(1, 2, 2),
    V(1, 2, 2),
    V(1, 2, 2),

   
    V(2, 1, 1),	 
    V(2, 1, 1),
    V(0, 2, 2),
    V(2, 0, 2),

   
    V(5, 5, 2),	 
    V(4, 5, 2),
    V(5, 4, 2),
    V(5, 3, 2),

   
    V(3, 5, 1),	 
    V(4, 4, 1),

   
    V(2, 5, 1),	 
    V(5, 2, 1),

   
    V(0, 5, 1),	 
    V(3, 4, 1),

   
    V(4, 3, 1),	 
    V(3, 3, 1)
};




static
union huffpair const hufftab8[] = {
    PTR(16, 4),
    PTR(32, 4),
    V(1, 2, 4),
    V(2, 1, 4),
    V(1, 1, 2),
    V(1, 1, 2),
    V(1, 1, 2),
    V(1, 1, 2),
    V(0, 1, 3),
    V(0, 1, 3),
    V(1, 0, 3),
    V(1, 0, 3),
    V(0, 0, 2),
    V(0, 0, 2),
    V(0, 0, 2),
    V(0, 0, 2),

   
    PTR(48, 3),	 
    PTR(56, 2),
    PTR(60, 1),
    V(1, 5, 4),
    V(5, 1, 4),
    PTR(62, 1),
    PTR(64, 1),
    V(2, 4, 4),
    V(4, 2, 4),
    V(1, 4, 4),
    V(4, 1, 3),
    V(4, 1, 3),
    V(0, 4, 4),
    V(4, 0, 4),
    V(2, 3, 4),
    V(3, 2, 4),

   
    V(1, 3, 4),	 
    V(3, 1, 4),
    V(0, 3, 4),
    V(3, 0, 4),
    V(2, 2, 2),
    V(2, 2, 2),
    V(2, 2, 2),
    V(2, 2, 2),
    V(0, 2, 2),
    V(0, 2, 2),
    V(0, 2, 2),
    V(0, 2, 2),
    V(2, 0, 2),
    V(2, 0, 2),
    V(2, 0, 2),
    V(2, 0, 2),

   
    V(5, 5, 3),	 
    V(5, 4, 3),
    V(4, 5, 2),
    V(4, 5, 2),
    V(5, 3, 1),
    V(5, 3, 1),
    V(5, 3, 1),
    V(5, 3, 1),

   
    V(3, 5, 2),	 
    V(4, 4, 2),
    V(2, 5, 1),
    V(2, 5, 1),

   
    V(5, 2, 1),	 
    V(0, 5, 1),

   
    V(3, 4, 1),	 
    V(4, 3, 1),

   
    V(5, 0, 1),	 
    V(3, 3, 1)
};


static
union huffpair const hufftab9[] = {
    PTR(16, 4),
    PTR(32, 3),
    PTR(40, 2),
    PTR(44, 2),
    PTR(48, 1),
    V(1, 2, 4),
    V(2, 1, 4),
    V(2, 0, 4),
    V(1, 1, 3),
    V(1, 1, 3),
    V(0, 1, 3),
    V(0, 1, 3),
    V(1, 0, 3),
    V(1, 0, 3),
    V(0, 0, 3),
    V(0, 0, 3),

   
    PTR(50, 1),	 
    V(3, 5, 4),
    V(5, 3, 4),
    PTR(52, 1),
    V(4, 4, 4),
    V(2, 5, 4),
    V(5, 2, 4),
    V(1, 5, 4),
    V(5, 1, 3),
    V(5, 1, 3),
    V(3, 4, 3),
    V(3, 4, 3),
    V(4, 3, 3),
    V(4, 3, 3),
    V(5, 0, 4),
    V(0, 4, 4),

   
    V(2, 4, 3),	 
    V(4, 2, 3),
    V(3, 3, 3),
    V(4, 0, 3),
    V(1, 4, 2),
    V(1, 4, 2),
    V(4, 1, 2),
    V(4, 1, 2),

   
    V(2, 3, 2),	 
    V(3, 2, 2),
    V(1, 3, 1),
    V(1, 3, 1),

   
    V(3, 1, 1),	 
    V(3, 1, 1),
    V(0, 3, 2),
    V(3, 0, 2),

   
    V(2, 2, 1),	 
    V(0, 2, 1),

   
    V(5, 5, 1),	 
    V(4, 5, 1),

   
    V(5, 4, 1),	 
    V(0, 5, 1)
};

static
union huffpair const hufftab10[] = {
    PTR(16, 4),
    PTR(32, 4),
    PTR(48, 2),
    V(1, 1, 4),
    V(0, 1, 3),
    V(0, 1, 3),
    V(1, 0, 3),
    V(1, 0, 3),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),

   
    PTR(52, 3),	 
    PTR(60, 2),
    PTR(64, 3),
    PTR(72, 1),
    PTR(74, 2),
    PTR(78, 2),
    PTR(82, 2),
    V(1, 7, 4),
    V(7, 1, 4),
    PTR(86, 1),
    PTR(88, 2),
    PTR(92, 2),
    V(1, 6, 4),
    V(6, 1, 4),
    V(6, 0, 4),
    PTR(96, 1),

   
    PTR(98, 1),	 
    PTR(100, 1),
    V(1, 4, 4),
    V(4, 1, 4),
    V(4, 0, 4),
    V(2, 3, 4),
    V(3, 2, 4),
    V(0, 3, 4),
    V(1, 3, 3),
    V(1, 3, 3),
    V(3, 1, 3),
    V(3, 1, 3),
    V(3, 0, 3),
    V(3, 0, 3),
    V(2, 2, 3),
    V(2, 2, 3),

   
    V(1, 2, 2),	 
    V(2, 1, 2),
    V(0, 2, 2),
    V(2, 0, 2),

   
    V(7, 7, 3),	 
    V(6, 7, 3),
    V(7, 6, 3),
    V(5, 7, 3),
    V(7, 5, 3),
    V(6, 6, 3),
    V(4, 7, 2),
    V(4, 7, 2),

   
    V(7, 4, 2),	 
    V(5, 6, 2),
    V(6, 5, 2),
    V(3, 7, 2),

   
    V(7, 3, 2),	 
    V(7, 3, 2),
    V(4, 6, 2),
    V(4, 6, 2),
    V(5, 5, 3),
    V(5, 4, 3),
    V(6, 3, 2),
    V(6, 3, 2),

   
    V(2, 7, 1),	 
    V(7, 2, 1),

   
    V(6, 4, 2),	 
    V(0, 7, 2),
    V(7, 0, 1),
    V(7, 0, 1),

   
    V(6, 2, 1),	 
    V(6, 2, 1),
    V(4, 5, 2),
    V(3, 5, 2),

   
    V(0, 6, 1),	 
    V(0, 6, 1),
    V(5, 3, 2),
    V(4, 4, 2),

   
    V(3, 6, 1),	 
    V(2, 6, 1),

   
    V(2, 5, 2),	 
    V(5, 2, 2),
    V(1, 5, 1),
    V(1, 5, 1),

   
    V(5, 1, 1),	 
    V(5, 1, 1),
    V(3, 4, 2),
    V(4, 3, 2),

   
    V(0, 5, 1),	 
    V(5, 0, 1),

   
    V(2, 4, 1),	 
    V(4, 2, 1),

   
    V(3, 3, 1),	 
    V(0, 4, 1)
};

static
union huffpair const hufftab11[] = {
    PTR(16, 4),
    PTR(32, 4),
    PTR(48, 4),
    PTR(64, 3),
    V(1, 2, 4),
    PTR(72, 1),
    V(1, 1, 3),
    V(1, 1, 3),
    V(0, 1, 3),
    V(0, 1, 3),
    V(1, 0, 3),
    V(1, 0, 3),
    V(0, 0, 2),
    V(0, 0, 2),
    V(0, 0, 2),
    V(0, 0, 2),

   
    PTR(74, 2),	 
    PTR(78, 3),
    PTR(86, 2),
    PTR(90, 1),
    PTR(92, 2),
    V(2, 7, 4),
    V(7, 2, 4),
    PTR(96, 1),
    V(7, 1, 3),
    V(7, 1, 3),
    V(1, 7, 4),
    V(7, 0, 4),
    V(3, 6, 4),
    V(6, 3, 4),
    V(6, 0, 4),
    PTR(98, 1),

   
    PTR(100, 1),	 
    V(1, 5, 4),
    V(6, 2, 3),
    V(6, 2, 3),
    V(2, 6, 4),
    V(0, 6, 4),
    V(1, 6, 3),
    V(1, 6, 3),
    V(6, 1, 3),
    V(6, 1, 3),
    V(5, 1, 4),
    V(3, 4, 4),
    V(5, 0, 4),
    PTR(102, 1),
    V(2, 4, 4),
    V(4, 2, 4),

   
    V(1, 4, 4),	 
    V(4, 1, 4),
    V(0, 4, 4),
    V(4, 0, 4),
    V(2, 3, 3),
    V(2, 3, 3),
    V(3, 2, 3),
    V(3, 2, 3),
    V(1, 3, 2),
    V(1, 3, 2),
    V(1, 3, 2),
    V(1, 3, 2),
    V(3, 1, 2),
    V(3, 1, 2),
    V(3, 1, 2),
    V(3, 1, 2),

   
    V(0, 3, 3),	 
    V(3, 0, 3),
    V(2, 2, 2),
    V(2, 2, 2),
    V(2, 1, 1),
    V(2, 1, 1),
    V(2, 1, 1),
    V(2, 1, 1),

   
    V(0, 2, 1),	 
    V(2, 0, 1),

   
    V(7, 7, 2),	 
    V(6, 7, 2),
    V(7, 6, 2),
    V(7, 5, 2),

   
    V(6, 6, 2),	 
    V(6, 6, 2),
    V(4, 7, 2),
    V(4, 7, 2),
    V(7, 4, 2),
    V(7, 4, 2),
    V(5, 7, 3),
    V(5, 5, 3),

   
    V(5, 6, 2),	 
    V(6, 5, 2),
    V(3, 7, 1),
    V(3, 7, 1),

   
    V(7, 3, 1),	 
    V(4, 6, 1),

   
    V(4, 5, 2),	 
    V(5, 4, 2),
    V(3, 5, 2),
    V(5, 3, 2),

   
    V(6, 4, 1),	 
    V(0, 7, 1),

   
    V(4, 4, 1),	 
    V(2, 5, 1),

   
    V(5, 2, 1),	 
    V(0, 5, 1),

   
    V(4, 3, 1),	 
    V(3, 3, 1)
};

static
union huffpair const hufftab12[] = {
    PTR(16, 4),
    PTR(32, 4),
    PTR(48, 4),
    PTR(64, 2),
    PTR(68, 3),
    PTR(76, 1),
    V(1, 2, 4),
    V(2, 1, 4),
    PTR(78, 1),
    V(0, 0, 4),
    V(1, 1, 3),
    V(1, 1, 3),
    V(0, 1, 3),
    V(0, 1, 3),
    V(1, 0, 3),
    V(1, 0, 3),

   
    PTR(80, 2),	 
    PTR(84, 1),
    PTR(86, 1),
    PTR(88, 1),
    V(5, 6, 4),
    V(3, 7, 4),
    PTR(90, 1),
    V(2, 7, 4),
    V(7, 2, 4),
    V(4, 6, 4),
    V(6, 4, 4),
    V(1, 7, 4),
    V(7, 1, 4),
    PTR(92, 1),
    V(3, 6, 4),
    V(6, 3, 4),

   
    V(4, 5, 4),	 
    V(5, 4, 4),
    V(4, 4, 4),
    PTR(94, 1),
    V(2, 6, 3),
    V(2, 6, 3),
    V(6, 2, 3),
    V(6, 2, 3),
    V(6, 1, 3),
    V(6, 1, 3),
    V(1, 6, 4),
    V(6, 0, 4),
    V(3, 5, 4),
    V(5, 3, 4),
    V(2, 5, 4),
    V(5, 2, 4),

   
    V(1, 5, 3),	 
    V(1, 5, 3),
    V(5, 1, 3),
    V(5, 1, 3),
    V(3, 4, 3),
    V(3, 4, 3),
    V(4, 3, 3),
    V(4, 3, 3),
    V(5, 0, 4),
    V(0, 4, 4),
    V(2, 4, 3),
    V(2, 4, 3),
    V(4, 2, 3),
    V(4, 2, 3),
    V(1, 4, 3),
    V(1, 4, 3),

   
    V(3, 3, 2),	 
    V(4, 1, 2),
    V(2, 3, 2),
    V(3, 2, 2),

   
    V(4, 0, 3),	 
    V(0, 3, 3),
    V(3, 0, 2),
    V(3, 0, 2),
    V(1, 3, 1),
    V(1, 3, 1),
    V(1, 3, 1),
    V(1, 3, 1),

   
    V(3, 1, 1),	 
    V(2, 2, 1),

   
    V(0, 2, 1),	 
    V(2, 0, 1),

   
    V(7, 7, 2),	 
    V(6, 7, 2),
    V(7, 6, 1),
    V(7, 6, 1),

   
    V(5, 7, 1),	 
    V(7, 5, 1),

   
    V(6, 6, 1),	 
    V(4, 7, 1),

   
    V(7, 4, 1),	 
    V(6, 5, 1),

   
    V(7, 3, 1),	 
    V(5, 5, 1),

   
    V(0, 7, 1),	 
    V(7, 0, 1),

   
    V(0, 6, 1),	 
    V(0, 5, 1)
};

static
union huffpair const hufftab13[] = {
    PTR(16, 4),
    PTR(32, 4),
    PTR(48, 4),
    PTR(64, 2),
    V(1, 1, 4),
    V(0, 1, 4),
    V(1, 0, 3),
    V(1, 0, 3),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),

   
    PTR(68, 4),	 
    PTR(84, 4),
    PTR(100, 4),
    PTR(116, 4),
    PTR(132, 4),
    PTR(148, 4),
    PTR(164, 3),
    PTR(172, 3),
    PTR(180, 3),
    PTR(188, 3),
    PTR(196, 3),
    PTR(204, 3),
    PTR(212, 1),
    PTR(214, 2),
    PTR(218, 3),
    PTR(226, 1),

   
    PTR(228, 2),	 
    PTR(232, 2),
    PTR(236, 2),
    PTR(240, 2),
    V(8, 1, 4),
    PTR(244, 1),
    PTR(246, 1),
    PTR(248, 1),
    PTR(250, 2),
    PTR(254, 1),
    V(1, 5, 4),
    V(5, 1, 4),
    PTR(256, 1),
    PTR(258, 1),
    PTR(260, 1),
    V(1, 4, 4),

   
    V(4, 1, 3),	 
    V(4, 1, 3),
    V(0, 4, 4),
    V(4, 0, 4),
    V(2, 3, 4),
    V(3, 2, 4),
    V(1, 3, 3),
    V(1, 3, 3),
    V(3, 1, 3),
    V(3, 1, 3),
    V(0, 3, 3),
    V(0, 3, 3),
    V(3, 0, 3),
    V(3, 0, 3),
    V(2, 2, 3),
    V(2, 2, 3),

   
    V(1, 2, 2),	 
    V(2, 1, 2),
    V(0, 2, 2),
    V(2, 0, 2),

   
    PTR(262, 4),	 
    PTR(278, 4),
    PTR(294, 4),
    PTR(310, 3),
    PTR(318, 2),
    PTR(322, 2),
    PTR(326, 3),
    PTR(334, 2),
    PTR(338, 1),
    PTR(340, 2),
    PTR(344, 2),
    PTR(348, 2),
    PTR(352, 2),
    PTR(356, 2),
    V(1, 15, 4),
    V(15, 1, 4),

   
    V(15, 0, 4),	 
    PTR(360, 1),
    PTR(362, 1),
    PTR(364, 1),
    V(14, 2, 4),
    PTR(366, 1),
    V(1, 14, 4),
    V(14, 1, 4),
    PTR(368, 1),
    PTR(370, 1),
    PTR(372, 1),
    PTR(374, 1),
    PTR(376, 1),
    PTR(378, 1),
    V(12, 6, 4),
    V(3, 13, 4),

   
    PTR(380, 1),	 
    V(2, 13, 4),
    V(13, 2, 4),
    V(1, 13, 4),
    V(11, 7, 4),
    PTR(382, 1),
    PTR(384, 1),
    V(12, 3, 4),
    PTR(386, 1),
    V(4, 11, 4),
    V(13, 1, 3),
    V(13, 1, 3),
    V(0, 13, 4),
    V(13, 0, 4),
    V(8, 10, 4),
    V(10, 8, 4),

   
    V(4, 12, 4),	 
    V(12, 4, 4),
    V(6, 11, 4),
    V(11, 6, 4),
    V(3, 12, 3),
    V(3, 12, 3),
    V(2, 12, 3),
    V(2, 12, 3),
    V(12, 2, 3),
    V(12, 2, 3),
    V(5, 11, 3),
    V(5, 11, 3),
    V(11, 5, 4),
    V(8, 9, 4),
    V(1, 12, 3),
    V(1, 12, 3),

   
    V(12, 1, 3),	 
    V(12, 1, 3),
    V(9, 8, 4),
    V(0, 12, 4),
    V(12, 0, 3),
    V(12, 0, 3),
    V(11, 4, 4),
    V(6, 10, 4),
    V(10, 6, 4),
    V(7, 9, 4),
    V(3, 11, 3),
    V(3, 11, 3),
    V(11, 3, 3),
    V(11, 3, 3),
    V(8, 8, 4),
    V(5, 10, 4),

   
    V(2, 11, 3),	 
    V(2, 11, 3),
    V(10, 5, 4),
    V(6, 9, 4),
    V(10, 4, 3),
    V(10, 4, 3),
    V(7, 8, 4),
    V(8, 7, 4),
    V(9, 4, 3),
    V(9, 4, 3),
    V(7, 7, 4),
    V(7, 6, 4),
    V(11, 2, 2),
    V(11, 2, 2),
    V(11, 2, 2),
    V(11, 2, 2),

   
    V(1, 11, 2),	 
    V(1, 11, 2),
    V(11, 1, 2),
    V(11, 1, 2),
    V(0, 11, 3),
    V(11, 0, 3),
    V(9, 6, 3),
    V(4, 10, 3),

   
    V(3, 10, 3),	 
    V(10, 3, 3),
    V(5, 9, 3),
    V(9, 5, 3),
    V(2, 10, 2),
    V(2, 10, 2),
    V(10, 2, 2),
    V(10, 2, 2),

   
    V(1, 10, 2),	 
    V(1, 10, 2),
    V(10, 1, 2),
    V(10, 1, 2),
    V(0, 10, 3),
    V(6, 8, 3),
    V(10, 0, 2),
    V(10, 0, 2),

   
    V(8, 6, 3),	 
    V(4, 9, 3),
    V(9, 3, 2),
    V(9, 3, 2),
    V(3, 9, 3),
    V(5, 8, 3),
    V(8, 5, 3),
    V(6, 7, 3),

   
    V(2, 9, 2),	 
    V(2, 9, 2),
    V(9, 2, 2),
    V(9, 2, 2),
    V(5, 7, 3),
    V(7, 5, 3),
    V(3, 8, 2),
    V(3, 8, 2),

   
    V(8, 3, 2),	 
    V(8, 3, 2),
    V(6, 6, 3),
    V(4, 7, 3),
    V(7, 4, 3),
    V(5, 6, 3),
    V(6, 5, 3),
    V(7, 3, 3),

   
    V(1, 9, 1),	 
    V(9, 1, 1),

   
    V(0, 9, 2),	 
    V(9, 0, 2),
    V(4, 8, 2),
    V(8, 4, 2),

   
    V(7, 2, 2),	 
    V(7, 2, 2),
    V(4, 6, 3),
    V(6, 4, 3),
    V(2, 8, 1),
    V(2, 8, 1),
    V(2, 8, 1),
    V(2, 8, 1),

   
    V(8, 2, 1),	 
    V(1, 8, 1),

   
    V(3, 7, 2),	 
    V(2, 7, 2),
    V(1, 7, 1),
    V(1, 7, 1),

   
    V(7, 1, 1),	 
    V(7, 1, 1),
    V(5, 5, 2),
    V(0, 7, 2),

   
    V(7, 0, 2),	 
    V(3, 6, 2),
    V(6, 3, 2),
    V(4, 5, 2),

   
    V(5, 4, 2),	 
    V(2, 6, 2),
    V(6, 2, 2),
    V(3, 5, 2),

   
    V(0, 8, 1),	 
    V(8, 0, 1),

   
    V(1, 6, 1),	 
    V(6, 1, 1),

   
    V(0, 6, 1),	 
    V(6, 0, 1),

   
    V(5, 3, 2),	 
    V(4, 4, 2),
    V(2, 5, 1),
    V(2, 5, 1),

   
    V(5, 2, 1),	 
    V(0, 5, 1),

   
    V(3, 4, 1),	 
    V(4, 3, 1),

   
    V(5, 0, 1),	 
    V(2, 4, 1),

   
    V(4, 2, 1),	 
    V(3, 3, 1),

   
    PTR(388, 3),	 
    V(15, 15, 4),
    V(14, 15, 4),
    V(13, 15, 4),
    V(14, 14, 4),
    V(12, 15, 4),
    V(13, 14, 4),
    V(11, 15, 4),
    V(15, 11, 4),
    V(12, 14, 4),
    V(13, 12, 4),
    PTR(396, 1),
    V(14, 12, 3),
    V(14, 12, 3),
    V(13, 13, 3),
    V(13, 13, 3),

   
    V(15, 10, 4),	 
    V(12, 13, 4),
    V(11, 14, 3),
    V(11, 14, 3),
    V(14, 11, 3),
    V(14, 11, 3),
    V(9, 15, 3),
    V(9, 15, 3),
    V(15, 9, 3),
    V(15, 9, 3),
    V(14, 10, 3),
    V(14, 10, 3),
    V(11, 13, 3),
    V(11, 13, 3),
    V(13, 11, 3),
    V(13, 11, 3),

   
    V(8, 15, 3),	 
    V(8, 15, 3),
    V(15, 8, 3),
    V(15, 8, 3),
    V(12, 12, 3),
    V(12, 12, 3),
    V(10, 14, 4),
    V(9, 14, 4),
    V(8, 14, 3),
    V(8, 14, 3),
    V(7, 15, 4),
    V(7, 14, 4),
    V(15, 7, 2),
    V(15, 7, 2),
    V(15, 7, 2),
    V(15, 7, 2),

   
    V(13, 10, 2),	 
    V(13, 10, 2),
    V(10, 13, 3),
    V(11, 12, 3),
    V(12, 11, 3),
    V(15, 6, 3),
    V(6, 15, 2),
    V(6, 15, 2),

   
    V(14, 8, 2),	 
    V(5, 15, 2),
    V(9, 13, 2),
    V(13, 9, 2),

   
    V(15, 5, 2),	 
    V(14, 7, 2),
    V(10, 12, 2),
    V(11, 11, 2),

   
    V(4, 15, 2),	 
    V(4, 15, 2),
    V(15, 4, 2),
    V(15, 4, 2),
    V(12, 10, 3),
    V(14, 6, 3),
    V(15, 3, 2),
    V(15, 3, 2),

   
    V(3, 15, 1),	 
    V(3, 15, 1),
    V(8, 13, 2),
    V(13, 8, 2),

   
    V(2, 15, 1),	 
    V(15, 2, 1),

   
    V(6, 14, 2),	 
    V(9, 12, 2),
    V(0, 15, 1),
    V(0, 15, 1),

   
    V(12, 9, 2),	 
    V(5, 14, 2),
    V(10, 11, 1),
    V(10, 11, 1),

   
    V(7, 13, 2),	 
    V(13, 7, 2),
    V(4, 14, 1),
    V(4, 14, 1),

   
    V(12, 8, 2),	 
    V(13, 6, 2),
    V(3, 14, 1),
    V(3, 14, 1),

   
    V(11, 9, 1),	 
    V(11, 9, 1),
    V(9, 11, 2),
    V(10, 10, 2),

   
    V(11, 10, 1),	 
    V(14, 5, 1),

   
    V(14, 4, 1),	 
    V(8, 12, 1),

   
    V(6, 13, 1),	 
    V(14, 3, 1),

   
    V(2, 14, 1),	 
    V(0, 14, 1),

   
    V(14, 0, 1),	 
    V(5, 13, 1),

   
    V(13, 5, 1),	 
    V(7, 12, 1),

   
    V(12, 7, 1),	 
    V(4, 13, 1),

   
    V(8, 11, 1),	 
    V(11, 8, 1),

   
    V(13, 4, 1),	 
    V(9, 10, 1),

   
    V(10, 9, 1),	 
    V(6, 12, 1),

   
    V(13, 3, 1),	 
    V(7, 11, 1),

   
    V(5, 12, 1),	 
    V(12, 5, 1),

   
    V(9, 9, 1),	 
    V(7, 10, 1),

   
    V(10, 7, 1),	 
    V(9, 7, 1),

   
    V(15, 14, 3),	 
    V(15, 12, 3),
    V(15, 13, 2),
    V(15, 13, 2),
    V(14, 13, 1),
    V(14, 13, 1),
    V(14, 13, 1),
    V(14, 13, 1),

   
    V(10, 15, 1),	 
    V(14, 9, 1)
};

static
union huffpair const hufftab15[] = {
    PTR(16, 4),
    PTR(32, 4),
    PTR(48, 4),
    PTR(64, 4),
    PTR(80, 4),
    PTR(96, 3),
    PTR(104, 3),
    PTR(112, 2),
    PTR(116, 1),
    PTR(118, 1),
    V(1, 1, 3),
    V(1, 1, 3),
    V(0, 1, 4),
    V(1, 0, 4),
    V(0, 0, 3),
    V(0, 0, 3),

   
    PTR(120, 4),	 
    PTR(136, 4),
    PTR(152, 4),
    PTR(168, 4),
    PTR(184, 4),
    PTR(200, 3),
    PTR(208, 3),
    PTR(216, 4),
    PTR(232, 3),
    PTR(240, 3),
    PTR(248, 3),
    PTR(256, 3),
    PTR(264, 2),
    PTR(268, 3),
    PTR(276, 3),
    PTR(284, 2),

   
    PTR(288, 2),	 
    PTR(292, 2),
    PTR(296, 2),
    PTR(300, 2),
    PTR(304, 2),
    PTR(308, 2),
    PTR(312, 2),
    PTR(316, 2),
    PTR(320, 1),
    PTR(322, 1),
    PTR(324, 1),
    PTR(326, 2),
    PTR(330, 1),
    PTR(332, 1),
    PTR(334, 2),
    PTR(338, 1),

   
    PTR(340, 1),	 
    PTR(342, 1),
    V(9, 1, 4),
    PTR(344, 1),
    PTR(346, 1),
    PTR(348, 1),
    PTR(350, 1),
    PTR(352, 1),
    V(2, 8, 4),
    V(8, 2, 4),
    V(1, 8, 4),
    V(8, 1, 4),
    PTR(354, 1),
    PTR(356, 1),
    PTR(358, 1),
    PTR(360, 1),

   
    V(2, 7, 4),	 
    V(7, 2, 4),
    V(6, 4, 4),
    V(1, 7, 4),
    V(5, 5, 4),
    V(7, 1, 4),
    PTR(362, 1),
    V(3, 6, 4),
    V(6, 3, 4),
    V(4, 5, 4),
    V(5, 4, 4),
    V(2, 6, 4),
    V(6, 2, 4),
    V(1, 6, 4),
    PTR(364, 1),
    V(3, 5, 4),

   
    V(6, 1, 3),	 
    V(6, 1, 3),
    V(5, 3, 4),
    V(4, 4, 4),
    V(2, 5, 3),
    V(2, 5, 3),
    V(5, 2, 3),
    V(5, 2, 3),
    V(1, 5, 3),
    V(1, 5, 3),
    V(5, 1, 3),
    V(5, 1, 3),
    V(0, 5, 4),
    V(5, 0, 4),
    V(3, 4, 3),
    V(3, 4, 3),

   
    V(4, 3, 3),	 
    V(2, 4, 3),
    V(4, 2, 3),
    V(3, 3, 3),
    V(4, 1, 2),
    V(4, 1, 2),
    V(1, 4, 3),
    V(0, 4, 3),

   
    V(2, 3, 2),	 
    V(2, 3, 2),
    V(3, 2, 2),
    V(3, 2, 2),
    V(4, 0, 3),
    V(0, 3, 3),
    V(1, 3, 2),
    V(1, 3, 2),

   
    V(3, 1, 2),	 
    V(3, 0, 2),
    V(2, 2, 1),
    V(2, 2, 1),

   
    V(1, 2, 1),	 
    V(2, 1, 1),

   
    V(0, 2, 1),	 
    V(2, 0, 1),

   
    PTR(366, 1),	 
    PTR(368, 1),
    V(14, 14, 4),
    PTR(370, 1),
    PTR(372, 1),
    PTR(374, 1),
    V(15, 11, 4),
    PTR(376, 1),
    V(13, 13, 4),
    V(10, 15, 4),
    V(15, 10, 4),
    V(11, 14, 4),
    V(14, 11, 4),
    V(12, 13, 4),
    V(13, 12, 4),
    V(9, 15, 4),

   
    V(15, 9, 4),	 
    V(14, 10, 4),
    V(11, 13, 4),
    V(13, 11, 4),
    V(8, 15, 4),
    V(15, 8, 4),
    V(12, 12, 4),
    V(9, 14, 4),
    V(14, 9, 4),
    V(7, 15, 4),
    V(15, 7, 4),
    V(10, 13, 4),
    V(13, 10, 4),
    V(11, 12, 4),
    V(6, 15, 4),
    PTR(378, 1),


   
    V(12, 11, 3),	 
    V(12, 11, 3),
    V(15, 6, 3),
    V(15, 6, 3),
    V(8, 14, 4),
    V(14, 8, 4),
    V(5, 15, 4),
    V(9, 13, 4),
    V(15, 5, 3),
    V(15, 5, 3),
    V(7, 14, 3),
    V(7, 14, 3),
    V(14, 7, 3),
    V(14, 7, 3),
    V(10, 12, 3),
    V(10, 12, 3),

   
    V(12, 10, 3),	 
    V(12, 10, 3),
    V(11, 11, 3),
    V(11, 11, 3),
    V(13, 9, 4),
    V(8, 13, 4),
    V(4, 15, 3),
    V(4, 15, 3),
    V(15, 4, 3),
    V(15, 4, 3),
    V(3, 15, 3),
    V(3, 15, 3),
    V(15, 3, 3),
    V(15, 3, 3),
    V(13, 8, 3),
    V(13, 8, 3),

   
    V(14, 6, 3),	 
    V(14, 6, 3),
    V(2, 15, 3),
    V(2, 15, 3),
    V(15, 2, 3),
    V(15, 2, 3),
    V(6, 14, 4),
    V(15, 0, 4),
    V(1, 15, 3),
    V(1, 15, 3),
    V(15, 1, 3),
    V(15, 1, 3),
    V(9, 12, 3),
    V(9, 12, 3),
    V(12, 9, 3),
    V(12, 9, 3),

   
    V(5, 14, 3),	 
    V(10, 11, 3),
    V(11, 10, 3),
    V(14, 5, 3),
    V(7, 13, 3),
    V(13, 7, 3),
    V(4, 14, 3),
    V(14, 4, 3),

   
    V(8, 12, 3),	 
    V(12, 8, 3),
    V(3, 14, 3),
    V(6, 13, 3),
    V(13, 6, 3),
    V(14, 3, 3),
    V(9, 11, 3),
    V(11, 9, 3),

   
    V(2, 14, 3),	 
    V(2, 14, 3),
    V(10, 10, 3),
    V(10, 10, 3),
    V(14, 2, 3),
    V(14, 2, 3),
    V(1, 14, 3),
    V(1, 14, 3),
    V(14, 1, 3),
    V(14, 1, 3),
    V(0, 14, 4),
    V(14, 0, 4),
    V(5, 13, 3),
    V(5, 13, 3),
    V(13, 5, 3),
    V(13, 5, 3),

   
    V(7, 12, 3),	 
    V(12, 7, 3),
    V(4, 13, 3),
    V(8, 11, 3),
    V(13, 4, 2),
    V(13, 4, 2),
    V(11, 8, 3),
    V(9, 10, 3),

   
    V(10, 9, 3),	 
    V(6, 12, 3),
    V(12, 6, 3),
    V(3, 13, 3),
    V(13, 3, 2),
    V(13, 3, 2),
    V(13, 2, 2),
    V(13, 2, 2),

   
    V(2, 13, 3),	 
    V(0, 13, 3),
    V(1, 13, 2),
    V(1, 13, 2),
    V(7, 11, 2),
    V(7, 11, 2),
    V(11, 7, 2),
    V(11, 7, 2),

   
    V(13, 1, 2),	 
    V(13, 1, 2),
    V(5, 12, 3),
    V(13, 0, 3),
    V(12, 5, 2),
    V(12, 5, 2),
    V(8, 10, 2),
    V(8, 10, 2),

   
    V(10, 8, 2),	 
    V(4, 12, 2),
    V(12, 4, 2),
    V(6, 11, 2),

   
    V(11, 6, 2),	 
    V(11, 6, 2),
    V(9, 9, 3),
    V(0, 12, 3),
    V(3, 12, 2),
    V(3, 12, 2),
    V(12, 3, 2),
    V(12, 3, 2),

   
    V(7, 10, 2),	 
    V(7, 10, 2),
    V(10, 7, 2),
    V(10, 7, 2),
    V(10, 6, 2),
    V(10, 6, 2),
    V(12, 0, 3),
    V(0, 11, 3),

   
    V(12, 2, 1),	 
    V(12, 2, 1),
    V(2, 12, 2),
    V(5, 11, 2),

   
    V(11, 5, 2),	 
    V(1, 12, 2),
    V(8, 9, 2),
    V(9, 8, 2),

   
    V(12, 1, 2),	 
    V(4, 11, 2),
    V(11, 4, 2),
    V(6, 10, 2),

   
    V(3, 11, 2),	 
    V(7, 9, 2),
    V(11, 3, 1),
    V(11, 3, 1),

   
    V(9, 7, 2),	 
    V(8, 8, 2),
    V(2, 11, 2),
    V(5, 10, 2),

   
    V(11, 2, 1),	 
    V(11, 2, 1),
    V(10, 5, 2),
    V(1, 11, 2),

   
    V(11, 1, 1),	 
    V(11, 1, 1),
    V(11, 0, 2),
    V(6, 9, 2),

   
    V(9, 6, 2),	 
    V(4, 10, 2),
    V(10, 4, 2),
    V(7, 8, 2),

   
    V(8, 7, 2),	 
    V(3, 10, 2),
    V(10, 3, 1),
    V(10, 3, 1),

   
    V(5, 9, 1),	 
    V(9, 5, 1),

   
    V(2, 10, 1),	 
    V(10, 2, 1),

   
    V(1, 10, 1),	 
    V(10, 1, 1),

   
    V(0, 10, 2),	 
    V(10, 0, 2),
    V(6, 8, 1),
    V(6, 8, 1),

   
    V(8, 6, 1),	 
    V(4, 9, 1),

   
    V(9, 4, 1),	 
    V(3, 9, 1),

   
    V(9, 3, 1),	 
    V(9, 3, 1),
    V(7, 7, 2),
    V(0, 9, 2),

   
    V(5, 8, 1),	 
    V(8, 5, 1),

   
    V(2, 9, 1),	 
    V(6, 7, 1),

   
    V(7, 6, 1),	 
    V(9, 2, 1),

   
    V(1, 9, 1),	 
    V(9, 0, 1),

   
    V(4, 8, 1),	 
    V(8, 4, 1),

   
    V(5, 7, 1),	 
    V(7, 5, 1),

   
    V(3, 8, 1),	 
    V(8, 3, 1),

   
    V(6, 6, 1),	 
    V(4, 7, 1),

   
    V(7, 4, 1),	 
    V(0, 8, 1),

   
    V(8, 0, 1),	 
    V(5, 6, 1),

   
    V(6, 5, 1),	 
    V(3, 7, 1),

   
    V(7, 3, 1),	 
    V(4, 6, 1),

   
    V(0, 7, 1),	 
    V(7, 0, 1),

   
    V(0, 6, 1),	 
    V(6, 0, 1),

   
    V(15, 15, 1),	 
    V(14, 15, 1),

   
    V(15, 14, 1),	 
    V(13, 15, 1),

   
    V(15, 13, 1),	 
    V(12, 15, 1),

   
    V(15, 12, 1),	 
    V(13, 14, 1),

   
    V(14, 13, 1),	 
    V(11, 15, 1),

   
    V(12, 14, 1),	 
    V(14, 12, 1),

   
    V(10, 14, 1),	 
    V(0, 15, 1)
};

static
union huffpair const hufftab16[] = {
    PTR(16, 4),
    PTR(32, 4),
    PTR(48, 4),
    PTR(64, 2),
    V(1, 1, 4),
    V(0, 1, 4),
    V(1, 0, 3),
    V(1, 0, 3),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),
    V(0, 0, 1),

   
    PTR(68, 3),	 
    PTR(76, 3),
    PTR(84, 2),
    V(15, 15, 4),
    PTR(88, 2),
    PTR(92, 1),
    PTR(94, 4),
    V(15, 2, 4),
    PTR(110, 1),
    V(1, 15, 4),
    V(15, 1, 4),
    PTR(112, 4),
    PTR(128, 4),
    PTR(144, 4),
    PTR(160, 4),
    PTR(176, 4),

   
    PTR(192, 4),	 
    PTR(208, 3),
    PTR(216, 3),
    PTR(224, 3),
    PTR(232, 3),
    PTR(240, 3),
    PTR(248, 3),
    PTR(256, 3),
    PTR(264, 2),
    PTR(268, 2),
    PTR(272, 1),
    PTR(274, 2),
    PTR(278, 2),
    PTR(282, 1),
    V(5, 1, 4),
    PTR(284, 1),

   
    PTR(286, 1),	 
    PTR(288, 1),
    PTR(290, 1),
    V(1, 4, 4),
    V(4, 1, 4),
    PTR(292, 1),
    V(2, 3, 4),
    V(3, 2, 4),
    V(1, 3, 3),
    V(1, 3, 3),
    V(3, 1, 3),
    V(3, 1, 3),
    V(0, 3, 4),
    V(3, 0, 4),
    V(2, 2, 3),
    V(2, 2, 3),

   
    V(1, 2, 2),	 
    V(2, 1, 2),
    V(0, 2, 2),
    V(2, 0, 2),

   
    V(14, 15, 3),	 
    V(15, 14, 3),
    V(13, 15, 3),
    V(15, 13, 3),
    V(12, 15, 3),
    V(15, 12, 3),
    V(11, 15, 3),
    V(15, 11, 3),

   
    V(10, 15, 2),	 
    V(10, 15, 2),
    V(15, 10, 3),
    V(9, 15, 3),
    V(15, 9, 3),
    V(15, 8, 3),
    V(8, 15, 2),
    V(8, 15, 2),

   
    V(7, 15, 2),	 
    V(15, 7, 2),
    V(6, 15, 2),
    V(15, 6, 2),

   
    V(5, 15, 2),	 
    V(15, 5, 2),
    V(4, 15, 1),
    V(4, 15, 1),

   
    V(15, 4, 1),	 
    V(15, 3, 1),

   
    V(15, 0, 1),	 
    V(15, 0, 1),
    V(15, 0, 1),
    V(15, 0, 1),
    V(15, 0, 1),
    V(15, 0, 1),
    V(15, 0, 1),
    V(15, 0, 1),
    V(3, 15, 2),
    V(3, 15, 2),
    V(3, 15, 2),
    V(3, 15, 2),
    PTR(294, 4),
    PTR(310, 3),
    PTR(318, 3),
    PTR(326, 3),

   
    V(2, 15, 1),	 
    V(0, 15, 1),

   
    PTR(334, 2),	 
    PTR(338, 2),
    PTR(342, 2),
    PTR(346, 1),
    PTR(348, 2),
    PTR(352, 2),
    PTR(356, 1),
    PTR(358, 2),
    PTR(362, 2),
    PTR(366, 2),
    PTR(370, 2),
    V(14, 3, 4),
    PTR(374, 1),
    PTR(376, 1),
    PTR(378, 1),
    PTR(380, 1),

   
    PTR(382, 1),	 
    PTR(384, 1),
    PTR(386, 1),
    V(0, 13, 4),
    PTR(388, 1),
    PTR(390, 1),
    PTR(392, 1),
    V(3, 12, 4),
    PTR(394, 1),
    V(1, 12, 4),
    V(12, 0, 4),
    PTR(396, 1),
    V(14, 2, 3),
    V(14, 2, 3),
    V(2, 14, 4),
    V(1, 14, 4),

   
    V(13, 3, 4),	 
    V(2, 13, 4),
    V(13, 2, 4),
    V(13, 1, 4),
    V(3, 11, 4),
    PTR(398, 1),
    V(1, 13, 3),
    V(1, 13, 3),
    V(12, 4, 4),
    V(6, 11, 4),
    V(12, 3, 4),
    V(10, 7, 4),
    V(2, 12, 3),
    V(2, 12, 3),
    V(12, 2, 4),
    V(11, 5, 4),

   
    V(12, 1, 4),	 
    V(0, 12, 4),
    V(4, 11, 4),
    V(11, 4, 4),
    V(6, 10, 4),
    V(10, 6, 4),
    V(11, 3, 3),
    V(11, 3, 3),
    V(5, 10, 4),
    V(10, 5, 4),
    V(2, 11, 3),
    V(2, 11, 3),
    V(11, 2, 3),
    V(11, 2, 3),
    V(1, 11, 3),
    V(1, 11, 3),

   
    V(11, 1, 3),	 
    V(11, 1, 3),
    V(0, 11, 4),
    V(11, 0, 4),
    V(6, 9, 4),
    V(9, 6, 4),
    V(4, 10, 4),
    V(10, 4, 4),
    V(7, 8, 4),
    V(8, 7, 4),
    V(10, 3, 3),
    V(10, 3, 3),
    V(3, 10, 4),
    V(5, 9, 4),
    V(2, 10, 3),
    V(2, 10, 3),

   
    V(9, 5, 4),	 
    V(6, 8, 4),
    V(10, 1, 3),
    V(10, 1, 3),
    V(8, 6, 4),
    V(7, 7, 4),
    V(9, 4, 3),
    V(9, 4, 3),
    V(4, 9, 4),
    V(5, 7, 4),
    V(6, 7, 3),
    V(6, 7, 3),
    V(10, 2, 2),
    V(10, 2, 2),
    V(10, 2, 2),
    V(10, 2, 2),

   
    V(1, 10, 2),	 
    V(1, 10, 2),
    V(0, 10, 3),
    V(10, 0, 3),
    V(3, 9, 3),
    V(9, 3, 3),
    V(5, 8, 3),
    V(8, 5, 3),

   
    V(2, 9, 2),	 
    V(2, 9, 2),
    V(9, 2, 2),
    V(9, 2, 2),
    V(7, 6, 3),
    V(0, 9, 3),
    V(1, 9, 2),
    V(1, 9, 2),

   
    V(9, 1, 2),	 
    V(9, 1, 2),
    V(9, 0, 3),
    V(4, 8, 3),
    V(8, 4, 3),
    V(7, 5, 3),
    V(3, 8, 3),
    V(8, 3, 3),

   
    V(6, 6, 3),	 
    V(2, 8, 3),
    V(8, 2, 2),
    V(8, 2, 2),
    V(4, 7, 3),
    V(7, 4, 3),
    V(1, 8, 2),
    V(1, 8, 2),

   
    V(8, 1, 2),	 
    V(8, 1, 2),
    V(8, 0, 2),
    V(8, 0, 2),
    V(0, 8, 3),
    V(5, 6, 3),
    V(3, 7, 2),
    V(3, 7, 2),

   
    V(7, 3, 2),	 
    V(7, 3, 2),
    V(6, 5, 3),
    V(4, 6, 3),
    V(2, 7, 2),
    V(2, 7, 2),
    V(7, 2, 2),
    V(7, 2, 2),

   
    V(6, 4, 3),	 
    V(5, 5, 3),
    V(0, 7, 2),
    V(0, 7, 2),
    V(1, 7, 1),
    V(1, 7, 1),
    V(1, 7, 1),
    V(1, 7, 1),

   
    V(7, 1, 1),	 
    V(7, 1, 1),
    V(7, 0, 2),
    V(3, 6, 2),

   
    V(6, 3, 2),	 
    V(4, 5, 2),
    V(5, 4, 2),
    V(2, 6, 2),

   
    V(6, 2, 1),	 
    V(1, 6, 1),

   
    V(6, 1, 1),	 
    V(6, 1, 1),
    V(0, 6, 2),
    V(6, 0, 2),

   
    V(5, 3, 1),	 
    V(5, 3, 1),
    V(3, 5, 2),
    V(4, 4, 2),

   
    V(2, 5, 1),	 
    V(5, 2, 1),

   
    V(1, 5, 1),	 
    V(0, 5, 1),

   
    V(3, 4, 1),	 
    V(4, 3, 1),

   
    V(5, 0, 1),	 
    V(2, 4, 1),

   
    V(4, 2, 1),	 
    V(3, 3, 1),

   
    V(0, 4, 1),	 
    V(4, 0, 1),

   
    V(12, 14, 4),	 
    PTR(400, 1),
    V(13, 14, 3),
    V(13, 14, 3),
    V(14, 9, 3),
    V(14, 9, 3),
    V(14, 10, 4),
    V(13, 9, 4),
    V(14, 14, 2),
    V(14, 14, 2),
    V(14, 14, 2),
    V(14, 14, 2),
    V(14, 13, 3),
    V(14, 13, 3),
    V(14, 11, 3),
    V(14, 11, 3),

   
    V(11, 14, 2),	 
    V(11, 14, 2),
    V(12, 13, 2),
    V(12, 13, 2),
    V(13, 12, 3),
    V(13, 11, 3),
    V(10, 14, 2),
    V(10, 14, 2),

   
    V(12, 12, 2),	 
    V(12, 12, 2),
    V(10, 13, 3),
    V(13, 10, 3),
    V(7, 14, 3),
    V(10, 12, 3),
    V(12, 10, 2),
    V(12, 10, 2),

   
    V(12, 9, 3),	 
    V(7, 13, 3),
    V(5, 14, 2),
    V(5, 14, 2),
    V(11, 13, 1),
    V(11, 13, 1),
    V(11, 13, 1),
    V(11, 13, 1),

   
    V(9, 14, 1),	 
    V(9, 14, 1),
    V(11, 12, 2),
    V(12, 11, 2),

   
    V(8, 14, 2),	 
    V(14, 8, 2),
    V(9, 13, 2),
    V(14, 7, 2),

   
    V(11, 11, 2),	 
    V(8, 13, 2),
    V(13, 8, 2),
    V(6, 14, 2),

   
    V(14, 6, 1),	 
    V(9, 12, 1),

   
    V(10, 11, 2),	 
    V(11, 10, 2),
    V(14, 5, 2),
    V(13, 7, 2),

   
    V(4, 14, 1),	 
    V(4, 14, 1),
    V(14, 4, 2),
    V(8, 12, 2),

   
    V(12, 8, 1),	 
    V(3, 14, 1),

   
    V(6, 13, 1),	 
    V(6, 13, 1),
    V(13, 6, 2),
    V(9, 11, 2),

   
    V(11, 9, 2),	 
    V(10, 10, 2),
    V(14, 1, 1),
    V(14, 1, 1),

   
    V(13, 4, 1),	 
    V(13, 4, 1),
    V(11, 8, 2),
    V(10, 9, 2),

   
    V(7, 11, 1),	 
    V(7, 11, 1),
    V(11, 7, 2),
    V(13, 0, 2),

   
    V(0, 14, 1),	 
    V(14, 0, 1),

   
    V(5, 13, 1),	 
    V(13, 5, 1),

   
    V(7, 12, 1),	 
    V(12, 7, 1),

   
    V(4, 13, 1),	 
    V(8, 11, 1),

   
    V(9, 10, 1),	 
    V(6, 12, 1),

   
    V(12, 6, 1),	 
    V(3, 13, 1),

   
    V(5, 12, 1),	 
    V(12, 5, 1),

   
    V(8, 10, 1),	 
    V(10, 8, 1),

   
    V(9, 9, 1),	 
    V(4, 12, 1),

   
    V(11, 6, 1),	 
    V(7, 10, 1),

   
    V(5, 11, 1),	 
    V(8, 9, 1),

   
    V(9, 8, 1),	 
    V(7, 9, 1),

   
    V(9, 7, 1),	 
    V(8, 8, 1),

   
    V(14, 12, 1),	 
    V(13, 13, 1)
};

static
union huffpair const hufftab24[] = {
    PTR(16, 4),
    PTR(32, 4),
    PTR(48, 4),
    V(15, 15, 4),
    PTR(64, 4),
    PTR(80, 4),
    PTR(96, 4),
    PTR(112, 4),
    PTR(128, 4),
    PTR(144, 4),
    PTR(160, 3),
    PTR(168, 2),
    V(1, 1, 4),
    V(0, 1, 4),
    V(1, 0, 4),
    V(0, 0, 4),

   
    V(14, 15, 4),	 
    V(15, 14, 4),
    V(13, 15, 4),
    V(15, 13, 4),
    V(12, 15, 4),
    V(15, 12, 4),
    V(11, 15, 4),
    V(15, 11, 4),
    V(15, 10, 3),
    V(15, 10, 3),
    V(10, 15, 4),
    V(9, 15, 4),
    V(15, 9, 3),
    V(15, 9, 3),
    V(15, 8, 3),
    V(15, 8, 3),

   
    V(8, 15, 4),	 
    V(7, 15, 4),
    V(15, 7, 3),
    V(15, 7, 3),
    V(6, 15, 3),
    V(6, 15, 3),
    V(15, 6, 3),
    V(15, 6, 3),
    V(5, 15, 3),
    V(5, 15, 3),
    V(15, 5, 3),
    V(15, 5, 3),
    V(4, 15, 3),
    V(4, 15, 3),
    V(15, 4, 3),
    V(15, 4, 3),

   
    V(3, 15, 3),	 
    V(3, 15, 3),
    V(15, 3, 3),
    V(15, 3, 3),
    V(2, 15, 3),
    V(2, 15, 3),
    V(15, 2, 3),
    V(15, 2, 3),
    V(15, 1, 3),
    V(15, 1, 3),
    V(1, 15, 4),
    V(15, 0, 4),
    PTR(172, 3),
    PTR(180, 3),
    PTR(188, 3),
    PTR(196, 3),

   
    PTR(204, 4),	 
    PTR(220, 3),
    PTR(228, 3),
    PTR(236, 3),
    PTR(244, 2),
    PTR(248, 2),
    PTR(252, 2),
    PTR(256, 2),
    PTR(260, 2),
    PTR(264, 2),
    PTR(268, 2),
    PTR(272, 2),
    PTR(276, 2),
    PTR(280, 3),
    PTR(288, 2),
    PTR(292, 2),

   
    PTR(296, 2),	 
    PTR(300, 3),
    PTR(308, 2),
    PTR(312, 3),
    PTR(320, 1),
    PTR(322, 2),
    PTR(326, 2),
    PTR(330, 1),
    PTR(332, 2),
    PTR(336, 1),
    PTR(338, 1),
    PTR(340, 1),
    PTR(342, 1),
    PTR(344, 1),
    PTR(346, 1),
    PTR(348, 1),

   
    PTR(350, 1),	 
    PTR(352, 1),
    PTR(354, 1),
    PTR(356, 1),
    PTR(358, 1),
    PTR(360, 1),
    PTR(362, 1),
    PTR(364, 1),
    PTR(366, 1),
    PTR(368, 1),
    PTR(370, 2),
    PTR(374, 1),
    PTR(376, 2),
    V(7, 3, 4),
    PTR(380, 1),
    V(7, 2, 4),

   
    V(4, 6, 4),	 
    V(6, 4, 4),
    V(5, 5, 4),
    V(7, 1, 4),
    V(3, 6, 4),
    V(6, 3, 4),
    V(4, 5, 4),
    V(5, 4, 4),
    V(2, 6, 4),
    V(6, 2, 4),
    V(1, 6, 4),
    V(6, 1, 4),
    PTR(382, 1),
    V(3, 5, 4),
    V(5, 3, 4),
    V(4, 4, 4),

   
    V(2, 5, 4),	 
    V(5, 2, 4),
    V(1, 5, 4),
    PTR(384, 1),
    V(5, 1, 3),
    V(5, 1, 3),
    V(3, 4, 4),
    V(4, 3, 4),
    V(2, 4, 3),
    V(2, 4, 3),
    V(4, 2, 3),
    V(4, 2, 3),
    V(3, 3, 3),
    V(3, 3, 3),
    V(1, 4, 3),
    V(1, 4, 3),

   
    V(4, 1, 3),	 
    V(4, 1, 3),
    V(0, 4, 4),
    V(4, 0, 4),
    V(2, 3, 3),
    V(2, 3, 3),
    V(3, 2, 3),
    V(3, 2, 3),
    V(1, 3, 2),
    V(1, 3, 2),
    V(1, 3, 2),
    V(1, 3, 2),
    V(3, 1, 2),
    V(3, 1, 2),
    V(3, 1, 2),
    V(3, 1, 2),

   
    V(0, 3, 3),	 
    V(3, 0, 3),
    V(2, 2, 2),
    V(2, 2, 2),
    V(1, 2, 1),
    V(1, 2, 1),
    V(1, 2, 1),
    V(1, 2, 1),

   
    V(2, 1, 1),	 
    V(2, 1, 1),
    V(0, 2, 2),
    V(2, 0, 2),

   
    V(0, 15, 1),	 
    V(0, 15, 1),
    V(0, 15, 1),
    V(0, 15, 1),
    V(14, 14, 3),
    V(13, 14, 3),
    V(14, 13, 3),
    V(12, 14, 3),

   
    V(14, 12, 3),	 
    V(13, 13, 3),
    V(11, 14, 3),
    V(14, 11, 3),
    V(12, 13, 3),
    V(13, 12, 3),
    V(10, 14, 3),
    V(14, 10, 3),

   
    V(11, 13, 3),	 
    V(13, 11, 3),
    V(12, 12, 3),
    V(9, 14, 3),
    V(14, 9, 3),
    V(10, 13, 3),
    V(13, 10, 3),
    V(11, 12, 3),

   
    V(12, 11, 3),	 
    V(8, 14, 3),
    V(14, 8, 3),
    V(9, 13, 3),
    V(13, 9, 3),
    V(7, 14, 3),
    V(14, 7, 3),
    V(10, 12, 3),

   
    V(12, 10, 3),	 
    V(12, 10, 3),
    V(11, 11, 3),
    V(11, 11, 3),
    V(8, 13, 3),
    V(8, 13, 3),
    V(13, 8, 3),
    V(13, 8, 3),
    V(0, 14, 4),
    V(14, 0, 4),
    V(0, 13, 3),
    V(0, 13, 3),
    V(14, 6, 2),
    V(14, 6, 2),
    V(14, 6, 2),
    V(14, 6, 2),

   
    V(6, 14, 3),	 
    V(9, 12, 3),
    V(12, 9, 2),
    V(12, 9, 2),
    V(5, 14, 2),
    V(5, 14, 2),
    V(11, 10, 2),
    V(11, 10, 2),

   
    V(14, 5, 2),	 
    V(14, 5, 2),
    V(10, 11, 3),
    V(7, 13, 3),
    V(13, 7, 2),
    V(13, 7, 2),
    V(14, 4, 2),
    V(14, 4, 2),

   
    V(8, 12, 2),	 
    V(8, 12, 2),
    V(12, 8, 2),
    V(12, 8, 2),
    V(4, 14, 3),
    V(2, 14, 3),
    V(3, 14, 2),
    V(3, 14, 2),

   
    V(6, 13, 2),	 
    V(13, 6, 2),
    V(14, 3, 2),
    V(9, 11, 2),

   
    V(11, 9, 2),	 
    V(10, 10, 2),
    V(14, 2, 2),
    V(1, 14, 2),

   
    V(14, 1, 2),	 
    V(5, 13, 2),
    V(13, 5, 2),
    V(7, 12, 2),

   
    V(12, 7, 2),	 
    V(4, 13, 2),
    V(8, 11, 2),
    V(11, 8, 2),

   
    V(13, 4, 2),	 
    V(9, 10, 2),
    V(10, 9, 2),
    V(6, 12, 2),

   
    V(12, 6, 2),	 
    V(3, 13, 2),
    V(13, 3, 2),
    V(2, 13, 2),

   
    V(13, 2, 2),	 
    V(1, 13, 2),
    V(7, 11, 2),
    V(11, 7, 2),

   
    V(13, 1, 2),	 
    V(5, 12, 2),
    V(12, 5, 2),
    V(8, 10, 2),

   
    V(10, 8, 2),	 
    V(9, 9, 2),
    V(4, 12, 2),
    V(12, 4, 2),

   
    V(6, 11, 2),	 
    V(6, 11, 2),
    V(11, 6, 2),
    V(11, 6, 2),
    V(13, 0, 3),
    V(0, 12, 3),
    V(3, 12, 2),
    V(3, 12, 2),

   
    V(12, 3, 2),	 
    V(7, 10, 2),
    V(10, 7, 2),
    V(2, 12, 2),

   
    V(12, 2, 2),	 
    V(5, 11, 2),
    V(11, 5, 2),
    V(1, 12, 2),

   
    V(8, 9, 2),	 
    V(9, 8, 2),
    V(12, 1, 2),
    V(4, 11, 2),

   
    V(12, 0, 3),	 
    V(0, 11, 3),
    V(3, 11, 2),
    V(3, 11, 2),
    V(11, 0, 3),
    V(0, 10, 3),
    V(1, 10, 2),
    V(1, 10, 2),

   
    V(11, 4, 1),	 
    V(11, 4, 1),
    V(6, 10, 2),
    V(10, 6, 2),

   
    V(7, 9, 2),	 
    V(7, 9, 2),
    V(9, 7, 2),
    V(9, 7, 2),
    V(10, 0, 3),
    V(0, 9, 3),
    V(9, 0, 2),
    V(9, 0, 2),

   
    V(11, 3, 1),	 
    V(8, 8, 1),

   
    V(2, 11, 2),	 
    V(5, 10, 2),
    V(11, 2, 1),
    V(11, 2, 1),

   
    V(10, 5, 2),	 
    V(1, 11, 2),
    V(11, 1, 2),
    V(6, 9, 2),

   
    V(9, 6, 1),	 
    V(10, 4, 1),

   
    V(4, 10, 2),	 
    V(7, 8, 2),
    V(8, 7, 1),
    V(8, 7, 1),

   
    V(3, 10, 1),	 
    V(10, 3, 1),

   
    V(5, 9, 1),	 
    V(9, 5, 1),

   
    V(2, 10, 1),	 
    V(10, 2, 1),

   
    V(10, 1, 1),	 
    V(6, 8, 1),

   
    V(8, 6, 1),	 
    V(7, 7, 1),

   
    V(4, 9, 1),	 
    V(9, 4, 1),

   
    V(3, 9, 1),	 
    V(9, 3, 1),

   
    V(5, 8, 1),	 
    V(8, 5, 1),

   
    V(2, 9, 1),	 
    V(6, 7, 1),

   
    V(7, 6, 1),	 
    V(9, 2, 1),

   
    V(1, 9, 1),	 
    V(9, 1, 1),

   
    V(4, 8, 1),	 
    V(8, 4, 1),

   
    V(5, 7, 1),	 
    V(7, 5, 1),

   
    V(3, 8, 1),	 
    V(8, 3, 1),

   
    V(6, 6, 1),	 
    V(2, 8, 1),

   
    V(8, 2, 1),	 
    V(1, 8, 1),

   
    V(4, 7, 1),	 
    V(7, 4, 1),

   
    V(8, 1, 1),	 
    V(8, 1, 1),
    V(0, 8, 2),
    V(8, 0, 2),

   
    V(5, 6, 1),	 
    V(6, 5, 1),

   
    V(1, 7, 1),	 
    V(1, 7, 1),
    V(0, 7, 2),
    V(7, 0, 2),

   
    V(3, 7, 1),	 
    V(2, 7, 1),

   
    V(0, 6, 1),	 
    V(6, 0, 1),

   
    V(0, 5, 1),	 
    V(5, 0, 1)
};

# undef V
# undef PTR

 

union huffquad const *const mad_huff_quad_table[2] = { hufftabA, hufftabB };

struct hufftable const mad_huff_pair_table[32] = {
    { hufftab0,   0, 0 },
    { hufftab1,   0, 3 },
    { hufftab2,   0, 3 },
    { hufftab3,   0, 3 },
    { 0   },
    { hufftab5,   0, 3 },
    { hufftab6,   0, 4 },
    { hufftab7,   0, 4 },
    { hufftab8,   0, 4 },
    { hufftab9,   0, 4 },
    { hufftab10,  0, 4 },
    { hufftab11,  0, 4 },
    { hufftab12,  0, 4 },
    { hufftab13,  0, 4 },
    { 0   },
    { hufftab15,  0, 4 },
    { hufftab16,  1, 4 },
    { hufftab16,  2, 4 },
    { hufftab16,  3, 4 },
    { hufftab16,  4, 4 },
    { hufftab16,  6, 4 },
    { hufftab16,  8, 4 },
    { hufftab16, 10, 4 },
    { hufftab16, 13, 4 },
    { hufftab24,  4, 4 },
    { hufftab24,  5, 4 },
    { hufftab24,  6, 4 },
    { hufftab24,  7, 4 },
    { hufftab24,  8, 4 },
    { hufftab24,  9, 4 },
    { hufftab24, 11, 4 },
    { hufftab24, 13, 4 }
};


 




 

enum {
  count1table_select = 0x01,
  scalefac_scale     = 0x02,
  preflag	     = 0x04,
  mixed_block_flag   = 0x08
};

enum {
  I_STEREO  = 0x1,
  MS_STEREO = 0x2
};

struct channel {
       
      unsigned short part2_3_length;
      unsigned short big_values;
      unsigned short global_gain;
      unsigned short scalefac_compress;

      unsigned char flags;
      unsigned char block_type;
      unsigned char table_select[3];
      unsigned char subblock_gain[3];
      unsigned char region0_count;
      unsigned char region1_count;

       
      unsigned char scalefac[39];	 
} ;

struct granule {
    struct channel ch[2];
} ;


struct sideinfo {
  unsigned int main_data_begin;
  unsigned int private_bits;

  unsigned char scfsi[2];

  struct granule gr[2];
};

 
 
static
struct {
  unsigned char slen1;
  unsigned char slen2;
} const sflen_table[16] = {
  { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
  { 3, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
  { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 },
  { 3, 2 }, { 3, 3 }, { 4, 2 }, { 4, 3 }
};

 
static
unsigned char const nsfb_table[6][3][4] = {
  { {  6,  5,  5, 5 },
    {  9,  9,  9, 9 },
    {  6,  9,  9, 9 } },

  { {  6,  5,  7, 3 },
    {  9,  9, 12, 6 },
    {  6,  9, 12, 6 } },

  { { 11, 10,  0, 0 },
    { 18, 18,  0, 0 },
    { 15, 18,  0, 0 } },

  { {  7,  7,  7, 0 },
    { 12, 12, 12, 0 },
    {  6, 15, 12, 0 } },

  { {  6,  6,  6, 3 },
    { 12,  9,  9, 6 },
    {  6, 12,  9, 6 } },

  { {  8,  8,  5, 0 },
    { 15, 12,  9, 0 },
    {  6, 18,  9, 0 } }
};

 
static
unsigned char const sfb_48000_long[] = {
   4,  4,  4,  4,  4,  4,  6,  6,  6,   8,  10,
  12, 16, 18, 22, 28, 34, 40, 46, 54,  54, 192
};

static
unsigned char const sfb_44100_long[] = {
   4,  4,  4,  4,  4,  4,  6,  6,  8,   8,  10,
  12, 16, 20, 24, 28, 34, 42, 50, 54,  76, 158
};

static
unsigned char const sfb_32000_long[] = {
   4,  4,  4,  4,  4,  4,  6,  6,  8,  10,  12,
  16, 20, 24, 30, 38, 46, 56, 68, 84, 102,  26
};

static
unsigned char const sfb_48000_short[] = {
   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
   6,  6,  6,  6,  6, 10, 10, 10, 12, 12, 12, 14, 14,
  14, 16, 16, 16, 20, 20, 20, 26, 26, 26, 66, 66, 66
};

static
unsigned char const sfb_44100_short[] = {
   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
   6,  6,  8,  8,  8, 10, 10, 10, 12, 12, 12, 14, 14,
  14, 18, 18, 18, 22, 22, 22, 30, 30, 30, 56, 56, 56
};

static
unsigned char const sfb_32000_short[] = {
   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
   6,  6,  8,  8,  8, 12, 12, 12, 16, 16, 16, 20, 20,
  20, 26, 26, 26, 34, 34, 34, 42, 42, 42, 12, 12, 12
};

static
unsigned char const sfb_48000_mixed[] = {
      4,  4,  4,  4,  4,  4,  6,  6,
     4,  4,  4,  6,  6,  6,  6,  6,  6, 10,
              10, 10, 12, 12, 12, 14, 14, 14, 16, 16,
              16, 20, 20, 20, 26, 26, 26, 66, 66, 66
};

static
unsigned char const sfb_44100_mixed[] = {
      4,  4,  4,  4,  4,  4,  6,  6,
     4,  4,  4,  6,  6,  6,  8,  8,  8, 10,
              10, 10, 12, 12, 12, 14, 14, 14, 18, 18,
              18, 22, 22, 22, 30, 30, 30, 56, 56, 56
};

static
unsigned char const sfb_32000_mixed[] = {
      4,  4,  4,  4,  4,  4,  6,  6,
     4,  4,  4,  6,  6,  6,  8,  8,  8, 12,
              12, 12, 16, 16, 16, 20, 20, 20, 26, 26,
              26, 34, 34, 34, 42, 42, 42, 12, 12, 12
};

 
static
unsigned char const sfb_24000_long[] = {
   6,  6,  6,  6,  6,  6,  8, 10, 12,  14,  16,
  18, 22, 26, 32, 38, 46, 54, 62, 70,  76,  36
};

static
unsigned char const sfb_22050_long[] = {
   6,  6,  6,  6,  6,  6,  8, 10, 12,  14,  16,
  20, 24, 28, 32, 38, 46, 52, 60, 68,  58,  54
};

# define sfb_16000_long  sfb_22050_long

static
unsigned char const sfb_24000_short[] = {
   4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  8,
   8,  8, 10, 10, 10, 12, 12, 12, 14, 14, 14, 18, 18,
  18, 24, 24, 24, 32, 32, 32, 44, 44, 44, 12, 12, 12
};

static
unsigned char const sfb_22050_short[] = {
   4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  6,
   6,  6,  8,  8,  8, 10, 10, 10, 14, 14, 14, 18, 18,
  18, 26, 26, 26, 32, 32, 32, 42, 42, 42, 18, 18, 18
};

static
unsigned char const sfb_16000_short[] = {
   4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  8,
   8,  8, 10, 10, 10, 12, 12, 12, 14, 14, 14, 18, 18,
  18, 24, 24, 24, 30, 30, 30, 40, 40, 40, 18, 18, 18
};

static
unsigned char const sfb_24000_mixed[] = {
      6,  6,  6,  6,  6,  6,
     6,  6,  6,  8,  8,  8, 10, 10, 10, 12,
              12, 12, 14, 14, 14, 18, 18, 18, 24, 24,
              24, 32, 32, 32, 44, 44, 44, 12, 12, 12
};

static
unsigned char const sfb_22050_mixed[] = {
      6,  6,  6,  6,  6,  6,
     6,  6,  6,  6,  6,  6,  8,  8,  8, 10,
              10, 10, 14, 14, 14, 18, 18, 18, 26, 26,
              26, 32, 32, 32, 42, 42, 42, 18, 18, 18
};

static
unsigned char const sfb_16000_mixed[] = {
      6,  6,  6,  6,  6,  6,
     6,  6,  6,  8,  8,  8, 10, 10, 10, 12,
              12, 12, 14, 14, 14, 18, 18, 18, 24, 24,
              24, 30, 30, 30, 40, 40, 40, 18, 18, 18
};

 
# define sfb_12000_long  sfb_16000_long
# define sfb_11025_long  sfb_12000_long

static
unsigned char const sfb_8000_long[] = {
  12, 12, 12, 12, 12, 12, 16, 20, 24,  28,  32,
  40, 48, 56, 64, 76, 90,  2,  2,  2,   2,   2
};

# define sfb_12000_short  sfb_16000_short
# define sfb_11025_short  sfb_12000_short

static
unsigned char const sfb_8000_short[] = {
   8,  8,  8,  8,  8,  8,  8,  8,  8, 12, 12, 12, 16,
  16, 16, 20, 20, 20, 24, 24, 24, 28, 28, 28, 36, 36,
  36,  2,  2,  2,  2,  2,  2,  2,  2,  2, 26, 26, 26
};

# define sfb_12000_mixed  sfb_16000_mixed
# define sfb_11025_mixed  sfb_12000_mixed

 
static
unsigned char const sfb_8000_mixed[] = {
     12, 12, 12,
     4,  4,  4,  8,  8,  8, 12, 12, 12, 16, 16, 16,
              20, 20, 20, 24, 24, 24, 28, 28, 28, 36, 36, 36,
               2,  2,  2,  2,  2,  2,  2,  2,  2, 26, 26, 26
};

static
struct {
  unsigned char const *l;
  unsigned char const *s;
  unsigned char const *m;
} const sfbwidth_table[9] = {
  { sfb_48000_long, sfb_48000_short, sfb_48000_mixed },
  { sfb_44100_long, sfb_44100_short, sfb_44100_mixed },
  { sfb_32000_long, sfb_32000_short, sfb_32000_mixed },
  { sfb_24000_long, sfb_24000_short, sfb_24000_mixed },
  { sfb_22050_long, sfb_22050_short, sfb_22050_mixed },
  { sfb_16000_long, sfb_16000_short, sfb_16000_mixed },
  { sfb_12000_long, sfb_12000_short, sfb_12000_mixed },
  { sfb_11025_long, sfb_11025_short, sfb_11025_mixed },
  {  sfb_8000_long,  sfb_8000_short,  sfb_8000_mixed }
};

 
static
unsigned char const pretab[22] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0
};

 
static
struct fixedfloat {
  unsigned long mantissa  : 27;
  unsigned short exponent :  5;
} const rq_table[8207] = {

     { MAD_F(0x00000000)  ,  0 },
     { MAD_F(0x04000000)  ,  2 },
     { MAD_F(0x050a28be)  ,  3 },
     { MAD_F(0x0453a5cd)  ,  4 },
     { MAD_F(0x06597fa9)  ,  4 },
     { MAD_F(0x04466275)  ,  5 },
     { MAD_F(0x05738c72)  ,  5 },
     { MAD_F(0x06b1fc81)  ,  5 },
     { MAD_F(0x04000000)  ,  6 },
     { MAD_F(0x04ae20d7)  ,  6 },
     { MAD_F(0x0562d694)  ,  6 },
     { MAD_F(0x061dae96)  ,  6 },
     { MAD_F(0x06de47f4)  ,  6 },
     { MAD_F(0x07a44f7a)  ,  6 },
     { MAD_F(0x0437be65)  ,  7 },
     { MAD_F(0x049fc824)  ,  7 },

     { MAD_F(0x050a28be)  ,  7 },
     { MAD_F(0x0576c6f5)  ,  7 },
     { MAD_F(0x05e58c0b)  ,  7 },
     { MAD_F(0x06566361)  ,  7 },
     { MAD_F(0x06c93a2e)  ,  7 },
     { MAD_F(0x073dff3e)  ,  7 },
     { MAD_F(0x07b4a2bc)  ,  7 },
     { MAD_F(0x04168b05)  ,  8 },
     { MAD_F(0x0453a5cd)  ,  8 },
     { MAD_F(0x04919b6a)  ,  8 },
     { MAD_F(0x04d065fb)  ,  8 },
     { MAD_F(0x05100000)  ,  8 },
     { MAD_F(0x05506451)  ,  8 },
     { MAD_F(0x05918e15)  ,  8 },
     { MAD_F(0x05d378bb)  ,  8 },
     { MAD_F(0x06161ff3)  ,  8 },

     { MAD_F(0x06597fa9)  ,  8 },
     { MAD_F(0x069d9400)  ,  8 },
     { MAD_F(0x06e2594c)  ,  8 },
     { MAD_F(0x0727cc11)  ,  8 },
     { MAD_F(0x076de8fc)  ,  8 },
     { MAD_F(0x07b4ace3)  ,  8 },
     { MAD_F(0x07fc14bf)  ,  8 },
     { MAD_F(0x04220ed7)  ,  9 },
     { MAD_F(0x04466275)  ,  9 },
     { MAD_F(0x046b03e7)  ,  9 },
     { MAD_F(0x048ff1e8)  ,  9 },
     { MAD_F(0x04b52b3f)  ,  9 },
     { MAD_F(0x04daaec0)  ,  9 },
     { MAD_F(0x05007b49)  ,  9 },
     { MAD_F(0x05268fc6)  ,  9 },
     { MAD_F(0x054ceb2a)  ,  9 },

     { MAD_F(0x05738c72)  ,  9 },
     { MAD_F(0x059a72a5)  ,  9 },
     { MAD_F(0x05c19cd3)  ,  9 },
     { MAD_F(0x05e90a12)  ,  9 },
     { MAD_F(0x0610b982)  ,  9 },
     { MAD_F(0x0638aa48)  ,  9 },
     { MAD_F(0x0660db91)  ,  9 },
     { MAD_F(0x06894c90)  ,  9 },
     { MAD_F(0x06b1fc81)  ,  9 },
     { MAD_F(0x06daeaa1)  ,  9 },
     { MAD_F(0x07041636)  ,  9 },
     { MAD_F(0x072d7e8b)  ,  9 },
     { MAD_F(0x075722ef)  ,  9 },
     { MAD_F(0x078102b8)  ,  9 },
     { MAD_F(0x07ab1d3e)  ,  9 },
     { MAD_F(0x07d571e0)  ,  9 },

     { MAD_F(0x04000000)  , 10 },
     { MAD_F(0x04156381)  , 10 },
     { MAD_F(0x042ae32a)  , 10 },
     { MAD_F(0x04407eb1)  , 10 },
     { MAD_F(0x045635cf)  , 10 },
     { MAD_F(0x046c083e)  , 10 },
     { MAD_F(0x0481f5bb)  , 10 },
     { MAD_F(0x0497fe03)  , 10 },
     { MAD_F(0x04ae20d7)  , 10 },
     { MAD_F(0x04c45df6)  , 10 },
     { MAD_F(0x04dab524)  , 10 },
     { MAD_F(0x04f12624)  , 10 },
     { MAD_F(0x0507b0bc)  , 10 },
     { MAD_F(0x051e54b1)  , 10 },
     { MAD_F(0x053511cb)  , 10 },
     { MAD_F(0x054be7d4)  , 10 },

     { MAD_F(0x0562d694)  , 10 },
     { MAD_F(0x0579ddd8)  , 10 },
     { MAD_F(0x0590fd6c)  , 10 },
     { MAD_F(0x05a8351c)  , 10 },
     { MAD_F(0x05bf84b8)  , 10 },
     { MAD_F(0x05d6ec0e)  , 10 },
     { MAD_F(0x05ee6aef)  , 10 },
     { MAD_F(0x0606012b)  , 10 },
     { MAD_F(0x061dae96)  , 10 },
     { MAD_F(0x06357302)  , 10 },
     { MAD_F(0x064d4e43)  , 10 },
     { MAD_F(0x0665402d)  , 10 },
     { MAD_F(0x067d4896)  , 10 },
     { MAD_F(0x06956753)  , 10 },
     { MAD_F(0x06ad9c3d)  , 10 },
     { MAD_F(0x06c5e72b)  , 10 },

     { MAD_F(0x06de47f4)  , 10 },
     { MAD_F(0x06f6be73)  , 10 },
     { MAD_F(0x070f4a80)  , 10 },
     { MAD_F(0x0727ebf7)  , 10 },
     { MAD_F(0x0740a2b2)  , 10 },
     { MAD_F(0x07596e8d)  , 10 },
     { MAD_F(0x07724f64)  , 10 },
     { MAD_F(0x078b4514)  , 10 },
     { MAD_F(0x07a44f7a)  , 10 },
     { MAD_F(0x07bd6e75)  , 10 },
     { MAD_F(0x07d6a1e2)  , 10 },
     { MAD_F(0x07efe9a1)  , 10 },
     { MAD_F(0x0404a2c9)  , 11 },
     { MAD_F(0x04115aca)  , 11 },
     { MAD_F(0x041e1cc4)  , 11 },
     { MAD_F(0x042ae8a7)  , 11 },

     { MAD_F(0x0437be65)  , 11 },
     { MAD_F(0x04449dee)  , 11 },
     { MAD_F(0x04518733)  , 11 },
     { MAD_F(0x045e7a26)  , 11 },
     { MAD_F(0x046b76b9)  , 11 },
     { MAD_F(0x04787cdc)  , 11 },
     { MAD_F(0x04858c83)  , 11 },
     { MAD_F(0x0492a59f)  , 11 },
     { MAD_F(0x049fc824)  , 11 },
     { MAD_F(0x04acf402)  , 11 },
     { MAD_F(0x04ba292e)  , 11 },
     { MAD_F(0x04c7679a)  , 11 },
     { MAD_F(0x04d4af3a)  , 11 },
     { MAD_F(0x04e20000)  , 11 },
     { MAD_F(0x04ef59e0)  , 11 },
     { MAD_F(0x04fcbcce)  , 11 },

     { MAD_F(0x050a28be)  , 11 },
     { MAD_F(0x05179da4)  , 11 },
     { MAD_F(0x05251b73)  , 11 },
     { MAD_F(0x0532a220)  , 11 },
     { MAD_F(0x054031a0)  , 11 },
     { MAD_F(0x054dc9e7)  , 11 },
     { MAD_F(0x055b6ae9)  , 11 },
     { MAD_F(0x0569149c)  , 11 },
     { MAD_F(0x0576c6f5)  , 11 },
     { MAD_F(0x058481e9)  , 11 },
     { MAD_F(0x0592456d)  , 11 },
     { MAD_F(0x05a01176)  , 11 },
     { MAD_F(0x05ade5fa)  , 11 },
     { MAD_F(0x05bbc2ef)  , 11 },
     { MAD_F(0x05c9a84a)  , 11 },
     { MAD_F(0x05d79601)  , 11 },

     { MAD_F(0x05e58c0b)  , 11 },
     { MAD_F(0x05f38a5d)  , 11 },
     { MAD_F(0x060190ee)  , 11 },
     { MAD_F(0x060f9fb3)  , 11 },
     { MAD_F(0x061db6a5)  , 11 },
     { MAD_F(0x062bd5b8)  , 11 },
     { MAD_F(0x0639fce4)  , 11 },
     { MAD_F(0x06482c1f)  , 11 },
     { MAD_F(0x06566361)  , 11 },
     { MAD_F(0x0664a2a0)  , 11 },
     { MAD_F(0x0672e9d4)  , 11 },
     { MAD_F(0x068138f3)  , 11 },
     { MAD_F(0x068f8ff5)  , 11 },
     { MAD_F(0x069deed1)  , 11 },
     { MAD_F(0x06ac557f)  , 11 },
     { MAD_F(0x06bac3f6)  , 11 },

     { MAD_F(0x06c93a2e)  , 11 },
     { MAD_F(0x06d7b81f)  , 11 },
     { MAD_F(0x06e63dc0)  , 11 },
     { MAD_F(0x06f4cb09)  , 11 },
     { MAD_F(0x07035ff3)  , 11 },
     { MAD_F(0x0711fc75)  , 11 },
     { MAD_F(0x0720a087)  , 11 },
     { MAD_F(0x072f4c22)  , 11 },
     { MAD_F(0x073dff3e)  , 11 },
     { MAD_F(0x074cb9d3)  , 11 },
     { MAD_F(0x075b7bdb)  , 11 },
     { MAD_F(0x076a454c)  , 11 },
     { MAD_F(0x07791620)  , 11 },
     { MAD_F(0x0787ee50)  , 11 },
     { MAD_F(0x0796cdd4)  , 11 },
     { MAD_F(0x07a5b4a5)  , 11 },

     { MAD_F(0x07b4a2bc)  , 11 },
     { MAD_F(0x07c39812)  , 11 },
     { MAD_F(0x07d294a0)  , 11 },
     { MAD_F(0x07e1985f)  , 11 },
     { MAD_F(0x07f0a348)  , 11 },
     { MAD_F(0x07ffb554)  , 11 },
     { MAD_F(0x0407673f)  , 12 },
     { MAD_F(0x040ef75e)  , 12 },
     { MAD_F(0x04168b05)  , 12 },
     { MAD_F(0x041e2230)  , 12 },
     { MAD_F(0x0425bcdd)  , 12 },
     { MAD_F(0x042d5b07)  , 12 },
     { MAD_F(0x0434fcad)  , 12 },
     { MAD_F(0x043ca1c9)  , 12 },
     { MAD_F(0x04444a5a)  , 12 },
     { MAD_F(0x044bf65d)  , 12 },

     { MAD_F(0x0453a5cd)  , 12 },
     { MAD_F(0x045b58a9)  , 12 },
     { MAD_F(0x04630eed)  , 12 },
     { MAD_F(0x046ac896)  , 12 },
     { MAD_F(0x047285a2)  , 12 },
     { MAD_F(0x047a460c)  , 12 },
     { MAD_F(0x048209d3)  , 12 },
     { MAD_F(0x0489d0f4)  , 12 },
     { MAD_F(0x04919b6a)  , 12 },
     { MAD_F(0x04996935)  , 12 },
     { MAD_F(0x04a13a50)  , 12 },
     { MAD_F(0x04a90eba)  , 12 },
     { MAD_F(0x04b0e66e)  , 12 },
     { MAD_F(0x04b8c16c)  , 12 },
     { MAD_F(0x04c09faf)  , 12 },
     { MAD_F(0x04c88135)  , 12 },

     { MAD_F(0x04d065fb)  , 12 },
     { MAD_F(0x04d84dff)  , 12 },
     { MAD_F(0x04e0393e)  , 12 },
     { MAD_F(0x04e827b6)  , 12 },
     { MAD_F(0x04f01963)  , 12 },
     { MAD_F(0x04f80e44)  , 12 },
     { MAD_F(0x05000655)  , 12 },
     { MAD_F(0x05080195)  , 12 },
     { MAD_F(0x05100000)  , 12 },
     { MAD_F(0x05180194)  , 12 },
     { MAD_F(0x0520064f)  , 12 },
     { MAD_F(0x05280e2d)  , 12 },
     { MAD_F(0x0530192e)  , 12 },
     { MAD_F(0x0538274e)  , 12 },
     { MAD_F(0x0540388a)  , 12 },
     { MAD_F(0x05484ce2)  , 12 },

     { MAD_F(0x05506451)  , 12 },
     { MAD_F(0x05587ed5)  , 12 },
     { MAD_F(0x05609c6e)  , 12 },
     { MAD_F(0x0568bd17)  , 12 },
     { MAD_F(0x0570e0cf)  , 12 },
     { MAD_F(0x05790793)  , 12 },
     { MAD_F(0x05813162)  , 12 },
     { MAD_F(0x05895e39)  , 12 },
     { MAD_F(0x05918e15)  , 12 },
     { MAD_F(0x0599c0f4)  , 12 },
     { MAD_F(0x05a1f6d5)  , 12 },
     { MAD_F(0x05aa2fb5)  , 12 },
     { MAD_F(0x05b26b92)  , 12 },
     { MAD_F(0x05baaa69)  , 12 },
     { MAD_F(0x05c2ec39)  , 12 },
     { MAD_F(0x05cb3100)  , 12 },

     { MAD_F(0x05d378bb)  , 12 },
     { MAD_F(0x05dbc368)  , 12 },
     { MAD_F(0x05e41105)  , 12 },
     { MAD_F(0x05ec6190)  , 12 },
     { MAD_F(0x05f4b507)  , 12 },
     { MAD_F(0x05fd0b68)  , 12 },
     { MAD_F(0x060564b1)  , 12 },
     { MAD_F(0x060dc0e0)  , 12 },
     { MAD_F(0x06161ff3)  , 12 },
     { MAD_F(0x061e81e8)  , 12 },
     { MAD_F(0x0626e6bc)  , 12 },
     { MAD_F(0x062f4e6f)  , 12 },
     { MAD_F(0x0637b8fd)  , 12 },
     { MAD_F(0x06402666)  , 12 },
     { MAD_F(0x064896a7)  , 12 },
     { MAD_F(0x065109be)  , 12 },

     { MAD_F(0x06597fa9)  , 12 },
     { MAD_F(0x0661f867)  , 12 },
     { MAD_F(0x066a73f5)  , 12 },
     { MAD_F(0x0672f252)  , 12 },
     { MAD_F(0x067b737c)  , 12 },
     { MAD_F(0x0683f771)  , 12 },
     { MAD_F(0x068c7e2f)  , 12 },
     { MAD_F(0x069507b5)  , 12 },
     { MAD_F(0x069d9400)  , 12 },
     { MAD_F(0x06a6230f)  , 12 },
     { MAD_F(0x06aeb4e0)  , 12 },
     { MAD_F(0x06b74971)  , 12 },
     { MAD_F(0x06bfe0c0)  , 12 },
     { MAD_F(0x06c87acc)  , 12 },
     { MAD_F(0x06d11794)  , 12 },
     { MAD_F(0x06d9b714)  , 12 },

     { MAD_F(0x06e2594c)  , 12 },
     { MAD_F(0x06eafe3a)  , 12 },
     { MAD_F(0x06f3a5dc)  , 12 },
     { MAD_F(0x06fc5030)  , 12 },
     { MAD_F(0x0704fd35)  , 12 },
     { MAD_F(0x070dacea)  , 12 },
     { MAD_F(0x07165f4b)  , 12 },
     { MAD_F(0x071f1459)  , 12 },
     { MAD_F(0x0727cc11)  , 12 },
     { MAD_F(0x07308671)  , 12 },
     { MAD_F(0x07394378)  , 12 },
     { MAD_F(0x07420325)  , 12 },
     { MAD_F(0x074ac575)  , 12 },
     { MAD_F(0x07538a67)  , 12 },
     { MAD_F(0x075c51fa)  , 12 },
     { MAD_F(0x07651c2c)  , 12 },

     { MAD_F(0x076de8fc)  , 12 },
     { MAD_F(0x0776b867)  , 12 },
     { MAD_F(0x077f8a6d)  , 12 },
     { MAD_F(0x07885f0b)  , 12 },
     { MAD_F(0x07913641)  , 12 },
     { MAD_F(0x079a100c)  , 12 },
     { MAD_F(0x07a2ec6c)  , 12 },
     { MAD_F(0x07abcb5f)  , 12 },
     { MAD_F(0x07b4ace3)  , 12 },
     { MAD_F(0x07bd90f6)  , 12 },
     { MAD_F(0x07c67798)  , 12 },
     { MAD_F(0x07cf60c7)  , 12 },
     { MAD_F(0x07d84c81)  , 12 },
     { MAD_F(0x07e13ac5)  , 12 },
     { MAD_F(0x07ea2b92)  , 12 },
     { MAD_F(0x07f31ee6)  , 12 },

     { MAD_F(0x07fc14bf)  , 12 },
     { MAD_F(0x0402868e)  , 13 },
     { MAD_F(0x040703ff)  , 13 },
     { MAD_F(0x040b82b0)  , 13 },
     { MAD_F(0x041002a1)  , 13 },
     { MAD_F(0x041483d1)  , 13 },
     { MAD_F(0x04190640)  , 13 },
     { MAD_F(0x041d89ed)  , 13 },
     { MAD_F(0x04220ed7)  , 13 },
     { MAD_F(0x042694fe)  , 13 },
     { MAD_F(0x042b1c60)  , 13 },
     { MAD_F(0x042fa4fe)  , 13 },
     { MAD_F(0x04342ed7)  , 13 },
     { MAD_F(0x0438b9e9)  , 13 },
     { MAD_F(0x043d4635)  , 13 },
     { MAD_F(0x0441d3b9)  , 13 },

     { MAD_F(0x04466275)  , 13 },
     { MAD_F(0x044af269)  , 13 },
     { MAD_F(0x044f8393)  , 13 },
     { MAD_F(0x045415f3)  , 13 },
     { MAD_F(0x0458a989)  , 13 },
     { MAD_F(0x045d3e53)  , 13 },
     { MAD_F(0x0461d451)  , 13 },
     { MAD_F(0x04666b83)  , 13 },
     { MAD_F(0x046b03e7)  , 13 },
     { MAD_F(0x046f9d7e)  , 13 },
     { MAD_F(0x04743847)  , 13 },
     { MAD_F(0x0478d440)  , 13 },
     { MAD_F(0x047d716a)  , 13 },
     { MAD_F(0x04820fc3)  , 13 },
     { MAD_F(0x0486af4c)  , 13 },
     { MAD_F(0x048b5003)  , 13 },

     { MAD_F(0x048ff1e8)  , 13 },
     { MAD_F(0x049494fb)  , 13 },
     { MAD_F(0x0499393a)  , 13 },
     { MAD_F(0x049ddea5)  , 13 },
     { MAD_F(0x04a2853c)  , 13 },
     { MAD_F(0x04a72cfe)  , 13 },
     { MAD_F(0x04abd5ea)  , 13 },
     { MAD_F(0x04b08000)  , 13 },
     { MAD_F(0x04b52b3f)  , 13 },
     { MAD_F(0x04b9d7a7)  , 13 },
     { MAD_F(0x04be8537)  , 13 },
     { MAD_F(0x04c333ee)  , 13 },
     { MAD_F(0x04c7e3cc)  , 13 },
     { MAD_F(0x04cc94d1)  , 13 },
     { MAD_F(0x04d146fb)  , 13 },
     { MAD_F(0x04d5fa4b)  , 13 },

     { MAD_F(0x04daaec0)  , 13 },
     { MAD_F(0x04df6458)  , 13 },
     { MAD_F(0x04e41b14)  , 13 },
     { MAD_F(0x04e8d2f3)  , 13 },
     { MAD_F(0x04ed8bf5)  , 13 },
     { MAD_F(0x04f24618)  , 13 },
     { MAD_F(0x04f7015d)  , 13 },
     { MAD_F(0x04fbbdc3)  , 13 },
     { MAD_F(0x05007b49)  , 13 },
     { MAD_F(0x050539ef)  , 13 },
     { MAD_F(0x0509f9b4)  , 13 },
     { MAD_F(0x050eba98)  , 13 },
     { MAD_F(0x05137c9a)  , 13 },
     { MAD_F(0x05183fba)  , 13 },
     { MAD_F(0x051d03f7)  , 13 },
     { MAD_F(0x0521c950)  , 13 },

     { MAD_F(0x05268fc6)  , 13 },
     { MAD_F(0x052b5757)  , 13 },
     { MAD_F(0x05302003)  , 13 },
     { MAD_F(0x0534e9ca)  , 13 },
     { MAD_F(0x0539b4ab)  , 13 },
     { MAD_F(0x053e80a6)  , 13 },
     { MAD_F(0x05434db9)  , 13 },
     { MAD_F(0x05481be5)  , 13 },
     { MAD_F(0x054ceb2a)  , 13 },
     { MAD_F(0x0551bb85)  , 13 },
     { MAD_F(0x05568cf8)  , 13 },
     { MAD_F(0x055b5f81)  , 13 },
     { MAD_F(0x05603321)  , 13 },
     { MAD_F(0x056507d6)  , 13 },
     { MAD_F(0x0569dda0)  , 13 },
     { MAD_F(0x056eb47f)  , 13 },

     { MAD_F(0x05738c72)  , 13 },
     { MAD_F(0x05786578)  , 13 },
     { MAD_F(0x057d3f92)  , 13 },
     { MAD_F(0x05821abf)  , 13 },
     { MAD_F(0x0586f6fd)  , 13 },
     { MAD_F(0x058bd44e)  , 13 },
     { MAD_F(0x0590b2b0)  , 13 },
     { MAD_F(0x05959222)  , 13 },
     { MAD_F(0x059a72a5)  , 13 },
     { MAD_F(0x059f5438)  , 13 },
     { MAD_F(0x05a436da)  , 13 },
     { MAD_F(0x05a91a8c)  , 13 },
     { MAD_F(0x05adff4c)  , 13 },
     { MAD_F(0x05b2e51a)  , 13 },
     { MAD_F(0x05b7cbf5)  , 13 },
     { MAD_F(0x05bcb3de)  , 13 },

     { MAD_F(0x05c19cd3)  , 13 },
     { MAD_F(0x05c686d5)  , 13 },
     { MAD_F(0x05cb71e2)  , 13 },
     { MAD_F(0x05d05dfb)  , 13 },
     { MAD_F(0x05d54b1f)  , 13 },
     { MAD_F(0x05da394d)  , 13 },
     { MAD_F(0x05df2885)  , 13 },
     { MAD_F(0x05e418c7)  , 13 },
     { MAD_F(0x05e90a12)  , 13 },
     { MAD_F(0x05edfc66)  , 13 },
     { MAD_F(0x05f2efc2)  , 13 },
     { MAD_F(0x05f7e426)  , 13 },
     { MAD_F(0x05fcd992)  , 13 },
     { MAD_F(0x0601d004)  , 13 },
     { MAD_F(0x0606c77d)  , 13 },
     { MAD_F(0x060bbffd)  , 13 },

     { MAD_F(0x0610b982)  , 13 },
     { MAD_F(0x0615b40c)  , 13 },
     { MAD_F(0x061aaf9c)  , 13 },
     { MAD_F(0x061fac2f)  , 13 },
     { MAD_F(0x0624a9c7)  , 13 },
     { MAD_F(0x0629a863)  , 13 },
     { MAD_F(0x062ea802)  , 13 },
     { MAD_F(0x0633a8a3)  , 13 },
     { MAD_F(0x0638aa48)  , 13 },
     { MAD_F(0x063dacee)  , 13 },
     { MAD_F(0x0642b096)  , 13 },
     { MAD_F(0x0647b53f)  , 13 },
     { MAD_F(0x064cbae9)  , 13 },
     { MAD_F(0x0651c193)  , 13 },
     { MAD_F(0x0656c93d)  , 13 },
     { MAD_F(0x065bd1e7)  , 13 },

     { MAD_F(0x0660db91)  , 13 },
     { MAD_F(0x0665e639)  , 13 },
     { MAD_F(0x066af1df)  , 13 },
     { MAD_F(0x066ffe84)  , 13 },
     { MAD_F(0x06750c26)  , 13 },
     { MAD_F(0x067a1ac6)  , 13 },
     { MAD_F(0x067f2a62)  , 13 },
     { MAD_F(0x06843afb)  , 13 },
     { MAD_F(0x06894c90)  , 13 },
     { MAD_F(0x068e5f21)  , 13 },
     { MAD_F(0x069372ae)  , 13 },
     { MAD_F(0x06988735)  , 13 },
     { MAD_F(0x069d9cb7)  , 13 },
     { MAD_F(0x06a2b333)  , 13 },
     { MAD_F(0x06a7caa9)  , 13 },
     { MAD_F(0x06ace318)  , 13 },

     { MAD_F(0x06b1fc81)  , 13 },
     { MAD_F(0x06b716e2)  , 13 },
     { MAD_F(0x06bc323b)  , 13 },
     { MAD_F(0x06c14e8d)  , 13 },
     { MAD_F(0x06c66bd6)  , 13 },
     { MAD_F(0x06cb8a17)  , 13 },
     { MAD_F(0x06d0a94e)  , 13 },
     { MAD_F(0x06d5c97c)  , 13 },
     { MAD_F(0x06daeaa1)  , 13 },
     { MAD_F(0x06e00cbb)  , 13 },
     { MAD_F(0x06e52fca)  , 13 },
     { MAD_F(0x06ea53cf)  , 13 },
     { MAD_F(0x06ef78c8)  , 13 },
     { MAD_F(0x06f49eb6)  , 13 },
     { MAD_F(0x06f9c597)  , 13 },
     { MAD_F(0x06feed6d)  , 13 },

     { MAD_F(0x07041636)  , 13 },
     { MAD_F(0x07093ff2)  , 13 },
     { MAD_F(0x070e6aa0)  , 13 },
     { MAD_F(0x07139641)  , 13 },
     { MAD_F(0x0718c2d3)  , 13 },
     { MAD_F(0x071df058)  , 13 },
     { MAD_F(0x07231ecd)  , 13 },
     { MAD_F(0x07284e34)  , 13 },
     { MAD_F(0x072d7e8b)  , 13 },
     { MAD_F(0x0732afd2)  , 13 },
     { MAD_F(0x0737e209)  , 13 },
     { MAD_F(0x073d1530)  , 13 },
     { MAD_F(0x07424946)  , 13 },
     { MAD_F(0x07477e4b)  , 13 },
     { MAD_F(0x074cb43e)  , 13 },
     { MAD_F(0x0751eb20)  , 13 },

     { MAD_F(0x075722ef)  , 13 },
     { MAD_F(0x075c5bac)  , 13 },
     { MAD_F(0x07619557)  , 13 },
     { MAD_F(0x0766cfee)  , 13 },
     { MAD_F(0x076c0b72)  , 13 },
     { MAD_F(0x077147e2)  , 13 },
     { MAD_F(0x0776853e)  , 13 },
     { MAD_F(0x077bc385)  , 13 },
     { MAD_F(0x078102b8)  , 13 },
     { MAD_F(0x078642d6)  , 13 },
     { MAD_F(0x078b83de)  , 13 },
     { MAD_F(0x0790c5d1)  , 13 },
     { MAD_F(0x079608ae)  , 13 },
     { MAD_F(0x079b4c74)  , 13 },
     { MAD_F(0x07a09124)  , 13 },
     { MAD_F(0x07a5d6bd)  , 13 },

     { MAD_F(0x07ab1d3e)  , 13 },
     { MAD_F(0x07b064a8)  , 13 },
     { MAD_F(0x07b5acfb)  , 13 },
     { MAD_F(0x07baf635)  , 13 },
     { MAD_F(0x07c04056)  , 13 },
     { MAD_F(0x07c58b5f)  , 13 },
     { MAD_F(0x07cad74e)  , 13 },
     { MAD_F(0x07d02424)  , 13 },
     { MAD_F(0x07d571e0)  , 13 },
     { MAD_F(0x07dac083)  , 13 },
     { MAD_F(0x07e0100a)  , 13 },
     { MAD_F(0x07e56078)  , 13 },
     { MAD_F(0x07eab1ca)  , 13 },
     { MAD_F(0x07f00401)  , 13 },
     { MAD_F(0x07f5571d)  , 13 },
     { MAD_F(0x07faab1c)  , 13 },

     { MAD_F(0x04000000)  , 14 },
     { MAD_F(0x0402aae3)  , 14 },
     { MAD_F(0x04055638)  , 14 },
     { MAD_F(0x040801ff)  , 14 },
     { MAD_F(0x040aae37)  , 14 },
     { MAD_F(0x040d5ae0)  , 14 },
     { MAD_F(0x041007fa)  , 14 },
     { MAD_F(0x0412b586)  , 14 },
     { MAD_F(0x04156381)  , 14 },
     { MAD_F(0x041811ee)  , 14 },
     { MAD_F(0x041ac0cb)  , 14 },
     { MAD_F(0x041d7018)  , 14 },
     { MAD_F(0x04201fd5)  , 14 },
     { MAD_F(0x0422d003)  , 14 },
     { MAD_F(0x042580a0)  , 14 },
     { MAD_F(0x042831ad)  , 14 },

     { MAD_F(0x042ae32a)  , 14 },
     { MAD_F(0x042d9516)  , 14 },
     { MAD_F(0x04304772)  , 14 },
     { MAD_F(0x0432fa3d)  , 14 },
     { MAD_F(0x0435ad76)  , 14 },
     { MAD_F(0x0438611f)  , 14 },
     { MAD_F(0x043b1536)  , 14 },
     { MAD_F(0x043dc9bc)  , 14 },
     { MAD_F(0x04407eb1)  , 14 },
     { MAD_F(0x04433414)  , 14 },
     { MAD_F(0x0445e9e5)  , 14 },
     { MAD_F(0x0448a024)  , 14 },
     { MAD_F(0x044b56d1)  , 14 },
     { MAD_F(0x044e0dec)  , 14 },
     { MAD_F(0x0450c575)  , 14 },
     { MAD_F(0x04537d6b)  , 14 },

     { MAD_F(0x045635cf)  , 14 },
     { MAD_F(0x0458ee9f)  , 14 },
     { MAD_F(0x045ba7dd)  , 14 },
     { MAD_F(0x045e6188)  , 14 },
     { MAD_F(0x04611ba0)  , 14 },
     { MAD_F(0x0463d625)  , 14 },
     { MAD_F(0x04669116)  , 14 },
     { MAD_F(0x04694c74)  , 14 },
     { MAD_F(0x046c083e)  , 14 },
     { MAD_F(0x046ec474)  , 14 },
     { MAD_F(0x04718116)  , 14 },
     { MAD_F(0x04743e25)  , 14 },
     { MAD_F(0x0476fb9f)  , 14 },
     { MAD_F(0x0479b984)  , 14 },
     { MAD_F(0x047c77d6)  , 14 },
     { MAD_F(0x047f3693)  , 14 },

     { MAD_F(0x0481f5bb)  , 14 },
     { MAD_F(0x0484b54e)  , 14 },
     { MAD_F(0x0487754c)  , 14 },
     { MAD_F(0x048a35b6)  , 14 },
     { MAD_F(0x048cf68a)  , 14 },
     { MAD_F(0x048fb7c8)  , 14 },
     { MAD_F(0x04927972)  , 14 },
     { MAD_F(0x04953b85)  , 14 },
     { MAD_F(0x0497fe03)  , 14 },
     { MAD_F(0x049ac0eb)  , 14 },
     { MAD_F(0x049d843e)  , 14 },
     { MAD_F(0x04a047fa)  , 14 },
     { MAD_F(0x04a30c20)  , 14 },
     { MAD_F(0x04a5d0af)  , 14 },
     { MAD_F(0x04a895a8)  , 14 },
     { MAD_F(0x04ab5b0b)  , 14 },

     { MAD_F(0x04ae20d7)  , 14 },
     { MAD_F(0x04b0e70c)  , 14 },
     { MAD_F(0x04b3adaa)  , 14 },
     { MAD_F(0x04b674b1)  , 14 },
     { MAD_F(0x04b93c21)  , 14 },
     { MAD_F(0x04bc03fa)  , 14 },
     { MAD_F(0x04becc3b)  , 14 },
     { MAD_F(0x04c194e4)  , 14 },
     { MAD_F(0x04c45df6)  , 14 },
     { MAD_F(0x04c72771)  , 14 },
     { MAD_F(0x04c9f153)  , 14 },
     { MAD_F(0x04ccbb9d)  , 14 },
     { MAD_F(0x04cf864f)  , 14 },
     { MAD_F(0x04d25169)  , 14 },
     { MAD_F(0x04d51ceb)  , 14 },
     { MAD_F(0x04d7e8d4)  , 14 },

     { MAD_F(0x04dab524)  , 14 },
     { MAD_F(0x04dd81dc)  , 14 },
     { MAD_F(0x04e04efb)  , 14 },
     { MAD_F(0x04e31c81)  , 14 },
     { MAD_F(0x04e5ea6e)  , 14 },
     { MAD_F(0x04e8b8c2)  , 14 },
     { MAD_F(0x04eb877c)  , 14 },
     { MAD_F(0x04ee569d)  , 14 },
     { MAD_F(0x04f12624)  , 14 },
     { MAD_F(0x04f3f612)  , 14 },
     { MAD_F(0x04f6c666)  , 14 },
     { MAD_F(0x04f99721)  , 14 },
     { MAD_F(0x04fc6841)  , 14 },
     { MAD_F(0x04ff39c7)  , 14 },
     { MAD_F(0x05020bb3)  , 14 },
     { MAD_F(0x0504de05)  , 14 },

     { MAD_F(0x0507b0bc)  , 14 },
     { MAD_F(0x050a83d8)  , 14 },
     { MAD_F(0x050d575b)  , 14 },
     { MAD_F(0x05102b42)  , 14 },
     { MAD_F(0x0512ff8e)  , 14 },
     { MAD_F(0x0515d440)  , 14 },
     { MAD_F(0x0518a956)  , 14 },
     { MAD_F(0x051b7ed1)  , 14 },
     { MAD_F(0x051e54b1)  , 14 },
     { MAD_F(0x05212af5)  , 14 },
     { MAD_F(0x0524019e)  , 14 },
     { MAD_F(0x0526d8ab)  , 14 },
     { MAD_F(0x0529b01d)  , 14 },
     { MAD_F(0x052c87f2)  , 14 },
     { MAD_F(0x052f602c)  , 14 },
     { MAD_F(0x053238ca)  , 14 },

     { MAD_F(0x053511cb)  , 14 },
     { MAD_F(0x0537eb30)  , 14 },
     { MAD_F(0x053ac4f9)  , 14 },
     { MAD_F(0x053d9f25)  , 14 },
     { MAD_F(0x054079b5)  , 14 },
     { MAD_F(0x054354a8)  , 14 },
     { MAD_F(0x05462ffe)  , 14 },
     { MAD_F(0x05490bb7)  , 14 },
     { MAD_F(0x054be7d4)  , 14 },
     { MAD_F(0x054ec453)  , 14 },
     { MAD_F(0x0551a134)  , 14 },
     { MAD_F(0x05547e79)  , 14 },
     { MAD_F(0x05575c20)  , 14 },
     { MAD_F(0x055a3a2a)  , 14 },
     { MAD_F(0x055d1896)  , 14 },
     { MAD_F(0x055ff764)  , 14 },

     { MAD_F(0x0562d694)  , 14 },
     { MAD_F(0x0565b627)  , 14 },
     { MAD_F(0x0568961b)  , 14 },
     { MAD_F(0x056b7671)  , 14 },
     { MAD_F(0x056e5729)  , 14 },
     { MAD_F(0x05713843)  , 14 },
     { MAD_F(0x057419be)  , 14 },
     { MAD_F(0x0576fb9a)  , 14 },
     { MAD_F(0x0579ddd8)  , 14 },
     { MAD_F(0x057cc077)  , 14 },
     { MAD_F(0x057fa378)  , 14 },
     { MAD_F(0x058286d9)  , 14 },
     { MAD_F(0x05856a9b)  , 14 },
     { MAD_F(0x05884ebe)  , 14 },
     { MAD_F(0x058b3342)  , 14 },
     { MAD_F(0x058e1827)  , 14 },

     { MAD_F(0x0590fd6c)  , 14 },
     { MAD_F(0x0593e311)  , 14 },
     { MAD_F(0x0596c917)  , 14 },
     { MAD_F(0x0599af7d)  , 14 },
     { MAD_F(0x059c9643)  , 14 },
     { MAD_F(0x059f7d6a)  , 14 },
     { MAD_F(0x05a264f0)  , 14 },
     { MAD_F(0x05a54cd6)  , 14 },
     { MAD_F(0x05a8351c)  , 14 },
     { MAD_F(0x05ab1dc2)  , 14 },
     { MAD_F(0x05ae06c7)  , 14 },
     { MAD_F(0x05b0f02b)  , 14 },
     { MAD_F(0x05b3d9f0)  , 14 },
     { MAD_F(0x05b6c413)  , 14 },
     { MAD_F(0x05b9ae95)  , 14 },
     { MAD_F(0x05bc9977)  , 14 },

     { MAD_F(0x05bf84b8)  , 14 },
     { MAD_F(0x05c27057)  , 14 },
     { MAD_F(0x05c55c56)  , 14 },
     { MAD_F(0x05c848b3)  , 14 },
     { MAD_F(0x05cb356e)  , 14 },
     { MAD_F(0x05ce2289)  , 14 },
     { MAD_F(0x05d11001)  , 14 },
     { MAD_F(0x05d3fdd8)  , 14 },
     { MAD_F(0x05d6ec0e)  , 14 },
     { MAD_F(0x05d9daa1)  , 14 },
     { MAD_F(0x05dcc993)  , 14 },
     { MAD_F(0x05dfb8e2)  , 14 },
     { MAD_F(0x05e2a890)  , 14 },
     { MAD_F(0x05e5989b)  , 14 },
     { MAD_F(0x05e88904)  , 14 },
     { MAD_F(0x05eb79cb)  , 14 },

     { MAD_F(0x05ee6aef)  , 14 },
     { MAD_F(0x05f15c70)  , 14 },
     { MAD_F(0x05f44e4f)  , 14 },
     { MAD_F(0x05f7408b)  , 14 },
     { MAD_F(0x05fa3324)  , 14 },
     { MAD_F(0x05fd261b)  , 14 },
     { MAD_F(0x0600196e)  , 14 },
     { MAD_F(0x06030d1e)  , 14 },
     { MAD_F(0x0606012b)  , 14 },
     { MAD_F(0x0608f595)  , 14 },
     { MAD_F(0x060bea5c)  , 14 },
     { MAD_F(0x060edf7f)  , 14 },
     { MAD_F(0x0611d4fe)  , 14 },
     { MAD_F(0x0614cada)  , 14 },
     { MAD_F(0x0617c112)  , 14 },
     { MAD_F(0x061ab7a6)  , 14 },

     { MAD_F(0x061dae96)  , 14 },
     { MAD_F(0x0620a5e3)  , 14 },
     { MAD_F(0x06239d8b)  , 14 },
     { MAD_F(0x0626958f)  , 14 },
     { MAD_F(0x06298def)  , 14 },
     { MAD_F(0x062c86aa)  , 14 },
     { MAD_F(0x062f7fc1)  , 14 },
     { MAD_F(0x06327934)  , 14 },
     { MAD_F(0x06357302)  , 14 },
     { MAD_F(0x06386d2b)  , 14 },
     { MAD_F(0x063b67b0)  , 14 },
     { MAD_F(0x063e6290)  , 14 },
     { MAD_F(0x06415dcb)  , 14 },
     { MAD_F(0x06445960)  , 14 },
     { MAD_F(0x06475551)  , 14 },
     { MAD_F(0x064a519c)  , 14 },

     { MAD_F(0x064d4e43)  , 14 },
     { MAD_F(0x06504b44)  , 14 },
     { MAD_F(0x0653489f)  , 14 },
     { MAD_F(0x06564655)  , 14 },
     { MAD_F(0x06594465)  , 14 },
     { MAD_F(0x065c42d0)  , 14 },
     { MAD_F(0x065f4195)  , 14 },
     { MAD_F(0x066240b4)  , 14 },
     { MAD_F(0x0665402d)  , 14 },
     { MAD_F(0x06684000)  , 14 },
     { MAD_F(0x066b402d)  , 14 },
     { MAD_F(0x066e40b3)  , 14 },
     { MAD_F(0x06714194)  , 14 },
     { MAD_F(0x067442ce)  , 14 },
     { MAD_F(0x06774462)  , 14 },
     { MAD_F(0x067a464f)  , 14 },

     { MAD_F(0x067d4896)  , 14 },
     { MAD_F(0x06804b36)  , 14 },
     { MAD_F(0x06834e2f)  , 14 },
     { MAD_F(0x06865181)  , 14 },
     { MAD_F(0x0689552c)  , 14 },
     { MAD_F(0x068c5931)  , 14 },
     { MAD_F(0x068f5d8e)  , 14 },
     { MAD_F(0x06926245)  , 14 },
     { MAD_F(0x06956753)  , 14 },
     { MAD_F(0x06986cbb)  , 14 },
     { MAD_F(0x069b727b)  , 14 },
     { MAD_F(0x069e7894)  , 14 },
     { MAD_F(0x06a17f05)  , 14 },
     { MAD_F(0x06a485cf)  , 14 },
     { MAD_F(0x06a78cf1)  , 14 },
     { MAD_F(0x06aa946b)  , 14 },

     { MAD_F(0x06ad9c3d)  , 14 },
     { MAD_F(0x06b0a468)  , 14 },
     { MAD_F(0x06b3acea)  , 14 },
     { MAD_F(0x06b6b5c4)  , 14 },
     { MAD_F(0x06b9bef6)  , 14 },
     { MAD_F(0x06bcc880)  , 14 },
     { MAD_F(0x06bfd261)  , 14 },
     { MAD_F(0x06c2dc9a)  , 14 },
     { MAD_F(0x06c5e72b)  , 14 },
     { MAD_F(0x06c8f213)  , 14 },
     { MAD_F(0x06cbfd52)  , 14 },
     { MAD_F(0x06cf08e9)  , 14 },
     { MAD_F(0x06d214d7)  , 14 },
     { MAD_F(0x06d5211c)  , 14 },
     { MAD_F(0x06d82db8)  , 14 },
     { MAD_F(0x06db3aaa)  , 14 },

     { MAD_F(0x06de47f4)  , 14 },
     { MAD_F(0x06e15595)  , 14 },
     { MAD_F(0x06e4638d)  , 14 },
     { MAD_F(0x06e771db)  , 14 },
     { MAD_F(0x06ea807f)  , 14 },
     { MAD_F(0x06ed8f7b)  , 14 },
     { MAD_F(0x06f09ecc)  , 14 },
     { MAD_F(0x06f3ae75)  , 14 },
     { MAD_F(0x06f6be73)  , 14 },
     { MAD_F(0x06f9cec8)  , 14 },
     { MAD_F(0x06fcdf72)  , 14 },
     { MAD_F(0x06fff073)  , 14 },
     { MAD_F(0x070301ca)  , 14 },
     { MAD_F(0x07061377)  , 14 },
     { MAD_F(0x0709257a)  , 14 },
     { MAD_F(0x070c37d2)  , 14 },


     { MAD_F(0x070f4a80)  , 14 },
     { MAD_F(0x07125d84)  , 14 },
     { MAD_F(0x071570de)  , 14 },
     { MAD_F(0x0718848d)  , 14 },
     { MAD_F(0x071b9891)  , 14 },
     { MAD_F(0x071eaceb)  , 14 },
     { MAD_F(0x0721c19a)  , 14 },
     { MAD_F(0x0724d69e)  , 14 },
     { MAD_F(0x0727ebf7)  , 14 },
     { MAD_F(0x072b01a6)  , 14 },
     { MAD_F(0x072e17a9)  , 14 },
     { MAD_F(0x07312e01)  , 14 },
     { MAD_F(0x073444ae)  , 14 },
     { MAD_F(0x07375bb0)  , 14 },
     { MAD_F(0x073a7307)  , 14 },
     { MAD_F(0x073d8ab2)  , 14 },

     { MAD_F(0x0740a2b2)  , 14 },
     { MAD_F(0x0743bb06)  , 14 },
     { MAD_F(0x0746d3af)  , 14 },
     { MAD_F(0x0749ecac)  , 14 },
     { MAD_F(0x074d05fe)  , 14 },
     { MAD_F(0x07501fa3)  , 14 },
     { MAD_F(0x0753399d)  , 14 },
     { MAD_F(0x075653eb)  , 14 },
     { MAD_F(0x07596e8d)  , 14 },
     { MAD_F(0x075c8983)  , 14 },
     { MAD_F(0x075fa4cc)  , 14 },
     { MAD_F(0x0762c06a)  , 14 },
     { MAD_F(0x0765dc5b)  , 14 },
     { MAD_F(0x0768f8a0)  , 14 },
     { MAD_F(0x076c1538)  , 14 },
     { MAD_F(0x076f3224)  , 14 },

     { MAD_F(0x07724f64)  , 14 },
     { MAD_F(0x07756cf7)  , 14 },
     { MAD_F(0x07788add)  , 14 },
     { MAD_F(0x077ba916)  , 14 },
     { MAD_F(0x077ec7a3)  , 14 },
     { MAD_F(0x0781e683)  , 14 },
     { MAD_F(0x078505b5)  , 14 },
     { MAD_F(0x0788253b)  , 14 },
     { MAD_F(0x078b4514)  , 14 },
     { MAD_F(0x078e653f)  , 14 },
     { MAD_F(0x079185be)  , 14 },
     { MAD_F(0x0794a68f)  , 14 },
     { MAD_F(0x0797c7b2)  , 14 },
     { MAD_F(0x079ae929)  , 14 },
     { MAD_F(0x079e0af1)  , 14 },
     { MAD_F(0x07a12d0c)  , 14 },

     { MAD_F(0x07a44f7a)  , 14 },
     { MAD_F(0x07a7723a)  , 14 },
     { MAD_F(0x07aa954c)  , 14 },
     { MAD_F(0x07adb8b0)  , 14 },
     { MAD_F(0x07b0dc67)  , 14 },
     { MAD_F(0x07b4006f)  , 14 },
     { MAD_F(0x07b724ca)  , 14 },
     { MAD_F(0x07ba4976)  , 14 },
     { MAD_F(0x07bd6e75)  , 14 },
     { MAD_F(0x07c093c5)  , 14 },
     { MAD_F(0x07c3b967)  , 14 },
     { MAD_F(0x07c6df5a)  , 14 },
     { MAD_F(0x07ca059f)  , 14 },
     { MAD_F(0x07cd2c36)  , 14 },
     { MAD_F(0x07d0531e)  , 14 },
     { MAD_F(0x07d37a57)  , 14 },

     { MAD_F(0x07d6a1e2)  , 14 },
     { MAD_F(0x07d9c9be)  , 14 },
     { MAD_F(0x07dcf1ec)  , 14 },
     { MAD_F(0x07e01a6a)  , 14 },
     { MAD_F(0x07e3433a)  , 14 },
     { MAD_F(0x07e66c5a)  , 14 },
     { MAD_F(0x07e995cc)  , 14 },
     { MAD_F(0x07ecbf8e)  , 14 },
     { MAD_F(0x07efe9a1)  , 14 },
     { MAD_F(0x07f31405)  , 14 },
     { MAD_F(0x07f63eba)  , 14 },
     { MAD_F(0x07f969c0)  , 14 },
     { MAD_F(0x07fc9516)  , 14 },
     { MAD_F(0x07ffc0bc)  , 14 },
     { MAD_F(0x04017659)  , 15 },
     { MAD_F(0x04030c7d)  , 15 },

     { MAD_F(0x0404a2c9)  , 15 },
     { MAD_F(0x0406393d)  , 15 },
     { MAD_F(0x0407cfd9)  , 15 },
     { MAD_F(0x0409669d)  , 15 },
     { MAD_F(0x040afd89)  , 15 },
     { MAD_F(0x040c949e)  , 15 },
     { MAD_F(0x040e2bda)  , 15 },
     { MAD_F(0x040fc33e)  , 15 },
     { MAD_F(0x04115aca)  , 15 },
     { MAD_F(0x0412f27e)  , 15 },
     { MAD_F(0x04148a5a)  , 15 },
     { MAD_F(0x0416225d)  , 15 },
     { MAD_F(0x0417ba89)  , 15 },
     { MAD_F(0x041952dc)  , 15 },
     { MAD_F(0x041aeb57)  , 15 },
     { MAD_F(0x041c83fa)  , 15 },

     { MAD_F(0x041e1cc4)  , 15 },
     { MAD_F(0x041fb5b6)  , 15 },
     { MAD_F(0x04214ed0)  , 15 },
     { MAD_F(0x0422e811)  , 15 },
     { MAD_F(0x04248179)  , 15 },
     { MAD_F(0x04261b0a)  , 15 },
     { MAD_F(0x0427b4c2)  , 15 },
     { MAD_F(0x04294ea1)  , 15 },
     { MAD_F(0x042ae8a7)  , 15 },
     { MAD_F(0x042c82d6)  , 15 },
     { MAD_F(0x042e1d2b)  , 15 },
     { MAD_F(0x042fb7a8)  , 15 },
     { MAD_F(0x0431524c)  , 15 },
     { MAD_F(0x0432ed17)  , 15 },
     { MAD_F(0x0434880a)  , 15 },
     { MAD_F(0x04362324)  , 15 },

     { MAD_F(0x0437be65)  , 15 },
     { MAD_F(0x043959cd)  , 15 },
     { MAD_F(0x043af55d)  , 15 },
     { MAD_F(0x043c9113)  , 15 },
     { MAD_F(0x043e2cf1)  , 15 },
     { MAD_F(0x043fc8f6)  , 15 },
     { MAD_F(0x04416522)  , 15 },
     { MAD_F(0x04430174)  , 15 },
     { MAD_F(0x04449dee)  , 15 },
     { MAD_F(0x04463a8f)  , 15 },
     { MAD_F(0x0447d756)  , 15 },
     { MAD_F(0x04497445)  , 15 },
     { MAD_F(0x044b115a)  , 15 },
     { MAD_F(0x044cae96)  , 15 },
     { MAD_F(0x044e4bf9)  , 15 },
     { MAD_F(0x044fe983)  , 15 },

     { MAD_F(0x04518733)  , 15 },
     { MAD_F(0x0453250a)  , 15 },
     { MAD_F(0x0454c308)  , 15 },
     { MAD_F(0x0456612d)  , 15 },
     { MAD_F(0x0457ff78)  , 15 },
     { MAD_F(0x04599dea)  , 15 },
     { MAD_F(0x045b3c82)  , 15 },
     { MAD_F(0x045cdb41)  , 15 },
     { MAD_F(0x045e7a26)  , 15 },
     { MAD_F(0x04601932)  , 15 },
     { MAD_F(0x0461b864)  , 15 },
     { MAD_F(0x046357bd)  , 15 },
     { MAD_F(0x0464f73c)  , 15 },
     { MAD_F(0x046696e2)  , 15 },
     { MAD_F(0x046836ae)  , 15 },
     { MAD_F(0x0469d6a0)  , 15 },

     { MAD_F(0x046b76b9)  , 15 },
     { MAD_F(0x046d16f7)  , 15 },
     { MAD_F(0x046eb75c)  , 15 },
     { MAD_F(0x047057e8)  , 15 },
     { MAD_F(0x0471f899)  , 15 },
     { MAD_F(0x04739971)  , 15 },
     { MAD_F(0x04753a6f)  , 15 },
     { MAD_F(0x0476db92)  , 15 },
     { MAD_F(0x04787cdc)  , 15 },
     { MAD_F(0x047a1e4c)  , 15 },
     { MAD_F(0x047bbfe2)  , 15 },
     { MAD_F(0x047d619e)  , 15 },
     { MAD_F(0x047f0380)  , 15 },
     { MAD_F(0x0480a588)  , 15 },
     { MAD_F(0x048247b6)  , 15 },
     { MAD_F(0x0483ea0a)  , 15 },

     { MAD_F(0x04858c83)  , 15 },
     { MAD_F(0x04872f22)  , 15 },
     { MAD_F(0x0488d1e8)  , 15 },
     { MAD_F(0x048a74d3)  , 15 },
     { MAD_F(0x048c17e3)  , 15 },
     { MAD_F(0x048dbb1a)  , 15 },
     { MAD_F(0x048f5e76)  , 15 },
     { MAD_F(0x049101f8)  , 15 },
     { MAD_F(0x0492a59f)  , 15 },
     { MAD_F(0x0494496c)  , 15 },
     { MAD_F(0x0495ed5f)  , 15 },
     { MAD_F(0x04979177)  , 15 },
     { MAD_F(0x049935b5)  , 15 },
     { MAD_F(0x049ada19)  , 15 },
     { MAD_F(0x049c7ea1)  , 15 },
     { MAD_F(0x049e2350)  , 15 },

     { MAD_F(0x049fc824)  , 15 },
     { MAD_F(0x04a16d1d)  , 15 },
     { MAD_F(0x04a3123b)  , 15 },
     { MAD_F(0x04a4b77f)  , 15 },
     { MAD_F(0x04a65ce8)  , 15 },
     { MAD_F(0x04a80277)  , 15 },
     { MAD_F(0x04a9a82b)  , 15 },
     { MAD_F(0x04ab4e04)  , 15 },
     { MAD_F(0x04acf402)  , 15 },
     { MAD_F(0x04ae9a26)  , 15 },
     { MAD_F(0x04b0406e)  , 15 },
     { MAD_F(0x04b1e6dc)  , 15 },
     { MAD_F(0x04b38d6f)  , 15 },
     { MAD_F(0x04b53427)  , 15 },
     { MAD_F(0x04b6db05)  , 15 },
     { MAD_F(0x04b88207)  , 15 },

     { MAD_F(0x04ba292e)  , 15 },
     { MAD_F(0x04bbd07a)  , 15 },
     { MAD_F(0x04bd77ec)  , 15 },
     { MAD_F(0x04bf1f82)  , 15 },
     { MAD_F(0x04c0c73d)  , 15 },
     { MAD_F(0x04c26f1d)  , 15 },
     { MAD_F(0x04c41722)  , 15 },
     { MAD_F(0x04c5bf4c)  , 15 },
     { MAD_F(0x04c7679a)  , 15 },
     { MAD_F(0x04c9100d)  , 15 },
     { MAD_F(0x04cab8a6)  , 15 },
     { MAD_F(0x04cc6163)  , 15 },
     { MAD_F(0x04ce0a44)  , 15 },
     { MAD_F(0x04cfb34b)  , 15 },
     { MAD_F(0x04d15c76)  , 15 },
     { MAD_F(0x04d305c5)  , 15 },

     { MAD_F(0x04d4af3a)  , 15 },
     { MAD_F(0x04d658d2)  , 15 },
     { MAD_F(0x04d80290)  , 15 },
     { MAD_F(0x04d9ac72)  , 15 },
     { MAD_F(0x04db5679)  , 15 },
     { MAD_F(0x04dd00a4)  , 15 },
     { MAD_F(0x04deaaf3)  , 15 },
     { MAD_F(0x04e05567)  , 15 },
     { MAD_F(0x04e20000)  , 15 },
     { MAD_F(0x04e3aabd)  , 15 },
     { MAD_F(0x04e5559e)  , 15 },
     { MAD_F(0x04e700a3)  , 15 },
     { MAD_F(0x04e8abcd)  , 15 },
     { MAD_F(0x04ea571c)  , 15 },
     { MAD_F(0x04ec028e)  , 15 },
     { MAD_F(0x04edae25)  , 15 },

     { MAD_F(0x04ef59e0)  , 15 },
     { MAD_F(0x04f105bf)  , 15 },
     { MAD_F(0x04f2b1c3)  , 15 },
     { MAD_F(0x04f45dea)  , 15 },
     { MAD_F(0x04f60a36)  , 15 },
     { MAD_F(0x04f7b6a6)  , 15 },
     { MAD_F(0x04f9633a)  , 15 },
     { MAD_F(0x04fb0ff2)  , 15 },
     { MAD_F(0x04fcbcce)  , 15 },
     { MAD_F(0x04fe69ce)  , 15 },
     { MAD_F(0x050016f3)  , 15 },
     { MAD_F(0x0501c43b)  , 15 },
     { MAD_F(0x050371a7)  , 15 },
     { MAD_F(0x05051f37)  , 15 },
     { MAD_F(0x0506cceb)  , 15 },
     { MAD_F(0x05087ac2)  , 15 },

     { MAD_F(0x050a28be)  , 15 },
     { MAD_F(0x050bd6de)  , 15 },
     { MAD_F(0x050d8521)  , 15 },
     { MAD_F(0x050f3388)  , 15 },
     { MAD_F(0x0510e213)  , 15 },
     { MAD_F(0x051290c2)  , 15 },
     { MAD_F(0x05143f94)  , 15 },
     { MAD_F(0x0515ee8a)  , 15 },
     { MAD_F(0x05179da4)  , 15 },
     { MAD_F(0x05194ce1)  , 15 },
     { MAD_F(0x051afc42)  , 15 },
     { MAD_F(0x051cabc7)  , 15 },
     { MAD_F(0x051e5b6f)  , 15 },
     { MAD_F(0x05200b3a)  , 15 },
     { MAD_F(0x0521bb2a)  , 15 },
     { MAD_F(0x05236b3d)  , 15 },

     { MAD_F(0x05251b73)  , 15 },
     { MAD_F(0x0526cbcd)  , 15 },
     { MAD_F(0x05287c4a)  , 15 },
     { MAD_F(0x052a2cea)  , 15 },
     { MAD_F(0x052bddae)  , 15 },
     { MAD_F(0x052d8e96)  , 15 },
     { MAD_F(0x052f3fa1)  , 15 },
     { MAD_F(0x0530f0cf)  , 15 },
     { MAD_F(0x0532a220)  , 15 },
     { MAD_F(0x05345395)  , 15 },
     { MAD_F(0x0536052d)  , 15 },
     { MAD_F(0x0537b6e8)  , 15 },
     { MAD_F(0x053968c6)  , 15 },
     { MAD_F(0x053b1ac8)  , 15 },
     { MAD_F(0x053ccced)  , 15 },
     { MAD_F(0x053e7f35)  , 15 },

     { MAD_F(0x054031a0)  , 15 },
     { MAD_F(0x0541e42e)  , 15 },
     { MAD_F(0x054396df)  , 15 },
     { MAD_F(0x054549b4)  , 15 },
     { MAD_F(0x0546fcab)  , 15 },
     { MAD_F(0x0548afc6)  , 15 },
     { MAD_F(0x054a6303)  , 15 },
     { MAD_F(0x054c1663)  , 15 },
     { MAD_F(0x054dc9e7)  , 15 },
     { MAD_F(0x054f7d8d)  , 15 },
     { MAD_F(0x05513156)  , 15 },
     { MAD_F(0x0552e542)  , 15 },
     { MAD_F(0x05549951)  , 15 },
     { MAD_F(0x05564d83)  , 15 },
     { MAD_F(0x055801d8)  , 15 },
     { MAD_F(0x0559b64f)  , 15 },

     { MAD_F(0x055b6ae9)  , 15 },
     { MAD_F(0x055d1fa6)  , 15 },
     { MAD_F(0x055ed486)  , 15 },
     { MAD_F(0x05608988)  , 15 },
     { MAD_F(0x05623ead)  , 15 },
     { MAD_F(0x0563f3f5)  , 15 },
     { MAD_F(0x0565a960)  , 15 },
     { MAD_F(0x05675eed)  , 15 },
     { MAD_F(0x0569149c)  , 15 },
     { MAD_F(0x056aca6f)  , 15 },
     { MAD_F(0x056c8064)  , 15 },
     { MAD_F(0x056e367b)  , 15 },
     { MAD_F(0x056fecb5)  , 15 },
     { MAD_F(0x0571a311)  , 15 },
     { MAD_F(0x05735990)  , 15 },
     { MAD_F(0x05751032)  , 15 },

     { MAD_F(0x0576c6f5)  , 15 },
     { MAD_F(0x05787ddc)  , 15 },
     { MAD_F(0x057a34e4)  , 15 },
     { MAD_F(0x057bec0f)  , 15 },
     { MAD_F(0x057da35d)  , 15 },
     { MAD_F(0x057f5acc)  , 15 },
     { MAD_F(0x0581125e)  , 15 },
     { MAD_F(0x0582ca12)  , 15 },
     { MAD_F(0x058481e9)  , 15 },
     { MAD_F(0x058639e2)  , 15 },
     { MAD_F(0x0587f1fd)  , 15 },
     { MAD_F(0x0589aa3a)  , 15 },
     { MAD_F(0x058b629a)  , 15 },
     { MAD_F(0x058d1b1b)  , 15 },
     { MAD_F(0x058ed3bf)  , 15 },
     { MAD_F(0x05908c85)  , 15 },

     { MAD_F(0x0592456d)  , 15 },
     { MAD_F(0x0593fe77)  , 15 },
     { MAD_F(0x0595b7a3)  , 15 },
     { MAD_F(0x059770f1)  , 15 },
     { MAD_F(0x05992a61)  , 15 },
     { MAD_F(0x059ae3f3)  , 15 },
     { MAD_F(0x059c9da8)  , 15 },
     { MAD_F(0x059e577e)  , 15 },
     { MAD_F(0x05a01176)  , 15 },
     { MAD_F(0x05a1cb90)  , 15 },
     { MAD_F(0x05a385cc)  , 15 },
     { MAD_F(0x05a5402a)  , 15 },
     { MAD_F(0x05a6faa9)  , 15 },
     { MAD_F(0x05a8b54b)  , 15 },
     { MAD_F(0x05aa700e)  , 15 },
     { MAD_F(0x05ac2af3)  , 15 },

     { MAD_F(0x05ade5fa)  , 15 },
     { MAD_F(0x05afa123)  , 15 },
     { MAD_F(0x05b15c6d)  , 15 },
     { MAD_F(0x05b317d9)  , 15 },
     { MAD_F(0x05b4d367)  , 15 },
     { MAD_F(0x05b68f16)  , 15 },
     { MAD_F(0x05b84ae7)  , 15 },
     { MAD_F(0x05ba06da)  , 15 },
     { MAD_F(0x05bbc2ef)  , 15 },
     { MAD_F(0x05bd7f25)  , 15 },
     { MAD_F(0x05bf3b7c)  , 15 },
     { MAD_F(0x05c0f7f5)  , 15 },
     { MAD_F(0x05c2b490)  , 15 },
     { MAD_F(0x05c4714c)  , 15 },
     { MAD_F(0x05c62e2a)  , 15 },
     { MAD_F(0x05c7eb29)  , 15 },

     { MAD_F(0x05c9a84a)  , 15 },
     { MAD_F(0x05cb658c)  , 15 },
     { MAD_F(0x05cd22ef)  , 15 },
     { MAD_F(0x05cee074)  , 15 },
     { MAD_F(0x05d09e1b)  , 15 },
     { MAD_F(0x05d25be2)  , 15 },
     { MAD_F(0x05d419cb)  , 15 },
     { MAD_F(0x05d5d7d5)  , 15 },
     { MAD_F(0x05d79601)  , 15 },
     { MAD_F(0x05d9544e)  , 15 },
     { MAD_F(0x05db12bc)  , 15 },
     { MAD_F(0x05dcd14c)  , 15 },
     { MAD_F(0x05de8ffc)  , 15 },
     { MAD_F(0x05e04ece)  , 15 },
     { MAD_F(0x05e20dc1)  , 15 },
     { MAD_F(0x05e3ccd5)  , 15 },

     { MAD_F(0x05e58c0b)  , 15 },
     { MAD_F(0x05e74b61)  , 15 },
     { MAD_F(0x05e90ad9)  , 15 },
     { MAD_F(0x05eaca72)  , 15 },
     { MAD_F(0x05ec8a2b)  , 15 },
     { MAD_F(0x05ee4a06)  , 15 },
     { MAD_F(0x05f00a02)  , 15 },
     { MAD_F(0x05f1ca1f)  , 15 },
     { MAD_F(0x05f38a5d)  , 15 },
     { MAD_F(0x05f54abc)  , 15 },
     { MAD_F(0x05f70b3c)  , 15 },
     { MAD_F(0x05f8cbdc)  , 15 },
     { MAD_F(0x05fa8c9e)  , 15 },
     { MAD_F(0x05fc4d81)  , 15 },
     { MAD_F(0x05fe0e84)  , 15 },
     { MAD_F(0x05ffcfa8)  , 15 },

     { MAD_F(0x060190ee)  , 15 },
     { MAD_F(0x06035254)  , 15 },
     { MAD_F(0x060513da)  , 15 },
     { MAD_F(0x0606d582)  , 15 },
     { MAD_F(0x0608974a)  , 15 },
     { MAD_F(0x060a5934)  , 15 },
     { MAD_F(0x060c1b3d)  , 15 },
     { MAD_F(0x060ddd68)  , 15 },
     { MAD_F(0x060f9fb3)  , 15 },
     { MAD_F(0x0611621f)  , 15 },
     { MAD_F(0x061324ac)  , 15 },
     { MAD_F(0x0614e759)  , 15 },
     { MAD_F(0x0616aa27)  , 15 },
     { MAD_F(0x06186d16)  , 15 },
     { MAD_F(0x061a3025)  , 15 },
     { MAD_F(0x061bf354)  , 15 },

     { MAD_F(0x061db6a5)  , 15 },
     { MAD_F(0x061f7a15)  , 15 },
     { MAD_F(0x06213da7)  , 15 },
     { MAD_F(0x06230158)  , 15 },
     { MAD_F(0x0624c52a)  , 15 },
     { MAD_F(0x0626891d)  , 15 },
     { MAD_F(0x06284d30)  , 15 },
     { MAD_F(0x062a1164)  , 15 },
     { MAD_F(0x062bd5b8)  , 15 },
     { MAD_F(0x062d9a2c)  , 15 },
     { MAD_F(0x062f5ec1)  , 15 },
     { MAD_F(0x06312376)  , 15 },
     { MAD_F(0x0632e84b)  , 15 },
     { MAD_F(0x0634ad41)  , 15 },
     { MAD_F(0x06367257)  , 15 },
     { MAD_F(0x0638378d)  , 15 },

     { MAD_F(0x0639fce4)  , 15 },
     { MAD_F(0x063bc25b)  , 15 },
     { MAD_F(0x063d87f2)  , 15 },
     { MAD_F(0x063f4da9)  , 15 },
     { MAD_F(0x06411380)  , 15 },
     { MAD_F(0x0642d978)  , 15 },
     { MAD_F(0x06449f8f)  , 15 },
     { MAD_F(0x064665c7)  , 15 },
     { MAD_F(0x06482c1f)  , 15 },
     { MAD_F(0x0649f297)  , 15 },
     { MAD_F(0x064bb92f)  , 15 },
     { MAD_F(0x064d7fe8)  , 15 },
     { MAD_F(0x064f46c0)  , 15 },
     { MAD_F(0x06510db8)  , 15 },
     { MAD_F(0x0652d4d0)  , 15 },
     { MAD_F(0x06549c09)  , 15 },

     { MAD_F(0x06566361)  , 15 },
     { MAD_F(0x06582ad9)  , 15 },
     { MAD_F(0x0659f271)  , 15 },
     { MAD_F(0x065bba29)  , 15 },
     { MAD_F(0x065d8201)  , 15 },
     { MAD_F(0x065f49f9)  , 15 },
     { MAD_F(0x06611211)  , 15 },
     { MAD_F(0x0662da49)  , 15 },
     { MAD_F(0x0664a2a0)  , 15 },
     { MAD_F(0x06666b17)  , 15 },
     { MAD_F(0x066833ae)  , 15 },
     { MAD_F(0x0669fc65)  , 15 },
     { MAD_F(0x066bc53c)  , 15 },
     { MAD_F(0x066d8e32)  , 15 },
     { MAD_F(0x066f5748)  , 15 },
     { MAD_F(0x0671207e)  , 15 },

     { MAD_F(0x0672e9d4)  , 15 },
     { MAD_F(0x0674b349)  , 15 },
     { MAD_F(0x06767cde)  , 15 },
     { MAD_F(0x06784692)  , 15 },
     { MAD_F(0x067a1066)  , 15 },
     { MAD_F(0x067bda5a)  , 15 },
     { MAD_F(0x067da46d)  , 15 },
     { MAD_F(0x067f6ea0)  , 15 },
     { MAD_F(0x068138f3)  , 15 },
     { MAD_F(0x06830365)  , 15 },
     { MAD_F(0x0684cdf6)  , 15 },
     { MAD_F(0x068698a8)  , 15 },
     { MAD_F(0x06886378)  , 15 },
     { MAD_F(0x068a2e68)  , 15 },
     { MAD_F(0x068bf978)  , 15 },
     { MAD_F(0x068dc4a7)  , 15 },

     { MAD_F(0x068f8ff5)  , 15 },
     { MAD_F(0x06915b63)  , 15 },
     { MAD_F(0x069326f0)  , 15 },
     { MAD_F(0x0694f29c)  , 15 },
     { MAD_F(0x0696be68)  , 15 },
     { MAD_F(0x06988a54)  , 15 },
     { MAD_F(0x069a565e)  , 15 },
     { MAD_F(0x069c2288)  , 15 },
     { MAD_F(0x069deed1)  , 15 },
     { MAD_F(0x069fbb3a)  , 15 },
     { MAD_F(0x06a187c1)  , 15 },
     { MAD_F(0x06a35468)  , 15 },
     { MAD_F(0x06a5212f)  , 15 },
     { MAD_F(0x06a6ee14)  , 15 },
     { MAD_F(0x06a8bb18)  , 15 },
     { MAD_F(0x06aa883c)  , 15 },

     { MAD_F(0x06ac557f)  , 15 },
     { MAD_F(0x06ae22e1)  , 15 },
     { MAD_F(0x06aff062)  , 15 },
     { MAD_F(0x06b1be03)  , 15 },
     { MAD_F(0x06b38bc2)  , 15 },
     { MAD_F(0x06b559a1)  , 15 },
     { MAD_F(0x06b7279e)  , 15 },
     { MAD_F(0x06b8f5bb)  , 15 },
     { MAD_F(0x06bac3f6)  , 15 },
     { MAD_F(0x06bc9251)  , 15 },
     { MAD_F(0x06be60cb)  , 15 },
     { MAD_F(0x06c02f63)  , 15 },
     { MAD_F(0x06c1fe1b)  , 15 },
     { MAD_F(0x06c3ccf1)  , 15 },
     { MAD_F(0x06c59be7)  , 15 },
     { MAD_F(0x06c76afb)  , 15 },

     { MAD_F(0x06c93a2e)  , 15 },
     { MAD_F(0x06cb0981)  , 15 },
     { MAD_F(0x06ccd8f2)  , 15 },
     { MAD_F(0x06cea881)  , 15 },
     { MAD_F(0x06d07830)  , 15 },
     { MAD_F(0x06d247fe)  , 15 },
     { MAD_F(0x06d417ea)  , 15 },
     { MAD_F(0x06d5e7f5)  , 15 },
     { MAD_F(0x06d7b81f)  , 15 },
     { MAD_F(0x06d98868)  , 15 },
     { MAD_F(0x06db58cf)  , 15 },
     { MAD_F(0x06dd2955)  , 15 },
     { MAD_F(0x06def9fa)  , 15 },
     { MAD_F(0x06e0cabe)  , 15 },
     { MAD_F(0x06e29ba0)  , 15 },
     { MAD_F(0x06e46ca1)  , 15 },

     { MAD_F(0x06e63dc0)  , 15 },
     { MAD_F(0x06e80efe)  , 15 },
     { MAD_F(0x06e9e05b)  , 15 },
     { MAD_F(0x06ebb1d6)  , 15 },
     { MAD_F(0x06ed8370)  , 15 },
     { MAD_F(0x06ef5529)  , 15 },
     { MAD_F(0x06f12700)  , 15 },
     { MAD_F(0x06f2f8f5)  , 15 },
     { MAD_F(0x06f4cb09)  , 15 },
     { MAD_F(0x06f69d3c)  , 15 },
     { MAD_F(0x06f86f8d)  , 15 },
     { MAD_F(0x06fa41fd)  , 15 },
     { MAD_F(0x06fc148b)  , 15 },
     { MAD_F(0x06fde737)  , 15 },
     { MAD_F(0x06ffba02)  , 15 },
     { MAD_F(0x07018ceb)  , 15 },

     { MAD_F(0x07035ff3)  , 15 },
     { MAD_F(0x07053319)  , 15 },
     { MAD_F(0x0707065d)  , 15 },
     { MAD_F(0x0708d9c0)  , 15 },
     { MAD_F(0x070aad41)  , 15 },
     { MAD_F(0x070c80e1)  , 15 },
     { MAD_F(0x070e549f)  , 15 },
     { MAD_F(0x0710287b)  , 15 },
     { MAD_F(0x0711fc75)  , 15 },
     { MAD_F(0x0713d08d)  , 15 },
     { MAD_F(0x0715a4c4)  , 15 },
     { MAD_F(0x07177919)  , 15 },
     { MAD_F(0x07194d8c)  , 15 },
     { MAD_F(0x071b221e)  , 15 },
     { MAD_F(0x071cf6ce)  , 15 },
     { MAD_F(0x071ecb9b)  , 15 },

     { MAD_F(0x0720a087)  , 15 },
     { MAD_F(0x07227591)  , 15 },
     { MAD_F(0x07244ab9)  , 15 },
     { MAD_F(0x07262000)  , 15 },
     { MAD_F(0x0727f564)  , 15 },
     { MAD_F(0x0729cae7)  , 15 },
     { MAD_F(0x072ba087)  , 15 },
     { MAD_F(0x072d7646)  , 15 },
     { MAD_F(0x072f4c22)  , 15 },
     { MAD_F(0x0731221d)  , 15 },
     { MAD_F(0x0732f835)  , 15 },
     { MAD_F(0x0734ce6c)  , 15 },
     { MAD_F(0x0736a4c1)  , 15 },
     { MAD_F(0x07387b33)  , 15 },
     { MAD_F(0x073a51c4)  , 15 },
     { MAD_F(0x073c2872)  , 15 },

     { MAD_F(0x073dff3e)  , 15 },
     { MAD_F(0x073fd628)  , 15 },
     { MAD_F(0x0741ad30)  , 15 },
     { MAD_F(0x07438456)  , 15 },
     { MAD_F(0x07455b9a)  , 15 },
     { MAD_F(0x074732fc)  , 15 },
     { MAD_F(0x07490a7b)  , 15 },
     { MAD_F(0x074ae218)  , 15 },
     { MAD_F(0x074cb9d3)  , 15 },
     { MAD_F(0x074e91ac)  , 15 },
     { MAD_F(0x075069a3)  , 15 },
     { MAD_F(0x075241b7)  , 15 },
     { MAD_F(0x075419e9)  , 15 },
     { MAD_F(0x0755f239)  , 15 },
     { MAD_F(0x0757caa7)  , 15 },
     { MAD_F(0x0759a332)  , 15 },

     { MAD_F(0x075b7bdb)  , 15 },
     { MAD_F(0x075d54a1)  , 15 },
     { MAD_F(0x075f2d85)  , 15 },
     { MAD_F(0x07610687)  , 15 },
     { MAD_F(0x0762dfa6)  , 15 },
     { MAD_F(0x0764b8e3)  , 15 },
     { MAD_F(0x0766923e)  , 15 },
     { MAD_F(0x07686bb6)  , 15 },
     { MAD_F(0x076a454c)  , 15 },
     { MAD_F(0x076c1eff)  , 15 },
     { MAD_F(0x076df8d0)  , 15 },
     { MAD_F(0x076fd2be)  , 15 },
     { MAD_F(0x0771acca)  , 15 },
     { MAD_F(0x077386f3)  , 15 },
     { MAD_F(0x0775613a)  , 15 },
     { MAD_F(0x07773b9e)  , 15 },

     { MAD_F(0x07791620)  , 15 },
     { MAD_F(0x077af0bf)  , 15 },
     { MAD_F(0x077ccb7c)  , 15 },
     { MAD_F(0x077ea656)  , 15 },
     { MAD_F(0x0780814d)  , 15 },
     { MAD_F(0x07825c62)  , 15 },
     { MAD_F(0x07843794)  , 15 },
     { MAD_F(0x078612e3)  , 15 },
     { MAD_F(0x0787ee50)  , 15 },
     { MAD_F(0x0789c9da)  , 15 },
     { MAD_F(0x078ba581)  , 15 },
     { MAD_F(0x078d8146)  , 15 },
     { MAD_F(0x078f5d28)  , 15 },
     { MAD_F(0x07913927)  , 15 },
     { MAD_F(0x07931543)  , 15 },
     { MAD_F(0x0794f17d)  , 15 },

     { MAD_F(0x0796cdd4)  , 15 },
     { MAD_F(0x0798aa48)  , 15 },
     { MAD_F(0x079a86d9)  , 15 },
     { MAD_F(0x079c6388)  , 15 },
     { MAD_F(0x079e4053)  , 15 },
     { MAD_F(0x07a01d3c)  , 15 },
     { MAD_F(0x07a1fa42)  , 15 },
     { MAD_F(0x07a3d765)  , 15 },
     { MAD_F(0x07a5b4a5)  , 15 },
     { MAD_F(0x07a79202)  , 15 },
     { MAD_F(0x07a96f7d)  , 15 },
     { MAD_F(0x07ab4d14)  , 15 },
     { MAD_F(0x07ad2ac8)  , 15 },
     { MAD_F(0x07af089a)  , 15 },
     { MAD_F(0x07b0e688)  , 15 },
     { MAD_F(0x07b2c494)  , 15 },

     { MAD_F(0x07b4a2bc)  , 15 },
     { MAD_F(0x07b68102)  , 15 },
     { MAD_F(0x07b85f64)  , 15 },
     { MAD_F(0x07ba3de4)  , 15 },
     { MAD_F(0x07bc1c80)  , 15 },
     { MAD_F(0x07bdfb39)  , 15 },
     { MAD_F(0x07bfda0f)  , 15 },
     { MAD_F(0x07c1b902)  , 15 },
     { MAD_F(0x07c39812)  , 15 },
     { MAD_F(0x07c5773f)  , 15 },
     { MAD_F(0x07c75689)  , 15 },
     { MAD_F(0x07c935ef)  , 15 },
     { MAD_F(0x07cb1573)  , 15 },
     { MAD_F(0x07ccf513)  , 15 },
     { MAD_F(0x07ced4d0)  , 15 },
     { MAD_F(0x07d0b4aa)  , 15 },

     { MAD_F(0x07d294a0)  , 15 },
     { MAD_F(0x07d474b3)  , 15 },
     { MAD_F(0x07d654e4)  , 15 },
     { MAD_F(0x07d83530)  , 15 },
     { MAD_F(0x07da159a)  , 15 },
     { MAD_F(0x07dbf620)  , 15 },
     { MAD_F(0x07ddd6c3)  , 15 },
     { MAD_F(0x07dfb783)  , 15 },
     { MAD_F(0x07e1985f)  , 15 },
     { MAD_F(0x07e37958)  , 15 },
     { MAD_F(0x07e55a6e)  , 15 },
     { MAD_F(0x07e73ba0)  , 15 },
     { MAD_F(0x07e91cef)  , 15 },
     { MAD_F(0x07eafe5a)  , 15 },
     { MAD_F(0x07ecdfe2)  , 15 },
     { MAD_F(0x07eec187)  , 15 },

     { MAD_F(0x07f0a348)  , 15 },
     { MAD_F(0x07f28526)  , 15 },
     { MAD_F(0x07f46720)  , 15 },
     { MAD_F(0x07f64937)  , 15 },
     { MAD_F(0x07f82b6a)  , 15 },
     { MAD_F(0x07fa0dba)  , 15 },
     { MAD_F(0x07fbf026)  , 15 },
     { MAD_F(0x07fdd2af)  , 15 },
     { MAD_F(0x07ffb554)  , 15 },
     { MAD_F(0x0400cc0b)  , 16 },
     { MAD_F(0x0401bd7a)  , 16 },
     { MAD_F(0x0402aef7)  , 16 },
     { MAD_F(0x0403a083)  , 16 },
     { MAD_F(0x0404921c)  , 16 },
     { MAD_F(0x040583c4)  , 16 },
     { MAD_F(0x0406757a)  , 16 },

     { MAD_F(0x0407673f)  , 16 },
     { MAD_F(0x04085911)  , 16 },
     { MAD_F(0x04094af1)  , 16 },
     { MAD_F(0x040a3ce0)  , 16 },
     { MAD_F(0x040b2edd)  , 16 },
     { MAD_F(0x040c20e8)  , 16 },
     { MAD_F(0x040d1301)  , 16 },
     { MAD_F(0x040e0529)  , 16 },
     { MAD_F(0x040ef75e)  , 16 },
     { MAD_F(0x040fe9a1)  , 16 },
     { MAD_F(0x0410dbf3)  , 16 },
     { MAD_F(0x0411ce53)  , 16 },
     { MAD_F(0x0412c0c1)  , 16 },
     { MAD_F(0x0413b33d)  , 16 },
     { MAD_F(0x0414a5c7)  , 16 },
     { MAD_F(0x0415985f)  , 16 },

     { MAD_F(0x04168b05)  , 16 },
     { MAD_F(0x04177db9)  , 16 },
     { MAD_F(0x0418707c)  , 16 },
     { MAD_F(0x0419634c)  , 16 },
     { MAD_F(0x041a562a)  , 16 },
     { MAD_F(0x041b4917)  , 16 },
     { MAD_F(0x041c3c11)  , 16 },
     { MAD_F(0x041d2f1a)  , 16 },
     { MAD_F(0x041e2230)  , 16 },
     { MAD_F(0x041f1555)  , 16 },
     { MAD_F(0x04200888)  , 16 },
     { MAD_F(0x0420fbc8)  , 16 },
     { MAD_F(0x0421ef17)  , 16 },
     { MAD_F(0x0422e273)  , 16 },
     { MAD_F(0x0423d5de)  , 16 },
     { MAD_F(0x0424c956)  , 16 },

     { MAD_F(0x0425bcdd)  , 16 },
     { MAD_F(0x0426b071)  , 16 },
     { MAD_F(0x0427a414)  , 16 },
     { MAD_F(0x042897c4)  , 16 },
     { MAD_F(0x04298b83)  , 16 },
     { MAD_F(0x042a7f4f)  , 16 },
     { MAD_F(0x042b7329)  , 16 },
     { MAD_F(0x042c6711)  , 16 },
     { MAD_F(0x042d5b07)  , 16 },
     { MAD_F(0x042e4f0b)  , 16 },
     { MAD_F(0x042f431d)  , 16 },
     { MAD_F(0x0430373d)  , 16 },
     { MAD_F(0x04312b6b)  , 16 },
     { MAD_F(0x04321fa6)  , 16 },
     { MAD_F(0x043313f0)  , 16 },
     { MAD_F(0x04340847)  , 16 },

     { MAD_F(0x0434fcad)  , 16 },
     { MAD_F(0x0435f120)  , 16 },
     { MAD_F(0x0436e5a1)  , 16 },
     { MAD_F(0x0437da2f)  , 16 },
     { MAD_F(0x0438cecc)  , 16 },
     { MAD_F(0x0439c377)  , 16 },
     { MAD_F(0x043ab82f)  , 16 },
     { MAD_F(0x043bacf5)  , 16 },
     { MAD_F(0x043ca1c9)  , 16 },
     { MAD_F(0x043d96ab)  , 16 },
     { MAD_F(0x043e8b9b)  , 16 },
     { MAD_F(0x043f8098)  , 16 },
     { MAD_F(0x044075a3)  , 16 },
     { MAD_F(0x04416abc)  , 16 },
     { MAD_F(0x04425fe3)  , 16 },
     { MAD_F(0x04435518)  , 16 },

     { MAD_F(0x04444a5a)  , 16 },
     { MAD_F(0x04453fab)  , 16 },
     { MAD_F(0x04463508)  , 16 },
     { MAD_F(0x04472a74)  , 16 },
     { MAD_F(0x04481fee)  , 16 },
     { MAD_F(0x04491575)  , 16 },
     { MAD_F(0x044a0b0a)  , 16 },
     { MAD_F(0x044b00ac)  , 16 },
     { MAD_F(0x044bf65d)  , 16 },
     { MAD_F(0x044cec1b)  , 16 },
     { MAD_F(0x044de1e7)  , 16 },
     { MAD_F(0x044ed7c0)  , 16 },
     { MAD_F(0x044fcda8)  , 16 },
     { MAD_F(0x0450c39c)  , 16 },
     { MAD_F(0x0451b99f)  , 16 },
     { MAD_F(0x0452afaf)  , 16 },

     { MAD_F(0x0453a5cd)  , 16 },
     { MAD_F(0x04549bf9)  , 16 },
     { MAD_F(0x04559232)  , 16 },
     { MAD_F(0x04568879)  , 16 },
     { MAD_F(0x04577ece)  , 16 },
     { MAD_F(0x04587530)  , 16 },
     { MAD_F(0x04596ba0)  , 16 },
     { MAD_F(0x045a621e)  , 16 },
     { MAD_F(0x045b58a9)  , 16 },
     { MAD_F(0x045c4f42)  , 16 },
     { MAD_F(0x045d45e9)  , 16 },
     { MAD_F(0x045e3c9d)  , 16 },
     { MAD_F(0x045f335e)  , 16 },
     { MAD_F(0x04602a2e)  , 16 },
     { MAD_F(0x0461210b)  , 16 },
     { MAD_F(0x046217f5)  , 16 },

     { MAD_F(0x04630eed)  , 16 },
     { MAD_F(0x046405f3)  , 16 },
     { MAD_F(0x0464fd06)  , 16 },
     { MAD_F(0x0465f427)  , 16 },
     { MAD_F(0x0466eb55)  , 16 },
     { MAD_F(0x0467e291)  , 16 },
     { MAD_F(0x0468d9db)  , 16 },
     { MAD_F(0x0469d132)  , 16 },
     { MAD_F(0x046ac896)  , 16 },
     { MAD_F(0x046bc009)  , 16 },
     { MAD_F(0x046cb788)  , 16 },
     { MAD_F(0x046daf15)  , 16 },
     { MAD_F(0x046ea6b0)  , 16 },
     { MAD_F(0x046f9e58)  , 16 },
     { MAD_F(0x0470960e)  , 16 },
     { MAD_F(0x04718dd1)  , 16 },

     { MAD_F(0x047285a2)  , 16 },
     { MAD_F(0x04737d80)  , 16 },
     { MAD_F(0x0474756c)  , 16 },
     { MAD_F(0x04756d65)  , 16 },
     { MAD_F(0x0476656b)  , 16 },
     { MAD_F(0x04775d7f)  , 16 },
     { MAD_F(0x047855a1)  , 16 },
     { MAD_F(0x04794dd0)  , 16 },
     { MAD_F(0x047a460c)  , 16 },
     { MAD_F(0x047b3e56)  , 16 },
     { MAD_F(0x047c36ae)  , 16 },
     { MAD_F(0x047d2f12)  , 16 },
     { MAD_F(0x047e2784)  , 16 },
     { MAD_F(0x047f2004)  , 16 },
     { MAD_F(0x04801891)  , 16 },
     { MAD_F(0x0481112b)  , 16 },

     { MAD_F(0x048209d3)  , 16 },
     { MAD_F(0x04830288)  , 16 },
     { MAD_F(0x0483fb4b)  , 16 },
     { MAD_F(0x0484f41b)  , 16 },
     { MAD_F(0x0485ecf8)  , 16 },
     { MAD_F(0x0486e5e3)  , 16 },
     { MAD_F(0x0487dedb)  , 16 },
     { MAD_F(0x0488d7e1)  , 16 },
     { MAD_F(0x0489d0f4)  , 16 },
     { MAD_F(0x048aca14)  , 16 },
     { MAD_F(0x048bc341)  , 16 },
     { MAD_F(0x048cbc7c)  , 16 },
     { MAD_F(0x048db5c4)  , 16 },
     { MAD_F(0x048eaf1a)  , 16 },
     { MAD_F(0x048fa87d)  , 16 },
     { MAD_F(0x0490a1ed)  , 16 },

     { MAD_F(0x04919b6a)  , 16 },
     { MAD_F(0x049294f5)  , 16 },
     { MAD_F(0x04938e8d)  , 16 },
     { MAD_F(0x04948833)  , 16 },
     { MAD_F(0x049581e5)  , 16 },
     { MAD_F(0x04967ba5)  , 16 },
     { MAD_F(0x04977573)  , 16 },
     { MAD_F(0x04986f4d)  , 16 },
     { MAD_F(0x04996935)  , 16 },
     { MAD_F(0x049a632a)  , 16 },
     { MAD_F(0x049b5d2c)  , 16 },
     { MAD_F(0x049c573c)  , 16 },
     { MAD_F(0x049d5159)  , 16 },
     { MAD_F(0x049e4b83)  , 16 },
     { MAD_F(0x049f45ba)  , 16 },
     { MAD_F(0x04a03ffe)  , 16 },

     { MAD_F(0x04a13a50)  , 16 },
     { MAD_F(0x04a234af)  , 16 },
     { MAD_F(0x04a32f1b)  , 16 },
     { MAD_F(0x04a42995)  , 16 },
     { MAD_F(0x04a5241b)  , 16 },
     { MAD_F(0x04a61eaf)  , 16 },
     { MAD_F(0x04a71950)  , 16 },
     { MAD_F(0x04a813fe)  , 16 },
     { MAD_F(0x04a90eba)  , 16 },
     { MAD_F(0x04aa0982)  , 16 },
     { MAD_F(0x04ab0458)  , 16 },
     { MAD_F(0x04abff3b)  , 16 },
     { MAD_F(0x04acfa2b)  , 16 },
     { MAD_F(0x04adf528)  , 16 },
     { MAD_F(0x04aef032)  , 16 },
     { MAD_F(0x04afeb4a)  , 16 },

     { MAD_F(0x04b0e66e)  , 16 },
     { MAD_F(0x04b1e1a0)  , 16 },
     { MAD_F(0x04b2dcdf)  , 16 },
     { MAD_F(0x04b3d82b)  , 16 },
     { MAD_F(0x04b4d384)  , 16 },
     { MAD_F(0x04b5ceea)  , 16 },
     { MAD_F(0x04b6ca5e)  , 16 },
     { MAD_F(0x04b7c5de)  , 16 },
     { MAD_F(0x04b8c16c)  , 16 },
     { MAD_F(0x04b9bd06)  , 16 },
     { MAD_F(0x04bab8ae)  , 16 },
     { MAD_F(0x04bbb463)  , 16 },
     { MAD_F(0x04bcb024)  , 16 },
     { MAD_F(0x04bdabf3)  , 16 },
     { MAD_F(0x04bea7cf)  , 16 },
     { MAD_F(0x04bfa3b8)  , 16 },

     { MAD_F(0x04c09faf)  , 16 },
     { MAD_F(0x04c19bb2)  , 16 },
     { MAD_F(0x04c297c2)  , 16 },
     { MAD_F(0x04c393df)  , 16 },
     { MAD_F(0x04c49009)  , 16 },
     { MAD_F(0x04c58c41)  , 16 },
     { MAD_F(0x04c68885)  , 16 },
     { MAD_F(0x04c784d6)  , 16 },
     { MAD_F(0x04c88135)  , 16 },
     { MAD_F(0x04c97da0)  , 16 },
     { MAD_F(0x04ca7a18)  , 16 },
     { MAD_F(0x04cb769e)  , 16 },
     { MAD_F(0x04cc7330)  , 16 },
     { MAD_F(0x04cd6fcf)  , 16 },
     { MAD_F(0x04ce6c7b)  , 16 },
     { MAD_F(0x04cf6935)  , 16 },

     { MAD_F(0x04d065fb)  , 16 },
     { MAD_F(0x04d162ce)  , 16 },
     { MAD_F(0x04d25fae)  , 16 },
     { MAD_F(0x04d35c9b)  , 16 },
     { MAD_F(0x04d45995)  , 16 },
     { MAD_F(0x04d5569c)  , 16 },
     { MAD_F(0x04d653b0)  , 16 },
     { MAD_F(0x04d750d1)  , 16 },
     { MAD_F(0x04d84dff)  , 16 },
     { MAD_F(0x04d94b3a)  , 16 },
     { MAD_F(0x04da4881)  , 16 },
     { MAD_F(0x04db45d6)  , 16 },
     { MAD_F(0x04dc4337)  , 16 },
     { MAD_F(0x04dd40a6)  , 16 },
     { MAD_F(0x04de3e21)  , 16 },
     { MAD_F(0x04df3ba9)  , 16 },

     { MAD_F(0x04e0393e)  , 16 },
     { MAD_F(0x04e136e0)  , 16 },
     { MAD_F(0x04e2348f)  , 16 },
     { MAD_F(0x04e3324b)  , 16 },
     { MAD_F(0x04e43013)  , 16 },
     { MAD_F(0x04e52de9)  , 16 },
     { MAD_F(0x04e62bcb)  , 16 },
     { MAD_F(0x04e729ba)  , 16 },
     { MAD_F(0x04e827b6)  , 16 },
     { MAD_F(0x04e925bf)  , 16 },
     { MAD_F(0x04ea23d4)  , 16 },
     { MAD_F(0x04eb21f7)  , 16 },
     { MAD_F(0x04ec2026)  , 16 },
     { MAD_F(0x04ed1e62)  , 16 },
     { MAD_F(0x04ee1cab)  , 16 },
     { MAD_F(0x04ef1b01)  , 16 },

     { MAD_F(0x04f01963)  , 16 },
     { MAD_F(0x04f117d3)  , 16 },
     { MAD_F(0x04f2164f)  , 16 },
     { MAD_F(0x04f314d8)  , 16 },
     { MAD_F(0x04f4136d)  , 16 },
     { MAD_F(0x04f51210)  , 16 },
     { MAD_F(0x04f610bf)  , 16 },
     { MAD_F(0x04f70f7b)  , 16 },
     { MAD_F(0x04f80e44)  , 16 },
     { MAD_F(0x04f90d19)  , 16 },
     { MAD_F(0x04fa0bfc)  , 16 },
     { MAD_F(0x04fb0aeb)  , 16 },
     { MAD_F(0x04fc09e7)  , 16 },
     { MAD_F(0x04fd08ef)  , 16 },
     { MAD_F(0x04fe0805)  , 16 },
     { MAD_F(0x04ff0727)  , 16 },

     { MAD_F(0x05000655)  , 16 },
     { MAD_F(0x05010591)  , 16 },
     { MAD_F(0x050204d9)  , 16 },
     { MAD_F(0x0503042e)  , 16 },
     { MAD_F(0x0504038f)  , 16 },
     { MAD_F(0x050502fe)  , 16 },
     { MAD_F(0x05060279)  , 16 },
     { MAD_F(0x05070200)  , 16 },
     { MAD_F(0x05080195)  , 16 },
     { MAD_F(0x05090136)  , 16 },
     { MAD_F(0x050a00e3)  , 16 },
     { MAD_F(0x050b009e)  , 16 },
     { MAD_F(0x050c0065)  , 16 },
     { MAD_F(0x050d0039)  , 16 },
     { MAD_F(0x050e0019)  , 16 },
     { MAD_F(0x050f0006)  , 16 },

     { MAD_F(0x05100000)  , 16 },
     { MAD_F(0x05110006)  , 16 },
     { MAD_F(0x05120019)  , 16 },
     { MAD_F(0x05130039)  , 16 },
     { MAD_F(0x05140065)  , 16 },
     { MAD_F(0x0515009e)  , 16 },
     { MAD_F(0x051600e3)  , 16 },
     { MAD_F(0x05170135)  , 16 },
     { MAD_F(0x05180194)  , 16 },
     { MAD_F(0x051901ff)  , 16 },
     { MAD_F(0x051a0277)  , 16 },
     { MAD_F(0x051b02fc)  , 16 },
     { MAD_F(0x051c038d)  , 16 },
     { MAD_F(0x051d042a)  , 16 },
     { MAD_F(0x051e04d4)  , 16 },
     { MAD_F(0x051f058b)  , 16 },

     { MAD_F(0x0520064f)  , 16 },
     { MAD_F(0x0521071f)  , 16 },
     { MAD_F(0x052207fb)  , 16 },
     { MAD_F(0x052308e4)  , 16 },
     { MAD_F(0x052409da)  , 16 },
     { MAD_F(0x05250adc)  , 16 },
     { MAD_F(0x05260bea)  , 16 },
     { MAD_F(0x05270d06)  , 16 },
     { MAD_F(0x05280e2d)  , 16 },
     { MAD_F(0x05290f62)  , 16 },
     { MAD_F(0x052a10a3)  , 16 },
     { MAD_F(0x052b11f0)  , 16 },
     { MAD_F(0x052c134a)  , 16 },
     { MAD_F(0x052d14b0)  , 16 },
     { MAD_F(0x052e1623)  , 16 },
     { MAD_F(0x052f17a2)  , 16 },

     { MAD_F(0x0530192e)  , 16 },
     { MAD_F(0x05311ac6)  , 16 },
     { MAD_F(0x05321c6b)  , 16 },
     { MAD_F(0x05331e1c)  , 16 },
     { MAD_F(0x05341fda)  , 16 },
     { MAD_F(0x053521a4)  , 16 },
     { MAD_F(0x0536237b)  , 16 },
     { MAD_F(0x0537255e)  , 16 },
     { MAD_F(0x0538274e)  , 16 },
     { MAD_F(0x0539294a)  , 16 },
     { MAD_F(0x053a2b52)  , 16 },
     { MAD_F(0x053b2d67)  , 16 },
     { MAD_F(0x053c2f89)  , 16 },
     { MAD_F(0x053d31b6)  , 16 },
     { MAD_F(0x053e33f1)  , 16 },
     { MAD_F(0x053f3637)  , 16 },

     { MAD_F(0x0540388a)  , 16 },
     { MAD_F(0x05413aea)  , 16 },
     { MAD_F(0x05423d56)  , 16 },
     { MAD_F(0x05433fce)  , 16 },
     { MAD_F(0x05444253)  , 16 },
     { MAD_F(0x054544e4)  , 16 },
     { MAD_F(0x05464781)  , 16 },
     { MAD_F(0x05474a2b)  , 16 },
     { MAD_F(0x05484ce2)  , 16 },
     { MAD_F(0x05494fa4)  , 16 },
     { MAD_F(0x054a5273)  , 16 },
     { MAD_F(0x054b554e)  , 16 },
     { MAD_F(0x054c5836)  , 16 },
     { MAD_F(0x054d5b2a)  , 16 },
     { MAD_F(0x054e5e2b)  , 16 },
     { MAD_F(0x054f6138)  , 16 },

     { MAD_F(0x05506451)  , 16 },
     { MAD_F(0x05516776)  , 16 },
     { MAD_F(0x05526aa8)  , 16 },
     { MAD_F(0x05536de6)  , 16 },
     { MAD_F(0x05547131)  , 16 },
     { MAD_F(0x05557487)  , 16 },
     { MAD_F(0x055677ea)  , 16 },
     { MAD_F(0x05577b5a)  , 16 },
     { MAD_F(0x05587ed5)  , 16 },
     { MAD_F(0x0559825e)  , 16 },
     { MAD_F(0x055a85f2)  , 16 },
     { MAD_F(0x055b8992)  , 16 },
     { MAD_F(0x055c8d3f)  , 16 },
     { MAD_F(0x055d90f9)  , 16 },
     { MAD_F(0x055e94be)  , 16 },
     { MAD_F(0x055f9890)  , 16 },

     { MAD_F(0x05609c6e)  , 16 },
     { MAD_F(0x0561a058)  , 16 },
     { MAD_F(0x0562a44f)  , 16 },
     { MAD_F(0x0563a851)  , 16 },
     { MAD_F(0x0564ac60)  , 16 },
     { MAD_F(0x0565b07c)  , 16 },
     { MAD_F(0x0566b4a3)  , 16 },
     { MAD_F(0x0567b8d7)  , 16 },
     { MAD_F(0x0568bd17)  , 16 },
     { MAD_F(0x0569c163)  , 16 },
     { MAD_F(0x056ac5bc)  , 16 },
     { MAD_F(0x056bca20)  , 16 },
     { MAD_F(0x056cce91)  , 16 },
     { MAD_F(0x056dd30e)  , 16 },
     { MAD_F(0x056ed798)  , 16 },
     { MAD_F(0x056fdc2d)  , 16 },

     { MAD_F(0x0570e0cf)  , 16 },
     { MAD_F(0x0571e57d)  , 16 },
     { MAD_F(0x0572ea37)  , 16 },
     { MAD_F(0x0573eefd)  , 16 },
     { MAD_F(0x0574f3d0)  , 16 },
     { MAD_F(0x0575f8ae)  , 16 },
     { MAD_F(0x0576fd99)  , 16 },
     { MAD_F(0x05780290)  , 16 },
     { MAD_F(0x05790793)  , 16 },
     { MAD_F(0x057a0ca3)  , 16 },
     { MAD_F(0x057b11be)  , 16 },
     { MAD_F(0x057c16e6)  , 16 },
     { MAD_F(0x057d1c1a)  , 16 },
     { MAD_F(0x057e2159)  , 16 },
     { MAD_F(0x057f26a6)  , 16 },
     { MAD_F(0x05802bfe)  , 16 },

     { MAD_F(0x05813162)  , 16 },
     { MAD_F(0x058236d2)  , 16 },
     { MAD_F(0x05833c4f)  , 16 },
     { MAD_F(0x058441d8)  , 16 },
     { MAD_F(0x0585476c)  , 16 },
     { MAD_F(0x05864d0d)  , 16 },
     { MAD_F(0x058752ba)  , 16 },
     { MAD_F(0x05885873)  , 16 },
     { MAD_F(0x05895e39)  , 16 },
     { MAD_F(0x058a640a)  , 16 },
     { MAD_F(0x058b69e7)  , 16 },
     { MAD_F(0x058c6fd1)  , 16 },
     { MAD_F(0x058d75c6)  , 16 },
     { MAD_F(0x058e7bc8)  , 16 },
     { MAD_F(0x058f81d5)  , 16 },
     { MAD_F(0x059087ef)  , 16 },

     { MAD_F(0x05918e15)  , 16 },
     { MAD_F(0x05929447)  , 16 },
     { MAD_F(0x05939a84)  , 16 },
     { MAD_F(0x0594a0ce)  , 16 },
     { MAD_F(0x0595a724)  , 16 },
     { MAD_F(0x0596ad86)  , 16 },
     { MAD_F(0x0597b3f4)  , 16 },
     { MAD_F(0x0598ba6e)  , 16 },
     { MAD_F(0x0599c0f4)  , 16 },
     { MAD_F(0x059ac786)  , 16 },
     { MAD_F(0x059bce25)  , 16 },
     { MAD_F(0x059cd4cf)  , 16 },
     { MAD_F(0x059ddb85)  , 16 },
     { MAD_F(0x059ee247)  , 16 },
     { MAD_F(0x059fe915)  , 16 },
     { MAD_F(0x05a0efef)  , 16 },

     { MAD_F(0x05a1f6d5)  , 16 },
     { MAD_F(0x05a2fdc7)  , 16 },
     { MAD_F(0x05a404c5)  , 16 },
     { MAD_F(0x05a50bcf)  , 16 },
     { MAD_F(0x05a612e5)  , 16 },
     { MAD_F(0x05a71a07)  , 16 },
     { MAD_F(0x05a82135)  , 16 },
     { MAD_F(0x05a9286f)  , 16 },
     { MAD_F(0x05aa2fb5)  , 16 },
     { MAD_F(0x05ab3707)  , 16 },
     { MAD_F(0x05ac3e65)  , 16 },
     { MAD_F(0x05ad45ce)  , 16 },
     { MAD_F(0x05ae4d44)  , 16 },
     { MAD_F(0x05af54c6)  , 16 },
     { MAD_F(0x05b05c53)  , 16 },
     { MAD_F(0x05b163ed)  , 16 },

     { MAD_F(0x05b26b92)  , 16 },
     { MAD_F(0x05b37343)  , 16 },
     { MAD_F(0x05b47b00)  , 16 },
     { MAD_F(0x05b582c9)  , 16 },
     { MAD_F(0x05b68a9e)  , 16 },
     { MAD_F(0x05b7927f)  , 16 },
     { MAD_F(0x05b89a6c)  , 16 },
     { MAD_F(0x05b9a265)  , 16 },
     { MAD_F(0x05baaa69)  , 16 },
     { MAD_F(0x05bbb27a)  , 16 },
     { MAD_F(0x05bcba96)  , 16 },
     { MAD_F(0x05bdc2be)  , 16 },
     { MAD_F(0x05becaf2)  , 16 },
     { MAD_F(0x05bfd332)  , 16 },
     { MAD_F(0x05c0db7e)  , 16 },
     { MAD_F(0x05c1e3d6)  , 16 },

     { MAD_F(0x05c2ec39)  , 16 },
     { MAD_F(0x05c3f4a9)  , 16 },
     { MAD_F(0x05c4fd24)  , 16 },
     { MAD_F(0x05c605ab)  , 16 },
     { MAD_F(0x05c70e3e)  , 16 },
     { MAD_F(0x05c816dd)  , 16 },
     { MAD_F(0x05c91f87)  , 16 },
     { MAD_F(0x05ca283e)  , 16 },
     { MAD_F(0x05cb3100)  , 16 },
     { MAD_F(0x05cc39ce)  , 16 },
     { MAD_F(0x05cd42a8)  , 16 },
     { MAD_F(0x05ce4b8d)  , 16 },
     { MAD_F(0x05cf547f)  , 16 },
     { MAD_F(0x05d05d7c)  , 16 },
     { MAD_F(0x05d16685)  , 16 },
     { MAD_F(0x05d26f9a)  , 16 },

     { MAD_F(0x05d378bb)  , 16 },
     { MAD_F(0x05d481e7)  , 16 },
     { MAD_F(0x05d58b1f)  , 16 },
     { MAD_F(0x05d69463)  , 16 },
     { MAD_F(0x05d79db3)  , 16 },
     { MAD_F(0x05d8a70f)  , 16 },
     { MAD_F(0x05d9b076)  , 16 },
     { MAD_F(0x05dab9e9)  , 16 },
     { MAD_F(0x05dbc368)  , 16 },
     { MAD_F(0x05dcccf2)  , 16 },
     { MAD_F(0x05ddd689)  , 16 },
     { MAD_F(0x05dee02b)  , 16 },
     { MAD_F(0x05dfe9d8)  , 16 },
     { MAD_F(0x05e0f392)  , 16 },
     { MAD_F(0x05e1fd57)  , 16 },
     { MAD_F(0x05e30728)  , 16 },

     { MAD_F(0x05e41105)  , 16 },
     { MAD_F(0x05e51aed)  , 16 },
     { MAD_F(0x05e624e1)  , 16 },
     { MAD_F(0x05e72ee1)  , 16 },
     { MAD_F(0x05e838ed)  , 16 },
     { MAD_F(0x05e94304)  , 16 },
     { MAD_F(0x05ea4d27)  , 16 },
     { MAD_F(0x05eb5756)  , 16 },
     { MAD_F(0x05ec6190)  , 16 },
     { MAD_F(0x05ed6bd6)  , 16 },
     { MAD_F(0x05ee7628)  , 16 },
     { MAD_F(0x05ef8085)  , 16 },
     { MAD_F(0x05f08aee)  , 16 },
     { MAD_F(0x05f19563)  , 16 },
     { MAD_F(0x05f29fe3)  , 16 },
     { MAD_F(0x05f3aa6f)  , 16 },

     { MAD_F(0x05f4b507)  , 16 },
     { MAD_F(0x05f5bfab)  , 16 },
     { MAD_F(0x05f6ca5a)  , 16 },
     { MAD_F(0x05f7d514)  , 16 },
     { MAD_F(0x05f8dfdb)  , 16 },
     { MAD_F(0x05f9eaad)  , 16 },
     { MAD_F(0x05faf58a)  , 16 },
     { MAD_F(0x05fc0073)  , 16 },
     { MAD_F(0x05fd0b68)  , 16 },
     { MAD_F(0x05fe1669)  , 16 },
     { MAD_F(0x05ff2175)  , 16 },
     { MAD_F(0x06002c8d)  , 16 },
     { MAD_F(0x060137b0)  , 16 },
     { MAD_F(0x060242df)  , 16 },
     { MAD_F(0x06034e19)  , 16 },
     { MAD_F(0x0604595f)  , 16 },

     { MAD_F(0x060564b1)  , 16 },
     { MAD_F(0x0606700f)  , 16 },
     { MAD_F(0x06077b77)  , 16 },
     { MAD_F(0x060886ec)  , 16 },
     { MAD_F(0x0609926c)  , 16 },
     { MAD_F(0x060a9df8)  , 16 },
     { MAD_F(0x060ba98f)  , 16 },
     { MAD_F(0x060cb532)  , 16 },
     { MAD_F(0x060dc0e0)  , 16 },
     { MAD_F(0x060ecc9a)  , 16 },
     { MAD_F(0x060fd860)  , 16 },
     { MAD_F(0x0610e431)  , 16 },
     { MAD_F(0x0611f00d)  , 16 },
     { MAD_F(0x0612fbf5)  , 16 },
     { MAD_F(0x061407e9)  , 16 },
     { MAD_F(0x061513e8)  , 16 },

     { MAD_F(0x06161ff3)  , 16 },
     { MAD_F(0x06172c09)  , 16 },
     { MAD_F(0x0618382b)  , 16 },
     { MAD_F(0x06194458)  , 16 },
     { MAD_F(0x061a5091)  , 16 },
     { MAD_F(0x061b5cd5)  , 16 },
     { MAD_F(0x061c6925)  , 16 },
     { MAD_F(0x061d7581)  , 16 },
     { MAD_F(0x061e81e8)  , 16 },
     { MAD_F(0x061f8e5a)  , 16 },
     { MAD_F(0x06209ad8)  , 16 },
     { MAD_F(0x0621a761)  , 16 },
     { MAD_F(0x0622b3f6)  , 16 },
     { MAD_F(0x0623c096)  , 16 },
     { MAD_F(0x0624cd42)  , 16 },
     { MAD_F(0x0625d9f9)  , 16 },

     { MAD_F(0x0626e6bc)  , 16 },
     { MAD_F(0x0627f38a)  , 16 },
     { MAD_F(0x06290064)  , 16 },
     { MAD_F(0x062a0d49)  , 16 },
     { MAD_F(0x062b1a3a)  , 16 },
     { MAD_F(0x062c2736)  , 16 },
     { MAD_F(0x062d343d)  , 16 },
     { MAD_F(0x062e4150)  , 16 },
     { MAD_F(0x062f4e6f)  , 16 },
     { MAD_F(0x06305b99)  , 16 },
     { MAD_F(0x063168ce)  , 16 },
     { MAD_F(0x0632760f)  , 16 },
     { MAD_F(0x0633835b)  , 16 },
     { MAD_F(0x063490b2)  , 16 },
     { MAD_F(0x06359e15)  , 16 },
     { MAD_F(0x0636ab83)  , 16 },

     { MAD_F(0x0637b8fd)  , 16 },
     { MAD_F(0x0638c682)  , 16 },
     { MAD_F(0x0639d413)  , 16 },
     { MAD_F(0x063ae1af)  , 16 },
     { MAD_F(0x063bef56)  , 16 },
     { MAD_F(0x063cfd09)  , 16 },
     { MAD_F(0x063e0ac7)  , 16 },
     { MAD_F(0x063f1891)  , 16 },
     { MAD_F(0x06402666)  , 16 },
     { MAD_F(0x06413446)  , 16 },
     { MAD_F(0x06424232)  , 16 },
     { MAD_F(0x06435029)  , 16 },
     { MAD_F(0x06445e2b)  , 16 },
     { MAD_F(0x06456c39)  , 16 },
     { MAD_F(0x06467a52)  , 16 },
     { MAD_F(0x06478877)  , 16 },

     { MAD_F(0x064896a7)  , 16 },
     { MAD_F(0x0649a4e2)  , 16 },
     { MAD_F(0x064ab328)  , 16 },
     { MAD_F(0x064bc17a)  , 16 },
     { MAD_F(0x064ccfd8)  , 16 },
     { MAD_F(0x064dde40)  , 16 },
     { MAD_F(0x064eecb4)  , 16 },
     { MAD_F(0x064ffb33)  , 16 },
     { MAD_F(0x065109be)  , 16 },
     { MAD_F(0x06521854)  , 16 },
     { MAD_F(0x065326f5)  , 16 },
     { MAD_F(0x065435a1)  , 16 },
     { MAD_F(0x06554459)  , 16 },
     { MAD_F(0x0656531c)  , 16 },
     { MAD_F(0x065761ea)  , 16 },
     { MAD_F(0x065870c4)  , 16 },

     { MAD_F(0x06597fa9)  , 16 },
     { MAD_F(0x065a8e99)  , 16 },
     { MAD_F(0x065b9d95)  , 16 },
     { MAD_F(0x065cac9c)  , 16 },
     { MAD_F(0x065dbbae)  , 16 },
     { MAD_F(0x065ecacb)  , 16 },
     { MAD_F(0x065fd9f4)  , 16 },
     { MAD_F(0x0660e928)  , 16 },
     { MAD_F(0x0661f867)  , 16 },
     { MAD_F(0x066307b1)  , 16 },
     { MAD_F(0x06641707)  , 16 },
     { MAD_F(0x06652668)  , 16 },
     { MAD_F(0x066635d4)  , 16 },
     { MAD_F(0x0667454c)  , 16 },
     { MAD_F(0x066854ce)  , 16 },
     { MAD_F(0x0669645c)  , 16 },

     { MAD_F(0x066a73f5)  , 16 },
     { MAD_F(0x066b839a)  , 16 },
     { MAD_F(0x066c9349)  , 16 },
     { MAD_F(0x066da304)  , 16 },
     { MAD_F(0x066eb2ca)  , 16 },
     { MAD_F(0x066fc29b)  , 16 },
     { MAD_F(0x0670d278)  , 16 },
     { MAD_F(0x0671e25f)  , 16 },
     { MAD_F(0x0672f252)  , 16 },
     { MAD_F(0x06740250)  , 16 },
     { MAD_F(0x0675125a)  , 16 },
     { MAD_F(0x0676226e)  , 16 },
     { MAD_F(0x0677328e)  , 16 },
     { MAD_F(0x067842b9)  , 16 },
     { MAD_F(0x067952ef)  , 16 },
     { MAD_F(0x067a6330)  , 16 },

     { MAD_F(0x067b737c)  , 16 },
     { MAD_F(0x067c83d4)  , 16 },
     { MAD_F(0x067d9436)  , 16 },
     { MAD_F(0x067ea4a4)  , 16 },
     { MAD_F(0x067fb51d)  , 16 },
     { MAD_F(0x0680c5a2)  , 16 },
     { MAD_F(0x0681d631)  , 16 },
     { MAD_F(0x0682e6cb)  , 16 },
     { MAD_F(0x0683f771)  , 16 },
     { MAD_F(0x06850822)  , 16 },
     { MAD_F(0x068618de)  , 16 },
     { MAD_F(0x068729a5)  , 16 },
     { MAD_F(0x06883a77)  , 16 },
     { MAD_F(0x06894b55)  , 16 },
     { MAD_F(0x068a5c3d)  , 16 },
     { MAD_F(0x068b6d31)  , 16 },

     { MAD_F(0x068c7e2f)  , 16 },
     { MAD_F(0x068d8f39)  , 16 },
     { MAD_F(0x068ea04e)  , 16 },
     { MAD_F(0x068fb16e)  , 16 },
     { MAD_F(0x0690c299)  , 16 },
     { MAD_F(0x0691d3cf)  , 16 },
     { MAD_F(0x0692e511)  , 16 },
     { MAD_F(0x0693f65d)  , 16 },
     { MAD_F(0x069507b5)  , 16 },
     { MAD_F(0x06961917)  , 16 },
     { MAD_F(0x06972a85)  , 16 },
     { MAD_F(0x06983bfe)  , 16 },
     { MAD_F(0x06994d82)  , 16 },
     { MAD_F(0x069a5f11)  , 16 },
     { MAD_F(0x069b70ab)  , 16 },
     { MAD_F(0x069c8250)  , 16 },

     { MAD_F(0x069d9400)  , 16 },
     { MAD_F(0x069ea5bb)  , 16 },
     { MAD_F(0x069fb781)  , 16 },
     { MAD_F(0x06a0c953)  , 16 },
     { MAD_F(0x06a1db2f)  , 16 },
     { MAD_F(0x06a2ed16)  , 16 },
     { MAD_F(0x06a3ff09)  , 16 },
     { MAD_F(0x06a51106)  , 16 },
     { MAD_F(0x06a6230f)  , 16 },
     { MAD_F(0x06a73522)  , 16 },
     { MAD_F(0x06a84741)  , 16 },
     { MAD_F(0x06a9596a)  , 16 },
     { MAD_F(0x06aa6b9f)  , 16 },
     { MAD_F(0x06ab7ddf)  , 16 },
     { MAD_F(0x06ac9029)  , 16 },
     { MAD_F(0x06ada27f)  , 16 },

     { MAD_F(0x06aeb4e0)  , 16 },
     { MAD_F(0x06afc74b)  , 16 },
     { MAD_F(0x06b0d9c2)  , 16 },
     { MAD_F(0x06b1ec43)  , 16 },
     { MAD_F(0x06b2fed0)  , 16 },
     { MAD_F(0x06b41168)  , 16 },
     { MAD_F(0x06b5240a)  , 16 },
     { MAD_F(0x06b636b8)  , 16 },
     { MAD_F(0x06b74971)  , 16 },
     { MAD_F(0x06b85c34)  , 16 },
     { MAD_F(0x06b96f03)  , 16 },
     { MAD_F(0x06ba81dc)  , 16 },
     { MAD_F(0x06bb94c1)  , 16 },
     { MAD_F(0x06bca7b0)  , 16 },
     { MAD_F(0x06bdbaaa)  , 16 },
     { MAD_F(0x06becdb0)  , 16 },

     { MAD_F(0x06bfe0c0)  , 16 },
     { MAD_F(0x06c0f3db)  , 16 },
     { MAD_F(0x06c20702)  , 16 },
     { MAD_F(0x06c31a33)  , 16 },
     { MAD_F(0x06c42d6f)  , 16 },
     { MAD_F(0x06c540b6)  , 16 },
     { MAD_F(0x06c65408)  , 16 },
     { MAD_F(0x06c76765)  , 16 },
     { MAD_F(0x06c87acc)  , 16 },
     { MAD_F(0x06c98e3f)  , 16 },
     { MAD_F(0x06caa1bd)  , 16 },
     { MAD_F(0x06cbb545)  , 16 },
     { MAD_F(0x06ccc8d9)  , 16 },
     { MAD_F(0x06cddc77)  , 16 },
     { MAD_F(0x06cef020)  , 16 },
     { MAD_F(0x06d003d4)  , 16 },

     { MAD_F(0x06d11794)  , 16 },
     { MAD_F(0x06d22b5e)  , 16 },
     { MAD_F(0x06d33f32)  , 16 },
     { MAD_F(0x06d45312)  , 16 },
     { MAD_F(0x06d566fd)  , 16 },
     { MAD_F(0x06d67af2)  , 16 },
     { MAD_F(0x06d78ef3)  , 16 },
     { MAD_F(0x06d8a2fe)  , 16 },
     { MAD_F(0x06d9b714)  , 16 },
     { MAD_F(0x06dacb35)  , 16 },
     { MAD_F(0x06dbdf61)  , 16 },
     { MAD_F(0x06dcf398)  , 16 },
     { MAD_F(0x06de07d9)  , 16 },
     { MAD_F(0x06df1c26)  , 16 },
     { MAD_F(0x06e0307d)  , 16 },
     { MAD_F(0x06e144df)  , 16 },

     { MAD_F(0x06e2594c)  , 16 },
     { MAD_F(0x06e36dc4)  , 16 },
     { MAD_F(0x06e48246)  , 16 },
     { MAD_F(0x06e596d4)  , 16 },
     { MAD_F(0x06e6ab6c)  , 16 },
     { MAD_F(0x06e7c00f)  , 16 },
     { MAD_F(0x06e8d4bd)  , 16 },
     { MAD_F(0x06e9e976)  , 16 },
     { MAD_F(0x06eafe3a)  , 16 },
     { MAD_F(0x06ec1308)  , 16 },
     { MAD_F(0x06ed27e2)  , 16 },
     { MAD_F(0x06ee3cc6)  , 16 },
     { MAD_F(0x06ef51b4)  , 16 },
     { MAD_F(0x06f066ae)  , 16 },
     { MAD_F(0x06f17bb3)  , 16 },
     { MAD_F(0x06f290c2)  , 16 },

     { MAD_F(0x06f3a5dc)  , 16 },
     { MAD_F(0x06f4bb01)  , 16 },
     { MAD_F(0x06f5d030)  , 16 },
     { MAD_F(0x06f6e56b)  , 16 },
     { MAD_F(0x06f7fab0)  , 16 },
     { MAD_F(0x06f91000)  , 16 },
     { MAD_F(0x06fa255a)  , 16 },
     { MAD_F(0x06fb3ac0)  , 16 },
     { MAD_F(0x06fc5030)  , 16 },
     { MAD_F(0x06fd65ab)  , 16 },
     { MAD_F(0x06fe7b31)  , 16 },
     { MAD_F(0x06ff90c2)  , 16 },
     { MAD_F(0x0700a65d)  , 16 },
     { MAD_F(0x0701bc03)  , 16 },
     { MAD_F(0x0702d1b4)  , 16 },
     { MAD_F(0x0703e76f)  , 16 },

     { MAD_F(0x0704fd35)  , 16 },
     { MAD_F(0x07061306)  , 16 },
     { MAD_F(0x070728e2)  , 16 },
     { MAD_F(0x07083ec9)  , 16 },
     { MAD_F(0x070954ba)  , 16 },
     { MAD_F(0x070a6ab6)  , 16 },
     { MAD_F(0x070b80bc)  , 16 },
     { MAD_F(0x070c96ce)  , 16 },
     { MAD_F(0x070dacea)  , 16 },
     { MAD_F(0x070ec310)  , 16 },
     { MAD_F(0x070fd942)  , 16 },
     { MAD_F(0x0710ef7e)  , 16 },
     { MAD_F(0x071205c5)  , 16 },
     { MAD_F(0x07131c17)  , 16 },
     { MAD_F(0x07143273)  , 16 },
     { MAD_F(0x071548da)  , 16 },

     { MAD_F(0x07165f4b)  , 16 },
     { MAD_F(0x071775c8)  , 16 },
     { MAD_F(0x07188c4f)  , 16 },
     { MAD_F(0x0719a2e0)  , 16 },
     { MAD_F(0x071ab97d)  , 16 },
     { MAD_F(0x071bd024)  , 16 },
     { MAD_F(0x071ce6d6)  , 16 },
     { MAD_F(0x071dfd92)  , 16 },
     { MAD_F(0x071f1459)  , 16 },
     { MAD_F(0x07202b2b)  , 16 },
     { MAD_F(0x07214207)  , 16 },
     { MAD_F(0x072258ee)  , 16 },
     { MAD_F(0x07236fe0)  , 16 },
     { MAD_F(0x072486dc)  , 16 },
     { MAD_F(0x07259de3)  , 16 },
     { MAD_F(0x0726b4f4)  , 16 },

     { MAD_F(0x0727cc11)  , 16 },
     { MAD_F(0x0728e338)  , 16 },
     { MAD_F(0x0729fa69)  , 16 },
     { MAD_F(0x072b11a5)  , 16 },
     { MAD_F(0x072c28ec)  , 16 },
     { MAD_F(0x072d403d)  , 16 },
     { MAD_F(0x072e5799)  , 16 },
     { MAD_F(0x072f6f00)  , 16 },
     { MAD_F(0x07308671)  , 16 },
     { MAD_F(0x07319ded)  , 16 },
     { MAD_F(0x0732b573)  , 16 },
     { MAD_F(0x0733cd04)  , 16 },
     { MAD_F(0x0734e4a0)  , 16 },
     { MAD_F(0x0735fc46)  , 16 },
     { MAD_F(0x073713f7)  , 16 },
     { MAD_F(0x07382bb2)  , 16 },

     { MAD_F(0x07394378)  , 16 },
     { MAD_F(0x073a5b49)  , 16 },
     { MAD_F(0x073b7324)  , 16 },
     { MAD_F(0x073c8b0a)  , 16 },
     { MAD_F(0x073da2fa)  , 16 },
     { MAD_F(0x073ebaf5)  , 16 },
     { MAD_F(0x073fd2fa)  , 16 },
     { MAD_F(0x0740eb0a)  , 16 },
     { MAD_F(0x07420325)  , 16 },
     { MAD_F(0x07431b4a)  , 16 },
     { MAD_F(0x0744337a)  , 16 },
     { MAD_F(0x07454bb4)  , 16 },
     { MAD_F(0x074663f8)  , 16 },
     { MAD_F(0x07477c48)  , 16 },
     { MAD_F(0x074894a2)  , 16 },
     { MAD_F(0x0749ad06)  , 16 },

     { MAD_F(0x074ac575)  , 16 },
     { MAD_F(0x074bddee)  , 16 },
     { MAD_F(0x074cf672)  , 16 },
     { MAD_F(0x074e0f01)  , 16 },
     { MAD_F(0x074f279a)  , 16 },
     { MAD_F(0x0750403e)  , 16 },
     { MAD_F(0x075158ec)  , 16 },
     { MAD_F(0x075271a4)  , 16 },
     { MAD_F(0x07538a67)  , 16 },
     { MAD_F(0x0754a335)  , 16 },
     { MAD_F(0x0755bc0d)  , 16 },
     { MAD_F(0x0756d4f0)  , 16 },
     { MAD_F(0x0757eddd)  , 16 },
     { MAD_F(0x075906d5)  , 16 },
     { MAD_F(0x075a1fd7)  , 16 },
     { MAD_F(0x075b38e3)  , 16 },

     { MAD_F(0x075c51fa)  , 16 },
     { MAD_F(0x075d6b1c)  , 16 },
     { MAD_F(0x075e8448)  , 16 },
     { MAD_F(0x075f9d7f)  , 16 },
     { MAD_F(0x0760b6c0)  , 16 },
     { MAD_F(0x0761d00b)  , 16 },
     { MAD_F(0x0762e961)  , 16 },
     { MAD_F(0x076402c1)  , 16 },
     { MAD_F(0x07651c2c)  , 16 },
     { MAD_F(0x076635a2)  , 16 },
     { MAD_F(0x07674f22)  , 16 },
     { MAD_F(0x076868ac)  , 16 },
     { MAD_F(0x07698240)  , 16 },
     { MAD_F(0x076a9be0)  , 16 },
     { MAD_F(0x076bb589)  , 16 },
     { MAD_F(0x076ccf3d)  , 16 },

     { MAD_F(0x076de8fc)  , 16 },
     { MAD_F(0x076f02c5)  , 16 },
     { MAD_F(0x07701c98)  , 16 },
     { MAD_F(0x07713676)  , 16 },
     { MAD_F(0x0772505e)  , 16 },
     { MAD_F(0x07736a51)  , 16 },
     { MAD_F(0x0774844e)  , 16 },
     { MAD_F(0x07759e55)  , 16 },
     { MAD_F(0x0776b867)  , 16 },
     { MAD_F(0x0777d283)  , 16 },
     { MAD_F(0x0778ecaa)  , 16 },
     { MAD_F(0x077a06db)  , 16 },
     { MAD_F(0x077b2117)  , 16 },
     { MAD_F(0x077c3b5d)  , 16 },
     { MAD_F(0x077d55ad)  , 16 },
     { MAD_F(0x077e7008)  , 16 },

     { MAD_F(0x077f8a6d)  , 16 },
     { MAD_F(0x0780a4dc)  , 16 },
     { MAD_F(0x0781bf56)  , 16 },
     { MAD_F(0x0782d9da)  , 16 },
     { MAD_F(0x0783f469)  , 16 },
     { MAD_F(0x07850f02)  , 16 },
     { MAD_F(0x078629a5)  , 16 },
     { MAD_F(0x07874453)  , 16 },
     { MAD_F(0x07885f0b)  , 16 },
     { MAD_F(0x078979ce)  , 16 },
     { MAD_F(0x078a949a)  , 16 },
     { MAD_F(0x078baf72)  , 16 },
     { MAD_F(0x078cca53)  , 16 },
     { MAD_F(0x078de53f)  , 16 },
     { MAD_F(0x078f0035)  , 16 },
     { MAD_F(0x07901b36)  , 16 },

     { MAD_F(0x07913641)  , 16 },
     { MAD_F(0x07925156)  , 16 },
     { MAD_F(0x07936c76)  , 16 },
     { MAD_F(0x079487a0)  , 16 },
     { MAD_F(0x0795a2d4)  , 16 },
     { MAD_F(0x0796be13)  , 16 },
     { MAD_F(0x0797d95c)  , 16 },
     { MAD_F(0x0798f4af)  , 16 },
     { MAD_F(0x079a100c)  , 16 },
     { MAD_F(0x079b2b74)  , 16 },
     { MAD_F(0x079c46e7)  , 16 },
     { MAD_F(0x079d6263)  , 16 },
     { MAD_F(0x079e7dea)  , 16 },
     { MAD_F(0x079f997b)  , 16 },
     { MAD_F(0x07a0b516)  , 16 },
     { MAD_F(0x07a1d0bc)  , 16 },

     { MAD_F(0x07a2ec6c)  , 16 },
     { MAD_F(0x07a40827)  , 16 },
     { MAD_F(0x07a523eb)  , 16 },
     { MAD_F(0x07a63fba)  , 16 },
     { MAD_F(0x07a75b93)  , 16 },
     { MAD_F(0x07a87777)  , 16 },
     { MAD_F(0x07a99364)  , 16 },
     { MAD_F(0x07aaaf5c)  , 16 },
     { MAD_F(0x07abcb5f)  , 16 },
     { MAD_F(0x07ace76b)  , 16 },
     { MAD_F(0x07ae0382)  , 16 },
     { MAD_F(0x07af1fa3)  , 16 },
     { MAD_F(0x07b03bcf)  , 16 },
     { MAD_F(0x07b15804)  , 16 },
     { MAD_F(0x07b27444)  , 16 },
     { MAD_F(0x07b3908e)  , 16 },

     { MAD_F(0x07b4ace3)  , 16 },
     { MAD_F(0x07b5c941)  , 16 },
     { MAD_F(0x07b6e5aa)  , 16 },
     { MAD_F(0x07b8021d)  , 16 },
     { MAD_F(0x07b91e9b)  , 16 },
     { MAD_F(0x07ba3b22)  , 16 },
     { MAD_F(0x07bb57b4)  , 16 },
     { MAD_F(0x07bc7450)  , 16 },
     { MAD_F(0x07bd90f6)  , 16 },
     { MAD_F(0x07beada7)  , 16 },
     { MAD_F(0x07bfca61)  , 16 },
     { MAD_F(0x07c0e726)  , 16 },
     { MAD_F(0x07c203f5)  , 16 },
     { MAD_F(0x07c320cf)  , 16 },
     { MAD_F(0x07c43db2)  , 16 },
     { MAD_F(0x07c55aa0)  , 16 },

     { MAD_F(0x07c67798)  , 16 },
     { MAD_F(0x07c7949a)  , 16 },
     { MAD_F(0x07c8b1a7)  , 16 },
     { MAD_F(0x07c9cebd)  , 16 },
     { MAD_F(0x07caebde)  , 16 },
     { MAD_F(0x07cc0909)  , 16 },
     { MAD_F(0x07cd263e)  , 16 },
     { MAD_F(0x07ce437d)  , 16 },
     { MAD_F(0x07cf60c7)  , 16 },
     { MAD_F(0x07d07e1b)  , 16 },
     { MAD_F(0x07d19b79)  , 16 },
     { MAD_F(0x07d2b8e1)  , 16 },
     { MAD_F(0x07d3d653)  , 16 },
     { MAD_F(0x07d4f3cf)  , 16 },
     { MAD_F(0x07d61156)  , 16 },
     { MAD_F(0x07d72ee6)  , 16 },

     { MAD_F(0x07d84c81)  , 16 },
     { MAD_F(0x07d96a26)  , 16 },
     { MAD_F(0x07da87d5)  , 16 },
     { MAD_F(0x07dba58f)  , 16 },
     { MAD_F(0x07dcc352)  , 16 },
     { MAD_F(0x07dde120)  , 16 },
     { MAD_F(0x07defef7)  , 16 },
     { MAD_F(0x07e01cd9)  , 16 },
     { MAD_F(0x07e13ac5)  , 16 },
     { MAD_F(0x07e258bc)  , 16 },
     { MAD_F(0x07e376bc)  , 16 },
     { MAD_F(0x07e494c6)  , 16 },
     { MAD_F(0x07e5b2db)  , 16 },
     { MAD_F(0x07e6d0f9)  , 16 },
     { MAD_F(0x07e7ef22)  , 16 },
     { MAD_F(0x07e90d55)  , 16 },

     { MAD_F(0x07ea2b92)  , 16 },
     { MAD_F(0x07eb49d9)  , 16 },
     { MAD_F(0x07ec682a)  , 16 },
     { MAD_F(0x07ed8686)  , 16 },
     { MAD_F(0x07eea4eb)  , 16 },
     { MAD_F(0x07efc35b)  , 16 },
     { MAD_F(0x07f0e1d4)  , 16 },
     { MAD_F(0x07f20058)  , 16 },
     { MAD_F(0x07f31ee6)  , 16 },
     { MAD_F(0x07f43d7e)  , 16 },
     { MAD_F(0x07f55c20)  , 16 },
     { MAD_F(0x07f67acc)  , 16 },
     { MAD_F(0x07f79982)  , 16 },
     { MAD_F(0x07f8b842)  , 16 },
     { MAD_F(0x07f9d70c)  , 16 },
     { MAD_F(0x07faf5e1)  , 16 },

     { MAD_F(0x07fc14bf)  , 16 },
     { MAD_F(0x07fd33a8)  , 16 },
     { MAD_F(0x07fe529a)  , 16 },
     { MAD_F(0x07ff7197)  , 16 },
     { MAD_F(0x0400484f)  , 17 },
     { MAD_F(0x0400d7d7)  , 17 },
     { MAD_F(0x04016764)  , 17 },
     { MAD_F(0x0401f6f7)  , 17 },
     { MAD_F(0x0402868e)  , 17 },
     { MAD_F(0x0403162b)  , 17 },
     { MAD_F(0x0403a5cc)  , 17 },
     { MAD_F(0x04043573)  , 17 },
     { MAD_F(0x0404c51e)  , 17 },
     { MAD_F(0x040554cf)  , 17 },
     { MAD_F(0x0405e484)  , 17 },
     { MAD_F(0x0406743f)  , 17 },

     { MAD_F(0x040703ff)  , 17 },
     { MAD_F(0x040793c3)  , 17 },
     { MAD_F(0x0408238d)  , 17 },
     { MAD_F(0x0408b35b)  , 17 },
     { MAD_F(0x0409432f)  , 17 },
     { MAD_F(0x0409d308)  , 17 },
     { MAD_F(0x040a62e5)  , 17 },
     { MAD_F(0x040af2c8)  , 17 },
     { MAD_F(0x040b82b0)  , 17 },
     { MAD_F(0x040c129c)  , 17 },
     { MAD_F(0x040ca28e)  , 17 },
     { MAD_F(0x040d3284)  , 17 },
     { MAD_F(0x040dc280)  , 17 },
     { MAD_F(0x040e5281)  , 17 },
     { MAD_F(0x040ee286)  , 17 },
     { MAD_F(0x040f7291)  , 17 },

     { MAD_F(0x041002a1)  , 17 },
     { MAD_F(0x041092b5)  , 17 },
     { MAD_F(0x041122cf)  , 17 },
     { MAD_F(0x0411b2ed)  , 17 },
     { MAD_F(0x04124311)  , 17 },
     { MAD_F(0x0412d339)  , 17 },
     { MAD_F(0x04136367)  , 17 },
     { MAD_F(0x0413f399)  , 17 },
     { MAD_F(0x041483d1)  , 17 },
     { MAD_F(0x0415140d)  , 17 },
     { MAD_F(0x0415a44f)  , 17 },
     { MAD_F(0x04163495)  , 17 },
     { MAD_F(0x0416c4e1)  , 17 },
     { MAD_F(0x04175531)  , 17 },
     { MAD_F(0x0417e586)  , 17 },
     { MAD_F(0x041875e1)  , 17 },

     { MAD_F(0x04190640)  , 17 },
     { MAD_F(0x041996a4)  , 17 },
     { MAD_F(0x041a270d)  , 17 },
     { MAD_F(0x041ab77b)  , 17 },
     { MAD_F(0x041b47ef)  , 17 },
     { MAD_F(0x041bd867)  , 17 },
     { MAD_F(0x041c68e4)  , 17 },
     { MAD_F(0x041cf966)  , 17 },
     { MAD_F(0x041d89ed)  , 17 },
     { MAD_F(0x041e1a79)  , 17 },
     { MAD_F(0x041eab0a)  , 17 },
     { MAD_F(0x041f3b9f)  , 17 },
     { MAD_F(0x041fcc3a)  , 17 },
     { MAD_F(0x04205cda)  , 17 },
     { MAD_F(0x0420ed7f)  , 17 },
     { MAD_F(0x04217e28)  , 17 },

     { MAD_F(0x04220ed7)  , 17 },
     { MAD_F(0x04229f8a)  , 17 },
     { MAD_F(0x04233043)  , 17 },
     { MAD_F(0x0423c100)  , 17 },
     { MAD_F(0x042451c3)  , 17 },
     { MAD_F(0x0424e28a)  , 17 },
     { MAD_F(0x04257356)  , 17 },
     { MAD_F(0x04260428)  , 17 },
     { MAD_F(0x042694fe)  , 17 },
     { MAD_F(0x042725d9)  , 17 },
     { MAD_F(0x0427b6b9)  , 17 },
     { MAD_F(0x0428479e)  , 17 },
     { MAD_F(0x0428d888)  , 17 },
     { MAD_F(0x04296976)  , 17 },
     { MAD_F(0x0429fa6a)  , 17 },
     { MAD_F(0x042a8b63)  , 17 },

     { MAD_F(0x042b1c60)  , 17 },
     { MAD_F(0x042bad63)  , 17 },
     { MAD_F(0x042c3e6a)  , 17 },
     { MAD_F(0x042ccf77)  , 17 },
     { MAD_F(0x042d6088)  , 17 },
     { MAD_F(0x042df19e)  , 17 },
     { MAD_F(0x042e82b9)  , 17 },
     { MAD_F(0x042f13d9)  , 17 },
     { MAD_F(0x042fa4fe)  , 17 },
     { MAD_F(0x04303628)  , 17 },
     { MAD_F(0x0430c757)  , 17 },
     { MAD_F(0x0431588b)  , 17 },
     { MAD_F(0x0431e9c3)  , 17 },
     { MAD_F(0x04327b01)  , 17 },
     { MAD_F(0x04330c43)  , 17 },
     { MAD_F(0x04339d8a)  , 17 },

     { MAD_F(0x04342ed7)  , 17 },
     { MAD_F(0x0434c028)  , 17 },
     { MAD_F(0x0435517e)  , 17 },
     { MAD_F(0x0435e2d9)  , 17 },
     { MAD_F(0x04367439)  , 17 },
     { MAD_F(0x0437059e)  , 17 },
     { MAD_F(0x04379707)  , 17 },
     { MAD_F(0x04382876)  , 17 },
     { MAD_F(0x0438b9e9)  , 17 },
     { MAD_F(0x04394b61)  , 17 },
     { MAD_F(0x0439dcdf)  , 17 },
     { MAD_F(0x043a6e61)  , 17 },
     { MAD_F(0x043affe8)  , 17 },
     { MAD_F(0x043b9174)  , 17 },
     { MAD_F(0x043c2305)  , 17 },
     { MAD_F(0x043cb49a)  , 17 },

     { MAD_F(0x043d4635)  , 17 },
     { MAD_F(0x043dd7d4)  , 17 },
     { MAD_F(0x043e6979)  , 17 },
     { MAD_F(0x043efb22)  , 17 },
     { MAD_F(0x043f8cd0)  , 17 },
     { MAD_F(0x04401e83)  , 17 },
     { MAD_F(0x0440b03b)  , 17 },
     { MAD_F(0x044141f7)  , 17 },
     { MAD_F(0x0441d3b9)  , 17 },
     { MAD_F(0x04426580)  , 17 },
     { MAD_F(0x0442f74b)  , 17 },
     { MAD_F(0x0443891b)  , 17 },
     { MAD_F(0x04441af0)  , 17 },
     { MAD_F(0x0444acca)  , 17 },
     { MAD_F(0x04453ea9)  , 17 },
     { MAD_F(0x0445d08d)  , 17 },

     { MAD_F(0x04466275)  , 17 },
     { MAD_F(0x0446f463)  , 17 },
     { MAD_F(0x04478655)  , 17 },
     { MAD_F(0x0448184c)  , 17 },
     { MAD_F(0x0448aa48)  , 17 },
     { MAD_F(0x04493c49)  , 17 },
     { MAD_F(0x0449ce4f)  , 17 },
     { MAD_F(0x044a6059)  , 17 },
     { MAD_F(0x044af269)  , 17 },
     { MAD_F(0x044b847d)  , 17 },
     { MAD_F(0x044c1696)  , 17 },
     { MAD_F(0x044ca8b4)  , 17 },
     { MAD_F(0x044d3ad7)  , 17 },
     { MAD_F(0x044dccff)  , 17 },
     { MAD_F(0x044e5f2b)  , 17 },
     { MAD_F(0x044ef15d)  , 17 },

     { MAD_F(0x044f8393)  , 17 },
     { MAD_F(0x045015ce)  , 17 },
     { MAD_F(0x0450a80e)  , 17 },
     { MAD_F(0x04513a53)  , 17 },
     { MAD_F(0x0451cc9c)  , 17 },
     { MAD_F(0x04525eeb)  , 17 },
     { MAD_F(0x0452f13e)  , 17 },
     { MAD_F(0x04538396)  , 17 },
     { MAD_F(0x045415f3)  , 17 },
     { MAD_F(0x0454a855)  , 17 },
     { MAD_F(0x04553abb)  , 17 },
     { MAD_F(0x0455cd27)  , 17 },
     { MAD_F(0x04565f97)  , 17 },
     { MAD_F(0x0456f20c)  , 17 },
     { MAD_F(0x04578486)  , 17 },
     { MAD_F(0x04581705)  , 17 },

     { MAD_F(0x0458a989)  , 17 },
     { MAD_F(0x04593c11)  , 17 },
     { MAD_F(0x0459ce9e)  , 17 },
     { MAD_F(0x045a6130)  , 17 },
     { MAD_F(0x045af3c7)  , 17 },
     { MAD_F(0x045b8663)  , 17 },
     { MAD_F(0x045c1903)  , 17 },
     { MAD_F(0x045caba9)  , 17 },
     { MAD_F(0x045d3e53)  , 17 },
     { MAD_F(0x045dd102)  , 17 },
     { MAD_F(0x045e63b6)  , 17 },
     { MAD_F(0x045ef66e)  , 17 },
     { MAD_F(0x045f892b)  , 17 },
     { MAD_F(0x04601bee)  , 17 },
     { MAD_F(0x0460aeb5)  , 17 },
     { MAD_F(0x04614180)  , 17 },

     { MAD_F(0x0461d451)  , 17 },
     { MAD_F(0x04626727)  , 17 },
     { MAD_F(0x0462fa01)  , 17 },
     { MAD_F(0x04638ce0)  , 17 },
     { MAD_F(0x04641fc4)  , 17 },
     { MAD_F(0x0464b2ac)  , 17 },
     { MAD_F(0x0465459a)  , 17 },
     { MAD_F(0x0465d88c)  , 17 },
     { MAD_F(0x04666b83)  , 17 },
     { MAD_F(0x0466fe7f)  , 17 },
     { MAD_F(0x0467917f)  , 17 },
     { MAD_F(0x04682485)  , 17 },
     { MAD_F(0x0468b78f)  , 17 },
     { MAD_F(0x04694a9e)  , 17 },
     { MAD_F(0x0469ddb2)  , 17 },
     { MAD_F(0x046a70ca)  , 17 },

     { MAD_F(0x046b03e7)  , 17 },
     { MAD_F(0x046b970a)  , 17 },
     { MAD_F(0x046c2a31)  , 17 },
     { MAD_F(0x046cbd5c)  , 17 },
     { MAD_F(0x046d508d)  , 17 },
     { MAD_F(0x046de3c2)  , 17 },
     { MAD_F(0x046e76fc)  , 17 },
     { MAD_F(0x046f0a3b)  , 17 },
     { MAD_F(0x046f9d7e)  , 17 },
     { MAD_F(0x047030c7)  , 17 },
     { MAD_F(0x0470c414)  , 17 },
     { MAD_F(0x04715766)  , 17 },
     { MAD_F(0x0471eabc)  , 17 },
     { MAD_F(0x04727e18)  , 17 },
     { MAD_F(0x04731178)  , 17 },
     { MAD_F(0x0473a4dd)  , 17 },

     { MAD_F(0x04743847)  , 17 },
     { MAD_F(0x0474cbb5)  , 17 },
     { MAD_F(0x04755f29)  , 17 },
     { MAD_F(0x0475f2a1)  , 17 },
     { MAD_F(0x0476861d)  , 17 },
     { MAD_F(0x0477199f)  , 17 },
     { MAD_F(0x0477ad25)  , 17 },
     { MAD_F(0x047840b0)  , 17 },
     { MAD_F(0x0478d440)  , 17 },
     { MAD_F(0x047967d5)  , 17 },
     { MAD_F(0x0479fb6e)  , 17 },
     { MAD_F(0x047a8f0c)  , 17 },
     { MAD_F(0x047b22af)  , 17 },
     { MAD_F(0x047bb657)  , 17 },
     { MAD_F(0x047c4a03)  , 17 },
     { MAD_F(0x047cddb4)  , 17 },

     { MAD_F(0x047d716a)  , 17 },
     { MAD_F(0x047e0524)  , 17 },
     { MAD_F(0x047e98e4)  , 17 },
     { MAD_F(0x047f2ca8)  , 17 },
     { MAD_F(0x047fc071)  , 17 },
     { MAD_F(0x0480543e)  , 17 },
     { MAD_F(0x0480e811)  , 17 },
     { MAD_F(0x04817be8)  , 17 },
     { MAD_F(0x04820fc3)  , 17 },
     { MAD_F(0x0482a3a4)  , 17 },
     { MAD_F(0x04833789)  , 17 },
     { MAD_F(0x0483cb73)  , 17 },
     { MAD_F(0x04845f62)  , 17 },
     { MAD_F(0x0484f355)  , 17 },
     { MAD_F(0x0485874d)  , 17 },
     { MAD_F(0x04861b4a)  , 17 },

     { MAD_F(0x0486af4c)  , 17 },
     { MAD_F(0x04874352)  , 17 },
     { MAD_F(0x0487d75d)  , 17 },
     { MAD_F(0x04886b6d)  , 17 },
     { MAD_F(0x0488ff82)  , 17 },
     { MAD_F(0x0489939b)  , 17 },
     { MAD_F(0x048a27b9)  , 17 },
     { MAD_F(0x048abbdc)  , 17 },
     { MAD_F(0x048b5003)  , 17 },
     { MAD_F(0x048be42f)  , 17 },
     { MAD_F(0x048c7860)  , 17 },
     { MAD_F(0x048d0c96)  , 17 },
     { MAD_F(0x048da0d0)  , 17 },
     { MAD_F(0x048e350f)  , 17 },
     { MAD_F(0x048ec953)  , 17 },
     { MAD_F(0x048f5d9b)  , 17 },

     { MAD_F(0x048ff1e8)  , 17 },
     { MAD_F(0x0490863a)  , 17 },
     { MAD_F(0x04911a91)  , 17 },
     { MAD_F(0x0491aeec)  , 17 },
     { MAD_F(0x0492434c)  , 17 },
     { MAD_F(0x0492d7b0)  , 17 },
     { MAD_F(0x04936c1a)  , 17 },
     { MAD_F(0x04940088)  , 17 },
     { MAD_F(0x049494fb)  , 17 },
     { MAD_F(0x04952972)  , 17 },
     { MAD_F(0x0495bdee)  , 17 },
     { MAD_F(0x0496526f)  , 17 },
     { MAD_F(0x0496e6f5)  , 17 },
     { MAD_F(0x04977b7f)  , 17 },
     { MAD_F(0x0498100e)  , 17 },
     { MAD_F(0x0498a4a1)  , 17 },

     { MAD_F(0x0499393a)  , 17 },
     { MAD_F(0x0499cdd7)  , 17 },
     { MAD_F(0x049a6278)  , 17 },
     { MAD_F(0x049af71f)  , 17 },
     { MAD_F(0x049b8bca)  , 17 },
     { MAD_F(0x049c207a)  , 17 },
     { MAD_F(0x049cb52e)  , 17 },
     { MAD_F(0x049d49e7)  , 17 },
     { MAD_F(0x049ddea5)  , 17 },
     { MAD_F(0x049e7367)  , 17 },
     { MAD_F(0x049f082f)  , 17 },
     { MAD_F(0x049f9cfa)  , 17 },
     { MAD_F(0x04a031cb)  , 17 },
     { MAD_F(0x04a0c6a0)  , 17 },
     { MAD_F(0x04a15b7a)  , 17 },
     { MAD_F(0x04a1f059)  , 17 },

     { MAD_F(0x04a2853c)  , 17 },
     { MAD_F(0x04a31a24)  , 17 },
     { MAD_F(0x04a3af10)  , 17 },
     { MAD_F(0x04a44401)  , 17 },
     { MAD_F(0x04a4d8f7)  , 17 },
     { MAD_F(0x04a56df2)  , 17 },
     { MAD_F(0x04a602f1)  , 17 },
     { MAD_F(0x04a697f5)  , 17 },
     { MAD_F(0x04a72cfe)  , 17 },
     { MAD_F(0x04a7c20b)  , 17 },
     { MAD_F(0x04a8571d)  , 17 },
     { MAD_F(0x04a8ec33)  , 17 },
     { MAD_F(0x04a9814e)  , 17 },
     { MAD_F(0x04aa166e)  , 17 },
     { MAD_F(0x04aaab93)  , 17 },
     { MAD_F(0x04ab40bc)  , 17 },

     { MAD_F(0x04abd5ea)  , 17 },
     { MAD_F(0x04ac6b1c)  , 17 },
     { MAD_F(0x04ad0053)  , 17 },
     { MAD_F(0x04ad958f)  , 17 },
     { MAD_F(0x04ae2ad0)  , 17 },
     { MAD_F(0x04aec015)  , 17 },
     { MAD_F(0x04af555e)  , 17 },
     { MAD_F(0x04afeaad)  , 17 },
     { MAD_F(0x04b08000)  , 17 },
     { MAD_F(0x04b11557)  , 17 },
     { MAD_F(0x04b1aab4)  , 17 },
     { MAD_F(0x04b24015)  , 17 },
     { MAD_F(0x04b2d57a)  , 17 },
     { MAD_F(0x04b36ae4)  , 17 },
     { MAD_F(0x04b40053)  , 17 },
     { MAD_F(0x04b495c7)  , 17 },

     { MAD_F(0x04b52b3f)  , 17 },
     { MAD_F(0x04b5c0bc)  , 17 },
     { MAD_F(0x04b6563d)  , 17 },
     { MAD_F(0x04b6ebc3)  , 17 },
     { MAD_F(0x04b7814e)  , 17 },
     { MAD_F(0x04b816dd)  , 17 },
     { MAD_F(0x04b8ac71)  , 17 },
     { MAD_F(0x04b9420a)  , 17 },
     { MAD_F(0x04b9d7a7)  , 17 },
     { MAD_F(0x04ba6d49)  , 17 },
     { MAD_F(0x04bb02ef)  , 17 },
     { MAD_F(0x04bb989a)  , 17 },
     { MAD_F(0x04bc2e4a)  , 17 },
     { MAD_F(0x04bcc3fe)  , 17 },
     { MAD_F(0x04bd59b7)  , 17 },
     { MAD_F(0x04bdef74)  , 17 },

     { MAD_F(0x04be8537)  , 17 },
     { MAD_F(0x04bf1afd)  , 17 },
     { MAD_F(0x04bfb0c9)  , 17 },
     { MAD_F(0x04c04699)  , 17 },
     { MAD_F(0x04c0dc6d)  , 17 },
     { MAD_F(0x04c17247)  , 17 },
     { MAD_F(0x04c20824)  , 17 },
     { MAD_F(0x04c29e07)  , 17 },
     { MAD_F(0x04c333ee)  , 17 },
     { MAD_F(0x04c3c9da)  , 17 },
     { MAD_F(0x04c45fca)  , 17 },
     { MAD_F(0x04c4f5bf)  , 17 },
     { MAD_F(0x04c58bb8)  , 17 },
     { MAD_F(0x04c621b6)  , 17 },
     { MAD_F(0x04c6b7b9)  , 17 },
     { MAD_F(0x04c74dc0)  , 17 },

     { MAD_F(0x04c7e3cc)  , 17 },
     { MAD_F(0x04c879dd)  , 17 },
     { MAD_F(0x04c90ff2)  , 17 },
     { MAD_F(0x04c9a60c)  , 17 },
     { MAD_F(0x04ca3c2a)  , 17 },
     { MAD_F(0x04cad24d)  , 17 },
     { MAD_F(0x04cb6874)  , 17 },
     { MAD_F(0x04cbfea0)  , 17 },
     { MAD_F(0x04cc94d1)  , 17 },
     { MAD_F(0x04cd2b06)  , 17 },
     { MAD_F(0x04cdc140)  , 17 },
     { MAD_F(0x04ce577f)  , 17 },
     { MAD_F(0x04ceedc2)  , 17 },
     { MAD_F(0x04cf8409)  , 17 },
     { MAD_F(0x04d01a55)  , 17 },
     { MAD_F(0x04d0b0a6)  , 17 },

     { MAD_F(0x04d146fb)  , 17 },
     { MAD_F(0x04d1dd55)  , 17 },
     { MAD_F(0x04d273b4)  , 17 },
     { MAD_F(0x04d30a17)  , 17 },
     { MAD_F(0x04d3a07f)  , 17 },
     { MAD_F(0x04d436eb)  , 17 },
     { MAD_F(0x04d4cd5c)  , 17 },
     { MAD_F(0x04d563d1)  , 17 },
     { MAD_F(0x04d5fa4b)  , 17 },
     { MAD_F(0x04d690ca)  , 17 },
     { MAD_F(0x04d7274d)  , 17 },
     { MAD_F(0x04d7bdd5)  , 17 },
     { MAD_F(0x04d85461)  , 17 },
     { MAD_F(0x04d8eaf2)  , 17 },
     { MAD_F(0x04d98187)  , 17 },
     { MAD_F(0x04da1821)  , 17 },

     { MAD_F(0x04daaec0)  , 17 },
     { MAD_F(0x04db4563)  , 17 },
     { MAD_F(0x04dbdc0a)  , 17 },
     { MAD_F(0x04dc72b7)  , 17 },
     { MAD_F(0x04dd0967)  , 17 },
     { MAD_F(0x04dda01d)  , 17 },
     { MAD_F(0x04de36d7)  , 17 },
     { MAD_F(0x04decd95)  , 17 },
     { MAD_F(0x04df6458)  , 17 },
     { MAD_F(0x04dffb20)  , 17 },
     { MAD_F(0x04e091ec)  , 17 },
     { MAD_F(0x04e128bc)  , 17 },
     { MAD_F(0x04e1bf92)  , 17 },
     { MAD_F(0x04e2566b)  , 17 },
     { MAD_F(0x04e2ed4a)  , 17 },
     { MAD_F(0x04e3842d)  , 17 },

     { MAD_F(0x04e41b14)  , 17 },
     { MAD_F(0x04e4b200)  , 17 },
     { MAD_F(0x04e548f1)  , 17 },
     { MAD_F(0x04e5dfe6)  , 17 },
     { MAD_F(0x04e676df)  , 17 },
     { MAD_F(0x04e70dde)  , 17 },
     { MAD_F(0x04e7a4e0)  , 17 },
     { MAD_F(0x04e83be7)  , 17 },
     { MAD_F(0x04e8d2f3)  , 17 },
     { MAD_F(0x04e96a04)  , 17 },
     { MAD_F(0x04ea0118)  , 17 },
     { MAD_F(0x04ea9832)  , 17 },
     { MAD_F(0x04eb2f50)  , 17 },
     { MAD_F(0x04ebc672)  , 17 },
     { MAD_F(0x04ec5d99)  , 17 },
     { MAD_F(0x04ecf4c5)  , 17 },

     { MAD_F(0x04ed8bf5)  , 17 },
     { MAD_F(0x04ee2329)  , 17 },
     { MAD_F(0x04eeba63)  , 17 },
     { MAD_F(0x04ef51a0)  , 17 },
     { MAD_F(0x04efe8e2)  , 17 },
     { MAD_F(0x04f08029)  , 17 },
     { MAD_F(0x04f11774)  , 17 },
     { MAD_F(0x04f1aec4)  , 17 },
     { MAD_F(0x04f24618)  , 17 },
     { MAD_F(0x04f2dd71)  , 17 },
     { MAD_F(0x04f374cf)  , 17 },
     { MAD_F(0x04f40c30)  , 17 },
     { MAD_F(0x04f4a397)  , 17 },
     { MAD_F(0x04f53b02)  , 17 },
     { MAD_F(0x04f5d271)  , 17 },
     { MAD_F(0x04f669e5)  , 17 },

     { MAD_F(0x04f7015d)  , 17 },
     { MAD_F(0x04f798da)  , 17 },
     { MAD_F(0x04f8305c)  , 17 },
     { MAD_F(0x04f8c7e2)  , 17 },
     { MAD_F(0x04f95f6c)  , 17 },
     { MAD_F(0x04f9f6fb)  , 17 },
     { MAD_F(0x04fa8e8f)  , 17 },
     { MAD_F(0x04fb2627)  , 17 },
     { MAD_F(0x04fbbdc3)  , 17 },
     { MAD_F(0x04fc5564)  , 17 },
     { MAD_F(0x04fced0a)  , 17 },
     { MAD_F(0x04fd84b4)  , 17 },
     { MAD_F(0x04fe1c62)  , 17 },
     { MAD_F(0x04feb415)  , 17 },
     { MAD_F(0x04ff4bcd)  , 17 },
     { MAD_F(0x04ffe389)  , 17 },

     { MAD_F(0x05007b49)  , 17 },
     { MAD_F(0x0501130e)  , 17 },
     { MAD_F(0x0501aad8)  , 17 },
     { MAD_F(0x050242a6)  , 17 },
     { MAD_F(0x0502da78)  , 17 },
     { MAD_F(0x0503724f)  , 17 },
     { MAD_F(0x05040a2b)  , 17 },
     { MAD_F(0x0504a20b)  , 17 },
     { MAD_F(0x050539ef)  , 17 },
     { MAD_F(0x0505d1d8)  , 17 },
     { MAD_F(0x050669c5)  , 17 },
     { MAD_F(0x050701b7)  , 17 },
     { MAD_F(0x050799ae)  , 17 },
     { MAD_F(0x050831a9)  , 17 },
     { MAD_F(0x0508c9a8)  , 17 },
     { MAD_F(0x050961ac)  , 17 },

     { MAD_F(0x0509f9b4)  , 17 },
     { MAD_F(0x050a91c1)  , 17 },
     { MAD_F(0x050b29d2)  , 17 },
     { MAD_F(0x050bc1e8)  , 17 },
     { MAD_F(0x050c5a02)  , 17 },
     { MAD_F(0x050cf221)  , 17 },
     { MAD_F(0x050d8a44)  , 17 },
     { MAD_F(0x050e226c)  , 17 },
     { MAD_F(0x050eba98)  , 17 },
     { MAD_F(0x050f52c9)  , 17 },
     { MAD_F(0x050feafe)  , 17 },
     { MAD_F(0x05108337)  , 17 },
     { MAD_F(0x05111b75)  , 17 },
     { MAD_F(0x0511b3b8)  , 17 },
     { MAD_F(0x05124bff)  , 17 },
     { MAD_F(0x0512e44a)  , 17 },

     { MAD_F(0x05137c9a)  , 17 },
     { MAD_F(0x051414ee)  , 17 },
     { MAD_F(0x0514ad47)  , 17 },
     { MAD_F(0x051545a5)  , 17 },
     { MAD_F(0x0515de06)  , 17 },
     { MAD_F(0x0516766d)  , 17 },
     { MAD_F(0x05170ed7)  , 17 },
     { MAD_F(0x0517a746)  , 17 },
     { MAD_F(0x05183fba)  , 17 },
     { MAD_F(0x0518d832)  , 17 },
     { MAD_F(0x051970ae)  , 17 },
     { MAD_F(0x051a092f)  , 17 },
     { MAD_F(0x051aa1b5)  , 17 },
     { MAD_F(0x051b3a3f)  , 17 },
     { MAD_F(0x051bd2cd)  , 17 },
     { MAD_F(0x051c6b60)  , 17 },

     { MAD_F(0x051d03f7)  , 17 },
     { MAD_F(0x051d9c92)  , 17 },
     { MAD_F(0x051e3532)  , 17 },
     { MAD_F(0x051ecdd7)  , 17 },
     { MAD_F(0x051f6680)  , 17 },
     { MAD_F(0x051fff2d)  , 17 },
     { MAD_F(0x052097df)  , 17 },
     { MAD_F(0x05213095)  , 17 },
     { MAD_F(0x0521c950)  , 17 },
     { MAD_F(0x0522620f)  , 17 },
     { MAD_F(0x0522fad3)  , 17 },
     { MAD_F(0x0523939b)  , 17 },
     { MAD_F(0x05242c68)  , 17 },
     { MAD_F(0x0524c538)  , 17 },
     { MAD_F(0x05255e0e)  , 17 },
     { MAD_F(0x0525f6e8)  , 17 },

     { MAD_F(0x05268fc6)  , 17 },
     { MAD_F(0x052728a9)  , 17 },
     { MAD_F(0x0527c190)  , 17 },
     { MAD_F(0x05285a7b)  , 17 },
     { MAD_F(0x0528f36b)  , 17 },
     { MAD_F(0x05298c5f)  , 17 },
     { MAD_F(0x052a2558)  , 17 },
     { MAD_F(0x052abe55)  , 17 },
     { MAD_F(0x052b5757)  , 17 },
     { MAD_F(0x052bf05d)  , 17 },
     { MAD_F(0x052c8968)  , 17 },
     { MAD_F(0x052d2277)  , 17 },
     { MAD_F(0x052dbb8a)  , 17 },
     { MAD_F(0x052e54a2)  , 17 },
     { MAD_F(0x052eedbe)  , 17 },
     { MAD_F(0x052f86de)  , 17 },

     { MAD_F(0x05302003)  , 17 },
     { MAD_F(0x0530b92d)  , 17 },
     { MAD_F(0x0531525b)  , 17 },
     { MAD_F(0x0531eb8d)  , 17 },
     { MAD_F(0x053284c4)  , 17 },
     { MAD_F(0x05331dff)  , 17 },
     { MAD_F(0x0533b73e)  , 17 },
     { MAD_F(0x05345082)  , 17 },
     { MAD_F(0x0534e9ca)  , 17 },
     { MAD_F(0x05358317)  , 17 },
     { MAD_F(0x05361c68)  , 17 },
     { MAD_F(0x0536b5be)  , 17 },
     { MAD_F(0x05374f17)  , 17 },
     { MAD_F(0x0537e876)  , 17 },
     { MAD_F(0x053881d9)  , 17 },
     { MAD_F(0x05391b40)  , 17 },

     { MAD_F(0x0539b4ab)  , 17 },
     { MAD_F(0x053a4e1b)  , 17 },
     { MAD_F(0x053ae78f)  , 17 },
     { MAD_F(0x053b8108)  , 17 },
     { MAD_F(0x053c1a85)  , 17 },
     { MAD_F(0x053cb407)  , 17 },
     { MAD_F(0x053d4d8d)  , 17 },
     { MAD_F(0x053de717)  , 17 },
     { MAD_F(0x053e80a6)  , 17 },
     { MAD_F(0x053f1a39)  , 17 },
     { MAD_F(0x053fb3d0)  , 17 },
     { MAD_F(0x05404d6c)  , 17 },
     { MAD_F(0x0540e70c)  , 17 },
     { MAD_F(0x054180b1)  , 17 },
     { MAD_F(0x05421a5a)  , 17 },
     { MAD_F(0x0542b407)  , 17 },

     { MAD_F(0x05434db9)  , 17 },
     { MAD_F(0x0543e76f)  , 17 },
     { MAD_F(0x0544812a)  , 17 },
     { MAD_F(0x05451ae9)  , 17 },
     { MAD_F(0x0545b4ac)  , 17 },
     { MAD_F(0x05464e74)  , 17 },
     { MAD_F(0x0546e840)  , 17 },
     { MAD_F(0x05478211)  , 17 },
     { MAD_F(0x05481be5)  , 17 },
     { MAD_F(0x0548b5bf)  , 17 },
     { MAD_F(0x05494f9c)  , 17 },
     { MAD_F(0x0549e97e)  , 17 },
     { MAD_F(0x054a8364)  , 17 },
     { MAD_F(0x054b1d4f)  , 17 },
     { MAD_F(0x054bb73e)  , 17 },
     { MAD_F(0x054c5132)  , 17 },

     { MAD_F(0x054ceb2a)  , 17 },
     { MAD_F(0x054d8526)  , 17 },
     { MAD_F(0x054e1f26)  , 17 },
     { MAD_F(0x054eb92b)  , 17 },
     { MAD_F(0x054f5334)  , 17 },
     { MAD_F(0x054fed42)  , 17 },
     { MAD_F(0x05508754)  , 17 },
     { MAD_F(0x0551216b)  , 17 },
     { MAD_F(0x0551bb85)  , 17 },
     { MAD_F(0x055255a4)  , 17 },
     { MAD_F(0x0552efc8)  , 17 },
     { MAD_F(0x055389f0)  , 17 },
     { MAD_F(0x0554241c)  , 17 },
     { MAD_F(0x0554be4c)  , 17 },
     { MAD_F(0x05555881)  , 17 },
     { MAD_F(0x0555f2ba)  , 17 },

     { MAD_F(0x05568cf8)  , 17 },
     { MAD_F(0x0557273a)  , 17 },
     { MAD_F(0x0557c180)  , 17 },
     { MAD_F(0x05585bcb)  , 17 },
     { MAD_F(0x0558f61a)  , 17 },
     { MAD_F(0x0559906d)  , 17 },
     { MAD_F(0x055a2ac5)  , 17 },
     { MAD_F(0x055ac521)  , 17 },
     { MAD_F(0x055b5f81)  , 17 },
     { MAD_F(0x055bf9e6)  , 17 },
     { MAD_F(0x055c944f)  , 17 },
     { MAD_F(0x055d2ebd)  , 17 },
     { MAD_F(0x055dc92e)  , 17 },
     { MAD_F(0x055e63a5)  , 17 },
     { MAD_F(0x055efe1f)  , 17 },
     { MAD_F(0x055f989e)  , 17 },

     { MAD_F(0x05603321)  , 17 },
     { MAD_F(0x0560cda8)  , 17 },
     { MAD_F(0x05616834)  , 17 },
     { MAD_F(0x056202c4)  , 17 },
     { MAD_F(0x05629d59)  , 17 },
     { MAD_F(0x056337f2)  , 17 },
     { MAD_F(0x0563d28f)  , 17 },
     { MAD_F(0x05646d30)  , 17 },
     { MAD_F(0x056507d6)  , 17 },
     { MAD_F(0x0565a280)  , 17 },
     { MAD_F(0x05663d2f)  , 17 },
     { MAD_F(0x0566d7e1)  , 17 },
     { MAD_F(0x05677298)  , 17 },
     { MAD_F(0x05680d54)  , 17 },
     { MAD_F(0x0568a814)  , 17 },
     { MAD_F(0x056942d8)  , 17 },


     { MAD_F(0x0569dda0)  , 17 },
     { MAD_F(0x056a786d)  , 17 },
     { MAD_F(0x056b133e)  , 17 },
     { MAD_F(0x056bae13)  , 17 },
     { MAD_F(0x056c48ed)  , 17 },
     { MAD_F(0x056ce3cb)  , 17 },
     { MAD_F(0x056d7ead)  , 17 },
     { MAD_F(0x056e1994)  , 17 },
     { MAD_F(0x056eb47f)  , 17 },
     { MAD_F(0x056f4f6e)  , 17 },
     { MAD_F(0x056fea62)  , 17 },
     { MAD_F(0x0570855a)  , 17 },
     { MAD_F(0x05712056)  , 17 },
     { MAD_F(0x0571bb56)  , 17 },
     { MAD_F(0x0572565b)  , 17 },
     { MAD_F(0x0572f164)  , 17 },

     { MAD_F(0x05738c72)  , 17 },
     { MAD_F(0x05742784)  , 17 },
     { MAD_F(0x0574c29a)  , 17 },
     { MAD_F(0x05755db4)  , 17 },
     { MAD_F(0x0575f8d3)  , 17 },
     { MAD_F(0x057693f6)  , 17 },
     { MAD_F(0x05772f1d)  , 17 },
     { MAD_F(0x0577ca49)  , 17 },
     { MAD_F(0x05786578)  , 17 },
     { MAD_F(0x057900ad)  , 17 },
     { MAD_F(0x05799be5)  , 17 },
     { MAD_F(0x057a3722)  , 17 },
     { MAD_F(0x057ad263)  , 17 },
     { MAD_F(0x057b6da8)  , 17 },
     { MAD_F(0x057c08f2)  , 17 },
     { MAD_F(0x057ca440)  , 17 },

     { MAD_F(0x057d3f92)  , 17 },
     { MAD_F(0x057ddae9)  , 17 },
     { MAD_F(0x057e7644)  , 17 },
     { MAD_F(0x057f11a3)  , 17 },
     { MAD_F(0x057fad06)  , 17 },
     { MAD_F(0x0580486e)  , 17 },
     { MAD_F(0x0580e3da)  , 17 },
     { MAD_F(0x05817f4a)  , 17 },
     { MAD_F(0x05821abf)  , 17 },
     { MAD_F(0x0582b638)  , 17 },
     { MAD_F(0x058351b5)  , 17 },
     { MAD_F(0x0583ed36)  , 17 },
     { MAD_F(0x058488bc)  , 17 },
     { MAD_F(0x05852446)  , 17 },
     { MAD_F(0x0585bfd4)  , 17 },
     { MAD_F(0x05865b67)  , 17 },

     { MAD_F(0x0586f6fd)  , 17 },
     { MAD_F(0x05879298)  , 17 },
     { MAD_F(0x05882e38)  , 17 },
     { MAD_F(0x0588c9dc)  , 17 },
     { MAD_F(0x05896583)  , 17 },
     { MAD_F(0x058a0130)  , 17 },
     { MAD_F(0x058a9ce0)  , 17 },
     { MAD_F(0x058b3895)  , 17 },
     { MAD_F(0x058bd44e)  , 17 },
     { MAD_F(0x058c700b)  , 17 },
     { MAD_F(0x058d0bcd)  , 17 },
     { MAD_F(0x058da793)  , 17 },
     { MAD_F(0x058e435d)  , 17 },
     { MAD_F(0x058edf2b)  , 17 },
     { MAD_F(0x058f7afe)  , 17 },
     { MAD_F(0x059016d5)  , 17 },

     { MAD_F(0x0590b2b0)  , 17 },
     { MAD_F(0x05914e8f)  , 17 },
     { MAD_F(0x0591ea73)  , 17 },
     { MAD_F(0x0592865b)  , 17 },
     { MAD_F(0x05932247)  , 17 },
     { MAD_F(0x0593be37)  , 17 },
     { MAD_F(0x05945a2c)  , 17 },
     { MAD_F(0x0594f625)  , 17 },
     { MAD_F(0x05959222)  , 17 },
     { MAD_F(0x05962e24)  , 17 },
     { MAD_F(0x0596ca2a)  , 17 },
     { MAD_F(0x05976634)  , 17 },
     { MAD_F(0x05980242)  , 17 },
     { MAD_F(0x05989e54)  , 17 },
     { MAD_F(0x05993a6b)  , 17 },
     { MAD_F(0x0599d686)  , 17 },

     { MAD_F(0x059a72a5)  , 17 },
     { MAD_F(0x059b0ec9)  , 17 },
     { MAD_F(0x059baaf1)  , 17 },
     { MAD_F(0x059c471d)  , 17 },
     { MAD_F(0x059ce34d)  , 17 },
     { MAD_F(0x059d7f81)  , 17 },
     { MAD_F(0x059e1bba)  , 17 },
     { MAD_F(0x059eb7f7)  , 17 },
     { MAD_F(0x059f5438)  , 17 },
     { MAD_F(0x059ff07e)  , 17 },
     { MAD_F(0x05a08cc7)  , 17 },
     { MAD_F(0x05a12915)  , 17 },
     { MAD_F(0x05a1c567)  , 17 },
     { MAD_F(0x05a261be)  , 17 },
     { MAD_F(0x05a2fe18)  , 17 },
     { MAD_F(0x05a39a77)  , 17 },

     { MAD_F(0x05a436da)  , 17 },
     { MAD_F(0x05a4d342)  , 17 },
     { MAD_F(0x05a56fad)  , 17 },
     { MAD_F(0x05a60c1d)  , 17 },
     { MAD_F(0x05a6a891)  , 17 },
     { MAD_F(0x05a7450a)  , 17 },
     { MAD_F(0x05a7e186)  , 17 },
     { MAD_F(0x05a87e07)  , 17 },
     { MAD_F(0x05a91a8c)  , 17 },
     { MAD_F(0x05a9b715)  , 17 },
     { MAD_F(0x05aa53a2)  , 17 },
     { MAD_F(0x05aaf034)  , 17 },
     { MAD_F(0x05ab8cca)  , 17 },
     { MAD_F(0x05ac2964)  , 17 },
     { MAD_F(0x05acc602)  , 17 },
     { MAD_F(0x05ad62a5)  , 17 },

     { MAD_F(0x05adff4c)  , 17 },
     { MAD_F(0x05ae9bf7)  , 17 },
     { MAD_F(0x05af38a6)  , 17 },
     { MAD_F(0x05afd559)  , 17 },
     { MAD_F(0x05b07211)  , 17 },
     { MAD_F(0x05b10ecd)  , 17 },
     { MAD_F(0x05b1ab8d)  , 17 },
     { MAD_F(0x05b24851)  , 17 },
     { MAD_F(0x05b2e51a)  , 17 },
     { MAD_F(0x05b381e6)  , 17 },
     { MAD_F(0x05b41eb7)  , 17 },
     { MAD_F(0x05b4bb8c)  , 17 },
     { MAD_F(0x05b55866)  , 17 },
     { MAD_F(0x05b5f543)  , 17 },
     { MAD_F(0x05b69225)  , 17 },
     { MAD_F(0x05b72f0b)  , 17 },

     { MAD_F(0x05b7cbf5)  , 17 },
     { MAD_F(0x05b868e3)  , 17 },
     { MAD_F(0x05b905d6)  , 17 },
     { MAD_F(0x05b9a2cd)  , 17 },
     { MAD_F(0x05ba3fc8)  , 17 },
     { MAD_F(0x05badcc7)  , 17 },
     { MAD_F(0x05bb79ca)  , 17 },
     { MAD_F(0x05bc16d2)  , 17 },
     { MAD_F(0x05bcb3de)  , 17 },
     { MAD_F(0x05bd50ee)  , 17 },
     { MAD_F(0x05bdee02)  , 17 },
     { MAD_F(0x05be8b1a)  , 17 },
     { MAD_F(0x05bf2837)  , 17 },
     { MAD_F(0x05bfc558)  , 17 },
     { MAD_F(0x05c0627d)  , 17 },
     { MAD_F(0x05c0ffa6)  , 17 },

     { MAD_F(0x05c19cd3)  , 17 },
     { MAD_F(0x05c23a05)  , 17 },
     { MAD_F(0x05c2d73a)  , 17 },
     { MAD_F(0x05c37474)  , 17 },
     { MAD_F(0x05c411b2)  , 17 },
     { MAD_F(0x05c4aef5)  , 17 },
     { MAD_F(0x05c54c3b)  , 17 },
     { MAD_F(0x05c5e986)  , 17 },
     { MAD_F(0x05c686d5)  , 17 },
     { MAD_F(0x05c72428)  , 17 },
     { MAD_F(0x05c7c17f)  , 17 },
     { MAD_F(0x05c85eda)  , 17 },
     { MAD_F(0x05c8fc3a)  , 17 },
     { MAD_F(0x05c9999e)  , 17 },
     { MAD_F(0x05ca3706)  , 17 },
     { MAD_F(0x05cad472)  , 17 },

     { MAD_F(0x05cb71e2)  , 17 },
     { MAD_F(0x05cc0f57)  , 17 },
     { MAD_F(0x05ccaccf)  , 17 },
     { MAD_F(0x05cd4a4c)  , 17 },
     { MAD_F(0x05cde7cd)  , 17 },
     { MAD_F(0x05ce8552)  , 17 },
     { MAD_F(0x05cf22dc)  , 17 },
     { MAD_F(0x05cfc069)  , 17 },
     { MAD_F(0x05d05dfb)  , 17 },
     { MAD_F(0x05d0fb91)  , 17 },
     { MAD_F(0x05d1992b)  , 17 },
     { MAD_F(0x05d236c9)  , 17 },
     { MAD_F(0x05d2d46c)  , 17 },
     { MAD_F(0x05d37212)  , 17 },
     { MAD_F(0x05d40fbd)  , 17 },
     { MAD_F(0x05d4ad6c)  , 17 },

     { MAD_F(0x05d54b1f)  , 17 },
     { MAD_F(0x05d5e8d6)  , 17 },
     { MAD_F(0x05d68691)  , 17 },
     { MAD_F(0x05d72451)  , 17 },
     { MAD_F(0x05d7c215)  , 17 },
     { MAD_F(0x05d85fdc)  , 17 },
     { MAD_F(0x05d8fda8)  , 17 },
     { MAD_F(0x05d99b79)  , 17 },
     { MAD_F(0x05da394d)  , 17 },
     { MAD_F(0x05dad726)  , 17 },
     { MAD_F(0x05db7502)  , 17 },
     { MAD_F(0x05dc12e3)  , 17 },
     { MAD_F(0x05dcb0c8)  , 17 },
     { MAD_F(0x05dd4eb1)  , 17 },
     { MAD_F(0x05ddec9e)  , 17 },
     { MAD_F(0x05de8a90)  , 17 },

     { MAD_F(0x05df2885)  , 17 },
     { MAD_F(0x05dfc67f)  , 17 },
     { MAD_F(0x05e0647d)  , 17 },
     { MAD_F(0x05e1027f)  , 17 },
     { MAD_F(0x05e1a085)  , 17 },
     { MAD_F(0x05e23e8f)  , 17 },
     { MAD_F(0x05e2dc9e)  , 17 },
     { MAD_F(0x05e37ab0)  , 17 },
     { MAD_F(0x05e418c7)  , 17 },
     { MAD_F(0x05e4b6e2)  , 17 },
     { MAD_F(0x05e55501)  , 17 },
     { MAD_F(0x05e5f324)  , 17 },
     { MAD_F(0x05e6914c)  , 17 },
     { MAD_F(0x05e72f77)  , 17 },
     { MAD_F(0x05e7cda7)  , 17 },
     { MAD_F(0x05e86bda)  , 17 },

     { MAD_F(0x05e90a12)  , 17 },
     { MAD_F(0x05e9a84e)  , 17 },
     { MAD_F(0x05ea468e)  , 17 },
     { MAD_F(0x05eae4d3)  , 17 },
     { MAD_F(0x05eb831b)  , 17 },
     { MAD_F(0x05ec2168)  , 17 },
     { MAD_F(0x05ecbfb8)  , 17 },
     { MAD_F(0x05ed5e0d)  , 17 },
     { MAD_F(0x05edfc66)  , 17 },
     { MAD_F(0x05ee9ac3)  , 17 },
     { MAD_F(0x05ef3924)  , 17 },
     { MAD_F(0x05efd78a)  , 17 },
     { MAD_F(0x05f075f3)  , 17 },
     { MAD_F(0x05f11461)  , 17 },
     { MAD_F(0x05f1b2d3)  , 17 },
     { MAD_F(0x05f25148)  , 17 },

     { MAD_F(0x05f2efc2)  , 17 },
     { MAD_F(0x05f38e40)  , 17 },
     { MAD_F(0x05f42cc3)  , 17 },
     { MAD_F(0x05f4cb49)  , 17 },
     { MAD_F(0x05f569d3)  , 17 },
     { MAD_F(0x05f60862)  , 17 },
     { MAD_F(0x05f6a6f5)  , 17 },
     { MAD_F(0x05f7458b)  , 17 },
     { MAD_F(0x05f7e426)  , 17 },
     { MAD_F(0x05f882c5)  , 17 },
     { MAD_F(0x05f92169)  , 17 },
     { MAD_F(0x05f9c010)  , 17 },
     { MAD_F(0x05fa5ebb)  , 17 },
     { MAD_F(0x05fafd6b)  , 17 },
     { MAD_F(0x05fb9c1e)  , 17 },
     { MAD_F(0x05fc3ad6)  , 17 },

     { MAD_F(0x05fcd992)  , 17 },
     { MAD_F(0x05fd7852)  , 17 },
     { MAD_F(0x05fe1716)  , 17 },
     { MAD_F(0x05feb5de)  , 17 },
     { MAD_F(0x05ff54aa)  , 17 },
     { MAD_F(0x05fff37b)  , 17 },
     { MAD_F(0x0600924f)  , 17 },
     { MAD_F(0x06013128)  , 17 },
     { MAD_F(0x0601d004)  , 17 },
     { MAD_F(0x06026ee5)  , 17 },
     { MAD_F(0x06030dca)  , 17 },
     { MAD_F(0x0603acb3)  , 17 },
     { MAD_F(0x06044ba0)  , 17 },
     { MAD_F(0x0604ea91)  , 17 },
     { MAD_F(0x06058987)  , 17 },
     { MAD_F(0x06062880)  , 17 },

     { MAD_F(0x0606c77d)  , 17 },
     { MAD_F(0x0607667f)  , 17 },
     { MAD_F(0x06080585)  , 17 },
     { MAD_F(0x0608a48f)  , 17 },
     { MAD_F(0x0609439c)  , 17 },
     { MAD_F(0x0609e2ae)  , 17 },
     { MAD_F(0x060a81c4)  , 17 },
     { MAD_F(0x060b20df)  , 17 },
     { MAD_F(0x060bbffd)  , 17 },
     { MAD_F(0x060c5f1f)  , 17 },
     { MAD_F(0x060cfe46)  , 17 },
     { MAD_F(0x060d9d70)  , 17 },
     { MAD_F(0x060e3c9f)  , 17 },
     { MAD_F(0x060edbd1)  , 17 },
     { MAD_F(0x060f7b08)  , 17 },
     { MAD_F(0x06101a43)  , 17 },

     { MAD_F(0x0610b982)  , 17 },
     { MAD_F(0x061158c5)  , 17 },
     { MAD_F(0x0611f80c)  , 17 },
     { MAD_F(0x06129757)  , 17 },
     { MAD_F(0x061336a6)  , 17 },
     { MAD_F(0x0613d5fa)  , 17 },
     { MAD_F(0x06147551)  , 17 },
     { MAD_F(0x061514ad)  , 17 },
     { MAD_F(0x0615b40c)  , 17 },
     { MAD_F(0x06165370)  , 17 },
     { MAD_F(0x0616f2d8)  , 17 },
     { MAD_F(0x06179243)  , 17 },
     { MAD_F(0x061831b3)  , 17 },
     { MAD_F(0x0618d127)  , 17 },
     { MAD_F(0x0619709f)  , 17 },
     { MAD_F(0x061a101b)  , 17 },

     { MAD_F(0x061aaf9c)  , 17 },
     { MAD_F(0x061b4f20)  , 17 },
     { MAD_F(0x061beea8)  , 17 },
     { MAD_F(0x061c8e34)  , 17 },
     { MAD_F(0x061d2dc5)  , 17 },
     { MAD_F(0x061dcd59)  , 17 },
     { MAD_F(0x061e6cf2)  , 17 },
     { MAD_F(0x061f0c8f)  , 17 },
     { MAD_F(0x061fac2f)  , 17 },
     { MAD_F(0x06204bd4)  , 17 },
     { MAD_F(0x0620eb7d)  , 17 },
     { MAD_F(0x06218b2a)  , 17 },
     { MAD_F(0x06222adb)  , 17 },
     { MAD_F(0x0622ca90)  , 17 },
     { MAD_F(0x06236a49)  , 17 },
     { MAD_F(0x06240a06)  , 17 },

     { MAD_F(0x0624a9c7)  , 17 },
     { MAD_F(0x0625498d)  , 17 },
     { MAD_F(0x0625e956)  , 17 },
     { MAD_F(0x06268923)  , 17 },
     { MAD_F(0x062728f5)  , 17 },
     { MAD_F(0x0627c8ca)  , 17 },
     { MAD_F(0x062868a4)  , 17 },
     { MAD_F(0x06290881)  , 17 },
     { MAD_F(0x0629a863)  , 17 },
     { MAD_F(0x062a4849)  , 17 },
     { MAD_F(0x062ae832)  , 17 },
     { MAD_F(0x062b8820)  , 17 },
     { MAD_F(0x062c2812)  , 17 },
     { MAD_F(0x062cc808)  , 17 },
     { MAD_F(0x062d6802)  , 17 },
     { MAD_F(0x062e0800)  , 17 },

     { MAD_F(0x062ea802)  , 17 },
     { MAD_F(0x062f4808)  , 17 },
     { MAD_F(0x062fe812)  , 17 },
     { MAD_F(0x06308820)  , 17 },
     { MAD_F(0x06312832)  , 17 },
     { MAD_F(0x0631c849)  , 17 },
     { MAD_F(0x06326863)  , 17 },
     { MAD_F(0x06330881)  , 17 },
     { MAD_F(0x0633a8a3)  , 17 },
     { MAD_F(0x063448ca)  , 17 },
     { MAD_F(0x0634e8f4)  , 17 },
     { MAD_F(0x06358923)  , 17 },
     { MAD_F(0x06362955)  , 17 },
     { MAD_F(0x0636c98c)  , 17 },
     { MAD_F(0x063769c6)  , 17 },
     { MAD_F(0x06380a05)  , 17 },

     { MAD_F(0x0638aa48)  , 17 },
     { MAD_F(0x06394a8e)  , 17 },
     { MAD_F(0x0639ead9)  , 17 },
     { MAD_F(0x063a8b28)  , 17 },
     { MAD_F(0x063b2b7b)  , 17 },
     { MAD_F(0x063bcbd1)  , 17 },
     { MAD_F(0x063c6c2c)  , 17 },
     { MAD_F(0x063d0c8b)  , 17 },
     { MAD_F(0x063dacee)  , 17 },
     { MAD_F(0x063e4d55)  , 17 },
     { MAD_F(0x063eedc0)  , 17 },
     { MAD_F(0x063f8e2f)  , 17 },
     { MAD_F(0x06402ea2)  , 17 },
     { MAD_F(0x0640cf19)  , 17 },
     { MAD_F(0x06416f94)  , 17 },
     { MAD_F(0x06421013)  , 17 },

     { MAD_F(0x0642b096)  , 17 },
     { MAD_F(0x0643511d)  , 17 },
     { MAD_F(0x0643f1a8)  , 17 },
     { MAD_F(0x06449237)  , 17 },
     { MAD_F(0x064532ca)  , 17 },
     { MAD_F(0x0645d361)  , 17 },
     { MAD_F(0x064673fc)  , 17 },
     { MAD_F(0x0647149c)  , 17 },
     { MAD_F(0x0647b53f)  , 17 },
     { MAD_F(0x064855e6)  , 17 },
     { MAD_F(0x0648f691)  , 17 },
     { MAD_F(0x06499740)  , 17 },
     { MAD_F(0x064a37f4)  , 17 },
     { MAD_F(0x064ad8ab)  , 17 },
     { MAD_F(0x064b7966)  , 17 },
     { MAD_F(0x064c1a25)  , 17 },

     { MAD_F(0x064cbae9)  , 17 },
     { MAD_F(0x064d5bb0)  , 17 },
     { MAD_F(0x064dfc7b)  , 17 },
     { MAD_F(0x064e9d4b)  , 17 },
     { MAD_F(0x064f3e1e)  , 17 },
     { MAD_F(0x064fdef5)  , 17 },
     { MAD_F(0x06507fd0)  , 17 },
     { MAD_F(0x065120b0)  , 17 },
     { MAD_F(0x0651c193)  , 17 },
     { MAD_F(0x0652627a)  , 17 },
     { MAD_F(0x06530366)  , 17 },
     { MAD_F(0x0653a455)  , 17 },
     { MAD_F(0x06544548)  , 17 },
     { MAD_F(0x0654e640)  , 17 },
     { MAD_F(0x0655873b)  , 17 },
     { MAD_F(0x0656283a)  , 17 },

     { MAD_F(0x0656c93d)  , 17 },
     { MAD_F(0x06576a45)  , 17 },
     { MAD_F(0x06580b50)  , 17 },
     { MAD_F(0x0658ac5f)  , 17 },
     { MAD_F(0x06594d73)  , 17 },
     { MAD_F(0x0659ee8a)  , 17 },
     { MAD_F(0x065a8fa5)  , 17 },
     { MAD_F(0x065b30c4)  , 17 },
     { MAD_F(0x065bd1e7)  , 17 },
     { MAD_F(0x065c730f)  , 17 },
     { MAD_F(0x065d143a)  , 17 },
     { MAD_F(0x065db569)  , 17 },
     { MAD_F(0x065e569c)  , 17 },
     { MAD_F(0x065ef7d3)  , 17 },
     { MAD_F(0x065f990e)  , 17 },
     { MAD_F(0x06603a4e)  , 17 },

     { MAD_F(0x0660db91)  , 17 },
     { MAD_F(0x06617cd8)  , 17 },
     { MAD_F(0x06621e23)  , 17 },
     { MAD_F(0x0662bf72)  , 17 },
     { MAD_F(0x066360c5)  , 17 },
     { MAD_F(0x0664021c)  , 17 },
     { MAD_F(0x0664a377)  , 17 },
     { MAD_F(0x066544d6)  , 17 },
     { MAD_F(0x0665e639)  , 17 },
     { MAD_F(0x066687a0)  , 17 },
     { MAD_F(0x0667290b)  , 17 },
     { MAD_F(0x0667ca79)  , 17 },
     { MAD_F(0x06686bec)  , 17 },
     { MAD_F(0x06690d63)  , 17 },
     { MAD_F(0x0669aede)  , 17 },
     { MAD_F(0x066a505d)  , 17 },

     { MAD_F(0x066af1df)  , 17 },
     { MAD_F(0x066b9366)  , 17 },
     { MAD_F(0x066c34f1)  , 17 },
     { MAD_F(0x066cd67f)  , 17 },
     { MAD_F(0x066d7812)  , 17 },
     { MAD_F(0x066e19a9)  , 17 },
     { MAD_F(0x066ebb43)  , 17 },
     { MAD_F(0x066f5ce2)  , 17 },
     { MAD_F(0x066ffe84)  , 17 },
     { MAD_F(0x0670a02a)  , 17 },
     { MAD_F(0x067141d5)  , 17 },
     { MAD_F(0x0671e383)  , 17 },
     { MAD_F(0x06728535)  , 17 },
     { MAD_F(0x067326ec)  , 17 },
     { MAD_F(0x0673c8a6)  , 17 },
     { MAD_F(0x06746a64)  , 17 },

     { MAD_F(0x06750c26)  , 17 },
     { MAD_F(0x0675adec)  , 17 },
     { MAD_F(0x06764fb6)  , 17 },
     { MAD_F(0x0676f184)  , 17 },
     { MAD_F(0x06779356)  , 17 },
     { MAD_F(0x0678352c)  , 17 },
     { MAD_F(0x0678d706)  , 17 },
     { MAD_F(0x067978e4)  , 17 },
     { MAD_F(0x067a1ac6)  , 17 },
     { MAD_F(0x067abcac)  , 17 },
     { MAD_F(0x067b5e95)  , 17 },
     { MAD_F(0x067c0083)  , 17 },
     { MAD_F(0x067ca275)  , 17 },
     { MAD_F(0x067d446a)  , 17 },
     { MAD_F(0x067de664)  , 17 },
     { MAD_F(0x067e8861)  , 17 },

     { MAD_F(0x067f2a62)  , 17 },
     { MAD_F(0x067fcc68)  , 17 },
     { MAD_F(0x06806e71)  , 17 },
     { MAD_F(0x0681107e)  , 17 },
     { MAD_F(0x0681b28f)  , 17 },
     { MAD_F(0x068254a4)  , 17 },
     { MAD_F(0x0682f6bd)  , 17 },
     { MAD_F(0x068398da)  , 17 },
     { MAD_F(0x06843afb)  , 17 },
     { MAD_F(0x0684dd20)  , 17 },
     { MAD_F(0x06857f49)  , 17 },
     { MAD_F(0x06862176)  , 17 },
     { MAD_F(0x0686c3a6)  , 17 },
     { MAD_F(0x068765db)  , 17 },
     { MAD_F(0x06880814)  , 17 },
     { MAD_F(0x0688aa50)  , 17 },

     { MAD_F(0x06894c90)  , 17 },
     { MAD_F(0x0689eed5)  , 17 },
     { MAD_F(0x068a911d)  , 17 },
     { MAD_F(0x068b3369)  , 17 },
     { MAD_F(0x068bd5b9)  , 17 },
     { MAD_F(0x068c780e)  , 17 },
     { MAD_F(0x068d1a66)  , 17 },
     { MAD_F(0x068dbcc1)  , 17 },
     { MAD_F(0x068e5f21)  , 17 },
     { MAD_F(0x068f0185)  , 17 },
     { MAD_F(0x068fa3ed)  , 17 },
     { MAD_F(0x06904658)  , 17 },
     { MAD_F(0x0690e8c8)  , 17 },
     { MAD_F(0x06918b3c)  , 17 },
     { MAD_F(0x06922db3)  , 17 },
     { MAD_F(0x0692d02e)  , 17 },

     { MAD_F(0x069372ae)  , 17 },
     { MAD_F(0x06941531)  , 17 },
     { MAD_F(0x0694b7b8)  , 17 },
     { MAD_F(0x06955a43)  , 17 },
     { MAD_F(0x0695fcd2)  , 17 },
     { MAD_F(0x06969f65)  , 17 },
     { MAD_F(0x069741fb)  , 17 },
     { MAD_F(0x0697e496)  , 17 },
     { MAD_F(0x06988735)  , 17 },
     { MAD_F(0x069929d7)  , 17 },
     { MAD_F(0x0699cc7e)  , 17 },
     { MAD_F(0x069a6f28)  , 17 },
     { MAD_F(0x069b11d6)  , 17 },
     { MAD_F(0x069bb489)  , 17 },
     { MAD_F(0x069c573f)  , 17 },
     { MAD_F(0x069cf9f9)  , 17 },

     { MAD_F(0x069d9cb7)  , 17 },
     { MAD_F(0x069e3f78)  , 17 },
     { MAD_F(0x069ee23e)  , 17 },
     { MAD_F(0x069f8508)  , 17 },
     { MAD_F(0x06a027d5)  , 17 },
     { MAD_F(0x06a0caa7)  , 17 },
     { MAD_F(0x06a16d7c)  , 17 },
     { MAD_F(0x06a21055)  , 17 },
     { MAD_F(0x06a2b333)  , 17 },
     { MAD_F(0x06a35614)  , 17 },
     { MAD_F(0x06a3f8f9)  , 17 },
     { MAD_F(0x06a49be2)  , 17 },
     { MAD_F(0x06a53ece)  , 17 },
     { MAD_F(0x06a5e1bf)  , 17 },
     { MAD_F(0x06a684b4)  , 17 },
     { MAD_F(0x06a727ac)  , 17 },

     { MAD_F(0x06a7caa9)  , 17 },
     { MAD_F(0x06a86da9)  , 17 },
     { MAD_F(0x06a910ad)  , 17 },
     { MAD_F(0x06a9b3b5)  , 17 },
     { MAD_F(0x06aa56c1)  , 17 },
     { MAD_F(0x06aaf9d1)  , 17 },
     { MAD_F(0x06ab9ce5)  , 17 },
     { MAD_F(0x06ac3ffc)  , 17 },
     { MAD_F(0x06ace318)  , 17 },
     { MAD_F(0x06ad8637)  , 17 },
     { MAD_F(0x06ae295b)  , 17 },
     { MAD_F(0x06aecc82)  , 17 },
     { MAD_F(0x06af6fad)  , 17 },
     { MAD_F(0x06b012dc)  , 17 },
     { MAD_F(0x06b0b60f)  , 17 },
     { MAD_F(0x06b15946)  , 17 },

     { MAD_F(0x06b1fc81)  , 17 },
     { MAD_F(0x06b29fbf)  , 17 },
     { MAD_F(0x06b34302)  , 17 },
     { MAD_F(0x06b3e648)  , 17 },
     { MAD_F(0x06b48992)  , 17 },
     { MAD_F(0x06b52ce0)  , 17 },
     { MAD_F(0x06b5d032)  , 17 },
     { MAD_F(0x06b67388)  , 17 },
     { MAD_F(0x06b716e2)  , 17 },
     { MAD_F(0x06b7ba3f)  , 17 },
     { MAD_F(0x06b85da1)  , 17 },
     { MAD_F(0x06b90106)  , 17 },
     { MAD_F(0x06b9a470)  , 17 },
     { MAD_F(0x06ba47dd)  , 17 },
     { MAD_F(0x06baeb4e)  , 17 },
     { MAD_F(0x06bb8ec3)  , 17 },

     { MAD_F(0x06bc323b)  , 17 },
     { MAD_F(0x06bcd5b8)  , 17 },
     { MAD_F(0x06bd7939)  , 17 },
     { MAD_F(0x06be1cbd)  , 17 },
     { MAD_F(0x06bec045)  , 17 },
     { MAD_F(0x06bf63d1)  , 17 },
     { MAD_F(0x06c00761)  , 17 },
     { MAD_F(0x06c0aaf5)  , 17 },
     { MAD_F(0x06c14e8d)  , 17 },
     { MAD_F(0x06c1f229)  , 17 },
     { MAD_F(0x06c295c8)  , 17 },
     { MAD_F(0x06c3396c)  , 17 },
     { MAD_F(0x06c3dd13)  , 17 },
     { MAD_F(0x06c480be)  , 17 },
     { MAD_F(0x06c5246d)  , 17 },
     { MAD_F(0x06c5c820)  , 17 },

     { MAD_F(0x06c66bd6)  , 17 },
     { MAD_F(0x06c70f91)  , 17 },
     { MAD_F(0x06c7b34f)  , 17 },
     { MAD_F(0x06c85712)  , 17 },
     { MAD_F(0x06c8fad8)  , 17 },
     { MAD_F(0x06c99ea2)  , 17 },
     { MAD_F(0x06ca4270)  , 17 },
     { MAD_F(0x06cae641)  , 17 },
     { MAD_F(0x06cb8a17)  , 17 },
     { MAD_F(0x06cc2df0)  , 17 },
     { MAD_F(0x06ccd1ce)  , 17 },
     { MAD_F(0x06cd75af)  , 17 },
     { MAD_F(0x06ce1994)  , 17 },
     { MAD_F(0x06cebd7d)  , 17 },
     { MAD_F(0x06cf6169)  , 17 },
     { MAD_F(0x06d0055a)  , 17 },

     { MAD_F(0x06d0a94e)  , 17 },
     { MAD_F(0x06d14d47)  , 17 },
     { MAD_F(0x06d1f143)  , 17 },
     { MAD_F(0x06d29543)  , 17 },
     { MAD_F(0x06d33947)  , 17 },
     { MAD_F(0x06d3dd4e)  , 17 },
     { MAD_F(0x06d4815a)  , 17 },
     { MAD_F(0x06d52569)  , 17 },
     { MAD_F(0x06d5c97c)  , 17 },
     { MAD_F(0x06d66d93)  , 17 },
     { MAD_F(0x06d711ae)  , 17 },
     { MAD_F(0x06d7b5cd)  , 17 },
     { MAD_F(0x06d859f0)  , 17 },
     { MAD_F(0x06d8fe16)  , 17 },
     { MAD_F(0x06d9a240)  , 17 },
     { MAD_F(0x06da466f)  , 17 },

     { MAD_F(0x06daeaa1)  , 17 },
     { MAD_F(0x06db8ed6)  , 17 },
     { MAD_F(0x06dc3310)  , 17 },
     { MAD_F(0x06dcd74d)  , 17 },
     { MAD_F(0x06dd7b8f)  , 17 },
     { MAD_F(0x06de1fd4)  , 17 },
     { MAD_F(0x06dec41d)  , 17 },
     { MAD_F(0x06df686a)  , 17 },
     { MAD_F(0x06e00cbb)  , 17 },
     { MAD_F(0x06e0b10f)  , 17 },
     { MAD_F(0x06e15567)  , 17 },
     { MAD_F(0x06e1f9c4)  , 17 },
     { MAD_F(0x06e29e24)  , 17 },
     { MAD_F(0x06e34287)  , 17 },
     { MAD_F(0x06e3e6ef)  , 17 },
     { MAD_F(0x06e48b5b)  , 17 },

     { MAD_F(0x06e52fca)  , 17 },
     { MAD_F(0x06e5d43d)  , 17 },
     { MAD_F(0x06e678b4)  , 17 },
     { MAD_F(0x06e71d2f)  , 17 },
     { MAD_F(0x06e7c1ae)  , 17 },
     { MAD_F(0x06e86630)  , 17 },
     { MAD_F(0x06e90ab7)  , 17 },
     { MAD_F(0x06e9af41)  , 17 },
     { MAD_F(0x06ea53cf)  , 17 },
     { MAD_F(0x06eaf860)  , 17 },
     { MAD_F(0x06eb9cf6)  , 17 },
     { MAD_F(0x06ec418f)  , 17 },
     { MAD_F(0x06ece62d)  , 17 },
     { MAD_F(0x06ed8ace)  , 17 },
     { MAD_F(0x06ee2f73)  , 17 },
     { MAD_F(0x06eed41b)  , 17 },

     { MAD_F(0x06ef78c8)  , 17 },
     { MAD_F(0x06f01d78)  , 17 },
     { MAD_F(0x06f0c22c)  , 17 },
     { MAD_F(0x06f166e4)  , 17 },
     { MAD_F(0x06f20ba0)  , 17 },
     { MAD_F(0x06f2b060)  , 17 },
     { MAD_F(0x06f35523)  , 17 },
     { MAD_F(0x06f3f9eb)  , 17 },
     { MAD_F(0x06f49eb6)  , 17 },
     { MAD_F(0x06f54385)  , 17 },
     { MAD_F(0x06f5e857)  , 17 },
     { MAD_F(0x06f68d2e)  , 17 },
     { MAD_F(0x06f73208)  , 17 },
     { MAD_F(0x06f7d6e6)  , 17 },
     { MAD_F(0x06f87bc8)  , 17 },
     { MAD_F(0x06f920ae)  , 17 },

     { MAD_F(0x06f9c597)  , 17 },
     { MAD_F(0x06fa6a85)  , 17 },
     { MAD_F(0x06fb0f76)  , 17 },
     { MAD_F(0x06fbb46b)  , 17 },
     { MAD_F(0x06fc5964)  , 17 },
     { MAD_F(0x06fcfe60)  , 17 },
     { MAD_F(0x06fda361)  , 17 },
     { MAD_F(0x06fe4865)  , 17 },
     { MAD_F(0x06feed6d)  , 17 },
     { MAD_F(0x06ff9279)  , 17 },
     { MAD_F(0x07003788)  , 17 },
     { MAD_F(0x0700dc9c)  , 17 },
     { MAD_F(0x070181b3)  , 17 },
     { MAD_F(0x070226ce)  , 17 },
     { MAD_F(0x0702cbed)  , 17 },
     { MAD_F(0x0703710f)  , 17 },

     { MAD_F(0x07041636)  , 17 },
     { MAD_F(0x0704bb60)  , 17 },
     { MAD_F(0x0705608e)  , 17 },
     { MAD_F(0x070605c0)  , 17 },
     { MAD_F(0x0706aaf5)  , 17 },
     { MAD_F(0x0707502f)  , 17 },
     { MAD_F(0x0707f56c)  , 17 },
     { MAD_F(0x07089aad)  , 17 },
     { MAD_F(0x07093ff2)  , 17 },
     { MAD_F(0x0709e53a)  , 17 },
     { MAD_F(0x070a8a86)  , 17 },
     { MAD_F(0x070b2fd7)  , 17 },
     { MAD_F(0x070bd52a)  , 17 },
     { MAD_F(0x070c7a82)  , 17 },
     { MAD_F(0x070d1fde)  , 17 },
     { MAD_F(0x070dc53d)  , 17 },

     { MAD_F(0x070e6aa0)  , 17 },
     { MAD_F(0x070f1007)  , 17 },
     { MAD_F(0x070fb571)  , 17 },
     { MAD_F(0x07105ae0)  , 17 },
     { MAD_F(0x07110052)  , 17 },
     { MAD_F(0x0711a5c8)  , 17 },
     { MAD_F(0x07124b42)  , 17 },
     { MAD_F(0x0712f0bf)  , 17 },
     { MAD_F(0x07139641)  , 17 },
     { MAD_F(0x07143bc6)  , 17 },
     { MAD_F(0x0714e14f)  , 17 },
     { MAD_F(0x071586db)  , 17 },
     { MAD_F(0x07162c6c)  , 17 },
     { MAD_F(0x0716d200)  , 17 },
     { MAD_F(0x07177798)  , 17 },
     { MAD_F(0x07181d34)  , 17 },

     { MAD_F(0x0718c2d3)  , 17 },
     { MAD_F(0x07196877)  , 17 },
     { MAD_F(0x071a0e1e)  , 17 },
     { MAD_F(0x071ab3c9)  , 17 },
     { MAD_F(0x071b5977)  , 17 },
     { MAD_F(0x071bff2a)  , 17 },
     { MAD_F(0x071ca4e0)  , 17 },
     { MAD_F(0x071d4a9a)  , 17 },
     { MAD_F(0x071df058)  , 17 },
     { MAD_F(0x071e9619)  , 17 },
     { MAD_F(0x071f3bde)  , 17 },
     { MAD_F(0x071fe1a8)  , 17 },
     { MAD_F(0x07208774)  , 17 },
     { MAD_F(0x07212d45)  , 17 },
     { MAD_F(0x0721d319)  , 17 },
     { MAD_F(0x072278f1)  , 17 },

     { MAD_F(0x07231ecd)  , 17 },
     { MAD_F(0x0723c4ad)  , 17 },
     { MAD_F(0x07246a90)  , 17 },
     { MAD_F(0x07251077)  , 17 },
     { MAD_F(0x0725b662)  , 17 },
     { MAD_F(0x07265c51)  , 17 },
     { MAD_F(0x07270244)  , 17 },
     { MAD_F(0x0727a83a)  , 17 },
     { MAD_F(0x07284e34)  , 17 },
     { MAD_F(0x0728f431)  , 17 },
     { MAD_F(0x07299a33)  , 17 },
     { MAD_F(0x072a4038)  , 17 },
     { MAD_F(0x072ae641)  , 17 },
     { MAD_F(0x072b8c4e)  , 17 },
     { MAD_F(0x072c325e)  , 17 },
     { MAD_F(0x072cd873)  , 17 },

     { MAD_F(0x072d7e8b)  , 17 },
     { MAD_F(0x072e24a7)  , 17 },
     { MAD_F(0x072ecac6)  , 17 },
     { MAD_F(0x072f70e9)  , 17 },
     { MAD_F(0x07301710)  , 17 },
     { MAD_F(0x0730bd3b)  , 17 },
     { MAD_F(0x0731636a)  , 17 },
     { MAD_F(0x0732099c)  , 17 },
     { MAD_F(0x0732afd2)  , 17 },
     { MAD_F(0x0733560c)  , 17 },
     { MAD_F(0x0733fc49)  , 17 },
     { MAD_F(0x0734a28b)  , 17 },
     { MAD_F(0x073548d0)  , 17 },
     { MAD_F(0x0735ef18)  , 17 },
     { MAD_F(0x07369565)  , 17 },
     { MAD_F(0x07373bb5)  , 17 },

     { MAD_F(0x0737e209)  , 17 },
     { MAD_F(0x07388861)  , 17 },
     { MAD_F(0x07392ebc)  , 17 },
     { MAD_F(0x0739d51c)  , 17 },
     { MAD_F(0x073a7b7f)  , 17 },
     { MAD_F(0x073b21e5)  , 17 },
     { MAD_F(0x073bc850)  , 17 },
     { MAD_F(0x073c6ebe)  , 17 },
     { MAD_F(0x073d1530)  , 17 },
     { MAD_F(0x073dbba6)  , 17 },
     { MAD_F(0x073e621f)  , 17 },
     { MAD_F(0x073f089c)  , 17 },
     { MAD_F(0x073faf1d)  , 17 },
     { MAD_F(0x074055a2)  , 17 },
     { MAD_F(0x0740fc2a)  , 17 },
     { MAD_F(0x0741a2b6)  , 17 },

     { MAD_F(0x07424946)  , 17 },
     { MAD_F(0x0742efd9)  , 17 },
     { MAD_F(0x07439671)  , 17 },
     { MAD_F(0x07443d0c)  , 17 },
     { MAD_F(0x0744e3aa)  , 17 },
     { MAD_F(0x07458a4d)  , 17 },
     { MAD_F(0x074630f3)  , 17 },
     { MAD_F(0x0746d79d)  , 17 },
     { MAD_F(0x07477e4b)  , 17 },
     { MAD_F(0x074824fc)  , 17 },
     { MAD_F(0x0748cbb1)  , 17 },
     { MAD_F(0x0749726a)  , 17 },
     { MAD_F(0x074a1927)  , 17 },
     { MAD_F(0x074abfe7)  , 17 },
     { MAD_F(0x074b66ab)  , 17 },
     { MAD_F(0x074c0d73)  , 17 },

     { MAD_F(0x074cb43e)  , 17 },
     { MAD_F(0x074d5b0d)  , 17 },
     { MAD_F(0x074e01e0)  , 17 },
     { MAD_F(0x074ea8b7)  , 17 },
     { MAD_F(0x074f4f91)  , 17 },
     { MAD_F(0x074ff66f)  , 17 },
     { MAD_F(0x07509d51)  , 17 },
     { MAD_F(0x07514437)  , 17 },
     { MAD_F(0x0751eb20)  , 17 },
     { MAD_F(0x0752920d)  , 17 },
     { MAD_F(0x075338fd)  , 17 },
     { MAD_F(0x0753dff2)  , 17 },
     { MAD_F(0x075486ea)  , 17 },
     { MAD_F(0x07552de6)  , 17 },
     { MAD_F(0x0755d4e5)  , 17 },
     { MAD_F(0x07567be8)  , 17 },

     { MAD_F(0x075722ef)  , 17 },
     { MAD_F(0x0757c9fa)  , 17 },
     { MAD_F(0x07587108)  , 17 },
     { MAD_F(0x0759181a)  , 17 },
     { MAD_F(0x0759bf30)  , 17 },
     { MAD_F(0x075a664a)  , 17 },
     { MAD_F(0x075b0d67)  , 17 },
     { MAD_F(0x075bb488)  , 17 },
     { MAD_F(0x075c5bac)  , 17 },
     { MAD_F(0x075d02d5)  , 17 },
     { MAD_F(0x075daa01)  , 17 },
     { MAD_F(0x075e5130)  , 17 },
     { MAD_F(0x075ef864)  , 17 },
     { MAD_F(0x075f9f9b)  , 17 },
     { MAD_F(0x076046d6)  , 17 },
     { MAD_F(0x0760ee14)  , 17 },

     { MAD_F(0x07619557)  , 17 },
     { MAD_F(0x07623c9d)  , 17 },
     { MAD_F(0x0762e3e6)  , 17 },
     { MAD_F(0x07638b34)  , 17 },
     { MAD_F(0x07643285)  , 17 },
     { MAD_F(0x0764d9d9)  , 17 },
     { MAD_F(0x07658132)  , 17 },
     { MAD_F(0x0766288e)  , 17 },
     { MAD_F(0x0766cfee)  , 17 },
     { MAD_F(0x07677751)  , 17 },
     { MAD_F(0x07681eb9)  , 17 },
     { MAD_F(0x0768c624)  , 17 },
     { MAD_F(0x07696d92)  , 17 },
     { MAD_F(0x076a1505)  , 17 },
     { MAD_F(0x076abc7b)  , 17 },
     { MAD_F(0x076b63f4)  , 17 },

     { MAD_F(0x076c0b72)  , 17 },
     { MAD_F(0x076cb2f3)  , 17 },
     { MAD_F(0x076d5a78)  , 17 },
     { MAD_F(0x076e0200)  , 17 },
     { MAD_F(0x076ea98c)  , 17 },
     { MAD_F(0x076f511c)  , 17 },
     { MAD_F(0x076ff8b0)  , 17 },
     { MAD_F(0x0770a047)  , 17 },
     { MAD_F(0x077147e2)  , 17 },
     { MAD_F(0x0771ef80)  , 17 },
     { MAD_F(0x07729723)  , 17 },
     { MAD_F(0x07733ec9)  , 17 },
     { MAD_F(0x0773e672)  , 17 },
     { MAD_F(0x07748e20)  , 17 },
     { MAD_F(0x077535d1)  , 17 },
     { MAD_F(0x0775dd85)  , 17 },

     { MAD_F(0x0776853e)  , 17 },
     { MAD_F(0x07772cfa)  , 17 },
     { MAD_F(0x0777d4ba)  , 17 },
     { MAD_F(0x07787c7d)  , 17 },
     { MAD_F(0x07792444)  , 17 },
     { MAD_F(0x0779cc0f)  , 17 },
     { MAD_F(0x077a73dd)  , 17 },
     { MAD_F(0x077b1baf)  , 17 },
     { MAD_F(0x077bc385)  , 17 },
     { MAD_F(0x077c6b5f)  , 17 },
     { MAD_F(0x077d133c)  , 17 },
     { MAD_F(0x077dbb1d)  , 17 },
     { MAD_F(0x077e6301)  , 17 },
     { MAD_F(0x077f0ae9)  , 17 },
     { MAD_F(0x077fb2d5)  , 17 },
     { MAD_F(0x07805ac5)  , 17 },

     { MAD_F(0x078102b8)  , 17 },
     { MAD_F(0x0781aaaf)  , 17 },
     { MAD_F(0x078252aa)  , 17 },
     { MAD_F(0x0782faa8)  , 17 },
     { MAD_F(0x0783a2aa)  , 17 },
     { MAD_F(0x07844aaf)  , 17 },
     { MAD_F(0x0784f2b8)  , 17 },
     { MAD_F(0x07859ac5)  , 17 },
     { MAD_F(0x078642d6)  , 17 },
     { MAD_F(0x0786eaea)  , 17 },
     { MAD_F(0x07879302)  , 17 },
     { MAD_F(0x07883b1e)  , 17 },
     { MAD_F(0x0788e33d)  , 17 },
     { MAD_F(0x07898b60)  , 17 },
     { MAD_F(0x078a3386)  , 17 },
     { MAD_F(0x078adbb0)  , 17 },

     { MAD_F(0x078b83de)  , 17 },
     { MAD_F(0x078c2c10)  , 17 },
     { MAD_F(0x078cd445)  , 17 },
     { MAD_F(0x078d7c7e)  , 17 },
     { MAD_F(0x078e24ba)  , 17 },
     { MAD_F(0x078eccfb)  , 17 },
     { MAD_F(0x078f753e)  , 17 },
     { MAD_F(0x07901d86)  , 17 },
     { MAD_F(0x0790c5d1)  , 17 },
     { MAD_F(0x07916e20)  , 17 },
     { MAD_F(0x07921672)  , 17 },
     { MAD_F(0x0792bec8)  , 17 },
     { MAD_F(0x07936722)  , 17 },
     { MAD_F(0x07940f80)  , 17 },
     { MAD_F(0x0794b7e1)  , 17 },
     { MAD_F(0x07956045)  , 17 },

     { MAD_F(0x079608ae)  , 17 },
     { MAD_F(0x0796b11a)  , 17 },
     { MAD_F(0x0797598a)  , 17 },
     { MAD_F(0x079801fd)  , 17 },
     { MAD_F(0x0798aa74)  , 17 },
     { MAD_F(0x079952ee)  , 17 },
     { MAD_F(0x0799fb6d)  , 17 },
     { MAD_F(0x079aa3ef)  , 17 },
     { MAD_F(0x079b4c74)  , 17 },
     { MAD_F(0x079bf4fd)  , 17 },
     { MAD_F(0x079c9d8a)  , 17 },
     { MAD_F(0x079d461b)  , 17 },
     { MAD_F(0x079deeaf)  , 17 },
     { MAD_F(0x079e9747)  , 17 },
     { MAD_F(0x079f3fe2)  , 17 },
     { MAD_F(0x079fe881)  , 17 },

     { MAD_F(0x07a09124)  , 17 },
     { MAD_F(0x07a139ca)  , 17 },
     { MAD_F(0x07a1e274)  , 17 },
     { MAD_F(0x07a28b22)  , 17 },
     { MAD_F(0x07a333d3)  , 17 },
     { MAD_F(0x07a3dc88)  , 17 },
     { MAD_F(0x07a48541)  , 17 },
     { MAD_F(0x07a52dfd)  , 17 },
     { MAD_F(0x07a5d6bd)  , 17 },
     { MAD_F(0x07a67f80)  , 17 },
     { MAD_F(0x07a72847)  , 17 },
     { MAD_F(0x07a7d112)  , 17 },
     { MAD_F(0x07a879e1)  , 17 },
     { MAD_F(0x07a922b3)  , 17 },
     { MAD_F(0x07a9cb88)  , 17 },
     { MAD_F(0x07aa7462)  , 17 },

     { MAD_F(0x07ab1d3e)  , 17 },
     { MAD_F(0x07abc61f)  , 17 },
     { MAD_F(0x07ac6f03)  , 17 },
     { MAD_F(0x07ad17eb)  , 17 },
     { MAD_F(0x07adc0d6)  , 17 },
     { MAD_F(0x07ae69c6)  , 17 },
     { MAD_F(0x07af12b8)  , 17 },
     { MAD_F(0x07afbbaf)  , 17 },
     { MAD_F(0x07b064a8)  , 17 },
     { MAD_F(0x07b10da6)  , 17 },
     { MAD_F(0x07b1b6a7)  , 17 },
     { MAD_F(0x07b25fac)  , 17 },
     { MAD_F(0x07b308b5)  , 17 },
     { MAD_F(0x07b3b1c1)  , 17 },
     { MAD_F(0x07b45ad0)  , 17 },
     { MAD_F(0x07b503e4)  , 17 },

     { MAD_F(0x07b5acfb)  , 17 },
     { MAD_F(0x07b65615)  , 17 },
     { MAD_F(0x07b6ff33)  , 17 },
     { MAD_F(0x07b7a855)  , 17 },
     { MAD_F(0x07b8517b)  , 17 },
     { MAD_F(0x07b8faa4)  , 17 },
     { MAD_F(0x07b9a3d0)  , 17 },
     { MAD_F(0x07ba4d01)  , 17 },
     { MAD_F(0x07baf635)  , 17 },
     { MAD_F(0x07bb9f6c)  , 17 },
     { MAD_F(0x07bc48a7)  , 17 },
     { MAD_F(0x07bcf1e6)  , 17 },
     { MAD_F(0x07bd9b28)  , 17 },
     { MAD_F(0x07be446e)  , 17 },
     { MAD_F(0x07beedb8)  , 17 },
     { MAD_F(0x07bf9705)  , 17 },

     { MAD_F(0x07c04056)  , 17 },
     { MAD_F(0x07c0e9aa)  , 17 },
     { MAD_F(0x07c19302)  , 17 },
     { MAD_F(0x07c23c5e)  , 17 },
     { MAD_F(0x07c2e5bd)  , 17 },
     { MAD_F(0x07c38f20)  , 17 },
     { MAD_F(0x07c43887)  , 17 },
     { MAD_F(0x07c4e1f1)  , 17 },
     { MAD_F(0x07c58b5f)  , 17 },
     { MAD_F(0x07c634d0)  , 17 },
     { MAD_F(0x07c6de45)  , 17 },
     { MAD_F(0x07c787bd)  , 17 },
     { MAD_F(0x07c83139)  , 17 },
     { MAD_F(0x07c8dab9)  , 17 },
     { MAD_F(0x07c9843c)  , 17 },
     { MAD_F(0x07ca2dc3)  , 17 },

     { MAD_F(0x07cad74e)  , 17 },
     { MAD_F(0x07cb80dc)  , 17 },
     { MAD_F(0x07cc2a6e)  , 17 },
     { MAD_F(0x07ccd403)  , 17 },
     { MAD_F(0x07cd7d9c)  , 17 },
     { MAD_F(0x07ce2739)  , 17 },
     { MAD_F(0x07ced0d9)  , 17 },
     { MAD_F(0x07cf7a7d)  , 17 },
     { MAD_F(0x07d02424)  , 17 },
     { MAD_F(0x07d0cdcf)  , 17 },
     { MAD_F(0x07d1777e)  , 17 },
     { MAD_F(0x07d22130)  , 17 },
     { MAD_F(0x07d2cae5)  , 17 },
     { MAD_F(0x07d3749f)  , 17 },
     { MAD_F(0x07d41e5c)  , 17 },
     { MAD_F(0x07d4c81c)  , 17 },

     { MAD_F(0x07d571e0)  , 17 },
     { MAD_F(0x07d61ba8)  , 17 },
     { MAD_F(0x07d6c573)  , 17 },
     { MAD_F(0x07d76f42)  , 17 },
     { MAD_F(0x07d81915)  , 17 },
     { MAD_F(0x07d8c2eb)  , 17 },
     { MAD_F(0x07d96cc4)  , 17 },
     { MAD_F(0x07da16a2)  , 17 },
     { MAD_F(0x07dac083)  , 17 },
     { MAD_F(0x07db6a67)  , 17 },
     { MAD_F(0x07dc144f)  , 17 },
     { MAD_F(0x07dcbe3b)  , 17 },
     { MAD_F(0x07dd682a)  , 17 },
     { MAD_F(0x07de121d)  , 17 },
     { MAD_F(0x07debc13)  , 17 },
     { MAD_F(0x07df660d)  , 17 },

     { MAD_F(0x07e0100a)  , 17 },
     { MAD_F(0x07e0ba0c)  , 17 },
     { MAD_F(0x07e16410)  , 17 },
     { MAD_F(0x07e20e19)  , 17 },
     { MAD_F(0x07e2b824)  , 17 },
     { MAD_F(0x07e36234)  , 17 },
     { MAD_F(0x07e40c47)  , 17 },
     { MAD_F(0x07e4b65e)  , 17 },
     { MAD_F(0x07e56078)  , 17 },
     { MAD_F(0x07e60a95)  , 17 },
     { MAD_F(0x07e6b4b7)  , 17 },
     { MAD_F(0x07e75edc)  , 17 },
     { MAD_F(0x07e80904)  , 17 },
     { MAD_F(0x07e8b330)  , 17 },
     { MAD_F(0x07e95d60)  , 17 },
     { MAD_F(0x07ea0793)  , 17 },

     { MAD_F(0x07eab1ca)  , 17 },
     { MAD_F(0x07eb5c04)  , 17 },
     { MAD_F(0x07ec0642)  , 17 },
     { MAD_F(0x07ecb084)  , 17 },
     { MAD_F(0x07ed5ac9)  , 17 },
     { MAD_F(0x07ee0512)  , 17 },
     { MAD_F(0x07eeaf5e)  , 17 },
     { MAD_F(0x07ef59ae)  , 17 },
     { MAD_F(0x07f00401)  , 17 },
     { MAD_F(0x07f0ae58)  , 17 },
     { MAD_F(0x07f158b3)  , 17 },
     { MAD_F(0x07f20311)  , 17 },
     { MAD_F(0x07f2ad72)  , 17 },
     { MAD_F(0x07f357d8)  , 17 },
     { MAD_F(0x07f40240)  , 17 },
     { MAD_F(0x07f4acad)  , 17 },

     { MAD_F(0x07f5571d)  , 17 },
     { MAD_F(0x07f60190)  , 17 },
     { MAD_F(0x07f6ac07)  , 17 },
     { MAD_F(0x07f75682)  , 17 },
     { MAD_F(0x07f80100)  , 17 },
     { MAD_F(0x07f8ab82)  , 17 },
     { MAD_F(0x07f95607)  , 17 },
     { MAD_F(0x07fa0090)  , 17 },
     { MAD_F(0x07faab1c)  , 17 },
     { MAD_F(0x07fb55ac)  , 17 },
     { MAD_F(0x07fc0040)  , 17 },
     { MAD_F(0x07fcaad7)  , 17 },
     { MAD_F(0x07fd5572)  , 17 },
     { MAD_F(0x07fe0010)  , 17 },
     { MAD_F(0x07feaab2)  , 17 },
     { MAD_F(0x07ff5557)  , 17 },

     { MAD_F(0x04000000)  , 18 },
     { MAD_F(0x04005556)  , 18 },
     { MAD_F(0x0400aaae)  , 18 },
     { MAD_F(0x04010008)  , 18 },
     { MAD_F(0x04015563)  , 18 },
     { MAD_F(0x0401aac1)  , 18 },
     { MAD_F(0x04020020)  , 18 },
     { MAD_F(0x04025581)  , 18 },
     { MAD_F(0x0402aae3)  , 18 },
     { MAD_F(0x04030048)  , 18 },
     { MAD_F(0x040355ae)  , 18 },
     { MAD_F(0x0403ab16)  , 18 },
     { MAD_F(0x04040080)  , 18 },
     { MAD_F(0x040455eb)  , 18 },
     { MAD_F(0x0404ab59)  , 18 },
     { MAD_F(0x040500c8)  , 18 },

     { MAD_F(0x04055638)  , 18 },
     { MAD_F(0x0405abab)  , 18 },
     { MAD_F(0x0406011f)  , 18 },
     { MAD_F(0x04065696)  , 18 },
     { MAD_F(0x0406ac0e)  , 18 },
     { MAD_F(0x04070187)  , 18 },
     { MAD_F(0x04075703)  , 18 },
     { MAD_F(0x0407ac80)  , 18 },
     { MAD_F(0x040801ff)  , 18 },
     { MAD_F(0x04085780)  , 18 },
     { MAD_F(0x0408ad02)  , 18 },
     { MAD_F(0x04090287)  , 18 },
     { MAD_F(0x0409580d)  , 18 },
     { MAD_F(0x0409ad95)  , 18 },
     { MAD_F(0x040a031e)  , 18 },
     { MAD_F(0x040a58aa)  , 18 },

     { MAD_F(0x040aae37)  , 18 },
     { MAD_F(0x040b03c6)  , 18 },
     { MAD_F(0x040b5957)  , 18 },
     { MAD_F(0x040baee9)  , 18 },
     { MAD_F(0x040c047e)  , 18 },
     { MAD_F(0x040c5a14)  , 18 },
     { MAD_F(0x040cafab)  , 18 },
     { MAD_F(0x040d0545)  , 18 },
     { MAD_F(0x040d5ae0)  , 18 },
     { MAD_F(0x040db07d)  , 18 },
     { MAD_F(0x040e061c)  , 18 },
     { MAD_F(0x040e5bbd)  , 18 },
     { MAD_F(0x040eb15f)  , 18 },
     { MAD_F(0x040f0703)  , 18 },
     { MAD_F(0x040f5ca9)  , 18 },
     { MAD_F(0x040fb251)  , 18 },

     { MAD_F(0x041007fa)  , 18 },
     { MAD_F(0x04105da6)  , 18 },
     { MAD_F(0x0410b353)  , 18 },
     { MAD_F(0x04110901)  , 18 },
     { MAD_F(0x04115eb2)  , 18 },
     { MAD_F(0x0411b464)  , 18 },
     { MAD_F(0x04120a18)  , 18 },
     { MAD_F(0x04125fce)  , 18 },
     { MAD_F(0x0412b586)  , 18 },
     { MAD_F(0x04130b3f)  , 18 },
     { MAD_F(0x041360fa)  , 18 },
     { MAD_F(0x0413b6b7)  , 18 },
     { MAD_F(0x04140c75)  , 18 },
     { MAD_F(0x04146236)  , 18 },
     { MAD_F(0x0414b7f8)  , 18 },
     { MAD_F(0x04150dbc)  , 18 },

     { MAD_F(0x04156381)  , 18 },
     { MAD_F(0x0415b949)  , 18 },
     { MAD_F(0x04160f12)  , 18 },
     { MAD_F(0x041664dd)  , 18 },
     { MAD_F(0x0416baaa)  , 18 },
     { MAD_F(0x04171078)  , 18 },
     { MAD_F(0x04176648)  , 18 },
     { MAD_F(0x0417bc1a)  , 18 },
     { MAD_F(0x041811ee)  , 18 },
     { MAD_F(0x041867c3)  , 18 },
     { MAD_F(0x0418bd9b)  , 18 },
     { MAD_F(0x04191374)  , 18 },
     { MAD_F(0x0419694e)  , 18 },
     { MAD_F(0x0419bf2b)  , 18 },
     { MAD_F(0x041a1509)  , 18 },
     { MAD_F(0x041a6ae9)  , 18 },

     { MAD_F(0x041ac0cb)  , 18 },
     { MAD_F(0x041b16ae)  , 18 },
     { MAD_F(0x041b6c94)  , 18 },
     { MAD_F(0x041bc27b)  , 18 },
     { MAD_F(0x041c1863)  , 18 },
     { MAD_F(0x041c6e4e)  , 18 },
     { MAD_F(0x041cc43a)  , 18 },
     { MAD_F(0x041d1a28)  , 18 },
     { MAD_F(0x041d7018)  , 18 },
     { MAD_F(0x041dc60a)  , 18 },
     { MAD_F(0x041e1bfd)  , 18 },
     { MAD_F(0x041e71f2)  , 18 },
     { MAD_F(0x041ec7e9)  , 18 },
     { MAD_F(0x041f1de1)  , 18 },
     { MAD_F(0x041f73dc)  , 18 },
     { MAD_F(0x041fc9d8)  , 18 },

     { MAD_F(0x04201fd5)  , 18 },
     { MAD_F(0x042075d5)  , 18 },
     { MAD_F(0x0420cbd6)  , 18 },
     { MAD_F(0x042121d9)  , 18 },
     { MAD_F(0x042177de)  , 18 },
     { MAD_F(0x0421cde5)  , 18 },
     { MAD_F(0x042223ed)  , 18 },
     { MAD_F(0x042279f7)  , 18 },
     { MAD_F(0x0422d003)  , 18 },
     { MAD_F(0x04232611)  , 18 },
     { MAD_F(0x04237c20)  , 18 },
     { MAD_F(0x0423d231)  , 18 },
     { MAD_F(0x04242844)  , 18 },
     { MAD_F(0x04247e58)  , 18 },
     { MAD_F(0x0424d46e)  , 18 },
     { MAD_F(0x04252a87)  , 18 },

     { MAD_F(0x042580a0)  , 18 },
     { MAD_F(0x0425d6bc)  , 18 },
     { MAD_F(0x04262cd9)  , 18 },
     { MAD_F(0x042682f8)  , 18 },
     { MAD_F(0x0426d919)  , 18 },
     { MAD_F(0x04272f3b)  , 18 },
     { MAD_F(0x04278560)  , 18 },
     { MAD_F(0x0427db86)  , 18 },
     { MAD_F(0x042831ad)  , 18 },
     { MAD_F(0x042887d7)  , 18 },
     { MAD_F(0x0428de02)  , 18 },
     { MAD_F(0x0429342f)  , 18 },
     { MAD_F(0x04298a5e)  , 18 },
     { MAD_F(0x0429e08e)  , 18 },
     { MAD_F(0x042a36c0)  , 18 },
     { MAD_F(0x042a8cf4)  , 18 },

     { MAD_F(0x042ae32a)  , 18 },
     { MAD_F(0x042b3962)  , 18 },
     { MAD_F(0x042b8f9b)  , 18 },
     { MAD_F(0x042be5d6)  , 18 },
     { MAD_F(0x042c3c12)  , 18 },
     { MAD_F(0x042c9251)  , 18 },
     { MAD_F(0x042ce891)  , 18 },
     { MAD_F(0x042d3ed3)  , 18 },
     { MAD_F(0x042d9516)  , 18 },
     { MAD_F(0x042deb5c)  , 18 },
     { MAD_F(0x042e41a3)  , 18 },
     { MAD_F(0x042e97ec)  , 18 },
     { MAD_F(0x042eee36)  , 18 },
     { MAD_F(0x042f4482)  , 18 },
     { MAD_F(0x042f9ad1)  , 18 },
     { MAD_F(0x042ff120)  , 18 },

     { MAD_F(0x04304772)  , 18 },
     { MAD_F(0x04309dc5)  , 18 },
     { MAD_F(0x0430f41a)  , 18 },
     { MAD_F(0x04314a71)  , 18 },
     { MAD_F(0x0431a0c9)  , 18 },
     { MAD_F(0x0431f723)  , 18 },
     { MAD_F(0x04324d7f)  , 18 },
     { MAD_F(0x0432a3dd)  , 18 },
     { MAD_F(0x0432fa3d)  , 18 },
     { MAD_F(0x0433509e)  , 18 },
     { MAD_F(0x0433a701)  , 18 },
     { MAD_F(0x0433fd65)  , 18 },
     { MAD_F(0x043453cc)  , 18 },
     { MAD_F(0x0434aa34)  , 18 },
     { MAD_F(0x0435009d)  , 18 },
     { MAD_F(0x04355709)  , 18 },

     { MAD_F(0x0435ad76)  , 18 },
     { MAD_F(0x043603e5)  , 18 },
     { MAD_F(0x04365a56)  , 18 },
     { MAD_F(0x0436b0c9)  , 18 },
     { MAD_F(0x0437073d)  , 18 },
     { MAD_F(0x04375db3)  , 18 },
     { MAD_F(0x0437b42a)  , 18 },
     { MAD_F(0x04380aa4)  , 18 },
     { MAD_F(0x0438611f)  , 18 },
     { MAD_F(0x0438b79c)  , 18 },
     { MAD_F(0x04390e1a)  , 18 },
     { MAD_F(0x0439649b)  , 18 },
     { MAD_F(0x0439bb1d)  , 18 },
     { MAD_F(0x043a11a1)  , 18 },
     { MAD_F(0x043a6826)  , 18 },
     { MAD_F(0x043abead)  , 18 },

     { MAD_F(0x043b1536)  , 18 },
     { MAD_F(0x043b6bc1)  , 18 },
     { MAD_F(0x043bc24d)  , 18 },
     { MAD_F(0x043c18dc)  , 18 },
     { MAD_F(0x043c6f6c)  , 18 },
     { MAD_F(0x043cc5fd)  , 18 },
     { MAD_F(0x043d1c91)  , 18 },
     { MAD_F(0x043d7326)  , 18 },
     { MAD_F(0x043dc9bc)  , 18 },
     { MAD_F(0x043e2055)  , 18 },
     { MAD_F(0x043e76ef)  , 18 },
     { MAD_F(0x043ecd8b)  , 18 },
     { MAD_F(0x043f2429)  , 18 },
     { MAD_F(0x043f7ac8)  , 18 },
     { MAD_F(0x043fd169)  , 18 },
     { MAD_F(0x0440280c)  , 18 },

     { MAD_F(0x04407eb1)  , 18 },
     { MAD_F(0x0440d557)  , 18 },
     { MAD_F(0x04412bff)  , 18 },
     { MAD_F(0x044182a9)  , 18 },
     { MAD_F(0x0441d955)  , 18 },
     { MAD_F(0x04423002)  , 18 },
     { MAD_F(0x044286b1)  , 18 },
     { MAD_F(0x0442dd61)  , 18 },
     { MAD_F(0x04433414)  , 18 },
     { MAD_F(0x04438ac8)  , 18 },
     { MAD_F(0x0443e17e)  , 18 },
     { MAD_F(0x04443835)  , 18 },
     { MAD_F(0x04448eef)  , 18 },
     { MAD_F(0x0444e5aa)  , 18 },
     { MAD_F(0x04453c66)  , 18 },
     { MAD_F(0x04459325)  , 18 },

     { MAD_F(0x0445e9e5)  , 18 },
     { MAD_F(0x044640a7)  , 18 },
     { MAD_F(0x0446976a)  , 18 },
     { MAD_F(0x0446ee30)  , 18 },
     { MAD_F(0x044744f7)  , 18 },
     { MAD_F(0x04479bc0)  , 18 },
     { MAD_F(0x0447f28a)  , 18 },
     { MAD_F(0x04484956)  , 18 },
     { MAD_F(0x0448a024)  , 18 },
     { MAD_F(0x0448f6f4)  , 18 },
     { MAD_F(0x04494dc5)  , 18 },
     { MAD_F(0x0449a498)  , 18 },
     { MAD_F(0x0449fb6d)  , 18 },
     { MAD_F(0x044a5243)  , 18 },
     { MAD_F(0x044aa91c)  , 18 },
     { MAD_F(0x044afff6)  , 18 },

     { MAD_F(0x044b56d1)  , 18 },
     { MAD_F(0x044badaf)  , 18 },
     { MAD_F(0x044c048e)  , 18 },
     { MAD_F(0x044c5b6f)  , 18 },
     { MAD_F(0x044cb251)  , 18 },
     { MAD_F(0x044d0935)  , 18 },
     { MAD_F(0x044d601b)  , 18 },
     { MAD_F(0x044db703)  , 18 },
     { MAD_F(0x044e0dec)  , 18 },
     { MAD_F(0x044e64d7)  , 18 },
     { MAD_F(0x044ebbc4)  , 18 },
     { MAD_F(0x044f12b3)  , 18 },
     { MAD_F(0x044f69a3)  , 18 },
     { MAD_F(0x044fc095)  , 18 },
     { MAD_F(0x04501788)  , 18 },
     { MAD_F(0x04506e7e)  , 18 },

     { MAD_F(0x0450c575)  , 18 },
     { MAD_F(0x04511c6e)  , 18 },
     { MAD_F(0x04517368)  , 18 },
     { MAD_F(0x0451ca64)  , 18 },
     { MAD_F(0x04522162)  , 18 },
     { MAD_F(0x04527862)  , 18 },
     { MAD_F(0x0452cf63)  , 18 },
     { MAD_F(0x04532666)  , 18 },
     { MAD_F(0x04537d6b)  , 18 },
     { MAD_F(0x0453d472)  , 18 },
     { MAD_F(0x04542b7a)  , 18 },
     { MAD_F(0x04548284)  , 18 },
     { MAD_F(0x0454d98f)  , 18 },
     { MAD_F(0x0455309c)  , 18 },
     { MAD_F(0x045587ab)  , 18 },
     { MAD_F(0x0455debc)  , 18 },

     { MAD_F(0x045635cf)  , 18 },
     { MAD_F(0x04568ce3)  , 18 },
     { MAD_F(0x0456e3f9)  , 18 },
     { MAD_F(0x04573b10)  , 18 },
     { MAD_F(0x04579229)  , 18 },
     { MAD_F(0x0457e944)  , 18 },
     { MAD_F(0x04584061)  , 18 },
     { MAD_F(0x0458977f)  , 18 },
     { MAD_F(0x0458ee9f)  , 18 },
     { MAD_F(0x045945c1)  , 18 },
     { MAD_F(0x04599ce5)  , 18 },
     { MAD_F(0x0459f40a)  , 18 },
     { MAD_F(0x045a4b31)  , 18 },
     { MAD_F(0x045aa259)  , 18 },
     { MAD_F(0x045af984)  , 18 },
     { MAD_F(0x045b50b0)  , 18 },

     { MAD_F(0x045ba7dd)  , 18 },
     { MAD_F(0x045bff0d)  , 18 },
     { MAD_F(0x045c563e)  , 18 },
     { MAD_F(0x045cad71)  , 18 },
     { MAD_F(0x045d04a5)  , 18 },
     { MAD_F(0x045d5bdc)  , 18 },
     { MAD_F(0x045db313)  , 18 },
     { MAD_F(0x045e0a4d)  , 18 },
     { MAD_F(0x045e6188)  , 18 },
     { MAD_F(0x045eb8c5)  , 18 },
     { MAD_F(0x045f1004)  , 18 },
     { MAD_F(0x045f6745)  , 18 },
     { MAD_F(0x045fbe87)  , 18 },
     { MAD_F(0x046015cb)  , 18 },
     { MAD_F(0x04606d10)  , 18 },
     { MAD_F(0x0460c457)  , 18 },

     { MAD_F(0x04611ba0)  , 18 },
     { MAD_F(0x046172eb)  , 18 },
     { MAD_F(0x0461ca37)  , 18 },
     { MAD_F(0x04622185)  , 18 },
     { MAD_F(0x046278d5)  , 18 },
     { MAD_F(0x0462d026)  , 18 },
     { MAD_F(0x0463277a)  , 18 },
     { MAD_F(0x04637ece)  , 18 },
     { MAD_F(0x0463d625)  , 18 },
     { MAD_F(0x04642d7d)  , 18 },
     { MAD_F(0x046484d7)  , 18 },
     { MAD_F(0x0464dc33)  , 18 },
     { MAD_F(0x04653390)  , 18 },
     { MAD_F(0x04658aef)  , 18 },
     { MAD_F(0x0465e250)  , 18 },
     { MAD_F(0x046639b2)  , 18 },

     { MAD_F(0x04669116)  , 18 },
     { MAD_F(0x0466e87c)  , 18 },
     { MAD_F(0x04673fe3)  , 18 },
     { MAD_F(0x0467974d)  , 18 },
     { MAD_F(0x0467eeb7)  , 18 },
     { MAD_F(0x04684624)  , 18 },
     { MAD_F(0x04689d92)  , 18 },
     { MAD_F(0x0468f502)  , 18 },
     { MAD_F(0x04694c74)  , 18 },
     { MAD_F(0x0469a3e7)  , 18 },
     { MAD_F(0x0469fb5c)  , 18 },
     { MAD_F(0x046a52d3)  , 18 },
     { MAD_F(0x046aaa4b)  , 18 },
     { MAD_F(0x046b01c5)  , 18 },
     { MAD_F(0x046b5941)  , 18 },
     { MAD_F(0x046bb0bf)  , 18 },

     { MAD_F(0x046c083e)  , 18 },
     { MAD_F(0x046c5fbf)  , 18 },
     { MAD_F(0x046cb741)  , 18 },
     { MAD_F(0x046d0ec5)  , 18 },
     { MAD_F(0x046d664b)  , 18 },
     { MAD_F(0x046dbdd3)  , 18 },
     { MAD_F(0x046e155c)  , 18 },
     { MAD_F(0x046e6ce7)  , 18 },
     { MAD_F(0x046ec474)  , 18 },
     { MAD_F(0x046f1c02)  , 18 },
     { MAD_F(0x046f7392)  , 18 },
     { MAD_F(0x046fcb24)  , 18 },
     { MAD_F(0x047022b8)  , 18 },
     { MAD_F(0x04707a4d)  , 18 },
     { MAD_F(0x0470d1e4)  , 18 },
     { MAD_F(0x0471297c)  , 18 },

     { MAD_F(0x04718116)  , 18 },
     { MAD_F(0x0471d8b2)  , 18 },
     { MAD_F(0x04723050)  , 18 },
     { MAD_F(0x047287ef)  , 18 },
     { MAD_F(0x0472df90)  , 18 },
     { MAD_F(0x04733733)  , 18 },
     { MAD_F(0x04738ed7)  , 18 },
     { MAD_F(0x0473e67d)  , 18 },
     { MAD_F(0x04743e25)  , 18 },
     { MAD_F(0x047495ce)  , 18 },
     { MAD_F(0x0474ed79)  , 18 },
     { MAD_F(0x04754526)  , 18 },
     { MAD_F(0x04759cd4)  , 18 },
     { MAD_F(0x0475f484)  , 18 },
     { MAD_F(0x04764c36)  , 18 },
     { MAD_F(0x0476a3ea)  , 18 },

     { MAD_F(0x0476fb9f)  , 18 },
     { MAD_F(0x04775356)  , 18 },
     { MAD_F(0x0477ab0e)  , 18 },
     { MAD_F(0x047802c8)  , 18 },
     { MAD_F(0x04785a84)  , 18 },
     { MAD_F(0x0478b242)  , 18 },
     { MAD_F(0x04790a01)  , 18 },
     { MAD_F(0x047961c2)  , 18 },
     { MAD_F(0x0479b984)  , 18 },
     { MAD_F(0x047a1149)  , 18 },
     { MAD_F(0x047a690f)  , 18 },
     { MAD_F(0x047ac0d6)  , 18 },
     { MAD_F(0x047b18a0)  , 18 },
     { MAD_F(0x047b706b)  , 18 },
     { MAD_F(0x047bc837)  , 18 },
     { MAD_F(0x047c2006)  , 18 },

     { MAD_F(0x047c77d6)  , 18 },
     { MAD_F(0x047ccfa8)  , 18 },
     { MAD_F(0x047d277b)  , 18 },
     { MAD_F(0x047d7f50)  , 18 },
     { MAD_F(0x047dd727)  , 18 },
     { MAD_F(0x047e2eff)  , 18 },
     { MAD_F(0x047e86d9)  , 18 },
     { MAD_F(0x047edeb5)  , 18 },
     { MAD_F(0x047f3693)  , 18 },
     { MAD_F(0x047f8e72)  , 18 },
     { MAD_F(0x047fe653)  , 18 },
     { MAD_F(0x04803e35)  , 18 },
     { MAD_F(0x04809619)  , 18 },
     { MAD_F(0x0480edff)  , 18 },
     { MAD_F(0x048145e7)  , 18 },
     { MAD_F(0x04819dd0)  , 18 },

     { MAD_F(0x0481f5bb)  , 18 },
     { MAD_F(0x04824da7)  , 18 },
     { MAD_F(0x0482a595)  , 18 },
     { MAD_F(0x0482fd85)  , 18 },
     { MAD_F(0x04835577)  , 18 },
     { MAD_F(0x0483ad6a)  , 18 },
     { MAD_F(0x0484055f)  , 18 },
     { MAD_F(0x04845d56)  , 18 },
     { MAD_F(0x0484b54e)  , 18 },
     { MAD_F(0x04850d48)  , 18 },
     { MAD_F(0x04856544)  , 18 },
     { MAD_F(0x0485bd41)  , 18 },
     { MAD_F(0x04861540)  , 18 },
     { MAD_F(0x04866d40)  , 18 },
     { MAD_F(0x0486c543)  , 18 },
     { MAD_F(0x04871d47)  , 18 },

     { MAD_F(0x0487754c)  , 18 },
     { MAD_F(0x0487cd54)  , 18 },
     { MAD_F(0x0488255d)  , 18 },
     { MAD_F(0x04887d67)  , 18 },
     { MAD_F(0x0488d574)  , 18 },
     { MAD_F(0x04892d82)  , 18 },
     { MAD_F(0x04898591)  , 18 },
     { MAD_F(0x0489dda3)  , 18 },
     { MAD_F(0x048a35b6)  , 18 },
     { MAD_F(0x048a8dca)  , 18 },
     { MAD_F(0x048ae5e1)  , 18 },
     { MAD_F(0x048b3df9)  , 18 },
     { MAD_F(0x048b9612)  , 18 },
     { MAD_F(0x048bee2e)  , 18 },
     { MAD_F(0x048c464b)  , 18 },
     { MAD_F(0x048c9e69)  , 18 },

     { MAD_F(0x048cf68a)  , 18 },
     { MAD_F(0x048d4eac)  , 18 },
     { MAD_F(0x048da6cf)  , 18 },
     { MAD_F(0x048dfef5)  , 18 },
     { MAD_F(0x048e571c)  , 18 },
     { MAD_F(0x048eaf44)  , 18 },
     { MAD_F(0x048f076f)  , 18 },
     { MAD_F(0x048f5f9b)  , 18 },
     { MAD_F(0x048fb7c8)  , 18 },
     { MAD_F(0x04900ff8)  , 18 },
     { MAD_F(0x04906829)  , 18 },
     { MAD_F(0x0490c05b)  , 18 },
     { MAD_F(0x04911890)  , 18 },
     { MAD_F(0x049170c6)  , 18 },
     { MAD_F(0x0491c8fd)  , 18 },
     { MAD_F(0x04922137)  , 18 },

     { MAD_F(0x04927972)  , 18 },
     { MAD_F(0x0492d1ae)  , 18 },
     { MAD_F(0x049329ed)  , 18 },
     { MAD_F(0x0493822c)  , 18 },
     { MAD_F(0x0493da6e)  , 18 },
     { MAD_F(0x049432b1)  , 18 },
     { MAD_F(0x04948af6)  , 18 },
     { MAD_F(0x0494e33d)  , 18 },
     { MAD_F(0x04953b85)  , 18 },
     { MAD_F(0x049593cf)  , 18 },
     { MAD_F(0x0495ec1b)  , 18 },
     { MAD_F(0x04964468)  , 18 },
     { MAD_F(0x04969cb7)  , 18 },
     { MAD_F(0x0496f508)  , 18 },
     { MAD_F(0x04974d5a)  , 18 },
     { MAD_F(0x0497a5ae)  , 18 },

     { MAD_F(0x0497fe03)  , 18 },
     { MAD_F(0x0498565a)  , 18 },
     { MAD_F(0x0498aeb3)  , 18 },
     { MAD_F(0x0499070e)  , 18 },
     { MAD_F(0x04995f6a)  , 18 },
     { MAD_F(0x0499b7c8)  , 18 },
     { MAD_F(0x049a1027)  , 18 },
     { MAD_F(0x049a6889)  , 18 },
     { MAD_F(0x049ac0eb)  , 18 },
     { MAD_F(0x049b1950)  , 18 },
     { MAD_F(0x049b71b6)  , 18 },
     { MAD_F(0x049bca1e)  , 18 },
     { MAD_F(0x049c2287)  , 18 },
     { MAD_F(0x049c7af2)  , 18 },
     { MAD_F(0x049cd35f)  , 18 },
     { MAD_F(0x049d2bce)  , 18 },

     { MAD_F(0x049d843e)  , 18 },
     { MAD_F(0x049ddcaf)  , 18 },
     { MAD_F(0x049e3523)  , 18 },
     { MAD_F(0x049e8d98)  , 18 },
     { MAD_F(0x049ee60e)  , 18 },
     { MAD_F(0x049f3e87)  , 18 },
     { MAD_F(0x049f9701)  , 18 },
     { MAD_F(0x049fef7c)  , 18 },
     { MAD_F(0x04a047fa)  , 18 },
     { MAD_F(0x04a0a079)  , 18 },
     { MAD_F(0x04a0f8f9)  , 18 },
     { MAD_F(0x04a1517c)  , 18 },
     { MAD_F(0x04a1a9ff)  , 18 },
     { MAD_F(0x04a20285)  , 18 },
     { MAD_F(0x04a25b0c)  , 18 },
     { MAD_F(0x04a2b395)  , 18 },

     { MAD_F(0x04a30c20)  , 18 },
     { MAD_F(0x04a364ac)  , 18 },
     { MAD_F(0x04a3bd3a)  , 18 },
     { MAD_F(0x04a415c9)  , 18 },
     { MAD_F(0x04a46e5a)  , 18 },
     { MAD_F(0x04a4c6ed)  , 18 },
     { MAD_F(0x04a51f81)  , 18 },
     { MAD_F(0x04a57818)  , 18 },
     { MAD_F(0x04a5d0af)  , 18 },
     { MAD_F(0x04a62949)  , 18 },
     { MAD_F(0x04a681e4)  , 18 },
     { MAD_F(0x04a6da80)  , 18 },
     { MAD_F(0x04a7331f)  , 18 },
     { MAD_F(0x04a78bbf)  , 18 },
     { MAD_F(0x04a7e460)  , 18 },
     { MAD_F(0x04a83d03)  , 18 },

     { MAD_F(0x04a895a8)  , 18 },
     { MAD_F(0x04a8ee4f)  , 18 },
     { MAD_F(0x04a946f7)  , 18 },
     { MAD_F(0x04a99fa1)  , 18 },
     { MAD_F(0x04a9f84c)  , 18 },
     { MAD_F(0x04aa50fa)  , 18 },
     { MAD_F(0x04aaa9a8)  , 18 },
     { MAD_F(0x04ab0259)  , 18 },
     { MAD_F(0x04ab5b0b)  , 18 },
     { MAD_F(0x04abb3bf)  , 18 },
     { MAD_F(0x04ac0c74)  , 18 },
     { MAD_F(0x04ac652b)  , 18 },
     { MAD_F(0x04acbde4)  , 18 },
     { MAD_F(0x04ad169e)  , 18 },
     { MAD_F(0x04ad6f5a)  , 18 },
     { MAD_F(0x04adc818)  , 18 },

     { MAD_F(0x04ae20d7)  , 18 },
     { MAD_F(0x04ae7998)  , 18 },
     { MAD_F(0x04aed25a)  , 18 },
     { MAD_F(0x04af2b1e)  , 18 },
     { MAD_F(0x04af83e4)  , 18 },
     { MAD_F(0x04afdcac)  , 18 },
     { MAD_F(0x04b03575)  , 18 },
     { MAD_F(0x04b08e40)  , 18 },
     { MAD_F(0x04b0e70c)  , 18 },
     { MAD_F(0x04b13fda)  , 18 },
     { MAD_F(0x04b198aa)  , 18 },
     { MAD_F(0x04b1f17b)  , 18 },
     { MAD_F(0x04b24a4e)  , 18 },
     { MAD_F(0x04b2a322)  , 18 },
     { MAD_F(0x04b2fbf9)  , 18 },
     { MAD_F(0x04b354d1)  , 18 },

     { MAD_F(0x04b3adaa)  , 18 },
     { MAD_F(0x04b40685)  , 18 },
     { MAD_F(0x04b45f62)  , 18 },
     { MAD_F(0x04b4b840)  , 18 },
     { MAD_F(0x04b51120)  , 18 },
     { MAD_F(0x04b56a02)  , 18 },
     { MAD_F(0x04b5c2e6)  , 18 },
     { MAD_F(0x04b61bcb)  , 18 },
     { MAD_F(0x04b674b1)  , 18 },
     { MAD_F(0x04b6cd99)  , 18 },
     { MAD_F(0x04b72683)  , 18 },
     { MAD_F(0x04b77f6f)  , 18 },
     { MAD_F(0x04b7d85c)  , 18 },
     { MAD_F(0x04b8314b)  , 18 },
     { MAD_F(0x04b88a3b)  , 18 },
     { MAD_F(0x04b8e32d)  , 18 },

     { MAD_F(0x04b93c21)  , 18 },
     { MAD_F(0x04b99516)  , 18 },
     { MAD_F(0x04b9ee0d)  , 18 },
     { MAD_F(0x04ba4706)  , 18 },
     { MAD_F(0x04baa000)  , 18 },
     { MAD_F(0x04baf8fc)  , 18 },
     { MAD_F(0x04bb51fa)  , 18 },
     { MAD_F(0x04bbaaf9)  , 18 },
     { MAD_F(0x04bc03fa)  , 18 },
     { MAD_F(0x04bc5cfc)  , 18 },
     { MAD_F(0x04bcb600)  , 18 },
     { MAD_F(0x04bd0f06)  , 18 },
     { MAD_F(0x04bd680d)  , 18 },
     { MAD_F(0x04bdc116)  , 18 },
     { MAD_F(0x04be1a21)  , 18 },
     { MAD_F(0x04be732d)  , 18 },

     { MAD_F(0x04becc3b)  , 18 },
     { MAD_F(0x04bf254a)  , 18 },
     { MAD_F(0x04bf7e5b)  , 18 },
     { MAD_F(0x04bfd76e)  , 18 },
     { MAD_F(0x04c03083)  , 18 },
     { MAD_F(0x04c08999)  , 18 },
     { MAD_F(0x04c0e2b0)  , 18 },
     { MAD_F(0x04c13bca)  , 18 },
     { MAD_F(0x04c194e4)  , 18 },
     { MAD_F(0x04c1ee01)  , 18 },
     { MAD_F(0x04c2471f)  , 18 },
     { MAD_F(0x04c2a03f)  , 18 },
     { MAD_F(0x04c2f960)  , 18 },
     { MAD_F(0x04c35283)  , 18 },
     { MAD_F(0x04c3aba8)  , 18 },
     { MAD_F(0x04c404ce)  , 18 },

     { MAD_F(0x04c45df6)  , 18 },
     { MAD_F(0x04c4b720)  , 18 },
     { MAD_F(0x04c5104b)  , 18 },
     { MAD_F(0x04c56978)  , 18 },
     { MAD_F(0x04c5c2a7)  , 18 },
     { MAD_F(0x04c61bd7)  , 18 },
     { MAD_F(0x04c67508)  , 18 },
     { MAD_F(0x04c6ce3c)  , 18 },
     { MAD_F(0x04c72771)  , 18 },
     { MAD_F(0x04c780a7)  , 18 },
     { MAD_F(0x04c7d9df)  , 18 },
     { MAD_F(0x04c83319)  , 18 },
     { MAD_F(0x04c88c55)  , 18 },
     { MAD_F(0x04c8e592)  , 18 },
     { MAD_F(0x04c93ed1)  , 18 },
     { MAD_F(0x04c99811)  , 18 },

     { MAD_F(0x04c9f153)  , 18 },
     { MAD_F(0x04ca4a97)  , 18 },
     { MAD_F(0x04caa3dc)  , 18 },
     { MAD_F(0x04cafd23)  , 18 },
     { MAD_F(0x04cb566b)  , 18 },
     { MAD_F(0x04cbafb5)  , 18 },
     { MAD_F(0x04cc0901)  , 18 },
     { MAD_F(0x04cc624e)  , 18 },
     { MAD_F(0x04ccbb9d)  , 18 },
     { MAD_F(0x04cd14ee)  , 18 },
     { MAD_F(0x04cd6e40)  , 18 },
     { MAD_F(0x04cdc794)  , 18 },
     { MAD_F(0x04ce20e9)  , 18 },
     { MAD_F(0x04ce7a40)  , 18 },
     { MAD_F(0x04ced399)  , 18 },
     { MAD_F(0x04cf2cf3)  , 18 },

     { MAD_F(0x04cf864f)  , 18 },
     { MAD_F(0x04cfdfad)  , 18 },
     { MAD_F(0x04d0390c)  , 18 },
     { MAD_F(0x04d0926d)  , 18 },
     { MAD_F(0x04d0ebcf)  , 18 },
     { MAD_F(0x04d14533)  , 18 },
     { MAD_F(0x04d19e99)  , 18 },
     { MAD_F(0x04d1f800)  , 18 },
     { MAD_F(0x04d25169)  , 18 },
     { MAD_F(0x04d2aad4)  , 18 },
     { MAD_F(0x04d30440)  , 18 },
     { MAD_F(0x04d35dae)  , 18 },
     { MAD_F(0x04d3b71d)  , 18 },
     { MAD_F(0x04d4108e)  , 18 },
     { MAD_F(0x04d46a01)  , 18 },
     { MAD_F(0x04d4c375)  , 18 },

     { MAD_F(0x04d51ceb)  , 18 },
     { MAD_F(0x04d57662)  , 18 },
     { MAD_F(0x04d5cfdb)  , 18 },
     { MAD_F(0x04d62956)  , 18 },
     { MAD_F(0x04d682d2)  , 18 },
     { MAD_F(0x04d6dc50)  , 18 },
     { MAD_F(0x04d735d0)  , 18 },
     { MAD_F(0x04d78f51)  , 18 },
     { MAD_F(0x04d7e8d4)  , 18 },
     { MAD_F(0x04d84258)  , 18 },
     { MAD_F(0x04d89bde)  , 18 },
     { MAD_F(0x04d8f566)  , 18 },
     { MAD_F(0x04d94eef)  , 18 },
     { MAD_F(0x04d9a87a)  , 18 },
     { MAD_F(0x04da0207)  , 18 },
     { MAD_F(0x04da5b95)  , 18 },

     { MAD_F(0x04dab524)  , 18 },
     { MAD_F(0x04db0eb6)  , 18 },
     { MAD_F(0x04db6849)  , 18 },
     { MAD_F(0x04dbc1dd)  , 18 },
     { MAD_F(0x04dc1b73)  , 18 },
     { MAD_F(0x04dc750b)  , 18 },
     { MAD_F(0x04dccea5)  , 18 },
     { MAD_F(0x04dd2840)  , 18 },
     { MAD_F(0x04dd81dc)  , 18 },
     { MAD_F(0x04dddb7a)  , 18 },
     { MAD_F(0x04de351a)  , 18 },
     { MAD_F(0x04de8ebc)  , 18 },
     { MAD_F(0x04dee85f)  , 18 },
     { MAD_F(0x04df4203)  , 18 },
     { MAD_F(0x04df9baa)  , 18 },
     { MAD_F(0x04dff552)  , 18 },

     { MAD_F(0x04e04efb)  , 18 },
     { MAD_F(0x04e0a8a6)  , 18 },
     { MAD_F(0x04e10253)  , 18 },
     { MAD_F(0x04e15c01)  , 18 },
     { MAD_F(0x04e1b5b1)  , 18 },
     { MAD_F(0x04e20f63)  , 18 },
     { MAD_F(0x04e26916)  , 18 },
     { MAD_F(0x04e2c2cb)  , 18 },
     { MAD_F(0x04e31c81)  , 18 },
     { MAD_F(0x04e37639)  , 18 },
     { MAD_F(0x04e3cff3)  , 18 },
     { MAD_F(0x04e429ae)  , 18 },
     { MAD_F(0x04e4836b)  , 18 },
     { MAD_F(0x04e4dd29)  , 18 },
     { MAD_F(0x04e536e9)  , 18 },
     { MAD_F(0x04e590ab)  , 18 },

     { MAD_F(0x04e5ea6e)  , 18 },
     { MAD_F(0x04e64433)  , 18 },
     { MAD_F(0x04e69df9)  , 18 },
     { MAD_F(0x04e6f7c1)  , 18 },
     { MAD_F(0x04e7518b)  , 18 },
     { MAD_F(0x04e7ab56)  , 18 },
     { MAD_F(0x04e80523)  , 18 },
     { MAD_F(0x04e85ef2)  , 18 },
     { MAD_F(0x04e8b8c2)  , 18 },
     { MAD_F(0x04e91293)  , 18 },
     { MAD_F(0x04e96c67)  , 18 },
     { MAD_F(0x04e9c63b)  , 18 },
     { MAD_F(0x04ea2012)  , 18 },
     { MAD_F(0x04ea79ea)  , 18 },
     { MAD_F(0x04ead3c4)  , 18 },
     { MAD_F(0x04eb2d9f)  , 18 },

     { MAD_F(0x04eb877c)  , 18 },
     { MAD_F(0x04ebe15b)  , 18 },
     { MAD_F(0x04ec3b3b)  , 18 },
     { MAD_F(0x04ec951c)  , 18 },
     { MAD_F(0x04ecef00)  , 18 },
     { MAD_F(0x04ed48e5)  , 18 },
     { MAD_F(0x04eda2cb)  , 18 },
     { MAD_F(0x04edfcb3)  , 18 },
     { MAD_F(0x04ee569d)  , 18 },
     { MAD_F(0x04eeb088)  , 18 },
     { MAD_F(0x04ef0a75)  , 18 },
     { MAD_F(0x04ef6464)  , 18 },
     { MAD_F(0x04efbe54)  , 18 },
     { MAD_F(0x04f01846)  , 18 },
     { MAD_F(0x04f07239)  , 18 },
     { MAD_F(0x04f0cc2e)  , 18 },

     { MAD_F(0x04f12624)  , 18 },
     { MAD_F(0x04f1801d)  , 18 },
     { MAD_F(0x04f1da16)  , 18 },
     { MAD_F(0x04f23412)  , 18 },
     { MAD_F(0x04f28e0f)  , 18 },
     { MAD_F(0x04f2e80d)  , 18 },
     { MAD_F(0x04f3420d)  , 18 },
     { MAD_F(0x04f39c0f)  , 18 },
     { MAD_F(0x04f3f612)  , 18 },
     { MAD_F(0x04f45017)  , 18 },
     { MAD_F(0x04f4aa1e)  , 18 },
     { MAD_F(0x04f50426)  , 18 },
     { MAD_F(0x04f55e30)  , 18 },
     { MAD_F(0x04f5b83b)  , 18 },
     { MAD_F(0x04f61248)  , 18 },
     { MAD_F(0x04f66c56)  , 18 },

     { MAD_F(0x04f6c666)  , 18 },
     { MAD_F(0x04f72078)  , 18 },
     { MAD_F(0x04f77a8b)  , 18 },
     { MAD_F(0x04f7d4a0)  , 18 },
     { MAD_F(0x04f82eb7)  , 18 },
     { MAD_F(0x04f888cf)  , 18 },
     { MAD_F(0x04f8e2e9)  , 18 },
     { MAD_F(0x04f93d04)  , 18 },
     { MAD_F(0x04f99721)  , 18 },
     { MAD_F(0x04f9f13f)  , 18 },
     { MAD_F(0x04fa4b5f)  , 18 },
     { MAD_F(0x04faa581)  , 18 },
     { MAD_F(0x04faffa4)  , 18 },
     { MAD_F(0x04fb59c9)  , 18 },
     { MAD_F(0x04fbb3ef)  , 18 },
     { MAD_F(0x04fc0e17)  , 18 },

     { MAD_F(0x04fc6841)  , 18 },
     { MAD_F(0x04fcc26c)  , 18 },
     { MAD_F(0x04fd1c99)  , 18 },
     { MAD_F(0x04fd76c7)  , 18 },
     { MAD_F(0x04fdd0f7)  , 18 },
     { MAD_F(0x04fe2b29)  , 18 },
     { MAD_F(0x04fe855c)  , 18 },
     { MAD_F(0x04fedf91)  , 18 },
     { MAD_F(0x04ff39c7)  , 18 },
     { MAD_F(0x04ff93ff)  , 18 },
     { MAD_F(0x04ffee38)  , 18 },
     { MAD_F(0x05004874)  , 18 },
     { MAD_F(0x0500a2b0)  , 18 },
     { MAD_F(0x0500fcef)  , 18 },
     { MAD_F(0x0501572e)  , 18 },
     { MAD_F(0x0501b170)  , 18 },

     { MAD_F(0x05020bb3)  , 18 },
     { MAD_F(0x050265f8)  , 18 },
     { MAD_F(0x0502c03e)  , 18 },
     { MAD_F(0x05031a86)  , 18 },
     { MAD_F(0x050374cf)  , 18 },
     { MAD_F(0x0503cf1a)  , 18 },
     { MAD_F(0x05042967)  , 18 },
     { MAD_F(0x050483b5)  , 18 },
     { MAD_F(0x0504de05)  , 18 },
     { MAD_F(0x05053856)  , 18 },
     { MAD_F(0x050592a9)  , 18 },
     { MAD_F(0x0505ecfd)  , 18 },
     { MAD_F(0x05064754)  , 18 },
     { MAD_F(0x0506a1ab)  , 18 },
     { MAD_F(0x0506fc04)  , 18 },
     { MAD_F(0x0507565f)  , 18 },

     { MAD_F(0x0507b0bc)  , 18 },
     { MAD_F(0x05080b1a)  , 18 },
     { MAD_F(0x05086579)  , 18 },
     { MAD_F(0x0508bfdb)  , 18 },
     { MAD_F(0x05091a3d)  , 18 },
     { MAD_F(0x050974a2)  , 18 },
     { MAD_F(0x0509cf08)  , 18 },
     { MAD_F(0x050a296f)  , 18 },
     { MAD_F(0x050a83d8)  , 18 },
     { MAD_F(0x050ade43)  , 18 },
     { MAD_F(0x050b38af)  , 18 },
     { MAD_F(0x050b931d)  , 18 },
     { MAD_F(0x050bed8d)  , 18 },
     { MAD_F(0x050c47fe)  , 18 },
     { MAD_F(0x050ca271)  , 18 },
     { MAD_F(0x050cfce5)  , 18 },

     { MAD_F(0x050d575b)  , 18 },
     { MAD_F(0x050db1d2)  , 18 },
     { MAD_F(0x050e0c4b)  , 18 },
     { MAD_F(0x050e66c5)  , 18 },
     { MAD_F(0x050ec141)  , 18 },
     { MAD_F(0x050f1bbf)  , 18 },
     { MAD_F(0x050f763e)  , 18 },
     { MAD_F(0x050fd0bf)  , 18 },
     { MAD_F(0x05102b42)  , 18 },
     { MAD_F(0x051085c6)  , 18 },
     { MAD_F(0x0510e04b)  , 18 },
     { MAD_F(0x05113ad3)  , 18 },
     { MAD_F(0x0511955b)  , 18 },
     { MAD_F(0x0511efe6)  , 18 },
     { MAD_F(0x05124a72)  , 18 },
     { MAD_F(0x0512a4ff)  , 18 },

     { MAD_F(0x0512ff8e)  , 18 },
     { MAD_F(0x05135a1f)  , 18 },
     { MAD_F(0x0513b4b1)  , 18 },
     { MAD_F(0x05140f45)  , 18 },
     { MAD_F(0x051469da)  , 18 },
     { MAD_F(0x0514c471)  , 18 },
     { MAD_F(0x05151f0a)  , 18 },
     { MAD_F(0x051579a4)  , 18 },
     { MAD_F(0x0515d440)  , 18 },
     { MAD_F(0x05162edd)  , 18 },
     { MAD_F(0x0516897c)  , 18 },
     { MAD_F(0x0516e41c)  , 18 },
     { MAD_F(0x05173ebe)  , 18 },
     { MAD_F(0x05179962)  , 18 },
     { MAD_F(0x0517f407)  , 18 },
     { MAD_F(0x05184eae)  , 18 },

     { MAD_F(0x0518a956)  , 18 },
     { MAD_F(0x05190400)  , 18 },
     { MAD_F(0x05195eab)  , 18 },
     { MAD_F(0x0519b958)  , 18 },
     { MAD_F(0x051a1407)  , 18 },
     { MAD_F(0x051a6eb7)  , 18 },
     { MAD_F(0x051ac969)  , 18 },
     { MAD_F(0x051b241c)  , 18 },
     { MAD_F(0x051b7ed1)  , 18 },
     { MAD_F(0x051bd987)  , 18 },
     { MAD_F(0x051c3440)  , 18 },
     { MAD_F(0x051c8ef9)  , 18 },
     { MAD_F(0x051ce9b4)  , 18 },
     { MAD_F(0x051d4471)  , 18 },
     { MAD_F(0x051d9f2f)  , 18 },
     { MAD_F(0x051df9ef)  , 18 },

     { MAD_F(0x051e54b1)  , 18 },
     { MAD_F(0x051eaf74)  , 18 },
     { MAD_F(0x051f0a38)  , 18 },
     { MAD_F(0x051f64ff)  , 18 },
     { MAD_F(0x051fbfc6)  , 18 },
     { MAD_F(0x05201a90)  , 18 },
     { MAD_F(0x0520755b)  , 18 },
     { MAD_F(0x0520d027)  , 18 },
     { MAD_F(0x05212af5)  , 18 },
     { MAD_F(0x052185c5)  , 18 },
     { MAD_F(0x0521e096)  , 18 },
     { MAD_F(0x05223b69)  , 18 },
     { MAD_F(0x0522963d)  , 18 },
     { MAD_F(0x0522f113)  , 18 },
     { MAD_F(0x05234bea)  , 18 },
     { MAD_F(0x0523a6c3)  , 18 },

     { MAD_F(0x0524019e)  , 18 },
     { MAD_F(0x05245c7a)  , 18 },
     { MAD_F(0x0524b758)  , 18 },
     { MAD_F(0x05251237)  , 18 },
     { MAD_F(0x05256d18)  , 18 },
     { MAD_F(0x0525c7fb)  , 18 },
     { MAD_F(0x052622df)  , 18 },
     { MAD_F(0x05267dc4)  , 18 },
     { MAD_F(0x0526d8ab)  , 18 },
     { MAD_F(0x05273394)  , 18 },
     { MAD_F(0x05278e7e)  , 18 },
     { MAD_F(0x0527e96a)  , 18 },
     { MAD_F(0x05284457)  , 18 },
     { MAD_F(0x05289f46)  , 18 },
     { MAD_F(0x0528fa37)  , 18 },
     { MAD_F(0x05295529)  , 18 },

     { MAD_F(0x0529b01d)  , 18 },
     { MAD_F(0x052a0b12)  , 18 },
     { MAD_F(0x052a6609)  , 18 },
     { MAD_F(0x052ac101)  , 18 },
     { MAD_F(0x052b1bfb)  , 18 },
     { MAD_F(0x052b76f7)  , 18 },
     { MAD_F(0x052bd1f4)  , 18 },
     { MAD_F(0x052c2cf2)  , 18 },
     { MAD_F(0x052c87f2)  , 18 },
     { MAD_F(0x052ce2f4)  , 18 },
     { MAD_F(0x052d3df7)  , 18 },
     { MAD_F(0x052d98fc)  , 18 },
     { MAD_F(0x052df403)  , 18 },
     { MAD_F(0x052e4f0b)  , 18 },
     { MAD_F(0x052eaa14)  , 18 },
     { MAD_F(0x052f051f)  , 18 },

     { MAD_F(0x052f602c)  , 18 },
     { MAD_F(0x052fbb3a)  , 18 },
     { MAD_F(0x0530164a)  , 18 },
     { MAD_F(0x0530715b)  , 18 },
     { MAD_F(0x0530cc6e)  , 18 },
     { MAD_F(0x05312783)  , 18 },
     { MAD_F(0x05318299)  , 18 },
     { MAD_F(0x0531ddb0)  , 18 },
     { MAD_F(0x053238ca)  , 18 },
     { MAD_F(0x053293e4)  , 18 },
     { MAD_F(0x0532ef01)  , 18 },
     { MAD_F(0x05334a1e)  , 18 },
     { MAD_F(0x0533a53e)  , 18 },
     { MAD_F(0x0534005f)  , 18 },
     { MAD_F(0x05345b81)  , 18 },
     { MAD_F(0x0534b6a5)  , 18 },

     { MAD_F(0x053511cb)  , 18 },
     { MAD_F(0x05356cf2)  , 18 },
     { MAD_F(0x0535c81b)  , 18 },
     { MAD_F(0x05362345)  , 18 },
     { MAD_F(0x05367e71)  , 18 },
     { MAD_F(0x0536d99f)  , 18 },
     { MAD_F(0x053734ce)  , 18 },
     { MAD_F(0x05378ffe)  , 18 },
     { MAD_F(0x0537eb30)  , 18 },
     { MAD_F(0x05384664)  , 18 },
     { MAD_F(0x0538a199)  , 18 },
     { MAD_F(0x0538fcd0)  , 18 },
     { MAD_F(0x05395808)  , 18 },
     { MAD_F(0x0539b342)  , 18 },
     { MAD_F(0x053a0e7d)  , 18 },
     { MAD_F(0x053a69ba)  , 18 },

     { MAD_F(0x053ac4f9)  , 18 },
     { MAD_F(0x053b2039)  , 18 },
     { MAD_F(0x053b7b7b)  , 18 },
     { MAD_F(0x053bd6be)  , 18 },
     { MAD_F(0x053c3203)  , 18 },
     { MAD_F(0x053c8d49)  , 18 },
     { MAD_F(0x053ce891)  , 18 },
     { MAD_F(0x053d43da)  , 18 },
     { MAD_F(0x053d9f25)  , 18 },
     { MAD_F(0x053dfa72)  , 18 },
     { MAD_F(0x053e55c0)  , 18 },
     { MAD_F(0x053eb10f)  , 18 },
     { MAD_F(0x053f0c61)  , 18 },
     { MAD_F(0x053f67b3)  , 18 },
     { MAD_F(0x053fc308)  , 18 },
     { MAD_F(0x05401e5e)  , 18 },

     { MAD_F(0x054079b5)  , 18 },
     { MAD_F(0x0540d50e)  , 18 },
     { MAD_F(0x05413068)  , 18 },
     { MAD_F(0x05418bc4)  , 18 },
     { MAD_F(0x0541e722)  , 18 },
     { MAD_F(0x05424281)  , 18 },
     { MAD_F(0x05429de2)  , 18 },
     { MAD_F(0x0542f944)  , 18 },
     { MAD_F(0x054354a8)  , 18 },
     { MAD_F(0x0543b00d)  , 18 },
     { MAD_F(0x05440b74)  , 18 },
     { MAD_F(0x054466dd)  , 18 },
     { MAD_F(0x0544c247)  , 18 },
     { MAD_F(0x05451db2)  , 18 },
     { MAD_F(0x0545791f)  , 18 },
     { MAD_F(0x0545d48e)  , 18 },

     { MAD_F(0x05462ffe)  , 18 },
     { MAD_F(0x05468b70)  , 18 },
     { MAD_F(0x0546e6e3)  , 18 },
     { MAD_F(0x05474258)  , 18 },
     { MAD_F(0x05479dce)  , 18 },
     { MAD_F(0x0547f946)  , 18 },
     { MAD_F(0x054854c0)  , 18 },
     { MAD_F(0x0548b03b)  , 18 },
     { MAD_F(0x05490bb7)  , 18 },
     { MAD_F(0x05496735)  , 18 },
     { MAD_F(0x0549c2b5)  , 18 },
     { MAD_F(0x054a1e36)  , 18 },
     { MAD_F(0x054a79b9)  , 18 },
     { MAD_F(0x054ad53d)  , 18 },
     { MAD_F(0x054b30c3)  , 18 },
     { MAD_F(0x054b8c4b)  , 18 },


     { MAD_F(0x054be7d4)  , 18 },
     { MAD_F(0x054c435e)  , 18 },
     { MAD_F(0x054c9eea)  , 18 },
     { MAD_F(0x054cfa78)  , 18 },
     { MAD_F(0x054d5607)  , 18 },
     { MAD_F(0x054db197)  , 18 },
     { MAD_F(0x054e0d2a)  , 18 },
     { MAD_F(0x054e68bd)  , 18 },
     { MAD_F(0x054ec453)  , 18 },
     { MAD_F(0x054f1fe9)  , 18 },
     { MAD_F(0x054f7b82)  , 18 },
     { MAD_F(0x054fd71c)  , 18 },
     { MAD_F(0x055032b7)  , 18 },
     { MAD_F(0x05508e54)  , 18 },
     { MAD_F(0x0550e9f3)  , 18 },
     { MAD_F(0x05514593)  , 18 },

     { MAD_F(0x0551a134)  , 18 },
     { MAD_F(0x0551fcd8)  , 18 },
     { MAD_F(0x0552587c)  , 18 },
     { MAD_F(0x0552b423)  , 18 },
     { MAD_F(0x05530fca)  , 18 },
     { MAD_F(0x05536b74)  , 18 },
     { MAD_F(0x0553c71f)  , 18 },
     { MAD_F(0x055422cb)  , 18 },
     { MAD_F(0x05547e79)  , 18 },
     { MAD_F(0x0554da29)  , 18 },
     { MAD_F(0x055535da)  , 18 },
     { MAD_F(0x0555918c)  , 18 },
     { MAD_F(0x0555ed40)  , 18 },
     { MAD_F(0x055648f6)  , 18 },
     { MAD_F(0x0556a4ad)  , 18 },
     { MAD_F(0x05570066)  , 18 },

     { MAD_F(0x05575c20)  , 18 },
     { MAD_F(0x0557b7dc)  , 18 },
     { MAD_F(0x05581399)  , 18 },
     { MAD_F(0x05586f58)  , 18 },
     { MAD_F(0x0558cb19)  , 18 },
     { MAD_F(0x055926db)  , 18 },
     { MAD_F(0x0559829e)  , 18 },
     { MAD_F(0x0559de63)  , 18 },
     { MAD_F(0x055a3a2a)  , 18 },
     { MAD_F(0x055a95f2)  , 18 },
     { MAD_F(0x055af1bb)  , 18 },
     { MAD_F(0x055b4d87)  , 18 },
     { MAD_F(0x055ba953)  , 18 },
     { MAD_F(0x055c0522)  , 18 },
     { MAD_F(0x055c60f1)  , 18 },
     { MAD_F(0x055cbcc3)  , 18 },

     { MAD_F(0x055d1896)  , 18 },
     { MAD_F(0x055d746a)  , 18 },
     { MAD_F(0x055dd040)  , 18 },
     { MAD_F(0x055e2c17)  , 18 },
     { MAD_F(0x055e87f0)  , 18 },
     { MAD_F(0x055ee3cb)  , 18 },
     { MAD_F(0x055f3fa7)  , 18 },
     { MAD_F(0x055f9b85)  , 18 },
     { MAD_F(0x055ff764)  , 18 },
     { MAD_F(0x05605344)  , 18 },
     { MAD_F(0x0560af27)  , 18 },
     { MAD_F(0x05610b0a)  , 18 },
     { MAD_F(0x056166f0)  , 18 },
     { MAD_F(0x0561c2d7)  , 18 },
     { MAD_F(0x05621ebf)  , 18 },
     { MAD_F(0x05627aa9)  , 18 },

     { MAD_F(0x0562d694)  , 18 },
     { MAD_F(0x05633281)  , 18 },
     { MAD_F(0x05638e70)  , 18 },
     { MAD_F(0x0563ea60)  , 18 },
     { MAD_F(0x05644651)  , 18 },
     { MAD_F(0x0564a244)  , 18 },
     { MAD_F(0x0564fe39)  , 18 },
     { MAD_F(0x05655a2f)  , 18 },
     { MAD_F(0x0565b627)  , 18 },
     { MAD_F(0x05661220)  , 18 },
     { MAD_F(0x05666e1a)  , 18 },
     { MAD_F(0x0566ca17)  , 18 },
     { MAD_F(0x05672614)  , 18 },
     { MAD_F(0x05678214)  , 18 },
     { MAD_F(0x0567de15)  , 18 },
     { MAD_F(0x05683a17)  , 18 },

     { MAD_F(0x0568961b)  , 18 },
     { MAD_F(0x0568f220)  , 18 },
     { MAD_F(0x05694e27)  , 18 },
     { MAD_F(0x0569aa30)  , 18 },
     { MAD_F(0x056a063a)  , 18 },
     { MAD_F(0x056a6245)  , 18 },
     { MAD_F(0x056abe52)  , 18 },
     { MAD_F(0x056b1a61)  , 18 },
     { MAD_F(0x056b7671)  , 18 },
     { MAD_F(0x056bd283)  , 18 },
     { MAD_F(0x056c2e96)  , 18 },
     { MAD_F(0x056c8aab)  , 18 },
     { MAD_F(0x056ce6c1)  , 18 },
     { MAD_F(0x056d42d9)  , 18 },
     { MAD_F(0x056d9ef2)  , 18 },
     { MAD_F(0x056dfb0d)  , 18 },

     { MAD_F(0x056e5729)  , 18 },
     { MAD_F(0x056eb347)  , 18 },
     { MAD_F(0x056f0f66)  , 18 },
     { MAD_F(0x056f6b87)  , 18 },
     { MAD_F(0x056fc7aa)  , 18 },
     { MAD_F(0x057023cd)  , 18 },
     { MAD_F(0x05707ff3)  , 18 },
     { MAD_F(0x0570dc1a)  , 18 },
     { MAD_F(0x05713843)  , 18 },
     { MAD_F(0x0571946d)  , 18 },
     { MAD_F(0x0571f098)  , 18 },
     { MAD_F(0x05724cc5)  , 18 },
     { MAD_F(0x0572a8f4)  , 18 },
     { MAD_F(0x05730524)  , 18 },
     { MAD_F(0x05736156)  , 18 },
     { MAD_F(0x0573bd89)  , 18 },

     { MAD_F(0x057419be)  , 18 },
     { MAD_F(0x057475f4)  , 18 },
     { MAD_F(0x0574d22c)  , 18 },
     { MAD_F(0x05752e65)  , 18 },
     { MAD_F(0x05758aa0)  , 18 },
     { MAD_F(0x0575e6dc)  , 18 },
     { MAD_F(0x0576431a)  , 18 },
     { MAD_F(0x05769f59)  , 18 },
     { MAD_F(0x0576fb9a)  , 18 },
     { MAD_F(0x057757dd)  , 18 },
     { MAD_F(0x0577b421)  , 18 },
     { MAD_F(0x05781066)  , 18 },
     { MAD_F(0x05786cad)  , 18 },
     { MAD_F(0x0578c8f5)  , 18 },
     { MAD_F(0x0579253f)  , 18 },
     { MAD_F(0x0579818b)  , 18 },

     { MAD_F(0x0579ddd8)  , 18 },
     { MAD_F(0x057a3a27)  , 18 },
     { MAD_F(0x057a9677)  , 18 },
     { MAD_F(0x057af2c8)  , 18 },
     { MAD_F(0x057b4f1c)  , 18 },
     { MAD_F(0x057bab70)  , 18 },
     { MAD_F(0x057c07c6)  , 18 },
     { MAD_F(0x057c641e)  , 18 },
     { MAD_F(0x057cc077)  , 18 },
     { MAD_F(0x057d1cd2)  , 18 },
     { MAD_F(0x057d792e)  , 18 },
     { MAD_F(0x057dd58c)  , 18 },
     { MAD_F(0x057e31eb)  , 18 },
     { MAD_F(0x057e8e4c)  , 18 },
     { MAD_F(0x057eeaae)  , 18 },
     { MAD_F(0x057f4712)  , 18 },

     { MAD_F(0x057fa378)  , 18 },
     { MAD_F(0x057fffde)  , 18 },
     { MAD_F(0x05805c47)  , 18 },
     { MAD_F(0x0580b8b1)  , 18 },
     { MAD_F(0x0581151c)  , 18 },
     { MAD_F(0x05817189)  , 18 },
     { MAD_F(0x0581cdf7)  , 18 },
     { MAD_F(0x05822a67)  , 18 },
     { MAD_F(0x058286d9)  , 18 },
     { MAD_F(0x0582e34c)  , 18 },
     { MAD_F(0x05833fc0)  , 18 },
     { MAD_F(0x05839c36)  , 18 },
     { MAD_F(0x0583f8ae)  , 18 },
     { MAD_F(0x05845527)  , 18 },
     { MAD_F(0x0584b1a1)  , 18 },
     { MAD_F(0x05850e1e)  , 18 },

     { MAD_F(0x05856a9b)  , 18 },
     { MAD_F(0x0585c71a)  , 18 },
     { MAD_F(0x0586239b)  , 18 },
     { MAD_F(0x0586801d)  , 18 },
     { MAD_F(0x0586dca1)  , 18 },
     { MAD_F(0x05873926)  , 18 },
     { MAD_F(0x058795ac)  , 18 },
     { MAD_F(0x0587f235)  , 18 },
     { MAD_F(0x05884ebe)  , 18 },
     { MAD_F(0x0588ab49)  , 18 },
     { MAD_F(0x058907d6)  , 18 },
     { MAD_F(0x05896464)  , 18 },
     { MAD_F(0x0589c0f4)  , 18 },
     { MAD_F(0x058a1d85)  , 18 },
     { MAD_F(0x058a7a18)  , 18 },
     { MAD_F(0x058ad6ac)  , 18 },

     { MAD_F(0x058b3342)  , 18 },
     { MAD_F(0x058b8fd9)  , 18 },
     { MAD_F(0x058bec72)  , 18 },
     { MAD_F(0x058c490c)  , 18 },
     { MAD_F(0x058ca5a8)  , 18 },
     { MAD_F(0x058d0246)  , 18 },
     { MAD_F(0x058d5ee4)  , 18 },
     { MAD_F(0x058dbb85)  , 18 },
     { MAD_F(0x058e1827)  , 18 },
     { MAD_F(0x058e74ca)  , 18 },
     { MAD_F(0x058ed16f)  , 18 },
     { MAD_F(0x058f2e15)  , 18 },
     { MAD_F(0x058f8abd)  , 18 },
     { MAD_F(0x058fe766)  , 18 },
     { MAD_F(0x05904411)  , 18 },
     { MAD_F(0x0590a0be)  , 18 },

     { MAD_F(0x0590fd6c)  , 18 },
     { MAD_F(0x05915a1b)  , 18 },
     { MAD_F(0x0591b6cc)  , 18 },
     { MAD_F(0x0592137e)  , 18 },
     { MAD_F(0x05927032)  , 18 },
     { MAD_F(0x0592cce8)  , 18 },
     { MAD_F(0x0593299f)  , 18 },
     { MAD_F(0x05938657)  , 18 },
     { MAD_F(0x0593e311)  , 18 },
     { MAD_F(0x05943fcd)  , 18 },
     { MAD_F(0x05949c8a)  , 18 },
     { MAD_F(0x0594f948)  , 18 },
     { MAD_F(0x05955608)  , 18 },
     { MAD_F(0x0595b2ca)  , 18 },
     { MAD_F(0x05960f8c)  , 18 },
     { MAD_F(0x05966c51)  , 18 },

     { MAD_F(0x0596c917)  , 18 },
     { MAD_F(0x059725de)  , 18 },
     { MAD_F(0x059782a7)  , 18 },
     { MAD_F(0x0597df72)  , 18 },
     { MAD_F(0x05983c3e)  , 18 },
     { MAD_F(0x0598990c)  , 18 },
     { MAD_F(0x0598f5db)  , 18 },
     { MAD_F(0x059952ab)  , 18 },
     { MAD_F(0x0599af7d)  , 18 },
     { MAD_F(0x059a0c51)  , 18 },
     { MAD_F(0x059a6926)  , 18 },
     { MAD_F(0x059ac5fc)  , 18 },
     { MAD_F(0x059b22d4)  , 18 },
     { MAD_F(0x059b7fae)  , 18 },
     { MAD_F(0x059bdc89)  , 18 },
     { MAD_F(0x059c3965)  , 18 },

     { MAD_F(0x059c9643)  , 18 },
     { MAD_F(0x059cf323)  , 18 },
     { MAD_F(0x059d5004)  , 18 },
     { MAD_F(0x059dace6)  , 18 },
     { MAD_F(0x059e09cb)  , 18 },
     { MAD_F(0x059e66b0)  , 18 },
     { MAD_F(0x059ec397)  , 18 },
     { MAD_F(0x059f2080)  , 18 },
     { MAD_F(0x059f7d6a)  , 18 },
     { MAD_F(0x059fda55)  , 18 },
     { MAD_F(0x05a03742)  , 18 },
     { MAD_F(0x05a09431)  , 18 },
     { MAD_F(0x05a0f121)  , 18 },
     { MAD_F(0x05a14e12)  , 18 },
     { MAD_F(0x05a1ab05)  , 18 },
     { MAD_F(0x05a207fa)  , 18 },

     { MAD_F(0x05a264f0)  , 18 },
     { MAD_F(0x05a2c1e7)  , 18 },
     { MAD_F(0x05a31ee1)  , 18 },
     { MAD_F(0x05a37bdb)  , 18 },
     { MAD_F(0x05a3d8d7)  , 18 },
     { MAD_F(0x05a435d5)  , 18 },
     { MAD_F(0x05a492d4)  , 18 },
     { MAD_F(0x05a4efd4)  , 18 },
     { MAD_F(0x05a54cd6)  , 18 },
     { MAD_F(0x05a5a9da)  , 18 },
     { MAD_F(0x05a606df)  , 18 },
     { MAD_F(0x05a663e5)  , 18 },
     { MAD_F(0x05a6c0ed)  , 18 },
     { MAD_F(0x05a71df7)  , 18 },
     { MAD_F(0x05a77b02)  , 18 },
     { MAD_F(0x05a7d80e)  , 18 },

     { MAD_F(0x05a8351c)  , 18 },
     { MAD_F(0x05a8922c)  , 18 },
     { MAD_F(0x05a8ef3c)  , 18 },
     { MAD_F(0x05a94c4f)  , 18 },
     { MAD_F(0x05a9a963)  , 18 },
     { MAD_F(0x05aa0678)  , 18 },
     { MAD_F(0x05aa638f)  , 18 },
     { MAD_F(0x05aac0a8)  , 18 },
     { MAD_F(0x05ab1dc2)  , 18 },
     { MAD_F(0x05ab7add)  , 18 },
     { MAD_F(0x05abd7fa)  , 18 },
     { MAD_F(0x05ac3518)  , 18 },
     { MAD_F(0x05ac9238)  , 18 },
     { MAD_F(0x05acef5a)  , 18 },
     { MAD_F(0x05ad4c7d)  , 18 },
     { MAD_F(0x05ada9a1)  , 18 },

     { MAD_F(0x05ae06c7)  , 18 },
     { MAD_F(0x05ae63ee)  , 18 },
     { MAD_F(0x05aec117)  , 18 },
     { MAD_F(0x05af1e41)  , 18 },
     { MAD_F(0x05af7b6d)  , 18 },
     { MAD_F(0x05afd89b)  , 18 },
     { MAD_F(0x05b035c9)  , 18 },
     { MAD_F(0x05b092fa)  , 18 },
     { MAD_F(0x05b0f02b)  , 18 },
     { MAD_F(0x05b14d5f)  , 18 },
     { MAD_F(0x05b1aa94)  , 18 },
     { MAD_F(0x05b207ca)  , 18 },
     { MAD_F(0x05b26502)  , 18 },
     { MAD_F(0x05b2c23b)  , 18 },
     { MAD_F(0x05b31f76)  , 18 },
     { MAD_F(0x05b37cb2)  , 18 },

     { MAD_F(0x05b3d9f0)  , 18 },
     { MAD_F(0x05b4372f)  , 18 },
     { MAD_F(0x05b4946f)  , 18 },
     { MAD_F(0x05b4f1b2)  , 18 },
     { MAD_F(0x05b54ef5)  , 18 },
     { MAD_F(0x05b5ac3a)  , 18 },
     { MAD_F(0x05b60981)  , 18 },
     { MAD_F(0x05b666c9)  , 18 },
     { MAD_F(0x05b6c413)  , 18 },
     { MAD_F(0x05b7215e)  , 18 },
     { MAD_F(0x05b77eab)  , 18 },
     { MAD_F(0x05b7dbf9)  , 18 },
     { MAD_F(0x05b83948)  , 18 },
     { MAD_F(0x05b89699)  , 18 },
     { MAD_F(0x05b8f3ec)  , 18 },
     { MAD_F(0x05b95140)  , 18 },

     { MAD_F(0x05b9ae95)  , 18 },
     { MAD_F(0x05ba0bec)  , 18 },
     { MAD_F(0x05ba6945)  , 18 },
     { MAD_F(0x05bac69f)  , 18 },
     { MAD_F(0x05bb23fa)  , 18 },
     { MAD_F(0x05bb8157)  , 18 },
     { MAD_F(0x05bbdeb6)  , 18 },
     { MAD_F(0x05bc3c16)  , 18 },
     { MAD_F(0x05bc9977)  , 18 },
     { MAD_F(0x05bcf6da)  , 18 },
     { MAD_F(0x05bd543e)  , 18 },
     { MAD_F(0x05bdb1a4)  , 18 },
     { MAD_F(0x05be0f0b)  , 18 },
     { MAD_F(0x05be6c74)  , 18 },
     { MAD_F(0x05bec9df)  , 18 },
     { MAD_F(0x05bf274a)  , 18 },

     { MAD_F(0x05bf84b8)  , 18 },
     { MAD_F(0x05bfe226)  , 18 },
     { MAD_F(0x05c03f97)  , 18 },
     { MAD_F(0x05c09d08)  , 18 },
     { MAD_F(0x05c0fa7c)  , 18 },
     { MAD_F(0x05c157f0)  , 18 },
     { MAD_F(0x05c1b566)  , 18 },
     { MAD_F(0x05c212de)  , 18 },
     { MAD_F(0x05c27057)  , 18 },
     { MAD_F(0x05c2cdd2)  , 18 },
     { MAD_F(0x05c32b4e)  , 18 },
     { MAD_F(0x05c388cb)  , 18 },
     { MAD_F(0x05c3e64b)  , 18 },
     { MAD_F(0x05c443cb)  , 18 },
     { MAD_F(0x05c4a14d)  , 18 },
     { MAD_F(0x05c4fed1)  , 18 },

     { MAD_F(0x05c55c56)  , 18 },
     { MAD_F(0x05c5b9dc)  , 18 },
     { MAD_F(0x05c61764)  , 18 },
     { MAD_F(0x05c674ed)  , 18 },
     { MAD_F(0x05c6d278)  , 18 },
     { MAD_F(0x05c73005)  , 18 },
     { MAD_F(0x05c78d93)  , 18 },
     { MAD_F(0x05c7eb22)  , 18 },
     { MAD_F(0x05c848b3)  , 18 },
     { MAD_F(0x05c8a645)  , 18 },
     { MAD_F(0x05c903d9)  , 18 },
     { MAD_F(0x05c9616e)  , 18 },
     { MAD_F(0x05c9bf05)  , 18 },
     { MAD_F(0x05ca1c9d)  , 18 },
     { MAD_F(0x05ca7a37)  , 18 },
     { MAD_F(0x05cad7d2)  , 18 },

     { MAD_F(0x05cb356e)  , 18 },
     { MAD_F(0x05cb930d)  , 18 },
     { MAD_F(0x05cbf0ac)  , 18 },
     { MAD_F(0x05cc4e4d)  , 18 },
     { MAD_F(0x05ccabf0)  , 18 },
     { MAD_F(0x05cd0994)  , 18 },
     { MAD_F(0x05cd6739)  , 18 },
     { MAD_F(0x05cdc4e0)  , 18 },
     { MAD_F(0x05ce2289)  , 18 },
     { MAD_F(0x05ce8033)  , 18 },
     { MAD_F(0x05ceddde)  , 18 },
     { MAD_F(0x05cf3b8b)  , 18 },
     { MAD_F(0x05cf9939)  , 18 },
     { MAD_F(0x05cff6e9)  , 18 },
     { MAD_F(0x05d0549a)  , 18 },
     { MAD_F(0x05d0b24d)  , 18 },

     { MAD_F(0x05d11001)  , 18 },
     { MAD_F(0x05d16db7)  , 18 },
     { MAD_F(0x05d1cb6e)  , 18 },
     { MAD_F(0x05d22927)  , 18 },
     { MAD_F(0x05d286e1)  , 18 },
     { MAD_F(0x05d2e49d)  , 18 },
     { MAD_F(0x05d3425a)  , 18 },
     { MAD_F(0x05d3a018)  , 18 },
     { MAD_F(0x05d3fdd8)  , 18 },
     { MAD_F(0x05d45b9a)  , 18 },
     { MAD_F(0x05d4b95d)  , 18 },
     { MAD_F(0x05d51721)  , 18 },
     { MAD_F(0x05d574e7)  , 18 },
     { MAD_F(0x05d5d2af)  , 18 },
     { MAD_F(0x05d63078)  , 18 },
     { MAD_F(0x05d68e42)  , 18 },

     { MAD_F(0x05d6ec0e)  , 18 },
     { MAD_F(0x05d749db)  , 18 },
     { MAD_F(0x05d7a7aa)  , 18 },
     { MAD_F(0x05d8057a)  , 18 },
     { MAD_F(0x05d8634c)  , 18 },
     { MAD_F(0x05d8c11f)  , 18 },
     { MAD_F(0x05d91ef4)  , 18 },
     { MAD_F(0x05d97cca)  , 18 },
     { MAD_F(0x05d9daa1)  , 18 },
     { MAD_F(0x05da387a)  , 18 },
     { MAD_F(0x05da9655)  , 18 },
     { MAD_F(0x05daf431)  , 18 },
     { MAD_F(0x05db520e)  , 18 },
     { MAD_F(0x05dbafed)  , 18 },
     { MAD_F(0x05dc0dce)  , 18 },
     { MAD_F(0x05dc6baf)  , 18 },

     { MAD_F(0x05dcc993)  , 18 },
     { MAD_F(0x05dd2778)  , 18 },
     { MAD_F(0x05dd855e)  , 18 },
     { MAD_F(0x05dde346)  , 18 },
     { MAD_F(0x05de412f)  , 18 },
     { MAD_F(0x05de9f1a)  , 18 },
     { MAD_F(0x05defd06)  , 18 },
     { MAD_F(0x05df5af3)  , 18 },
     { MAD_F(0x05dfb8e2)  , 18 },
     { MAD_F(0x05e016d3)  , 18 },
     { MAD_F(0x05e074c5)  , 18 },
     { MAD_F(0x05e0d2b8)  , 18 },
     { MAD_F(0x05e130ad)  , 18 },
     { MAD_F(0x05e18ea4)  , 18 },
     { MAD_F(0x05e1ec9c)  , 18 },
     { MAD_F(0x05e24a95)  , 18 },

     { MAD_F(0x05e2a890)  , 18 },
     { MAD_F(0x05e3068c)  , 18 },
     { MAD_F(0x05e3648a)  , 18 },
     { MAD_F(0x05e3c289)  , 18 },
     { MAD_F(0x05e4208a)  , 18 },
     { MAD_F(0x05e47e8c)  , 18 },
     { MAD_F(0x05e4dc8f)  , 18 },
     { MAD_F(0x05e53a94)  , 18 },
     { MAD_F(0x05e5989b)  , 18 },
     { MAD_F(0x05e5f6a3)  , 18 },
     { MAD_F(0x05e654ac)  , 18 },
     { MAD_F(0x05e6b2b7)  , 18 },
     { MAD_F(0x05e710c4)  , 18 },
     { MAD_F(0x05e76ed2)  , 18 },
     { MAD_F(0x05e7cce1)  , 18 },
     { MAD_F(0x05e82af2)  , 18 },

     { MAD_F(0x05e88904)  , 18 },
     { MAD_F(0x05e8e718)  , 18 },
     { MAD_F(0x05e9452d)  , 18 },
     { MAD_F(0x05e9a343)  , 18 },
     { MAD_F(0x05ea015c)  , 18 },
     { MAD_F(0x05ea5f75)  , 18 },
     { MAD_F(0x05eabd90)  , 18 },
     { MAD_F(0x05eb1bad)  , 18 },
     { MAD_F(0x05eb79cb)  , 18 },
     { MAD_F(0x05ebd7ea)  , 18 },
     { MAD_F(0x05ec360b)  , 18 },
     { MAD_F(0x05ec942d)  , 18 },
     { MAD_F(0x05ecf251)  , 18 },
     { MAD_F(0x05ed5076)  , 18 },
     { MAD_F(0x05edae9d)  , 18 },
     { MAD_F(0x05ee0cc5)  , 18 },

     { MAD_F(0x05ee6aef)  , 18 },
     { MAD_F(0x05eec91a)  , 18 },
     { MAD_F(0x05ef2746)  , 18 },
     { MAD_F(0x05ef8574)  , 18 },
     { MAD_F(0x05efe3a4)  , 18 },
     { MAD_F(0x05f041d5)  , 18 },
     { MAD_F(0x05f0a007)  , 18 },
     { MAD_F(0x05f0fe3b)  , 18 },
     { MAD_F(0x05f15c70)  , 18 },
     { MAD_F(0x05f1baa7)  , 18 },
     { MAD_F(0x05f218df)  , 18 },
     { MAD_F(0x05f27719)  , 18 },
     { MAD_F(0x05f2d554)  , 18 },
     { MAD_F(0x05f33390)  , 18 },
     { MAD_F(0x05f391cf)  , 18 },
     { MAD_F(0x05f3f00e)  , 18 },

     { MAD_F(0x05f44e4f)  , 18 },
     { MAD_F(0x05f4ac91)  , 18 },
     { MAD_F(0x05f50ad5)  , 18 },
     { MAD_F(0x05f5691b)  , 18 },
     { MAD_F(0x05f5c761)  , 18 },
     { MAD_F(0x05f625aa)  , 18 },
     { MAD_F(0x05f683f3)  , 18 },
     { MAD_F(0x05f6e23f)  , 18 },
     { MAD_F(0x05f7408b)  , 18 },
     { MAD_F(0x05f79ed9)  , 18 },
     { MAD_F(0x05f7fd29)  , 18 },
     { MAD_F(0x05f85b7a)  , 18 },
     { MAD_F(0x05f8b9cc)  , 18 },
     { MAD_F(0x05f91820)  , 18 },
     { MAD_F(0x05f97675)  , 18 },
     { MAD_F(0x05f9d4cc)  , 18 },

     { MAD_F(0x05fa3324)  , 18 },
     { MAD_F(0x05fa917e)  , 18 },
     { MAD_F(0x05faefd9)  , 18 },
     { MAD_F(0x05fb4e36)  , 18 },
     { MAD_F(0x05fbac94)  , 18 },
     { MAD_F(0x05fc0af3)  , 18 },
     { MAD_F(0x05fc6954)  , 18 },
     { MAD_F(0x05fcc7b7)  , 18 },
     { MAD_F(0x05fd261b)  , 18 },
     { MAD_F(0x05fd8480)  , 18 },
     { MAD_F(0x05fde2e7)  , 18 },
     { MAD_F(0x05fe414f)  , 18 },
     { MAD_F(0x05fe9fb9)  , 18 },
     { MAD_F(0x05fefe24)  , 18 },
     { MAD_F(0x05ff5c91)  , 18 },
     { MAD_F(0x05ffbaff)  , 18 },

     { MAD_F(0x0600196e)  , 18 },
     { MAD_F(0x060077df)  , 18 },
     { MAD_F(0x0600d651)  , 18 },
     { MAD_F(0x060134c5)  , 18 },
     { MAD_F(0x0601933b)  , 18 },
     { MAD_F(0x0601f1b1)  , 18 },
     { MAD_F(0x0602502a)  , 18 },
     { MAD_F(0x0602aea3)  , 18 },
     { MAD_F(0x06030d1e)  , 18 },
     { MAD_F(0x06036b9b)  , 18 },
     { MAD_F(0x0603ca19)  , 18 },
     { MAD_F(0x06042898)  , 18 },
     { MAD_F(0x06048719)  , 18 },
     { MAD_F(0x0604e59c)  , 18 },
     { MAD_F(0x0605441f)  , 18 },
     { MAD_F(0x0605a2a5)  , 18 },

     { MAD_F(0x0606012b)  , 18 },
     { MAD_F(0x06065fb4)  , 18 },
     { MAD_F(0x0606be3d)  , 18 },
     { MAD_F(0x06071cc8)  , 18 },
     { MAD_F(0x06077b55)  , 18 },
     { MAD_F(0x0607d9e3)  , 18 },
     { MAD_F(0x06083872)  , 18 },
     { MAD_F(0x06089703)  , 18 },
     { MAD_F(0x0608f595)  , 18 },
     { MAD_F(0x06095429)  , 18 },
     { MAD_F(0x0609b2be)  , 18 },
     { MAD_F(0x060a1155)  , 18 },
     { MAD_F(0x060a6fed)  , 18 },
     { MAD_F(0x060ace86)  , 18 },
     { MAD_F(0x060b2d21)  , 18 },
     { MAD_F(0x060b8bbe)  , 18 },

     { MAD_F(0x060bea5c)  , 18 },
     { MAD_F(0x060c48fb)  , 18 },
     { MAD_F(0x060ca79c)  , 18 },
     { MAD_F(0x060d063e)  , 18 },
     { MAD_F(0x060d64e1)  , 18 },
     { MAD_F(0x060dc387)  , 18 },
     { MAD_F(0x060e222d)  , 18 },
     { MAD_F(0x060e80d5)  , 18 },
     { MAD_F(0x060edf7f)  , 18 },
     { MAD_F(0x060f3e29)  , 18 },
     { MAD_F(0x060f9cd6)  , 18 },
     { MAD_F(0x060ffb83)  , 18 },
     { MAD_F(0x06105a33)  , 18 },
     { MAD_F(0x0610b8e3)  , 18 },
     { MAD_F(0x06111795)  , 18 },
     { MAD_F(0x06117649)  , 18 },

     { MAD_F(0x0611d4fe)  , 18 },
     { MAD_F(0x061233b4)  , 18 },
     { MAD_F(0x0612926c)  , 18 },
     { MAD_F(0x0612f125)  , 18 },
     { MAD_F(0x06134fe0)  , 18 },
     { MAD_F(0x0613ae9c)  , 18 },
     { MAD_F(0x06140d5a)  , 18 },
     { MAD_F(0x06146c19)  , 18 },
     { MAD_F(0x0614cada)  , 18 },
     { MAD_F(0x0615299c)  , 18 },
     { MAD_F(0x0615885f)  , 18 },
     { MAD_F(0x0615e724)  , 18 },
     { MAD_F(0x061645ea)  , 18 },
     { MAD_F(0x0616a4b2)  , 18 },
     { MAD_F(0x0617037b)  , 18 },
     { MAD_F(0x06176246)  , 18 },

     { MAD_F(0x0617c112)  , 18 },
     { MAD_F(0x06181fdf)  , 18 },
     { MAD_F(0x06187eae)  , 18 },
     { MAD_F(0x0618dd7e)  , 18 },
     { MAD_F(0x06193c50)  , 18 },
     { MAD_F(0x06199b24)  , 18 },
     { MAD_F(0x0619f9f8)  , 18 },
     { MAD_F(0x061a58ce)  , 18 },
     { MAD_F(0x061ab7a6)  , 18 },
     { MAD_F(0x061b167f)  , 18 },
     { MAD_F(0x061b7559)  , 18 },
     { MAD_F(0x061bd435)  , 18 },
     { MAD_F(0x061c3313)  , 18 },
     { MAD_F(0x061c91f1)  , 18 },
     { MAD_F(0x061cf0d2)  , 18 },
     { MAD_F(0x061d4fb3)  , 18 },

     { MAD_F(0x061dae96)  , 18 },
     { MAD_F(0x061e0d7b)  , 18 },
     { MAD_F(0x061e6c61)  , 18 },
     { MAD_F(0x061ecb48)  , 18 },
     { MAD_F(0x061f2a31)  , 18 },
     { MAD_F(0x061f891b)  , 18 },
     { MAD_F(0x061fe807)  , 18 },
     { MAD_F(0x062046f4)  , 18 },
     { MAD_F(0x0620a5e3)  , 18 },
     { MAD_F(0x062104d3)  , 18 },
     { MAD_F(0x062163c4)  , 18 },
     { MAD_F(0x0621c2b7)  , 18 },
     { MAD_F(0x062221ab)  , 18 },
     { MAD_F(0x062280a1)  , 18 },
     { MAD_F(0x0622df98)  , 18 },
     { MAD_F(0x06233e91)  , 18 },

     { MAD_F(0x06239d8b)  , 18 },
     { MAD_F(0x0623fc86)  , 18 },
     { MAD_F(0x06245b83)  , 18 },
     { MAD_F(0x0624ba82)  , 18 },
     { MAD_F(0x06251981)  , 18 },
     { MAD_F(0x06257883)  , 18 },
     { MAD_F(0x0625d785)  , 18 },
     { MAD_F(0x06263689)  , 18 },
     { MAD_F(0x0626958f)  , 18 },
     { MAD_F(0x0626f496)  , 18 },
     { MAD_F(0x0627539e)  , 18 },
     { MAD_F(0x0627b2a8)  , 18 },
     { MAD_F(0x062811b3)  , 18 },
     { MAD_F(0x062870c0)  , 18 },
     { MAD_F(0x0628cfce)  , 18 },
     { MAD_F(0x06292ede)  , 18 },

     { MAD_F(0x06298def)  , 18 },
     { MAD_F(0x0629ed01)  , 18 },
     { MAD_F(0x062a4c15)  , 18 },
     { MAD_F(0x062aab2a)  , 18 },
     { MAD_F(0x062b0a41)  , 18 },
     { MAD_F(0x062b6959)  , 18 },
     { MAD_F(0x062bc873)  , 18 },
     { MAD_F(0x062c278e)  , 18 },
     { MAD_F(0x062c86aa)  , 18 },
     { MAD_F(0x062ce5c8)  , 18 },
     { MAD_F(0x062d44e8)  , 18 },
     { MAD_F(0x062da408)  , 18 },
     { MAD_F(0x062e032a)  , 18 },
     { MAD_F(0x062e624e)  , 18 },
     { MAD_F(0x062ec173)  , 18 },
     { MAD_F(0x062f209a)  , 18 },

     { MAD_F(0x062f7fc1)  , 18 },
     { MAD_F(0x062fdeeb)  , 18 },
     { MAD_F(0x06303e16)  , 18 },
     { MAD_F(0x06309d42)  , 18 },
     { MAD_F(0x0630fc6f)  , 18 },
     { MAD_F(0x06315b9e)  , 18 },
     { MAD_F(0x0631bacf)  , 18 },
     { MAD_F(0x06321a01)  , 18 },
     { MAD_F(0x06327934)  , 18 },
     { MAD_F(0x0632d869)  , 18 },
     { MAD_F(0x0633379f)  , 18 },
     { MAD_F(0x063396d7)  , 18 },
     { MAD_F(0x0633f610)  , 18 },
     { MAD_F(0x0634554a)  , 18 },
     { MAD_F(0x0634b486)  , 18 },
     { MAD_F(0x063513c3)  , 18 },

     { MAD_F(0x06357302)  , 18 },
     { MAD_F(0x0635d242)  , 18 },
     { MAD_F(0x06363184)  , 18 },
     { MAD_F(0x063690c7)  , 18 },
     { MAD_F(0x0636f00b)  , 18 },
     { MAD_F(0x06374f51)  , 18 },
     { MAD_F(0x0637ae99)  , 18 },
     { MAD_F(0x06380de1)  , 18 },
     { MAD_F(0x06386d2b)  , 18 },
     { MAD_F(0x0638cc77)  , 18 },
     { MAD_F(0x06392bc4)  , 18 },
     { MAD_F(0x06398b12)  , 18 },
     { MAD_F(0x0639ea62)  , 18 },
     { MAD_F(0x063a49b4)  , 18 },
     { MAD_F(0x063aa906)  , 18 },
     { MAD_F(0x063b085a)  , 18 },

     { MAD_F(0x063b67b0)  , 18 },
     { MAD_F(0x063bc707)  , 18 },
     { MAD_F(0x063c265f)  , 18 },
     { MAD_F(0x063c85b9)  , 18 },
     { MAD_F(0x063ce514)  , 18 },
     { MAD_F(0x063d4471)  , 18 },
     { MAD_F(0x063da3cf)  , 18 },
     { MAD_F(0x063e032f)  , 18 },
     { MAD_F(0x063e6290)  , 18 },
     { MAD_F(0x063ec1f2)  , 18 },
     { MAD_F(0x063f2156)  , 18 },
     { MAD_F(0x063f80bb)  , 18 },
     { MAD_F(0x063fe022)  , 18 },
     { MAD_F(0x06403f8a)  , 18 },
     { MAD_F(0x06409ef3)  , 18 },
     { MAD_F(0x0640fe5e)  , 18 },

     { MAD_F(0x06415dcb)  , 18 },
     { MAD_F(0x0641bd38)  , 18 },
     { MAD_F(0x06421ca7)  , 18 },
     { MAD_F(0x06427c18)  , 18 },
     { MAD_F(0x0642db8a)  , 18 },
     { MAD_F(0x06433afd)  , 18 },
     { MAD_F(0x06439a72)  , 18 },
     { MAD_F(0x0643f9e9)  , 18 },
     { MAD_F(0x06445960)  , 18 },
     { MAD_F(0x0644b8d9)  , 18 },
     { MAD_F(0x06451854)  , 18 },
     { MAD_F(0x064577d0)  , 18 },
     { MAD_F(0x0645d74d)  , 18 },
     { MAD_F(0x064636cc)  , 18 },
     { MAD_F(0x0646964c)  , 18 },
     { MAD_F(0x0646f5ce)  , 18 },

     { MAD_F(0x06475551)  , 18 },
     { MAD_F(0x0647b4d5)  , 18 },
     { MAD_F(0x0648145b)  , 18 },
     { MAD_F(0x064873e3)  , 18 },
     { MAD_F(0x0648d36b)  , 18 },
     { MAD_F(0x064932f6)  , 18 },
     { MAD_F(0x06499281)  , 18 },
     { MAD_F(0x0649f20e)  , 18 },
     { MAD_F(0x064a519c)  , 18 },
     { MAD_F(0x064ab12c)  , 18 },
     { MAD_F(0x064b10be)  , 18 },
     { MAD_F(0x064b7050)  , 18 },
     { MAD_F(0x064bcfe4)  , 18 },
     { MAD_F(0x064c2f7a)  , 18 },
     { MAD_F(0x064c8f11)  , 18 },
     { MAD_F(0x064ceea9)  , 18 },

     { MAD_F(0x064d4e43)  , 18 },
     { MAD_F(0x064dadde)  , 18 },
     { MAD_F(0x064e0d7a)  , 18 },
     { MAD_F(0x064e6d18)  , 18 },
     { MAD_F(0x064eccb8)  , 18 },
     { MAD_F(0x064f2c59)  , 18 },
     { MAD_F(0x064f8bfb)  , 18 },
     { MAD_F(0x064feb9e)  , 18 },
     { MAD_F(0x06504b44)  , 18 },
     { MAD_F(0x0650aaea)  , 18 },
     { MAD_F(0x06510a92)  , 18 },
     { MAD_F(0x06516a3b)  , 18 },
     { MAD_F(0x0651c9e6)  , 18 },
     { MAD_F(0x06522992)  , 18 },
     { MAD_F(0x06528940)  , 18 },
     { MAD_F(0x0652e8ef)  , 18 },

     { MAD_F(0x0653489f)  , 18 },
     { MAD_F(0x0653a851)  , 18 },
     { MAD_F(0x06540804)  , 18 },
     { MAD_F(0x065467b9)  , 18 },
     { MAD_F(0x0654c76f)  , 18 },
     { MAD_F(0x06552726)  , 18 },
     { MAD_F(0x065586df)  , 18 },
     { MAD_F(0x0655e699)  , 18 },
     { MAD_F(0x06564655)  , 18 },
     { MAD_F(0x0656a612)  , 18 },
     { MAD_F(0x065705d0)  , 18 },
     { MAD_F(0x06576590)  , 18 },
     { MAD_F(0x0657c552)  , 18 },
     { MAD_F(0x06582514)  , 18 },
     { MAD_F(0x065884d9)  , 18 },
     { MAD_F(0x0658e49e)  , 18 },

     { MAD_F(0x06594465)  , 18 },
     { MAD_F(0x0659a42e)  , 18 },
     { MAD_F(0x065a03f7)  , 18 },
     { MAD_F(0x065a63c3)  , 18 },
     { MAD_F(0x065ac38f)  , 18 },
     { MAD_F(0x065b235d)  , 18 },
     { MAD_F(0x065b832d)  , 18 },
     { MAD_F(0x065be2fe)  , 18 },
     { MAD_F(0x065c42d0)  , 18 },
     { MAD_F(0x065ca2a3)  , 18 },
     { MAD_F(0x065d0279)  , 18 },
     { MAD_F(0x065d624f)  , 18 },
     { MAD_F(0x065dc227)  , 18 },
     { MAD_F(0x065e2200)  , 18 },
     { MAD_F(0x065e81db)  , 18 },
     { MAD_F(0x065ee1b7)  , 18 },

     { MAD_F(0x065f4195)  , 18 },
     { MAD_F(0x065fa174)  , 18 },
     { MAD_F(0x06600154)  , 18 },
     { MAD_F(0x06606136)  , 18 },
     { MAD_F(0x0660c119)  , 18 },
     { MAD_F(0x066120fd)  , 18 },
     { MAD_F(0x066180e3)  , 18 },
     { MAD_F(0x0661e0cb)  , 18 },
     { MAD_F(0x066240b4)  , 18 },
     { MAD_F(0x0662a09e)  , 18 },
     { MAD_F(0x06630089)  , 18 },
     { MAD_F(0x06636077)  , 18 },
     { MAD_F(0x0663c065)  , 18 },
     { MAD_F(0x06642055)  , 18 },
     { MAD_F(0x06648046)  , 18 },
     { MAD_F(0x0664e039)  , 18 },

     { MAD_F(0x0665402d)  , 18 },
     { MAD_F(0x0665a022)  , 18 },
     { MAD_F(0x06660019)  , 18 },
     { MAD_F(0x06666011)  , 18 },
     { MAD_F(0x0666c00b)  , 18 },
     { MAD_F(0x06672006)  , 18 },
     { MAD_F(0x06678003)  , 18 },
     { MAD_F(0x0667e000)  , 18 },
     { MAD_F(0x06684000)  , 18 },
     { MAD_F(0x0668a000)  , 18 },
     { MAD_F(0x06690003)  , 18 },
     { MAD_F(0x06696006)  , 18 },
     { MAD_F(0x0669c00b)  , 18 },
     { MAD_F(0x066a2011)  , 18 },
     { MAD_F(0x066a8019)  , 18 },
     { MAD_F(0x066ae022)  , 18 },

     { MAD_F(0x066b402d)  , 18 },
     { MAD_F(0x066ba039)  , 18 },
     { MAD_F(0x066c0046)  , 18 },
     { MAD_F(0x066c6055)  , 18 },
     { MAD_F(0x066cc065)  , 18 },
     { MAD_F(0x066d2076)  , 18 },
     { MAD_F(0x066d8089)  , 18 },
     { MAD_F(0x066de09e)  , 18 },
     { MAD_F(0x066e40b3)  , 18 },
     { MAD_F(0x066ea0cb)  , 18 },
     { MAD_F(0x066f00e3)  , 18 },
     { MAD_F(0x066f60fd)  , 18 },
     { MAD_F(0x066fc118)  , 18 },
     { MAD_F(0x06702135)  , 18 },
     { MAD_F(0x06708153)  , 18 },
     { MAD_F(0x0670e173)  , 18 },

     { MAD_F(0x06714194)  , 18 },
     { MAD_F(0x0671a1b6)  , 18 },
     { MAD_F(0x067201da)  , 18 },
     { MAD_F(0x067261ff)  , 18 },
     { MAD_F(0x0672c226)  , 18 },
     { MAD_F(0x0673224e)  , 18 },
     { MAD_F(0x06738277)  , 18 },
     { MAD_F(0x0673e2a2)  , 18 },
     { MAD_F(0x067442ce)  , 18 },
     { MAD_F(0x0674a2fc)  , 18 },
     { MAD_F(0x0675032b)  , 18 },
     { MAD_F(0x0675635b)  , 18 },
     { MAD_F(0x0675c38d)  , 18 },
     { MAD_F(0x067623c0)  , 18 },
     { MAD_F(0x067683f4)  , 18 },
     { MAD_F(0x0676e42a)  , 18 },

     { MAD_F(0x06774462)  , 18 },
     { MAD_F(0x0677a49b)  , 18 },
     { MAD_F(0x067804d5)  , 18 },
     { MAD_F(0x06786510)  , 18 },
     { MAD_F(0x0678c54d)  , 18 },
     { MAD_F(0x0679258c)  , 18 },
     { MAD_F(0x067985cb)  , 18 },
     { MAD_F(0x0679e60c)  , 18 },
     { MAD_F(0x067a464f)  , 18 },
     { MAD_F(0x067aa693)  , 18 },
     { MAD_F(0x067b06d8)  , 18 },
     { MAD_F(0x067b671f)  , 18 },
     { MAD_F(0x067bc767)  , 18 },
     { MAD_F(0x067c27b1)  , 18 },
     { MAD_F(0x067c87fc)  , 18 },
     { MAD_F(0x067ce848)  , 18 },

     { MAD_F(0x067d4896)  , 18 },
     { MAD_F(0x067da8e5)  , 18 },
     { MAD_F(0x067e0935)  , 18 },
     { MAD_F(0x067e6987)  , 18 },
     { MAD_F(0x067ec9da)  , 18 },
     { MAD_F(0x067f2a2f)  , 18 },
     { MAD_F(0x067f8a85)  , 18 },
     { MAD_F(0x067feadd)  , 18 },
     { MAD_F(0x06804b36)  , 18 },
     { MAD_F(0x0680ab90)  , 18 },
     { MAD_F(0x06810beb)  , 18 },
     { MAD_F(0x06816c49)  , 18 },
     { MAD_F(0x0681cca7)  , 18 },
     { MAD_F(0x06822d07)  , 18 },
     { MAD_F(0x06828d68)  , 18 },
     { MAD_F(0x0682edcb)  , 18 },

     { MAD_F(0x06834e2f)  , 18 },
     { MAD_F(0x0683ae94)  , 18 },
     { MAD_F(0x06840efb)  , 18 },
     { MAD_F(0x06846f63)  , 18 },
     { MAD_F(0x0684cfcd)  , 18 },
     { MAD_F(0x06853038)  , 18 },
     { MAD_F(0x068590a4)  , 18 },
     { MAD_F(0x0685f112)  , 18 },
     { MAD_F(0x06865181)  , 18 },
     { MAD_F(0x0686b1f2)  , 18 },
     { MAD_F(0x06871264)  , 18 },
     { MAD_F(0x068772d7)  , 18 },
     { MAD_F(0x0687d34c)  , 18 },
     { MAD_F(0x068833c2)  , 18 },
     { MAD_F(0x06889439)  , 18 },
     { MAD_F(0x0688f4b2)  , 18 },

     { MAD_F(0x0689552c)  , 18 },
     { MAD_F(0x0689b5a8)  , 18 },
     { MAD_F(0x068a1625)  , 18 },
     { MAD_F(0x068a76a4)  , 18 },
     { MAD_F(0x068ad724)  , 18 },
     { MAD_F(0x068b37a5)  , 18 },
     { MAD_F(0x068b9827)  , 18 },
     { MAD_F(0x068bf8ac)  , 18 },
     { MAD_F(0x068c5931)  , 18 },
     { MAD_F(0x068cb9b8)  , 18 },
     { MAD_F(0x068d1a40)  , 18 },
     { MAD_F(0x068d7aca)  , 18 },
     { MAD_F(0x068ddb54)  , 18 },
     { MAD_F(0x068e3be1)  , 18 },
     { MAD_F(0x068e9c6f)  , 18 },
     { MAD_F(0x068efcfe)  , 18 },

     { MAD_F(0x068f5d8e)  , 18 },
     { MAD_F(0x068fbe20)  , 18 },
     { MAD_F(0x06901eb4)  , 18 },
     { MAD_F(0x06907f48)  , 18 },
     { MAD_F(0x0690dfde)  , 18 },
     { MAD_F(0x06914076)  , 18 },
     { MAD_F(0x0691a10f)  , 18 },
     { MAD_F(0x069201a9)  , 18 },
     { MAD_F(0x06926245)  , 18 },
     { MAD_F(0x0692c2e2)  , 18 },
     { MAD_F(0x06932380)  , 18 },
     { MAD_F(0x06938420)  , 18 },
     { MAD_F(0x0693e4c1)  , 18 },
     { MAD_F(0x06944563)  , 18 },
     { MAD_F(0x0694a607)  , 18 },
     { MAD_F(0x069506ad)  , 18 },

     { MAD_F(0x06956753)  , 18 },
     { MAD_F(0x0695c7fc)  , 18 },
     { MAD_F(0x069628a5)  , 18 },
     { MAD_F(0x06968950)  , 18 },
     { MAD_F(0x0696e9fc)  , 18 },
     { MAD_F(0x06974aaa)  , 18 },
     { MAD_F(0x0697ab59)  , 18 },
     { MAD_F(0x06980c09)  , 18 },
     { MAD_F(0x06986cbb)  , 18 },
     { MAD_F(0x0698cd6e)  , 18 },
     { MAD_F(0x06992e23)  , 18 },
     { MAD_F(0x06998ed9)  , 18 },
     { MAD_F(0x0699ef90)  , 18 },
     { MAD_F(0x069a5049)  , 18 },
     { MAD_F(0x069ab103)  , 18 },
     { MAD_F(0x069b11bf)  , 18 },

     { MAD_F(0x069b727b)  , 18 },
     { MAD_F(0x069bd33a)  , 18 },
     { MAD_F(0x069c33f9)  , 18 },
     { MAD_F(0x069c94ba)  , 18 },
     { MAD_F(0x069cf57d)  , 18 },
     { MAD_F(0x069d5641)  , 18 },
     { MAD_F(0x069db706)  , 18 },
     { MAD_F(0x069e17cc)  , 18 },
     { MAD_F(0x069e7894)  , 18 },
     { MAD_F(0x069ed95e)  , 18 },
     { MAD_F(0x069f3a28)  , 18 },
     { MAD_F(0x069f9af4)  , 18 },
     { MAD_F(0x069ffbc2)  , 18 },
     { MAD_F(0x06a05c91)  , 18 },
     { MAD_F(0x06a0bd61)  , 18 },
     { MAD_F(0x06a11e32)  , 18 },

     { MAD_F(0x06a17f05)  , 18 },
     { MAD_F(0x06a1dfda)  , 18 },
     { MAD_F(0x06a240b0)  , 18 },
     { MAD_F(0x06a2a187)  , 18 },
     { MAD_F(0x06a3025f)  , 18 },
     { MAD_F(0x06a36339)  , 18 },
     { MAD_F(0x06a3c414)  , 18 },
     { MAD_F(0x06a424f1)  , 18 },
     { MAD_F(0x06a485cf)  , 18 },
     { MAD_F(0x06a4e6ae)  , 18 },
     { MAD_F(0x06a5478f)  , 18 },
     { MAD_F(0x06a5a871)  , 18 },
     { MAD_F(0x06a60955)  , 18 },
     { MAD_F(0x06a66a3a)  , 18 },
     { MAD_F(0x06a6cb20)  , 18 },
     { MAD_F(0x06a72c08)  , 18 },

     { MAD_F(0x06a78cf1)  , 18 },
     { MAD_F(0x06a7eddb)  , 18 },
     { MAD_F(0x06a84ec7)  , 18 },
     { MAD_F(0x06a8afb4)  , 18 },
     { MAD_F(0x06a910a3)  , 18 },
     { MAD_F(0x06a97193)  , 18 },
     { MAD_F(0x06a9d284)  , 18 },
     { MAD_F(0x06aa3377)  , 18 },
     { MAD_F(0x06aa946b)  , 18 },
     { MAD_F(0x06aaf561)  , 18 },
     { MAD_F(0x06ab5657)  , 18 },
     { MAD_F(0x06abb750)  , 18 },
     { MAD_F(0x06ac1849)  , 18 },
     { MAD_F(0x06ac7944)  , 18 },
     { MAD_F(0x06acda41)  , 18 },
     { MAD_F(0x06ad3b3e)  , 18 },

     { MAD_F(0x06ad9c3d)  , 18 },
     { MAD_F(0x06adfd3e)  , 18 },
     { MAD_F(0x06ae5e40)  , 18 },
     { MAD_F(0x06aebf43)  , 18 },
     { MAD_F(0x06af2047)  , 18 },
     { MAD_F(0x06af814d)  , 18 },
     { MAD_F(0x06afe255)  , 18 },
     { MAD_F(0x06b0435e)  , 18 },
     { MAD_F(0x06b0a468)  , 18 },
     { MAD_F(0x06b10573)  , 18 },
     { MAD_F(0x06b16680)  , 18 },
     { MAD_F(0x06b1c78e)  , 18 },
     { MAD_F(0x06b2289e)  , 18 },
     { MAD_F(0x06b289af)  , 18 },
     { MAD_F(0x06b2eac1)  , 18 },
     { MAD_F(0x06b34bd5)  , 18 },

     { MAD_F(0x06b3acea)  , 18 },
     { MAD_F(0x06b40e00)  , 18 },
     { MAD_F(0x06b46f18)  , 18 },
     { MAD_F(0x06b4d031)  , 18 },
     { MAD_F(0x06b5314c)  , 18 },
     { MAD_F(0x06b59268)  , 18 },
     { MAD_F(0x06b5f385)  , 18 },
     { MAD_F(0x06b654a4)  , 18 },
     { MAD_F(0x06b6b5c4)  , 18 },
     { MAD_F(0x06b716e6)  , 18 },
     { MAD_F(0x06b77808)  , 18 },
     { MAD_F(0x06b7d92d)  , 18 },
     { MAD_F(0x06b83a52)  , 18 },
     { MAD_F(0x06b89b79)  , 18 },
     { MAD_F(0x06b8fca1)  , 18 },
     { MAD_F(0x06b95dcb)  , 18 },

     { MAD_F(0x06b9bef6)  , 18 },
     { MAD_F(0x06ba2023)  , 18 },
     { MAD_F(0x06ba8150)  , 18 },
     { MAD_F(0x06bae280)  , 18 },
     { MAD_F(0x06bb43b0)  , 18 },
     { MAD_F(0x06bba4e2)  , 18 },
     { MAD_F(0x06bc0615)  , 18 },
     { MAD_F(0x06bc674a)  , 18 },
     { MAD_F(0x06bcc880)  , 18 },
     { MAD_F(0x06bd29b7)  , 18 },
     { MAD_F(0x06bd8af0)  , 18 },
     { MAD_F(0x06bdec2a)  , 18 },
     { MAD_F(0x06be4d66)  , 18 },
     { MAD_F(0x06beaea3)  , 18 },
     { MAD_F(0x06bf0fe1)  , 18 },
     { MAD_F(0x06bf7120)  , 18 },

     { MAD_F(0x06bfd261)  , 18 },
     { MAD_F(0x06c033a4)  , 18 },
     { MAD_F(0x06c094e7)  , 18 },
     { MAD_F(0x06c0f62c)  , 18 },
     { MAD_F(0x06c15773)  , 18 },
     { MAD_F(0x06c1b8bb)  , 18 },
     { MAD_F(0x06c21a04)  , 18 },
     { MAD_F(0x06c27b4e)  , 18 },
     { MAD_F(0x06c2dc9a)  , 18 },
     { MAD_F(0x06c33de8)  , 18 },
     { MAD_F(0x06c39f36)  , 18 },
     { MAD_F(0x06c40086)  , 18 },
     { MAD_F(0x06c461d8)  , 18 },
     { MAD_F(0x06c4c32a)  , 18 },
     { MAD_F(0x06c5247f)  , 18 },
     { MAD_F(0x06c585d4)  , 18 },

     { MAD_F(0x06c5e72b)  , 18 },
     { MAD_F(0x06c64883)  , 18 },
     { MAD_F(0x06c6a9dd)  , 18 },
     { MAD_F(0x06c70b38)  , 18 },
     { MAD_F(0x06c76c94)  , 18 },
     { MAD_F(0x06c7cdf2)  , 18 },
     { MAD_F(0x06c82f51)  , 18 },
     { MAD_F(0x06c890b1)  , 18 },
     { MAD_F(0x06c8f213)  , 18 },
     { MAD_F(0x06c95376)  , 18 },
     { MAD_F(0x06c9b4da)  , 18 },
     { MAD_F(0x06ca1640)  , 18 },
     { MAD_F(0x06ca77a8)  , 18 },
     { MAD_F(0x06cad910)  , 18 },
     { MAD_F(0x06cb3a7a)  , 18 },
     { MAD_F(0x06cb9be5)  , 18 },

     { MAD_F(0x06cbfd52)  , 18 },
     { MAD_F(0x06cc5ec0)  , 18 },
     { MAD_F(0x06ccc030)  , 18 },
     { MAD_F(0x06cd21a0)  , 18 },
     { MAD_F(0x06cd8313)  , 18 },
     { MAD_F(0x06cde486)  , 18 },
     { MAD_F(0x06ce45fb)  , 18 },
     { MAD_F(0x06cea771)  , 18 },
     { MAD_F(0x06cf08e9)  , 18 },
     { MAD_F(0x06cf6a62)  , 18 },
     { MAD_F(0x06cfcbdc)  , 18 },
     { MAD_F(0x06d02d58)  , 18 },
     { MAD_F(0x06d08ed5)  , 18 },
     { MAD_F(0x06d0f053)  , 18 },
     { MAD_F(0x06d151d3)  , 18 },
     { MAD_F(0x06d1b354)  , 18 },

     { MAD_F(0x06d214d7)  , 18 },
     { MAD_F(0x06d2765a)  , 18 },
     { MAD_F(0x06d2d7e0)  , 18 },
     { MAD_F(0x06d33966)  , 18 },
     { MAD_F(0x06d39aee)  , 18 },
     { MAD_F(0x06d3fc77)  , 18 },
     { MAD_F(0x06d45e02)  , 18 },
     { MAD_F(0x06d4bf8e)  , 18 },
     { MAD_F(0x06d5211c)  , 18 },
     { MAD_F(0x06d582aa)  , 18 },
     { MAD_F(0x06d5e43a)  , 18 },
     { MAD_F(0x06d645cc)  , 18 },
     { MAD_F(0x06d6a75f)  , 18 },
     { MAD_F(0x06d708f3)  , 18 },
     { MAD_F(0x06d76a88)  , 18 },
     { MAD_F(0x06d7cc1f)  , 18 },

     { MAD_F(0x06d82db8)  , 18 },
     { MAD_F(0x06d88f51)  , 18 },
     { MAD_F(0x06d8f0ec)  , 18 },
     { MAD_F(0x06d95288)  , 18 },
     { MAD_F(0x06d9b426)  , 18 },
     { MAD_F(0x06da15c5)  , 18 },
     { MAD_F(0x06da7766)  , 18 },
     { MAD_F(0x06dad907)  , 18 },
     { MAD_F(0x06db3aaa)  , 18 },
     { MAD_F(0x06db9c4f)  , 18 },
     { MAD_F(0x06dbfdf5)  , 18 },
     { MAD_F(0x06dc5f9c)  , 18 },
     { MAD_F(0x06dcc145)  , 18 },
     { MAD_F(0x06dd22ee)  , 18 },
     { MAD_F(0x06dd849a)  , 18 },
     { MAD_F(0x06dde646)  , 18 },

     { MAD_F(0x06de47f4)  , 18 },
     { MAD_F(0x06dea9a4)  , 18 },
     { MAD_F(0x06df0b54)  , 18 },
     { MAD_F(0x06df6d06)  , 18 },
     { MAD_F(0x06dfceba)  , 18 },
     { MAD_F(0x06e0306f)  , 18 },
     { MAD_F(0x06e09225)  , 18 },
     { MAD_F(0x06e0f3dc)  , 18 },
     { MAD_F(0x06e15595)  , 18 },
     { MAD_F(0x06e1b74f)  , 18 },
     { MAD_F(0x06e2190b)  , 18 },
     { MAD_F(0x06e27ac8)  , 18 },
     { MAD_F(0x06e2dc86)  , 18 },
     { MAD_F(0x06e33e46)  , 18 },
     { MAD_F(0x06e3a007)  , 18 },
     { MAD_F(0x06e401c9)  , 18 },

     { MAD_F(0x06e4638d)  , 18 },
     { MAD_F(0x06e4c552)  , 18 },
     { MAD_F(0x06e52718)  , 18 },
     { MAD_F(0x06e588e0)  , 18 },
     { MAD_F(0x06e5eaa9)  , 18 },
     { MAD_F(0x06e64c73)  , 18 },
     { MAD_F(0x06e6ae3f)  , 18 },
     { MAD_F(0x06e7100c)  , 18 },
     { MAD_F(0x06e771db)  , 18 },
     { MAD_F(0x06e7d3ab)  , 18 },
     { MAD_F(0x06e8357c)  , 18 },
     { MAD_F(0x06e8974e)  , 18 },
     { MAD_F(0x06e8f922)  , 18 },
     { MAD_F(0x06e95af8)  , 18 },
     { MAD_F(0x06e9bcce)  , 18 },
     { MAD_F(0x06ea1ea6)  , 18 },

     { MAD_F(0x06ea807f)  , 18 },
     { MAD_F(0x06eae25a)  , 18 },
     { MAD_F(0x06eb4436)  , 18 },
     { MAD_F(0x06eba614)  , 18 },
     { MAD_F(0x06ec07f2)  , 18 },
     { MAD_F(0x06ec69d2)  , 18 },
     { MAD_F(0x06eccbb4)  , 18 },
     { MAD_F(0x06ed2d97)  , 18 },
     { MAD_F(0x06ed8f7b)  , 18 },
     { MAD_F(0x06edf160)  , 18 },
     { MAD_F(0x06ee5347)  , 18 },
     { MAD_F(0x06eeb52f)  , 18 },
     { MAD_F(0x06ef1719)  , 18 },
     { MAD_F(0x06ef7904)  , 18 },
     { MAD_F(0x06efdaf0)  , 18 },
     { MAD_F(0x06f03cde)  , 18 },

     { MAD_F(0x06f09ecc)  , 18 },
     { MAD_F(0x06f100bd)  , 18 },
     { MAD_F(0x06f162ae)  , 18 },
     { MAD_F(0x06f1c4a1)  , 18 },
     { MAD_F(0x06f22696)  , 18 },
     { MAD_F(0x06f2888b)  , 18 },
     { MAD_F(0x06f2ea82)  , 18 },
     { MAD_F(0x06f34c7b)  , 18 },
     { MAD_F(0x06f3ae75)  , 18 },
     { MAD_F(0x06f41070)  , 18 },
     { MAD_F(0x06f4726c)  , 18 },
     { MAD_F(0x06f4d46a)  , 18 },
     { MAD_F(0x06f53669)  , 18 },
     { MAD_F(0x06f59869)  , 18 },
     { MAD_F(0x06f5fa6b)  , 18 },
     { MAD_F(0x06f65c6e)  , 18 },

     { MAD_F(0x06f6be73)  , 18 },
     { MAD_F(0x06f72079)  , 18 },
     { MAD_F(0x06f78280)  , 18 },
     { MAD_F(0x06f7e489)  , 18 },
     { MAD_F(0x06f84693)  , 18 },
     { MAD_F(0x06f8a89e)  , 18 },
     { MAD_F(0x06f90aaa)  , 18 },
     { MAD_F(0x06f96cb8)  , 18 },
     { MAD_F(0x06f9cec8)  , 18 },
     { MAD_F(0x06fa30d8)  , 18 },
     { MAD_F(0x06fa92ea)  , 18 },
     { MAD_F(0x06faf4fe)  , 18 },
     { MAD_F(0x06fb5712)  , 18 },
     { MAD_F(0x06fbb928)  , 18 },
     { MAD_F(0x06fc1b40)  , 18 },
     { MAD_F(0x06fc7d58)  , 18 },

     { MAD_F(0x06fcdf72)  , 18 },
     { MAD_F(0x06fd418e)  , 18 },
     { MAD_F(0x06fda3ab)  , 18 },
     { MAD_F(0x06fe05c9)  , 18 },
     { MAD_F(0x06fe67e8)  , 18 },
     { MAD_F(0x06feca09)  , 18 },
     { MAD_F(0x06ff2c2b)  , 18 },
     { MAD_F(0x06ff8e4f)  , 18 },
     { MAD_F(0x06fff073)  , 18 },
     { MAD_F(0x0700529a)  , 18 },
     { MAD_F(0x0700b4c1)  , 18 },
     { MAD_F(0x070116ea)  , 18 },
     { MAD_F(0x07017914)  , 18 },
     { MAD_F(0x0701db40)  , 18 },
     { MAD_F(0x07023d6c)  , 18 },
     { MAD_F(0x07029f9b)  , 18 },

     { MAD_F(0x070301ca)  , 18 },
     { MAD_F(0x070363fb)  , 18 },
     { MAD_F(0x0703c62d)  , 18 },
     { MAD_F(0x07042861)  , 18 },
     { MAD_F(0x07048a96)  , 18 },
     { MAD_F(0x0704eccc)  , 18 },
     { MAD_F(0x07054f04)  , 18 },
     { MAD_F(0x0705b13d)  , 18 },
     { MAD_F(0x07061377)  , 18 },
     { MAD_F(0x070675b3)  , 18 },
     { MAD_F(0x0706d7f0)  , 18 },
     { MAD_F(0x07073a2e)  , 18 },
     { MAD_F(0x07079c6e)  , 18 },
     { MAD_F(0x0707feaf)  , 18 },
     { MAD_F(0x070860f1)  , 18 },
     { MAD_F(0x0708c335)  , 18 },

     { MAD_F(0x0709257a)  , 18 },
     { MAD_F(0x070987c0)  , 18 },
     { MAD_F(0x0709ea08)  , 18 },
     { MAD_F(0x070a4c51)  , 18 },
     { MAD_F(0x070aae9b)  , 18 },
     { MAD_F(0x070b10e7)  , 18 },
     { MAD_F(0x070b7334)  , 18 },
     { MAD_F(0x070bd583)  , 18 },
     { MAD_F(0x070c37d2)  , 18 },
     { MAD_F(0x070c9a23)  , 18 },
     { MAD_F(0x070cfc76)  , 18 },
     { MAD_F(0x070d5eca)  , 18 },
     { MAD_F(0x070dc11f)  , 18 },
     { MAD_F(0x070e2375)  , 18 },
     { MAD_F(0x070e85cd)  , 18 },
     { MAD_F(0x070ee826)  , 18 },

     { MAD_F(0x070f4a80)  , 18 },
     { MAD_F(0x070facdc)  , 18 },
     { MAD_F(0x07100f39)  , 18 },
     { MAD_F(0x07107198)  , 18 },
     { MAD_F(0x0710d3f8)  , 18 },
     { MAD_F(0x07113659)  , 18 },
     { MAD_F(0x071198bb)  , 18 },
     { MAD_F(0x0711fb1f)  , 18 },
     { MAD_F(0x07125d84)  , 18 },
     { MAD_F(0x0712bfeb)  , 18 },
     { MAD_F(0x07132253)  , 18 },
     { MAD_F(0x071384bc)  , 18 },
     { MAD_F(0x0713e726)  , 18 },
     { MAD_F(0x07144992)  , 18 },
     { MAD_F(0x0714abff)  , 18 },
     { MAD_F(0x07150e6e)  , 18 },

     { MAD_F(0x071570de)  , 18 },
     { MAD_F(0x0715d34f)  , 18 },
     { MAD_F(0x071635c1)  , 18 },
     { MAD_F(0x07169835)  , 18 },
     { MAD_F(0x0716faaa)  , 18 },
     { MAD_F(0x07175d21)  , 18 },
     { MAD_F(0x0717bf99)  , 18 },
     { MAD_F(0x07182212)  , 18 },
     { MAD_F(0x0718848d)  , 18 },
     { MAD_F(0x0718e709)  , 18 },
     { MAD_F(0x07194986)  , 18 },
     { MAD_F(0x0719ac04)  , 18 },
     { MAD_F(0x071a0e84)  , 18 },
     { MAD_F(0x071a7105)  , 18 },
     { MAD_F(0x071ad388)  , 18 },
     { MAD_F(0x071b360c)  , 18 },

     { MAD_F(0x071b9891)  , 18 },
     { MAD_F(0x071bfb18)  , 18 },
     { MAD_F(0x071c5d9f)  , 18 },
     { MAD_F(0x071cc029)  , 18 },
     { MAD_F(0x071d22b3)  , 18 },
     { MAD_F(0x071d853f)  , 18 },
     { MAD_F(0x071de7cc)  , 18 },
     { MAD_F(0x071e4a5b)  , 18 },
     { MAD_F(0x071eaceb)  , 18 },
     { MAD_F(0x071f0f7c)  , 18 },
     { MAD_F(0x071f720e)  , 18 },
     { MAD_F(0x071fd4a2)  , 18 },
     { MAD_F(0x07203737)  , 18 },
     { MAD_F(0x072099ce)  , 18 },
     { MAD_F(0x0720fc66)  , 18 },
     { MAD_F(0x07215eff)  , 18 },

     { MAD_F(0x0721c19a)  , 18 },
     { MAD_F(0x07222436)  , 18 },
     { MAD_F(0x072286d3)  , 18 },
     { MAD_F(0x0722e971)  , 18 },
     { MAD_F(0x07234c11)  , 18 },
     { MAD_F(0x0723aeb2)  , 18 },
     { MAD_F(0x07241155)  , 18 },
     { MAD_F(0x072473f9)  , 18 },
     { MAD_F(0x0724d69e)  , 18 },
     { MAD_F(0x07253944)  , 18 },
     { MAD_F(0x07259bec)  , 18 },
     { MAD_F(0x0725fe95)  , 18 },
     { MAD_F(0x07266140)  , 18 },
     { MAD_F(0x0726c3ec)  , 18 },
     { MAD_F(0x07272699)  , 18 },
     { MAD_F(0x07278947)  , 18 },

     { MAD_F(0x0727ebf7)  , 18 },
     { MAD_F(0x07284ea8)  , 18 },
     { MAD_F(0x0728b15b)  , 18 },
     { MAD_F(0x0729140f)  , 18 },
     { MAD_F(0x072976c4)  , 18 },
     { MAD_F(0x0729d97a)  , 18 },
     { MAD_F(0x072a3c32)  , 18 },
     { MAD_F(0x072a9eeb)  , 18 },
     { MAD_F(0x072b01a6)  , 18 },
     { MAD_F(0x072b6461)  , 18 },
     { MAD_F(0x072bc71e)  , 18 },
     { MAD_F(0x072c29dd)  , 18 },
     { MAD_F(0x072c8c9d)  , 18 },
     { MAD_F(0x072cef5e)  , 18 },
     { MAD_F(0x072d5220)  , 18 },
     { MAD_F(0x072db4e4)  , 18 },

     { MAD_F(0x072e17a9)  , 18 },
     { MAD_F(0x072e7a6f)  , 18 },
     { MAD_F(0x072edd37)  , 18 },
     { MAD_F(0x072f4000)  , 18 },
     { MAD_F(0x072fa2ca)  , 18 },
     { MAD_F(0x07300596)  , 18 },
     { MAD_F(0x07306863)  , 18 },
     { MAD_F(0x0730cb32)  , 18 },
     { MAD_F(0x07312e01)  , 18 },
     { MAD_F(0x073190d2)  , 18 },
     { MAD_F(0x0731f3a5)  , 18 },
     { MAD_F(0x07325678)  , 18 },
     { MAD_F(0x0732b94d)  , 18 },
     { MAD_F(0x07331c23)  , 18 },
     { MAD_F(0x07337efb)  , 18 },
     { MAD_F(0x0733e1d4)  , 18 },

     { MAD_F(0x073444ae)  , 18 },
     { MAD_F(0x0734a78a)  , 18 },
     { MAD_F(0x07350a67)  , 18 },
     { MAD_F(0x07356d45)  , 18 },
     { MAD_F(0x0735d025)  , 18 },
     { MAD_F(0x07363306)  , 18 },
     { MAD_F(0x073695e8)  , 18 },
     { MAD_F(0x0736f8cb)  , 18 },
     { MAD_F(0x07375bb0)  , 18 },
     { MAD_F(0x0737be96)  , 18 },
     { MAD_F(0x0738217e)  , 18 },
     { MAD_F(0x07388467)  , 18 },
     { MAD_F(0x0738e751)  , 18 },
     { MAD_F(0x07394a3d)  , 18 },
     { MAD_F(0x0739ad29)  , 18 },
     { MAD_F(0x073a1017)  , 18 },

     { MAD_F(0x073a7307)  , 18 },
     { MAD_F(0x073ad5f8)  , 18 },
     { MAD_F(0x073b38ea)  , 18 },
     { MAD_F(0x073b9bdd)  , 18 },
     { MAD_F(0x073bfed2)  , 18 },
     { MAD_F(0x073c61c8)  , 18 },
     { MAD_F(0x073cc4bf)  , 18 },
     { MAD_F(0x073d27b8)  , 18 },
     { MAD_F(0x073d8ab2)  , 18 },
     { MAD_F(0x073dedae)  , 18 },
     { MAD_F(0x073e50aa)  , 18 },
     { MAD_F(0x073eb3a8)  , 18 },
     { MAD_F(0x073f16a8)  , 18 },
     { MAD_F(0x073f79a8)  , 18 },
     { MAD_F(0x073fdcaa)  , 18 },
     { MAD_F(0x07403fad)  , 18 },

     { MAD_F(0x0740a2b2)  , 18 },
     { MAD_F(0x074105b8)  , 18 },
     { MAD_F(0x074168bf)  , 18 },
     { MAD_F(0x0741cbc8)  , 18 },
     { MAD_F(0x07422ed2)  , 18 },
     { MAD_F(0x074291dd)  , 18 },
     { MAD_F(0x0742f4e9)  , 18 },
     { MAD_F(0x074357f7)  , 18 },
     { MAD_F(0x0743bb06)  , 18 },
     { MAD_F(0x07441e17)  , 18 },
     { MAD_F(0x07448129)  , 18 },
     { MAD_F(0x0744e43c)  , 18 },
     { MAD_F(0x07454750)  , 18 },
     { MAD_F(0x0745aa66)  , 18 },
     { MAD_F(0x07460d7d)  , 18 },
     { MAD_F(0x07467095)  , 18 },

     { MAD_F(0x0746d3af)  , 18 },
     { MAD_F(0x074736ca)  , 18 },
     { MAD_F(0x074799e7)  , 18 },
     { MAD_F(0x0747fd04)  , 18 },
     { MAD_F(0x07486023)  , 18 },
     { MAD_F(0x0748c344)  , 18 },
     { MAD_F(0x07492665)  , 18 },
     { MAD_F(0x07498988)  , 18 },
     { MAD_F(0x0749ecac)  , 18 },
     { MAD_F(0x074a4fd2)  , 18 },
     { MAD_F(0x074ab2f9)  , 18 },
     { MAD_F(0x074b1621)  , 18 },
     { MAD_F(0x074b794b)  , 18 },
     { MAD_F(0x074bdc75)  , 18 },
     { MAD_F(0x074c3fa1)  , 18 },
     { MAD_F(0x074ca2cf)  , 18 },

     { MAD_F(0x074d05fe)  , 18 },
     { MAD_F(0x074d692e)  , 18 },
     { MAD_F(0x074dcc5f)  , 18 },
     { MAD_F(0x074e2f92)  , 18 },
     { MAD_F(0x074e92c6)  , 18 },
     { MAD_F(0x074ef5fb)  , 18 },
     { MAD_F(0x074f5932)  , 18 },
     { MAD_F(0x074fbc6a)  , 18 },
     { MAD_F(0x07501fa3)  , 18 },
     { MAD_F(0x075082de)  , 18 },
     { MAD_F(0x0750e61a)  , 18 },
     { MAD_F(0x07514957)  , 18 },
     { MAD_F(0x0751ac96)  , 18 },
     { MAD_F(0x07520fd6)  , 18 },
     { MAD_F(0x07527317)  , 18 },
     { MAD_F(0x0752d659)  , 18 },

     { MAD_F(0x0753399d)  , 18 },
     { MAD_F(0x07539ce2)  , 18 },
     { MAD_F(0x07540029)  , 18 },
     { MAD_F(0x07546371)  , 18 },
     { MAD_F(0x0754c6ba)  , 18 },
     { MAD_F(0x07552a04)  , 18 },
     { MAD_F(0x07558d50)  , 18 },
     { MAD_F(0x0755f09d)  , 18 },
     { MAD_F(0x075653eb)  , 18 },
     { MAD_F(0x0756b73b)  , 18 },
     { MAD_F(0x07571a8c)  , 18 },
     { MAD_F(0x07577dde)  , 18 },
     { MAD_F(0x0757e131)  , 18 },
     { MAD_F(0x07584486)  , 18 },
     { MAD_F(0x0758a7dd)  , 18 },
     { MAD_F(0x07590b34)  , 18 },

     { MAD_F(0x07596e8d)  , 18 },
     { MAD_F(0x0759d1e7)  , 18 },
     { MAD_F(0x075a3542)  , 18 },
     { MAD_F(0x075a989f)  , 18 },
     { MAD_F(0x075afbfd)  , 18 },
     { MAD_F(0x075b5f5d)  , 18 },
     { MAD_F(0x075bc2bd)  , 18 },
     { MAD_F(0x075c261f)  , 18 },
     { MAD_F(0x075c8983)  , 18 },
     { MAD_F(0x075cece7)  , 18 },
     { MAD_F(0x075d504d)  , 18 },
     { MAD_F(0x075db3b5)  , 18 },
     { MAD_F(0x075e171d)  , 18 },
     { MAD_F(0x075e7a87)  , 18 },
     { MAD_F(0x075eddf2)  , 18 },
     { MAD_F(0x075f415f)  , 18 },

     { MAD_F(0x075fa4cc)  , 18 },
     { MAD_F(0x0760083b)  , 18 },
     { MAD_F(0x07606bac)  , 18 },
     { MAD_F(0x0760cf1e)  , 18 },
     { MAD_F(0x07613291)  , 18 },
     { MAD_F(0x07619605)  , 18 },
     { MAD_F(0x0761f97b)  , 18 },
     { MAD_F(0x07625cf2)  , 18 },
     { MAD_F(0x0762c06a)  , 18 },
     { MAD_F(0x076323e3)  , 18 },
     { MAD_F(0x0763875e)  , 18 },
     { MAD_F(0x0763eadb)  , 18 },
     { MAD_F(0x07644e58)  , 18 },
     { MAD_F(0x0764b1d7)  , 18 },
     { MAD_F(0x07651557)  , 18 },
     { MAD_F(0x076578d8)  , 18 },

     { MAD_F(0x0765dc5b)  , 18 },
     { MAD_F(0x07663fdf)  , 18 },
     { MAD_F(0x0766a364)  , 18 },
     { MAD_F(0x076706eb)  , 18 },
     { MAD_F(0x07676a73)  , 18 },
     { MAD_F(0x0767cdfc)  , 18 },
     { MAD_F(0x07683187)  , 18 },
     { MAD_F(0x07689513)  , 18 },
     { MAD_F(0x0768f8a0)  , 18 },
     { MAD_F(0x07695c2e)  , 18 },
     { MAD_F(0x0769bfbe)  , 18 },
     { MAD_F(0x076a234f)  , 18 },
     { MAD_F(0x076a86e2)  , 18 },
     { MAD_F(0x076aea75)  , 18 },
     { MAD_F(0x076b4e0a)  , 18 },
     { MAD_F(0x076bb1a1)  , 18 },

     { MAD_F(0x076c1538)  , 18 },
     { MAD_F(0x076c78d1)  , 18 },
     { MAD_F(0x076cdc6c)  , 18 },
     { MAD_F(0x076d4007)  , 18 },
     { MAD_F(0x076da3a4)  , 18 },
     { MAD_F(0x076e0742)  , 18 },
     { MAD_F(0x076e6ae2)  , 18 },
     { MAD_F(0x076ece82)  , 18 },
     { MAD_F(0x076f3224)  , 18 },
     { MAD_F(0x076f95c8)  , 18 },
     { MAD_F(0x076ff96c)  , 18 },
     { MAD_F(0x07705d12)  , 18 },
     { MAD_F(0x0770c0ba)  , 18 },
     { MAD_F(0x07712462)  , 18 },
     { MAD_F(0x0771880c)  , 18 },
     { MAD_F(0x0771ebb7)  , 18 },

     { MAD_F(0x07724f64)  , 18 },
     { MAD_F(0x0772b312)  , 18 },
     { MAD_F(0x077316c1)  , 18 },
     { MAD_F(0x07737a71)  , 18 },
     { MAD_F(0x0773de23)  , 18 },
     { MAD_F(0x077441d6)  , 18 },
     { MAD_F(0x0774a58a)  , 18 },
     { MAD_F(0x07750940)  , 18 },
     { MAD_F(0x07756cf7)  , 18 },
     { MAD_F(0x0775d0af)  , 18 },
     { MAD_F(0x07763468)  , 18 },
     { MAD_F(0x07769823)  , 18 },
     { MAD_F(0x0776fbdf)  , 18 },
     { MAD_F(0x07775f9d)  , 18 },
     { MAD_F(0x0777c35c)  , 18 },
     { MAD_F(0x0778271c)  , 18 },

     { MAD_F(0x07788add)  , 18 },
     { MAD_F(0x0778ee9f)  , 18 },
     { MAD_F(0x07795263)  , 18 },
     { MAD_F(0x0779b629)  , 18 },
     { MAD_F(0x077a19ef)  , 18 },
     { MAD_F(0x077a7db7)  , 18 },
     { MAD_F(0x077ae180)  , 18 },
     { MAD_F(0x077b454b)  , 18 },
     { MAD_F(0x077ba916)  , 18 },
     { MAD_F(0x077c0ce3)  , 18 },
     { MAD_F(0x077c70b2)  , 18 },
     { MAD_F(0x077cd481)  , 18 },
     { MAD_F(0x077d3852)  , 18 },
     { MAD_F(0x077d9c24)  , 18 },
     { MAD_F(0x077dfff8)  , 18 },
     { MAD_F(0x077e63cd)  , 18 },

     { MAD_F(0x077ec7a3)  , 18 },
     { MAD_F(0x077f2b7a)  , 18 },
     { MAD_F(0x077f8f53)  , 18 },
     { MAD_F(0x077ff32d)  , 18 },
     { MAD_F(0x07805708)  , 18 },
     { MAD_F(0x0780bae5)  , 18 },
     { MAD_F(0x07811ec3)  , 18 },
     { MAD_F(0x078182a2)  , 18 },
     { MAD_F(0x0781e683)  , 18 },
     { MAD_F(0x07824a64)  , 18 },
     { MAD_F(0x0782ae47)  , 18 },
     { MAD_F(0x0783122c)  , 18 },
     { MAD_F(0x07837612)  , 18 },
     { MAD_F(0x0783d9f9)  , 18 },
     { MAD_F(0x07843de1)  , 18 },
     { MAD_F(0x0784a1ca)  , 18 },


     { MAD_F(0x078505b5)  , 18 },
     { MAD_F(0x078569a2)  , 18 },
     { MAD_F(0x0785cd8f)  , 18 },
     { MAD_F(0x0786317e)  , 18 },
     { MAD_F(0x0786956e)  , 18 },
     { MAD_F(0x0786f95f)  , 18 },
     { MAD_F(0x07875d52)  , 18 },
     { MAD_F(0x0787c146)  , 18 },
     { MAD_F(0x0788253b)  , 18 },
     { MAD_F(0x07888932)  , 18 },
     { MAD_F(0x0788ed2a)  , 18 },
     { MAD_F(0x07895123)  , 18 },
     { MAD_F(0x0789b51d)  , 18 },
     { MAD_F(0x078a1919)  , 18 },
     { MAD_F(0x078a7d16)  , 18 },
     { MAD_F(0x078ae114)  , 18 },

     { MAD_F(0x078b4514)  , 18 },
     { MAD_F(0x078ba915)  , 18 },
     { MAD_F(0x078c0d17)  , 18 },
     { MAD_F(0x078c711a)  , 18 },
     { MAD_F(0x078cd51f)  , 18 },
     { MAD_F(0x078d3925)  , 18 },
     { MAD_F(0x078d9d2d)  , 18 },
     { MAD_F(0x078e0135)  , 18 },
     { MAD_F(0x078e653f)  , 18 },
     { MAD_F(0x078ec94b)  , 18 },
     { MAD_F(0x078f2d57)  , 18 },
     { MAD_F(0x078f9165)  , 18 },
     { MAD_F(0x078ff574)  , 18 },
     { MAD_F(0x07905985)  , 18 },
     { MAD_F(0x0790bd96)  , 18 },
     { MAD_F(0x079121a9)  , 18 },

     { MAD_F(0x079185be)  , 18 },
     { MAD_F(0x0791e9d3)  , 18 },
     { MAD_F(0x07924dea)  , 18 },
     { MAD_F(0x0792b202)  , 18 },
     { MAD_F(0x0793161c)  , 18 },
     { MAD_F(0x07937a37)  , 18 },
     { MAD_F(0x0793de53)  , 18 },
     { MAD_F(0x07944270)  , 18 },
     { MAD_F(0x0794a68f)  , 18 },
     { MAD_F(0x07950aaf)  , 18 },
     { MAD_F(0x07956ed0)  , 18 },
     { MAD_F(0x0795d2f2)  , 18 },
     { MAD_F(0x07963716)  , 18 },
     { MAD_F(0x07969b3b)  , 18 },
     { MAD_F(0x0796ff62)  , 18 },
     { MAD_F(0x07976389)  , 18 },

     { MAD_F(0x0797c7b2)  , 18 },
     { MAD_F(0x07982bdd)  , 18 },
     { MAD_F(0x07989008)  , 18 },
     { MAD_F(0x0798f435)  , 18 },
     { MAD_F(0x07995863)  , 18 },
     { MAD_F(0x0799bc92)  , 18 },
     { MAD_F(0x079a20c3)  , 18 },
     { MAD_F(0x079a84f5)  , 18 },
     { MAD_F(0x079ae929)  , 18 },
     { MAD_F(0x079b4d5d)  , 18 },
     { MAD_F(0x079bb193)  , 18 },
     { MAD_F(0x079c15ca)  , 18 },
     { MAD_F(0x079c7a03)  , 18 },
     { MAD_F(0x079cde3c)  , 18 },
     { MAD_F(0x079d4277)  , 18 },
     { MAD_F(0x079da6b4)  , 18 },

     { MAD_F(0x079e0af1)  , 18 },
     { MAD_F(0x079e6f30)  , 18 },
     { MAD_F(0x079ed370)  , 18 },
     { MAD_F(0x079f37b2)  , 18 },
     { MAD_F(0x079f9bf5)  , 18 },
     { MAD_F(0x07a00039)  , 18 },
     { MAD_F(0x07a0647e)  , 18 },
     { MAD_F(0x07a0c8c5)  , 18 },
     { MAD_F(0x07a12d0c)  , 18 },
     { MAD_F(0x07a19156)  , 18 },
     { MAD_F(0x07a1f5a0)  , 18 },
     { MAD_F(0x07a259ec)  , 18 },
     { MAD_F(0x07a2be39)  , 18 },
     { MAD_F(0x07a32287)  , 18 },
     { MAD_F(0x07a386d7)  , 18 },
     { MAD_F(0x07a3eb28)  , 18 },

     { MAD_F(0x07a44f7a)  , 18 },
     { MAD_F(0x07a4b3ce)  , 18 },
     { MAD_F(0x07a51822)  , 18 },
     { MAD_F(0x07a57c78)  , 18 },
     { MAD_F(0x07a5e0d0)  , 18 },
     { MAD_F(0x07a64528)  , 18 },
     { MAD_F(0x07a6a982)  , 18 },
     { MAD_F(0x07a70ddd)  , 18 },
     { MAD_F(0x07a7723a)  , 18 },
     { MAD_F(0x07a7d698)  , 18 },
     { MAD_F(0x07a83af7)  , 18 },
     { MAD_F(0x07a89f57)  , 18 },
     { MAD_F(0x07a903b9)  , 18 },
     { MAD_F(0x07a9681c)  , 18 },
     { MAD_F(0x07a9cc80)  , 18 },
     { MAD_F(0x07aa30e5)  , 18 },

     { MAD_F(0x07aa954c)  , 18 },
     { MAD_F(0x07aaf9b4)  , 18 },
     { MAD_F(0x07ab5e1e)  , 18 },
     { MAD_F(0x07abc288)  , 18 },
     { MAD_F(0x07ac26f4)  , 18 },
     { MAD_F(0x07ac8b61)  , 18 },
     { MAD_F(0x07acefd0)  , 18 },
     { MAD_F(0x07ad543f)  , 18 },
     { MAD_F(0x07adb8b0)  , 18 },
     { MAD_F(0x07ae1d23)  , 18 },
     { MAD_F(0x07ae8196)  , 18 },
     { MAD_F(0x07aee60b)  , 18 },
     { MAD_F(0x07af4a81)  , 18 },
     { MAD_F(0x07afaef9)  , 18 },
     { MAD_F(0x07b01372)  , 18 },
     { MAD_F(0x07b077ec)  , 18 },

     { MAD_F(0x07b0dc67)  , 18 },
     { MAD_F(0x07b140e4)  , 18 },
     { MAD_F(0x07b1a561)  , 18 },
     { MAD_F(0x07b209e1)  , 18 },
     { MAD_F(0x07b26e61)  , 18 },
     { MAD_F(0x07b2d2e3)  , 18 },
     { MAD_F(0x07b33766)  , 18 },
     { MAD_F(0x07b39bea)  , 18 },
     { MAD_F(0x07b4006f)  , 18 },
     { MAD_F(0x07b464f6)  , 18 },
     { MAD_F(0x07b4c97e)  , 18 },
     { MAD_F(0x07b52e08)  , 18 },
     { MAD_F(0x07b59292)  , 18 },
     { MAD_F(0x07b5f71e)  , 18 },
     { MAD_F(0x07b65bac)  , 18 },
     { MAD_F(0x07b6c03a)  , 18 },

     { MAD_F(0x07b724ca)  , 18 },
     { MAD_F(0x07b7895b)  , 18 },
     { MAD_F(0x07b7eded)  , 18 },
     { MAD_F(0x07b85281)  , 18 },
     { MAD_F(0x07b8b716)  , 18 },
     { MAD_F(0x07b91bac)  , 18 },
     { MAD_F(0x07b98044)  , 18 },
     { MAD_F(0x07b9e4dc)  , 18 },
     { MAD_F(0x07ba4976)  , 18 },
     { MAD_F(0x07baae12)  , 18 },
     { MAD_F(0x07bb12ae)  , 18 },
     { MAD_F(0x07bb774c)  , 18 },
     { MAD_F(0x07bbdbeb)  , 18 },
     { MAD_F(0x07bc408c)  , 18 },
     { MAD_F(0x07bca52d)  , 18 },
     { MAD_F(0x07bd09d0)  , 18 },

     { MAD_F(0x07bd6e75)  , 18 },
     { MAD_F(0x07bdd31a)  , 18 },
     { MAD_F(0x07be37c1)  , 18 },
     { MAD_F(0x07be9c69)  , 18 },
     { MAD_F(0x07bf0113)  , 18 },
     { MAD_F(0x07bf65bd)  , 18 },
     { MAD_F(0x07bfca69)  , 18 },
     { MAD_F(0x07c02f16)  , 18 },
     { MAD_F(0x07c093c5)  , 18 },
     { MAD_F(0x07c0f875)  , 18 },
     { MAD_F(0x07c15d26)  , 18 },
     { MAD_F(0x07c1c1d8)  , 18 },
     { MAD_F(0x07c2268b)  , 18 },
     { MAD_F(0x07c28b40)  , 18 },
     { MAD_F(0x07c2eff6)  , 18 },
     { MAD_F(0x07c354ae)  , 18 },

     { MAD_F(0x07c3b967)  , 18 },
     { MAD_F(0x07c41e21)  , 18 },
     { MAD_F(0x07c482dc)  , 18 },
     { MAD_F(0x07c4e798)  , 18 },
     { MAD_F(0x07c54c56)  , 18 },
     { MAD_F(0x07c5b115)  , 18 },
     { MAD_F(0x07c615d6)  , 18 },
     { MAD_F(0x07c67a97)  , 18 },
     { MAD_F(0x07c6df5a)  , 18 },
     { MAD_F(0x07c7441e)  , 18 },
     { MAD_F(0x07c7a8e4)  , 18 },
     { MAD_F(0x07c80daa)  , 18 },
     { MAD_F(0x07c87272)  , 18 },
     { MAD_F(0x07c8d73c)  , 18 },
     { MAD_F(0x07c93c06)  , 18 },
     { MAD_F(0x07c9a0d2)  , 18 },

     { MAD_F(0x07ca059f)  , 18 },
     { MAD_F(0x07ca6a6d)  , 18 },
     { MAD_F(0x07cacf3d)  , 18 },
     { MAD_F(0x07cb340e)  , 18 },
     { MAD_F(0x07cb98e0)  , 18 },
     { MAD_F(0x07cbfdb4)  , 18 },
     { MAD_F(0x07cc6288)  , 18 },
     { MAD_F(0x07ccc75e)  , 18 },
     { MAD_F(0x07cd2c36)  , 18 },
     { MAD_F(0x07cd910e)  , 18 },
     { MAD_F(0x07cdf5e8)  , 18 },
     { MAD_F(0x07ce5ac3)  , 18 },
     { MAD_F(0x07cebfa0)  , 18 },
     { MAD_F(0x07cf247d)  , 18 },
     { MAD_F(0x07cf895c)  , 18 },
     { MAD_F(0x07cfee3c)  , 18 },

     { MAD_F(0x07d0531e)  , 18 },
     { MAD_F(0x07d0b801)  , 18 },
     { MAD_F(0x07d11ce5)  , 18 },
     { MAD_F(0x07d181ca)  , 18 },
     { MAD_F(0x07d1e6b0)  , 18 },
     { MAD_F(0x07d24b98)  , 18 },
     { MAD_F(0x07d2b081)  , 18 },
     { MAD_F(0x07d3156c)  , 18 },
     { MAD_F(0x07d37a57)  , 18 },
     { MAD_F(0x07d3df44)  , 18 },
     { MAD_F(0x07d44432)  , 18 },
     { MAD_F(0x07d4a922)  , 18 },
     { MAD_F(0x07d50e13)  , 18 },
     { MAD_F(0x07d57305)  , 18 },
     { MAD_F(0x07d5d7f8)  , 18 },
     { MAD_F(0x07d63cec)  , 18 },

     { MAD_F(0x07d6a1e2)  , 18 },
     { MAD_F(0x07d706d9)  , 18 },
     { MAD_F(0x07d76bd2)  , 18 },
     { MAD_F(0x07d7d0cb)  , 18 },
     { MAD_F(0x07d835c6)  , 18 },
     { MAD_F(0x07d89ac2)  , 18 },
     { MAD_F(0x07d8ffc0)  , 18 },
     { MAD_F(0x07d964be)  , 18 },
     { MAD_F(0x07d9c9be)  , 18 },
     { MAD_F(0x07da2ebf)  , 18 },
     { MAD_F(0x07da93c2)  , 18 },
     { MAD_F(0x07daf8c6)  , 18 },
     { MAD_F(0x07db5dcb)  , 18 },
     { MAD_F(0x07dbc2d1)  , 18 },
     { MAD_F(0x07dc27d9)  , 18 },
     { MAD_F(0x07dc8ce1)  , 18 },

     { MAD_F(0x07dcf1ec)  , 18 },
     { MAD_F(0x07dd56f7)  , 18 },
     { MAD_F(0x07ddbc04)  , 18 },
     { MAD_F(0x07de2111)  , 18 },
     { MAD_F(0x07de8621)  , 18 },
     { MAD_F(0x07deeb31)  , 18 },
     { MAD_F(0x07df5043)  , 18 },
     { MAD_F(0x07dfb556)  , 18 },
     { MAD_F(0x07e01a6a)  , 18 },
     { MAD_F(0x07e07f80)  , 18 },
     { MAD_F(0x07e0e496)  , 18 },
     { MAD_F(0x07e149ae)  , 18 },
     { MAD_F(0x07e1aec8)  , 18 },
     { MAD_F(0x07e213e2)  , 18 },
     { MAD_F(0x07e278fe)  , 18 },
     { MAD_F(0x07e2de1b)  , 18 },

     { MAD_F(0x07e3433a)  , 18 },
     { MAD_F(0x07e3a859)  , 18 },
     { MAD_F(0x07e40d7a)  , 18 },
     { MAD_F(0x07e4729c)  , 18 },
     { MAD_F(0x07e4d7c0)  , 18 },
     { MAD_F(0x07e53ce4)  , 18 },
     { MAD_F(0x07e5a20a)  , 18 },
     { MAD_F(0x07e60732)  , 18 },
     { MAD_F(0x07e66c5a)  , 18 },
     { MAD_F(0x07e6d184)  , 18 },
     { MAD_F(0x07e736af)  , 18 },
     { MAD_F(0x07e79bdb)  , 18 },
     { MAD_F(0x07e80109)  , 18 },
     { MAD_F(0x07e86638)  , 18 },
     { MAD_F(0x07e8cb68)  , 18 },
     { MAD_F(0x07e93099)  , 18 },

     { MAD_F(0x07e995cc)  , 18 },
     { MAD_F(0x07e9fb00)  , 18 },
     { MAD_F(0x07ea6035)  , 18 },
     { MAD_F(0x07eac56b)  , 18 },
     { MAD_F(0x07eb2aa3)  , 18 },
     { MAD_F(0x07eb8fdc)  , 18 },
     { MAD_F(0x07ebf516)  , 18 },
     { MAD_F(0x07ec5a51)  , 18 },
     { MAD_F(0x07ecbf8e)  , 18 },
     { MAD_F(0x07ed24cc)  , 18 },
     { MAD_F(0x07ed8a0b)  , 18 },
     { MAD_F(0x07edef4c)  , 18 },
     { MAD_F(0x07ee548e)  , 18 },
     { MAD_F(0x07eeb9d1)  , 18 },
     { MAD_F(0x07ef1f15)  , 18 },
     { MAD_F(0x07ef845b)  , 18 },

     { MAD_F(0x07efe9a1)  , 18 },
     { MAD_F(0x07f04ee9)  , 18 },
     { MAD_F(0x07f0b433)  , 18 },
     { MAD_F(0x07f1197d)  , 18 },
     { MAD_F(0x07f17ec9)  , 18 },
     { MAD_F(0x07f1e416)  , 18 },
     { MAD_F(0x07f24965)  , 18 },
     { MAD_F(0x07f2aeb5)  , 18 },
     { MAD_F(0x07f31405)  , 18 },
     { MAD_F(0x07f37958)  , 18 },
     { MAD_F(0x07f3deab)  , 18 },
     { MAD_F(0x07f44400)  , 18 },
     { MAD_F(0x07f4a956)  , 18 },
     { MAD_F(0x07f50ead)  , 18 },
     { MAD_F(0x07f57405)  , 18 },
     { MAD_F(0x07f5d95f)  , 18 },

     { MAD_F(0x07f63eba)  , 18 },
     { MAD_F(0x07f6a416)  , 18 },
     { MAD_F(0x07f70974)  , 18 },
     { MAD_F(0x07f76ed3)  , 18 },
     { MAD_F(0x07f7d433)  , 18 },
     { MAD_F(0x07f83994)  , 18 },
     { MAD_F(0x07f89ef7)  , 18 },
     { MAD_F(0x07f9045a)  , 18 },
     { MAD_F(0x07f969c0)  , 18 },
     { MAD_F(0x07f9cf26)  , 18 },
     { MAD_F(0x07fa348e)  , 18 },
     { MAD_F(0x07fa99f6)  , 18 },
     { MAD_F(0x07faff60)  , 18 },
     { MAD_F(0x07fb64cc)  , 18 },
     { MAD_F(0x07fbca38)  , 18 },
     { MAD_F(0x07fc2fa6)  , 18 },

     { MAD_F(0x07fc9516)  , 18 },
     { MAD_F(0x07fcfa86)  , 18 },
     { MAD_F(0x07fd5ff8)  , 18 },
     { MAD_F(0x07fdc56b)  , 18 },
     { MAD_F(0x07fe2adf)  , 18 },
     { MAD_F(0x07fe9054)  , 18 },
     { MAD_F(0x07fef5cb)  , 18 },
     { MAD_F(0x07ff5b43)  , 18 },
     { MAD_F(0x07ffc0bc)  , 18 },
     { MAD_F(0x0400131b)  , 19 },
     { MAD_F(0x040045d9)  , 19 },
     { MAD_F(0x04007897)  , 19 },
     { MAD_F(0x0400ab57)  , 19 },
     { MAD_F(0x0400de16)  , 19 },
     { MAD_F(0x040110d7)  , 19 },
     { MAD_F(0x04014398)  , 19 },

     { MAD_F(0x04017659)  , 19 },
     { MAD_F(0x0401a91c)  , 19 },
     { MAD_F(0x0401dbdf)  , 19 },
     { MAD_F(0x04020ea2)  , 19 },
     { MAD_F(0x04024166)  , 19 },
     { MAD_F(0x0402742b)  , 19 },
     { MAD_F(0x0402a6f0)  , 19 },
     { MAD_F(0x0402d9b6)  , 19 },
     { MAD_F(0x04030c7d)  , 19 },
     { MAD_F(0x04033f44)  , 19 },
     { MAD_F(0x0403720c)  , 19 },
     { MAD_F(0x0403a4d5)  , 19 },
     { MAD_F(0x0403d79e)  , 19 },
     { MAD_F(0x04040a68)  , 19 },
     { MAD_F(0x04043d32)  , 19 },
     { MAD_F(0x04046ffd)  , 19 },

     { MAD_F(0x0404a2c9)  , 19 },
     { MAD_F(0x0404d595)  , 19 },
     { MAD_F(0x04050862)  , 19 },
     { MAD_F(0x04053b30)  , 19 },
     { MAD_F(0x04056dfe)  , 19 },
     { MAD_F(0x0405a0cd)  , 19 },
     { MAD_F(0x0405d39c)  , 19 },
     { MAD_F(0x0406066c)  , 19 },
     { MAD_F(0x0406393d)  , 19 },
     { MAD_F(0x04066c0e)  , 19 },
     { MAD_F(0x04069ee0)  , 19 },
     { MAD_F(0x0406d1b3)  , 19 },
     { MAD_F(0x04070486)  , 19 },
     { MAD_F(0x0407375a)  , 19 },
     { MAD_F(0x04076a2e)  , 19 },
     { MAD_F(0x04079d03)  , 19 },

     { MAD_F(0x0407cfd9)  , 19 },
     { MAD_F(0x040802af)  , 19 },
     { MAD_F(0x04083586)  , 19 },
     { MAD_F(0x0408685e)  , 19 },
     { MAD_F(0x04089b36)  , 19 },
     { MAD_F(0x0408ce0f)  , 19 },
     { MAD_F(0x040900e8)  , 19 },
     { MAD_F(0x040933c2)  , 19 },
     { MAD_F(0x0409669d)  , 19 },
     { MAD_F(0x04099978)  , 19 },
     { MAD_F(0x0409cc54)  , 19 },
     { MAD_F(0x0409ff31)  , 19 },
     { MAD_F(0x040a320e)  , 19 },
     { MAD_F(0x040a64ec)  , 19 },
     { MAD_F(0x040a97cb)  , 19 },
     { MAD_F(0x040acaaa)  , 19 },

     { MAD_F(0x040afd89)  , 19 },
     { MAD_F(0x040b306a)  , 19 },
     { MAD_F(0x040b634b)  , 19 },
     { MAD_F(0x040b962c)  , 19 },
     { MAD_F(0x040bc90e)  , 19 },
     { MAD_F(0x040bfbf1)  , 19 },
     { MAD_F(0x040c2ed5)  , 19 },
     { MAD_F(0x040c61b9)  , 19 },
     { MAD_F(0x040c949e)  , 19 },
     { MAD_F(0x040cc783)  , 19 },
     { MAD_F(0x040cfa69)  , 19 },
     { MAD_F(0x040d2d4f)  , 19 },
     { MAD_F(0x040d6037)  , 19 },
     { MAD_F(0x040d931e)  , 19 },
     { MAD_F(0x040dc607)  , 19 },
     { MAD_F(0x040df8f0)  , 19 },

     { MAD_F(0x040e2bda)  , 19 },
     { MAD_F(0x040e5ec4)  , 19 },
     { MAD_F(0x040e91af)  , 19 },
     { MAD_F(0x040ec49b)  , 19 },
     { MAD_F(0x040ef787)  , 19 },
     { MAD_F(0x040f2a74)  , 19 },
     { MAD_F(0x040f5d61)  , 19 },
     { MAD_F(0x040f904f)  , 19 },
     { MAD_F(0x040fc33e)  , 19 },
     { MAD_F(0x040ff62d)  , 19 },
     { MAD_F(0x0410291d)  , 19 },
     { MAD_F(0x04105c0e)  , 19 },
     { MAD_F(0x04108eff)  , 19 },
     { MAD_F(0x0410c1f1)  , 19 },
     { MAD_F(0x0410f4e3)  , 19 },
     { MAD_F(0x041127d6)  , 19 },

     { MAD_F(0x04115aca)  , 19 },
     { MAD_F(0x04118dbe)  , 19 },
     { MAD_F(0x0411c0b3)  , 19 },
     { MAD_F(0x0411f3a9)  , 19 },
     { MAD_F(0x0412269f)  , 19 },
     { MAD_F(0x04125996)  , 19 },
     { MAD_F(0x04128c8d)  , 19 },
     { MAD_F(0x0412bf85)  , 19 },
     { MAD_F(0x0412f27e)  , 19 },
     { MAD_F(0x04132577)  , 19 },
     { MAD_F(0x04135871)  , 19 },
     { MAD_F(0x04138b6c)  , 19 },
     { MAD_F(0x0413be67)  , 19 },
     { MAD_F(0x0413f163)  , 19 },
     { MAD_F(0x0414245f)  , 19 },
     { MAD_F(0x0414575c)  , 19 },

     { MAD_F(0x04148a5a)  , 19 },
     { MAD_F(0x0414bd58)  , 19 },
     { MAD_F(0x0414f057)  , 19 },
     { MAD_F(0x04152356)  , 19 },
     { MAD_F(0x04155657)  , 19 },
     { MAD_F(0x04158957)  , 19 },
     { MAD_F(0x0415bc59)  , 19 },
     { MAD_F(0x0415ef5b)  , 19 },
     { MAD_F(0x0416225d)  , 19 },
     { MAD_F(0x04165561)  , 19 },
     { MAD_F(0x04168864)  , 19 },
     { MAD_F(0x0416bb69)  , 19 },
     { MAD_F(0x0416ee6e)  , 19 },
     { MAD_F(0x04172174)  , 19 },
     { MAD_F(0x0417547a)  , 19 },
     { MAD_F(0x04178781)  , 19 },

     { MAD_F(0x0417ba89)  , 19 },
     { MAD_F(0x0417ed91)  , 19 },
     { MAD_F(0x0418209a)  , 19 },
     { MAD_F(0x041853a3)  , 19 },
     { MAD_F(0x041886ad)  , 19 },
     { MAD_F(0x0418b9b8)  , 19 },
     { MAD_F(0x0418ecc3)  , 19 },
     { MAD_F(0x04191fcf)  , 19 },
     { MAD_F(0x041952dc)  , 19 },
     { MAD_F(0x041985e9)  , 19 },
     { MAD_F(0x0419b8f7)  , 19 },
     { MAD_F(0x0419ec05)  , 19 },
     { MAD_F(0x041a1f15)  , 19 },
     { MAD_F(0x041a5224)  , 19 },
     { MAD_F(0x041a8534)  , 19 },
     { MAD_F(0x041ab845)  , 19 },

     { MAD_F(0x041aeb57)  , 19 },
     { MAD_F(0x041b1e69)  , 19 },
     { MAD_F(0x041b517c)  , 19 },
     { MAD_F(0x041b848f)  , 19 },
     { MAD_F(0x041bb7a3)  , 19 },
     { MAD_F(0x041beab8)  , 19 },
     { MAD_F(0x041c1dcd)  , 19 },
     { MAD_F(0x041c50e3)  , 19 },
     { MAD_F(0x041c83fa)  , 19 },
     { MAD_F(0x041cb711)  , 19 },
     { MAD_F(0x041cea28)  , 19 },
     { MAD_F(0x041d1d41)  , 19 },
     { MAD_F(0x041d505a)  , 19 },
     { MAD_F(0x041d8373)  , 19 },
     { MAD_F(0x041db68e)  , 19 },
     { MAD_F(0x041de9a8)  , 19 },

     { MAD_F(0x041e1cc4)  , 19 },
     { MAD_F(0x041e4fe0)  , 19 },
     { MAD_F(0x041e82fd)  , 19 },
     { MAD_F(0x041eb61a)  , 19 },
     { MAD_F(0x041ee938)  , 19 },
     { MAD_F(0x041f1c57)  , 19 },
     { MAD_F(0x041f4f76)  , 19 },
     { MAD_F(0x041f8296)  , 19 },
     { MAD_F(0x041fb5b6)  , 19 },
     { MAD_F(0x041fe8d7)  , 19 },
     { MAD_F(0x04201bf9)  , 19 },
     { MAD_F(0x04204f1b)  , 19 },
     { MAD_F(0x0420823e)  , 19 },
     { MAD_F(0x0420b561)  , 19 },
     { MAD_F(0x0420e885)  , 19 },
     { MAD_F(0x04211baa)  , 19 },

     { MAD_F(0x04214ed0)  , 19 },
     { MAD_F(0x042181f6)  , 19 },
     { MAD_F(0x0421b51c)  , 19 },
     { MAD_F(0x0421e843)  , 19 },
     { MAD_F(0x04221b6b)  , 19 },
     { MAD_F(0x04224e94)  , 19 },
     { MAD_F(0x042281bd)  , 19 },
     { MAD_F(0x0422b4e6)  , 19 },
     { MAD_F(0x0422e811)  , 19 },
     { MAD_F(0x04231b3c)  , 19 },
     { MAD_F(0x04234e67)  , 19 },
     { MAD_F(0x04238193)  , 19 },
     { MAD_F(0x0423b4c0)  , 19 },
     { MAD_F(0x0423e7ee)  , 19 },
     { MAD_F(0x04241b1c)  , 19 },
     { MAD_F(0x04244e4a)  , 19 },

     { MAD_F(0x04248179)  , 19 },
     { MAD_F(0x0424b4a9)  , 19 },
     { MAD_F(0x0424e7da)  , 19 },
     { MAD_F(0x04251b0b)  , 19 },
     { MAD_F(0x04254e3d)  , 19 },
     { MAD_F(0x0425816f)  , 19 },
     { MAD_F(0x0425b4a2)  , 19 },
     { MAD_F(0x0425e7d6)  , 19 },
     { MAD_F(0x04261b0a)  , 19 },
     { MAD_F(0x04264e3f)  , 19 },
     { MAD_F(0x04268174)  , 19 },
     { MAD_F(0x0426b4aa)  , 19 },
     { MAD_F(0x0426e7e1)  , 19 },
     { MAD_F(0x04271b18)  , 19 },
     { MAD_F(0x04274e50)  , 19 },
     { MAD_F(0x04278188)  , 19 },

     { MAD_F(0x0427b4c2)  , 19 },
     { MAD_F(0x0427e7fb)  , 19 },
     { MAD_F(0x04281b36)  , 19 },
     { MAD_F(0x04284e71)  , 19 },
     { MAD_F(0x042881ac)  , 19 },
     { MAD_F(0x0428b4e8)  , 19 },
     { MAD_F(0x0428e825)  , 19 },
     { MAD_F(0x04291b63)  , 19 },
     { MAD_F(0x04294ea1)  , 19 },
     { MAD_F(0x042981df)  , 19 },
     { MAD_F(0x0429b51f)  , 19 },
     { MAD_F(0x0429e85f)  , 19 },
     { MAD_F(0x042a1b9f)  , 19 },
     { MAD_F(0x042a4ee0)  , 19 },
     { MAD_F(0x042a8222)  , 19 },
     { MAD_F(0x042ab564)  , 19 },

     { MAD_F(0x042ae8a7)  , 19 },
     { MAD_F(0x042b1beb)  , 19 },
     { MAD_F(0x042b4f2f)  , 19 },
     { MAD_F(0x042b8274)  , 19 },
     { MAD_F(0x042bb5ba)  , 19 },
     { MAD_F(0x042be900)  , 19 },
     { MAD_F(0x042c1c46)  , 19 },
     { MAD_F(0x042c4f8e)  , 19 },
     { MAD_F(0x042c82d6)  , 19 },
     { MAD_F(0x042cb61e)  , 19 },
     { MAD_F(0x042ce967)  , 19 },
     { MAD_F(0x042d1cb1)  , 19 },
     { MAD_F(0x042d4ffb)  , 19 },
     { MAD_F(0x042d8346)  , 19 },
     { MAD_F(0x042db692)  , 19 },
     { MAD_F(0x042de9de)  , 19 },

     { MAD_F(0x042e1d2b)  , 19 },
     { MAD_F(0x042e5078)  , 19 },
     { MAD_F(0x042e83c6)  , 19 },
     { MAD_F(0x042eb715)  , 19 },
     { MAD_F(0x042eea64)  , 19 },
     { MAD_F(0x042f1db4)  , 19 },
     { MAD_F(0x042f5105)  , 19 },
     { MAD_F(0x042f8456)  , 19 },
     { MAD_F(0x042fb7a8)  , 19 },
     { MAD_F(0x042feafa)  , 19 },
     { MAD_F(0x04301e4d)  , 19 },
     { MAD_F(0x043051a1)  , 19 },
     { MAD_F(0x043084f5)  , 19 },
     { MAD_F(0x0430b84a)  , 19 },
     { MAD_F(0x0430eb9f)  , 19 },
     { MAD_F(0x04311ef5)  , 19 },

     { MAD_F(0x0431524c)  , 19 },
     { MAD_F(0x043185a3)  , 19 },
     { MAD_F(0x0431b8fb)  , 19 },
     { MAD_F(0x0431ec54)  , 19 },
     { MAD_F(0x04321fad)  , 19 },
     { MAD_F(0x04325306)  , 19 },
     { MAD_F(0x04328661)  , 19 },
     { MAD_F(0x0432b9bc)  , 19 },
     { MAD_F(0x0432ed17)  , 19 },
     { MAD_F(0x04332074)  , 19 },
     { MAD_F(0x043353d0)  , 19 },
     { MAD_F(0x0433872e)  , 19 },
     { MAD_F(0x0433ba8c)  , 19 },
     { MAD_F(0x0433edea)  , 19 },
     { MAD_F(0x0434214a)  , 19 },
     { MAD_F(0x043454aa)  , 19 },

     { MAD_F(0x0434880a)  , 19 },
     { MAD_F(0x0434bb6b)  , 19 },
     { MAD_F(0x0434eecd)  , 19 },
     { MAD_F(0x0435222f)  , 19 },
     { MAD_F(0x04355592)  , 19 },
     { MAD_F(0x043588f6)  , 19 },
     { MAD_F(0x0435bc5a)  , 19 },
     { MAD_F(0x0435efbf)  , 19 },
     { MAD_F(0x04362324)  , 19 },
     { MAD_F(0x0436568a)  , 19 },
     { MAD_F(0x043689f1)  , 19 },
     { MAD_F(0x0436bd58)  , 19 },
     { MAD_F(0x0436f0c0)  , 19 },
     { MAD_F(0x04372428)  , 19 },
     { MAD_F(0x04375791)  , 19 },
     { MAD_F(0x04378afb)  , 19 },

     { MAD_F(0x0437be65)  , 19 },
     { MAD_F(0x0437f1d0)  , 19 },
     { MAD_F(0x0438253c)  , 19 },
     { MAD_F(0x043858a8)  , 19 },
     { MAD_F(0x04388c14)  , 19 },
     { MAD_F(0x0438bf82)  , 19 },
     { MAD_F(0x0438f2f0)  , 19 },
     { MAD_F(0x0439265e)  , 19 },
     { MAD_F(0x043959cd)  , 19 },
     { MAD_F(0x04398d3d)  , 19 },
     { MAD_F(0x0439c0ae)  , 19 },
     { MAD_F(0x0439f41f)  , 19 },
     { MAD_F(0x043a2790)  , 19 },
     { MAD_F(0x043a5b02)  , 19 },
     { MAD_F(0x043a8e75)  , 19 },
     { MAD_F(0x043ac1e9)  , 19 },

     { MAD_F(0x043af55d)  , 19 },
     { MAD_F(0x043b28d2)  , 19 },
     { MAD_F(0x043b5c47)  , 19 },
     { MAD_F(0x043b8fbd)  , 19 },
     { MAD_F(0x043bc333)  , 19 },
     { MAD_F(0x043bf6aa)  , 19 },
     { MAD_F(0x043c2a22)  , 19 },
     { MAD_F(0x043c5d9a)  , 19 },
     { MAD_F(0x043c9113)  , 19 },
     { MAD_F(0x043cc48d)  , 19 },
     { MAD_F(0x043cf807)  , 19 },
     { MAD_F(0x043d2b82)  , 19 },
     { MAD_F(0x043d5efd)  , 19 },
     { MAD_F(0x043d9279)  , 19 },
     { MAD_F(0x043dc5f6)  , 19 },
     { MAD_F(0x043df973)  , 19 },

     { MAD_F(0x043e2cf1)  , 19 },
     { MAD_F(0x043e6070)  , 19 },
     { MAD_F(0x043e93ef)  , 19 },
     { MAD_F(0x043ec76e)  , 19 },
     { MAD_F(0x043efaef)  , 19 },
     { MAD_F(0x043f2e6f)  , 19 },
     { MAD_F(0x043f61f1)  , 19 },
     { MAD_F(0x043f9573)  , 19 },
     { MAD_F(0x043fc8f6)  , 19 },
     { MAD_F(0x043ffc79)  , 19 },
     { MAD_F(0x04402ffd)  , 19 },
     { MAD_F(0x04406382)  , 19 },
     { MAD_F(0x04409707)  , 19 },
     { MAD_F(0x0440ca8d)  , 19 },
     { MAD_F(0x0440fe13)  , 19 },
     { MAD_F(0x0441319a)  , 19 },

     { MAD_F(0x04416522)  , 19 },
     { MAD_F(0x044198aa)  , 19 },
     { MAD_F(0x0441cc33)  , 19 },
     { MAD_F(0x0441ffbc)  , 19 },
     { MAD_F(0x04423346)  , 19 },
     { MAD_F(0x044266d1)  , 19 },
     { MAD_F(0x04429a5c)  , 19 },
     { MAD_F(0x0442cde8)  , 19 },
     { MAD_F(0x04430174)  , 19 },
     { MAD_F(0x04433501)  , 19 },
     { MAD_F(0x0443688f)  , 19 },
     { MAD_F(0x04439c1d)  , 19 },
     { MAD_F(0x0443cfac)  , 19 },
     { MAD_F(0x0444033c)  , 19 },
     { MAD_F(0x044436cc)  , 19 },
     { MAD_F(0x04446a5d)  , 19 },

     { MAD_F(0x04449dee)  , 19 },
     { MAD_F(0x0444d180)  , 19 },
     { MAD_F(0x04450513)  , 19 },
     { MAD_F(0x044538a6)  , 19 },
     { MAD_F(0x04456c39)  , 19 },
     { MAD_F(0x04459fce)  , 19 },
     { MAD_F(0x0445d363)  , 19 },
     { MAD_F(0x044606f8)  , 19 },
     { MAD_F(0x04463a8f)  , 19 },
     { MAD_F(0x04466e25)  , 19 },
     { MAD_F(0x0446a1bd)  , 19 },
     { MAD_F(0x0446d555)  , 19 },
     { MAD_F(0x044708ee)  , 19 },
     { MAD_F(0x04473c87)  , 19 },
     { MAD_F(0x04477021)  , 19 },
     { MAD_F(0x0447a3bb)  , 19 },

     { MAD_F(0x0447d756)  , 19 },
     { MAD_F(0x04480af2)  , 19 },
     { MAD_F(0x04483e8e)  , 19 },
     { MAD_F(0x0448722b)  , 19 },
     { MAD_F(0x0448a5c9)  , 19 },
     { MAD_F(0x0448d967)  , 19 },
     { MAD_F(0x04490d05)  , 19 },
     { MAD_F(0x044940a5)  , 19 },
     { MAD_F(0x04497445)  , 19 },
     { MAD_F(0x0449a7e5)  , 19 },
     { MAD_F(0x0449db86)  , 19 },
     { MAD_F(0x044a0f28)  , 19 },
     { MAD_F(0x044a42ca)  , 19 },
     { MAD_F(0x044a766d)  , 19 },
     { MAD_F(0x044aaa11)  , 19 },
     { MAD_F(0x044addb5)  , 19 },

     { MAD_F(0x044b115a)  , 19 },
     { MAD_F(0x044b44ff)  , 19 },
     { MAD_F(0x044b78a5)  , 19 },
     { MAD_F(0x044bac4c)  , 19 },
     { MAD_F(0x044bdff3)  , 19 },
     { MAD_F(0x044c139b)  , 19 },
     { MAD_F(0x044c4743)  , 19 },
     { MAD_F(0x044c7aec)  , 19 },
     { MAD_F(0x044cae96)  , 19 },
     { MAD_F(0x044ce240)  , 19 },
     { MAD_F(0x044d15eb)  , 19 },
     { MAD_F(0x044d4997)  , 19 },
     { MAD_F(0x044d7d43)  , 19 },
     { MAD_F(0x044db0ef)  , 19 },
     { MAD_F(0x044de49d)  , 19 },
     { MAD_F(0x044e184b)  , 19 },

     { MAD_F(0x044e4bf9)  , 19 },
     { MAD_F(0x044e7fa8)  , 19 },
     { MAD_F(0x044eb358)  , 19 },
     { MAD_F(0x044ee708)  , 19 },
     { MAD_F(0x044f1ab9)  , 19 },
     { MAD_F(0x044f4e6b)  , 19 },
     { MAD_F(0x044f821d)  , 19 },
     { MAD_F(0x044fb5cf)  , 19 },
     { MAD_F(0x044fe983)  , 19 },
     { MAD_F(0x04501d37)  , 19 },
     { MAD_F(0x045050eb)  , 19 },
     { MAD_F(0x045084a0)  , 19 },
     { MAD_F(0x0450b856)  , 19 },
     { MAD_F(0x0450ec0d)  , 19 },
     { MAD_F(0x04511fc4)  , 19 },
     { MAD_F(0x0451537b)  , 19 },

     { MAD_F(0x04518733)  , 19 },
     { MAD_F(0x0451baec)  , 19 },
     { MAD_F(0x0451eea5)  , 19 },
     { MAD_F(0x0452225f)  , 19 },
     { MAD_F(0x0452561a)  , 19 },
     { MAD_F(0x045289d5)  , 19 },
     { MAD_F(0x0452bd91)  , 19 },
     { MAD_F(0x0452f14d)  , 19 },
     { MAD_F(0x0453250a)  , 19 },
     { MAD_F(0x045358c8)  , 19 },
     { MAD_F(0x04538c86)  , 19 },
     { MAD_F(0x0453c045)  , 19 },
     { MAD_F(0x0453f405)  , 19 },
     { MAD_F(0x045427c5)  , 19 },
     { MAD_F(0x04545b85)  , 19 },
     { MAD_F(0x04548f46)  , 19 },

     { MAD_F(0x0454c308)  , 19 },
     { MAD_F(0x0454f6cb)  , 19 },
     { MAD_F(0x04552a8e)  , 19 },
     { MAD_F(0x04555e51)  , 19 },
     { MAD_F(0x04559216)  , 19 },
     { MAD_F(0x0455c5db)  , 19 },
     { MAD_F(0x0455f9a0)  , 19 },
     { MAD_F(0x04562d66)  , 19 },
     { MAD_F(0x0456612d)  , 19 },
     { MAD_F(0x045694f4)  , 19 },
     { MAD_F(0x0456c8bc)  , 19 },
     { MAD_F(0x0456fc84)  , 19 },
     { MAD_F(0x0457304e)  , 19 },
     { MAD_F(0x04576417)  , 19 },
     { MAD_F(0x045797e2)  , 19 },
     { MAD_F(0x0457cbac)  , 19 },

     { MAD_F(0x0457ff78)  , 19 },
     { MAD_F(0x04583344)  , 19 },
     { MAD_F(0x04586711)  , 19 },
     { MAD_F(0x04589ade)  , 19 },
     { MAD_F(0x0458ceac)  , 19 },
     { MAD_F(0x0459027b)  , 19 },
     { MAD_F(0x0459364a)  , 19 },
     { MAD_F(0x04596a19)  , 19 },
     { MAD_F(0x04599dea)  , 19 },
     { MAD_F(0x0459d1bb)  , 19 },
     { MAD_F(0x045a058c)  , 19 },
     { MAD_F(0x045a395e)  , 19 },
     { MAD_F(0x045a6d31)  , 19 },
     { MAD_F(0x045aa104)  , 19 },
     { MAD_F(0x045ad4d8)  , 19 },
     { MAD_F(0x045b08ad)  , 19 },

     { MAD_F(0x045b3c82)  , 19 },
     { MAD_F(0x045b7058)  , 19 },
     { MAD_F(0x045ba42e)  , 19 },
     { MAD_F(0x045bd805)  , 19 },
     { MAD_F(0x045c0bdd)  , 19 },
     { MAD_F(0x045c3fb5)  , 19 },
     { MAD_F(0x045c738e)  , 19 },
     { MAD_F(0x045ca767)  , 19 },
     { MAD_F(0x045cdb41)  , 19 },
     { MAD_F(0x045d0f1b)  , 19 },
     { MAD_F(0x045d42f7)  , 19 },
     { MAD_F(0x045d76d2)  , 19 },
     { MAD_F(0x045daaaf)  , 19 },
     { MAD_F(0x045dde8c)  , 19 },
     { MAD_F(0x045e1269)  , 19 },
     { MAD_F(0x045e4647)  , 19 },

     { MAD_F(0x045e7a26)  , 19 },
     { MAD_F(0x045eae06)  , 19 },
     { MAD_F(0x045ee1e6)  , 19 },
     { MAD_F(0x045f15c6)  , 19 },
     { MAD_F(0x045f49a7)  , 19 },
     { MAD_F(0x045f7d89)  , 19 },
     { MAD_F(0x045fb16c)  , 19 },
     { MAD_F(0x045fe54f)  , 19 },
     { MAD_F(0x04601932)  , 19 },
     { MAD_F(0x04604d16)  , 19 },
     { MAD_F(0x046080fb)  , 19 },
     { MAD_F(0x0460b4e1)  , 19 },
     { MAD_F(0x0460e8c7)  , 19 },
     { MAD_F(0x04611cad)  , 19 },
     { MAD_F(0x04615094)  , 19 },
     { MAD_F(0x0461847c)  , 19 },

     { MAD_F(0x0461b864)  , 19 },
     { MAD_F(0x0461ec4d)  , 19 },
     { MAD_F(0x04622037)  , 19 },
     { MAD_F(0x04625421)  , 19 },
     { MAD_F(0x0462880c)  , 19 },
     { MAD_F(0x0462bbf7)  , 19 },
     { MAD_F(0x0462efe3)  , 19 },
     { MAD_F(0x046323d0)  , 19 },
     { MAD_F(0x046357bd)  , 19 },
     { MAD_F(0x04638bab)  , 19 },
     { MAD_F(0x0463bf99)  , 19 },
     { MAD_F(0x0463f388)  , 19 },
     { MAD_F(0x04642778)  , 19 },
     { MAD_F(0x04645b68)  , 19 },
     { MAD_F(0x04648f59)  , 19 },
     { MAD_F(0x0464c34a)  , 19 },

     { MAD_F(0x0464f73c)  , 19 },
     { MAD_F(0x04652b2f)  , 19 },
     { MAD_F(0x04655f22)  , 19 },
     { MAD_F(0x04659316)  , 19 },
     { MAD_F(0x0465c70a)  , 19 },
     { MAD_F(0x0465faff)  , 19 },
     { MAD_F(0x04662ef5)  , 19 },
     { MAD_F(0x046662eb)  , 19 },
     { MAD_F(0x046696e2)  , 19 },
     { MAD_F(0x0466cad9)  , 19 },
     { MAD_F(0x0466fed1)  , 19 },
     { MAD_F(0x046732ca)  , 19 },
     { MAD_F(0x046766c3)  , 19 },
     { MAD_F(0x04679abd)  , 19 },
     { MAD_F(0x0467ceb7)  , 19 },
     { MAD_F(0x046802b2)  , 19 },

     { MAD_F(0x046836ae)  , 19 },
     { MAD_F(0x04686aaa)  , 19 },
     { MAD_F(0x04689ea7)  , 19 },
     { MAD_F(0x0468d2a4)  , 19 },
     { MAD_F(0x046906a2)  , 19 },
     { MAD_F(0x04693aa1)  , 19 },
     { MAD_F(0x04696ea0)  , 19 },
     { MAD_F(0x0469a2a0)  , 19 },
     { MAD_F(0x0469d6a0)  , 19 },
     { MAD_F(0x046a0aa1)  , 19 },
     { MAD_F(0x046a3ea3)  , 19 },
     { MAD_F(0x046a72a5)  , 19 },
     { MAD_F(0x046aa6a8)  , 19 },
     { MAD_F(0x046adaab)  , 19 },
     { MAD_F(0x046b0eaf)  , 19 },
     { MAD_F(0x046b42b3)  , 19 },

     { MAD_F(0x046b76b9)  , 19 },
     { MAD_F(0x046baabe)  , 19 },
     { MAD_F(0x046bdec5)  , 19 },
     { MAD_F(0x046c12cc)  , 19 },
     { MAD_F(0x046c46d3)  , 19 },
     { MAD_F(0x046c7adb)  , 19 },
     { MAD_F(0x046caee4)  , 19 },
     { MAD_F(0x046ce2ee)  , 19 },
     { MAD_F(0x046d16f7)  , 19 },
     { MAD_F(0x046d4b02)  , 19 },
     { MAD_F(0x046d7f0d)  , 19 },
     { MAD_F(0x046db319)  , 19 },
     { MAD_F(0x046de725)  , 19 },
     { MAD_F(0x046e1b32)  , 19 },
     { MAD_F(0x046e4f40)  , 19 },
     { MAD_F(0x046e834e)  , 19 },

     { MAD_F(0x046eb75c)  , 19 },
     { MAD_F(0x046eeb6c)  , 19 },
     { MAD_F(0x046f1f7c)  , 19 },
     { MAD_F(0x046f538c)  , 19 },
     { MAD_F(0x046f879d)  , 19 },
     { MAD_F(0x046fbbaf)  , 19 },
     { MAD_F(0x046fefc1)  , 19 },
     { MAD_F(0x047023d4)  , 19 },
     { MAD_F(0x047057e8)  , 19 },
     { MAD_F(0x04708bfc)  , 19 },
     { MAD_F(0x0470c011)  , 19 },
     { MAD_F(0x0470f426)  , 19 },
     { MAD_F(0x0471283c)  , 19 },
     { MAD_F(0x04715c52)  , 19 },
     { MAD_F(0x04719069)  , 19 },
     { MAD_F(0x0471c481)  , 19 },

     { MAD_F(0x0471f899)  , 19 },
     { MAD_F(0x04722cb2)  , 19 },
     { MAD_F(0x047260cc)  , 19 },
     { MAD_F(0x047294e6)  , 19 },
     { MAD_F(0x0472c900)  , 19 },
     { MAD_F(0x0472fd1b)  , 19 },
     { MAD_F(0x04733137)  , 19 },
     { MAD_F(0x04736554)  , 19 },
     { MAD_F(0x04739971)  , 19 },
     { MAD_F(0x0473cd8e)  , 19 },
     { MAD_F(0x047401ad)  , 19 },
     { MAD_F(0x047435cb)  , 19 },
     { MAD_F(0x047469eb)  , 19 },
     { MAD_F(0x04749e0b)  , 19 },
     { MAD_F(0x0474d22c)  , 19 },
     { MAD_F(0x0475064d)  , 19 },

     { MAD_F(0x04753a6f)  , 19 },
     { MAD_F(0x04756e91)  , 19 },
     { MAD_F(0x0475a2b4)  , 19 },
     { MAD_F(0x0475d6d7)  , 19 },
     { MAD_F(0x04760afc)  , 19 },
     { MAD_F(0x04763f20)  , 19 },
     { MAD_F(0x04767346)  , 19 },
     { MAD_F(0x0476a76c)  , 19 },
     { MAD_F(0x0476db92)  , 19 },
     { MAD_F(0x04770fba)  , 19 },
     { MAD_F(0x047743e1)  , 19 },
     { MAD_F(0x0477780a)  , 19 },
     { MAD_F(0x0477ac33)  , 19 },
     { MAD_F(0x0477e05c)  , 19 },
     { MAD_F(0x04781486)  , 19 },
     { MAD_F(0x047848b1)  , 19 },

     { MAD_F(0x04787cdc)  , 19 },
     { MAD_F(0x0478b108)  , 19 },
     { MAD_F(0x0478e535)  , 19 },
     { MAD_F(0x04791962)  , 19 },
     { MAD_F(0x04794d8f)  , 19 },
     { MAD_F(0x047981be)  , 19 },
     { MAD_F(0x0479b5ed)  , 19 },
     { MAD_F(0x0479ea1c)  , 19 },
     { MAD_F(0x047a1e4c)  , 19 },
     { MAD_F(0x047a527d)  , 19 },
     { MAD_F(0x047a86ae)  , 19 },
     { MAD_F(0x047abae0)  , 19 },
     { MAD_F(0x047aef12)  , 19 },
     { MAD_F(0x047b2346)  , 19 },
     { MAD_F(0x047b5779)  , 19 },
     { MAD_F(0x047b8bad)  , 19 },

     { MAD_F(0x047bbfe2)  , 19 },
     { MAD_F(0x047bf418)  , 19 },
     { MAD_F(0x047c284e)  , 19 },
     { MAD_F(0x047c5c84)  , 19 },
     { MAD_F(0x047c90bb)  , 19 },
     { MAD_F(0x047cc4f3)  , 19 },
     { MAD_F(0x047cf92c)  , 19 },
     { MAD_F(0x047d2d65)  , 19 },
     { MAD_F(0x047d619e)  , 19 },
     { MAD_F(0x047d95d8)  , 19 },
     { MAD_F(0x047dca13)  , 19 },
     { MAD_F(0x047dfe4e)  , 19 },
     { MAD_F(0x047e328a)  , 19 },
     { MAD_F(0x047e66c7)  , 19 },
     { MAD_F(0x047e9b04)  , 19 },
     { MAD_F(0x047ecf42)  , 19 },

     { MAD_F(0x047f0380)  , 19 },
     { MAD_F(0x047f37bf)  , 19 },
     { MAD_F(0x047f6bff)  , 19 },
     { MAD_F(0x047fa03f)  , 19 },
     { MAD_F(0x047fd47f)  , 19 },
     { MAD_F(0x048008c1)  , 19 },
     { MAD_F(0x04803d02)  , 19 },
     { MAD_F(0x04807145)  , 19 },
     { MAD_F(0x0480a588)  , 19 },
     { MAD_F(0x0480d9cc)  , 19 },
     { MAD_F(0x04810e10)  , 19 },
     { MAD_F(0x04814255)  , 19 },
     { MAD_F(0x0481769a)  , 19 },
     { MAD_F(0x0481aae0)  , 19 },
     { MAD_F(0x0481df27)  , 19 },
     { MAD_F(0x0482136e)  , 19 },

     { MAD_F(0x048247b6)  , 19 },
     { MAD_F(0x04827bfe)  , 19 },
     { MAD_F(0x0482b047)  , 19 },
     { MAD_F(0x0482e491)  , 19 },
     { MAD_F(0x048318db)  , 19 },
     { MAD_F(0x04834d26)  , 19 },
     { MAD_F(0x04838171)  , 19 },
     { MAD_F(0x0483b5bd)  , 19 },
     { MAD_F(0x0483ea0a)  , 19 },
     { MAD_F(0x04841e57)  , 19 },
     { MAD_F(0x048452a4)  , 19 },
     { MAD_F(0x048486f3)  , 19 },
     { MAD_F(0x0484bb42)  , 19 },
     { MAD_F(0x0484ef91)  , 19 },
     { MAD_F(0x048523e1)  , 19 },
     { MAD_F(0x04855832)  , 19 },

     { MAD_F(0x04858c83)  , 19 },
     { MAD_F(0x0485c0d5)  , 19 },
     { MAD_F(0x0485f527)  , 19 },
     { MAD_F(0x0486297a)  , 19 },
     { MAD_F(0x04865dce)  , 19 },
     { MAD_F(0x04869222)  , 19 },
     { MAD_F(0x0486c677)  , 19 },
     { MAD_F(0x0486facc)  , 19 },
     { MAD_F(0x04872f22)  , 19 },
     { MAD_F(0x04876379)  , 19 },
     { MAD_F(0x048797d0)  , 19 },
     { MAD_F(0x0487cc28)  , 19 },
     { MAD_F(0x04880080)  , 19 },
     { MAD_F(0x048834d9)  , 19 },
     { MAD_F(0x04886933)  , 19 },
     { MAD_F(0x04889d8d)  , 19 },

     { MAD_F(0x0488d1e8)  , 19 },
     { MAD_F(0x04890643)  , 19 },
     { MAD_F(0x04893a9f)  , 19 },
     { MAD_F(0x04896efb)  , 19 },
     { MAD_F(0x0489a358)  , 19 },
     { MAD_F(0x0489d7b6)  , 19 },
     { MAD_F(0x048a0c14)  , 19 },
     { MAD_F(0x048a4073)  , 19 },
     { MAD_F(0x048a74d3)  , 19 },
     { MAD_F(0x048aa933)  , 19 },
     { MAD_F(0x048add93)  , 19 },
     { MAD_F(0x048b11f5)  , 19 },
     { MAD_F(0x048b4656)  , 19 },
     { MAD_F(0x048b7ab9)  , 19 },
     { MAD_F(0x048baf1c)  , 19 },
     { MAD_F(0x048be37f)  , 19 },

     { MAD_F(0x048c17e3)  , 19 },
     { MAD_F(0x048c4c48)  , 19 },
     { MAD_F(0x048c80ad)  , 19 },
     { MAD_F(0x048cb513)  , 19 },
     { MAD_F(0x048ce97a)  , 19 },
     { MAD_F(0x048d1de1)  , 19 },
     { MAD_F(0x048d5249)  , 19 },
     { MAD_F(0x048d86b1)  , 19 },
     { MAD_F(0x048dbb1a)  , 19 },
     { MAD_F(0x048def83)  , 19 },
     { MAD_F(0x048e23ed)  , 19 },
     { MAD_F(0x048e5858)  , 19 },
     { MAD_F(0x048e8cc3)  , 19 },
     { MAD_F(0x048ec12f)  , 19 },
     { MAD_F(0x048ef59b)  , 19 },
     { MAD_F(0x048f2a08)  , 19 },

     { MAD_F(0x048f5e76)  , 19 },
     { MAD_F(0x048f92e4)  , 19 },
     { MAD_F(0x048fc753)  , 19 },
     { MAD_F(0x048ffbc2)  , 19 },
     { MAD_F(0x04903032)  , 19 },
     { MAD_F(0x049064a3)  , 19 },
     { MAD_F(0x04909914)  , 19 },
     { MAD_F(0x0490cd86)  , 19 },
     { MAD_F(0x049101f8)  , 19 },
     { MAD_F(0x0491366b)  , 19 },
     { MAD_F(0x04916ade)  , 19 },
     { MAD_F(0x04919f52)  , 19 },
     { MAD_F(0x0491d3c7)  , 19 },
     { MAD_F(0x0492083c)  , 19 },
     { MAD_F(0x04923cb2)  , 19 },
     { MAD_F(0x04927128)  , 19 },

     { MAD_F(0x0492a59f)  , 19 },
     { MAD_F(0x0492da17)  , 19 },
     { MAD_F(0x04930e8f)  , 19 },
     { MAD_F(0x04934308)  , 19 },
     { MAD_F(0x04937781)  , 19 },
     { MAD_F(0x0493abfb)  , 19 },
     { MAD_F(0x0493e076)  , 19 },
     { MAD_F(0x049414f1)  , 19 },
     { MAD_F(0x0494496c)  , 19 },
     { MAD_F(0x04947de9)  , 19 },
     { MAD_F(0x0494b266)  , 19 },
     { MAD_F(0x0494e6e3)  , 19 },
     { MAD_F(0x04951b61)  , 19 },
     { MAD_F(0x04954fe0)  , 19 },
     { MAD_F(0x0495845f)  , 19 },
     { MAD_F(0x0495b8df)  , 19 },

     { MAD_F(0x0495ed5f)  , 19 },
     { MAD_F(0x049621e0)  , 19 },
     { MAD_F(0x04965662)  , 19 },
     { MAD_F(0x04968ae4)  , 19 },
     { MAD_F(0x0496bf67)  , 19 },
     { MAD_F(0x0496f3ea)  , 19 },
     { MAD_F(0x0497286e)  , 19 },
     { MAD_F(0x04975cf2)  , 19 },
     { MAD_F(0x04979177)  , 19 },
     { MAD_F(0x0497c5fd)  , 19 },
     { MAD_F(0x0497fa83)  , 19 },
     { MAD_F(0x04982f0a)  , 19 },
     { MAD_F(0x04986392)  , 19 },
     { MAD_F(0x0498981a)  , 19 },
     { MAD_F(0x0498cca2)  , 19 },
     { MAD_F(0x0499012c)  , 19 },

     { MAD_F(0x049935b5)  , 19 },
     { MAD_F(0x04996a40)  , 19 },
     { MAD_F(0x04999ecb)  , 19 },
     { MAD_F(0x0499d356)  , 19 },
     { MAD_F(0x049a07e2)  , 19 },
     { MAD_F(0x049a3c6f)  , 19 },
     { MAD_F(0x049a70fc)  , 19 },
     { MAD_F(0x049aa58a)  , 19 },
     { MAD_F(0x049ada19)  , 19 },
     { MAD_F(0x049b0ea8)  , 19 },
     { MAD_F(0x049b4337)  , 19 },
     { MAD_F(0x049b77c8)  , 19 },
     { MAD_F(0x049bac58)  , 19 },
     { MAD_F(0x049be0ea)  , 19 },
     { MAD_F(0x049c157c)  , 19 },
     { MAD_F(0x049c4a0e)  , 19 },

     { MAD_F(0x049c7ea1)  , 19 },
     { MAD_F(0x049cb335)  , 19 },
     { MAD_F(0x049ce7ca)  , 19 },
     { MAD_F(0x049d1c5e)  , 19 },
     { MAD_F(0x049d50f4)  , 19 },
     { MAD_F(0x049d858a)  , 19 },
     { MAD_F(0x049dba21)  , 19 },
     { MAD_F(0x049deeb8)  , 19 },
     { MAD_F(0x049e2350)  , 19 },
     { MAD_F(0x049e57e8)  , 19 },
     { MAD_F(0x049e8c81)  , 19 },
     { MAD_F(0x049ec11b)  , 19 },
     { MAD_F(0x049ef5b5)  , 19 },
     { MAD_F(0x049f2a50)  , 19 },
     { MAD_F(0x049f5eeb)  , 19 },
     { MAD_F(0x049f9387)  , 19 },

     { MAD_F(0x049fc824)  , 19 },
     { MAD_F(0x049ffcc1)  , 19 },
     { MAD_F(0x04a0315e)  , 19 },
     { MAD_F(0x04a065fd)  , 19 },
     { MAD_F(0x04a09a9b)  , 19 },
     { MAD_F(0x04a0cf3b)  , 19 },
     { MAD_F(0x04a103db)  , 19 },
     { MAD_F(0x04a1387b)  , 19 },
     { MAD_F(0x04a16d1d)  , 19 },
     { MAD_F(0x04a1a1be)  , 19 },
     { MAD_F(0x04a1d661)  , 19 },
     { MAD_F(0x04a20b04)  , 19 },
     { MAD_F(0x04a23fa7)  , 19 },
     { MAD_F(0x04a2744b)  , 19 },
     { MAD_F(0x04a2a8f0)  , 19 },
     { MAD_F(0x04a2dd95)  , 19 },

     { MAD_F(0x04a3123b)  , 19 },
     { MAD_F(0x04a346e2)  , 19 },
     { MAD_F(0x04a37b89)  , 19 },
     { MAD_F(0x04a3b030)  , 19 },
     { MAD_F(0x04a3e4d8)  , 19 },
     { MAD_F(0x04a41981)  , 19 },
     { MAD_F(0x04a44e2b)  , 19 },
     { MAD_F(0x04a482d5)  , 19 },
     { MAD_F(0x04a4b77f)  , 19 },
     { MAD_F(0x04a4ec2a)  , 19 },
     { MAD_F(0x04a520d6)  , 19 },
     { MAD_F(0x04a55582)  , 19 },
     { MAD_F(0x04a58a2f)  , 19 },
     { MAD_F(0x04a5bedd)  , 19 },
     { MAD_F(0x04a5f38b)  , 19 },
     { MAD_F(0x04a62839)  , 19 },

     { MAD_F(0x04a65ce8)  , 19 },
     { MAD_F(0x04a69198)  , 19 },
     { MAD_F(0x04a6c648)  , 19 },
     { MAD_F(0x04a6faf9)  , 19 },
     { MAD_F(0x04a72fab)  , 19 },
     { MAD_F(0x04a7645d)  , 19 },
     { MAD_F(0x04a79910)  , 19 },
     { MAD_F(0x04a7cdc3)  , 19 },
     { MAD_F(0x04a80277)  , 19 },
     { MAD_F(0x04a8372b)  , 19 },
     { MAD_F(0x04a86be0)  , 19 },
     { MAD_F(0x04a8a096)  , 19 },
     { MAD_F(0x04a8d54c)  , 19 },
     { MAD_F(0x04a90a03)  , 19 },
     { MAD_F(0x04a93eba)  , 19 },
     { MAD_F(0x04a97372)  , 19 },

     { MAD_F(0x04a9a82b)  , 19 },
     { MAD_F(0x04a9dce4)  , 19 },
     { MAD_F(0x04aa119d)  , 19 },
     { MAD_F(0x04aa4658)  , 19 },
     { MAD_F(0x04aa7b13)  , 19 },
     { MAD_F(0x04aaafce)  , 19 },
     { MAD_F(0x04aae48a)  , 19 },
     { MAD_F(0x04ab1947)  , 19 },
     { MAD_F(0x04ab4e04)  , 19 },
     { MAD_F(0x04ab82c2)  , 19 },
     { MAD_F(0x04abb780)  , 19 },
     { MAD_F(0x04abec3f)  , 19 },
     { MAD_F(0x04ac20fe)  , 19 },
     { MAD_F(0x04ac55be)  , 19 },
     { MAD_F(0x04ac8a7f)  , 19 },
     { MAD_F(0x04acbf40)  , 19 },

     { MAD_F(0x04acf402)  , 19 },
     { MAD_F(0x04ad28c5)  , 19 },
     { MAD_F(0x04ad5d88)  , 19 },
     { MAD_F(0x04ad924b)  , 19 },
     { MAD_F(0x04adc70f)  , 19 },
     { MAD_F(0x04adfbd4)  , 19 },
     { MAD_F(0x04ae3099)  , 19 },
     { MAD_F(0x04ae655f)  , 19 },
     { MAD_F(0x04ae9a26)  , 19 },
     { MAD_F(0x04aeceed)  , 19 },
     { MAD_F(0x04af03b4)  , 19 },
     { MAD_F(0x04af387d)  , 19 },
     { MAD_F(0x04af6d45)  , 19 },
     { MAD_F(0x04afa20f)  , 19 },
     { MAD_F(0x04afd6d9)  , 19 },
     { MAD_F(0x04b00ba3)  , 19 },

     { MAD_F(0x04b0406e)  , 19 },
     { MAD_F(0x04b0753a)  , 19 },
     { MAD_F(0x04b0aa06)  , 19 },
     { MAD_F(0x04b0ded3)  , 19 },
     { MAD_F(0x04b113a1)  , 19 },
     { MAD_F(0x04b1486f)  , 19 },
     { MAD_F(0x04b17d3d)  , 19 },
     { MAD_F(0x04b1b20c)  , 19 },
     { MAD_F(0x04b1e6dc)  , 19 },
     { MAD_F(0x04b21bad)  , 19 },
     { MAD_F(0x04b2507d)  , 19 },
     { MAD_F(0x04b2854f)  , 19 },
     { MAD_F(0x04b2ba21)  , 19 },
     { MAD_F(0x04b2eef4)  , 19 },
     { MAD_F(0x04b323c7)  , 19 },
     { MAD_F(0x04b3589b)  , 19 },

     { MAD_F(0x04b38d6f)  , 19 },
     { MAD_F(0x04b3c244)  , 19 },
     { MAD_F(0x04b3f71a)  , 19 },
     { MAD_F(0x04b42bf0)  , 19 },
     { MAD_F(0x04b460c7)  , 19 },
     { MAD_F(0x04b4959e)  , 19 },
     { MAD_F(0x04b4ca76)  , 19 },
     { MAD_F(0x04b4ff4e)  , 19 },
     { MAD_F(0x04b53427)  , 19 },
     { MAD_F(0x04b56901)  , 19 },
     { MAD_F(0x04b59ddb)  , 19 },
     { MAD_F(0x04b5d2b6)  , 19 },
     { MAD_F(0x04b60791)  , 19 },
     { MAD_F(0x04b63c6d)  , 19 },
     { MAD_F(0x04b6714a)  , 19 },
     { MAD_F(0x04b6a627)  , 19 },

     { MAD_F(0x04b6db05)  , 19 },
     { MAD_F(0x04b70fe3)  , 19 },
     { MAD_F(0x04b744c2)  , 19 },
     { MAD_F(0x04b779a1)  , 19 },
     { MAD_F(0x04b7ae81)  , 19 },
     { MAD_F(0x04b7e362)  , 19 },
     { MAD_F(0x04b81843)  , 19 },
     { MAD_F(0x04b84d24)  , 19 },
     { MAD_F(0x04b88207)  , 19 },
     { MAD_F(0x04b8b6ea)  , 19 },
     { MAD_F(0x04b8ebcd)  , 19 },
     { MAD_F(0x04b920b1)  , 19 },
     { MAD_F(0x04b95596)  , 19 },
     { MAD_F(0x04b98a7b)  , 19 },
     { MAD_F(0x04b9bf61)  , 19 },
     { MAD_F(0x04b9f447)  , 19 },

     { MAD_F(0x04ba292e)  , 19 },
     { MAD_F(0x04ba5e16)  , 19 },
     { MAD_F(0x04ba92fe)  , 19 },
     { MAD_F(0x04bac7e6)  , 19 },
     { MAD_F(0x04bafcd0)  , 19 },
     { MAD_F(0x04bb31b9)  , 19 },
     { MAD_F(0x04bb66a4)  , 19 },
     { MAD_F(0x04bb9b8f)  , 19 },
     { MAD_F(0x04bbd07a)  , 19 },
     { MAD_F(0x04bc0566)  , 19 },
     { MAD_F(0x04bc3a53)  , 19 },
     { MAD_F(0x04bc6f40)  , 19 },
     { MAD_F(0x04bca42e)  , 19 },
     { MAD_F(0x04bcd91d)  , 19 },
     { MAD_F(0x04bd0e0c)  , 19 },
     { MAD_F(0x04bd42fb)  , 19 },

     { MAD_F(0x04bd77ec)  , 19 },
     { MAD_F(0x04bdacdc)  , 19 },
     { MAD_F(0x04bde1ce)  , 19 },
     { MAD_F(0x04be16c0)  , 19 },
     { MAD_F(0x04be4bb2)  , 19 },
     { MAD_F(0x04be80a5)  , 19 },
     { MAD_F(0x04beb599)  , 19 },
     { MAD_F(0x04beea8d)  , 19 },
     { MAD_F(0x04bf1f82)  , 19 },
     { MAD_F(0x04bf5477)  , 19 },
     { MAD_F(0x04bf896d)  , 19 },
     { MAD_F(0x04bfbe64)  , 19 },
     { MAD_F(0x04bff35b)  , 19 },
     { MAD_F(0x04c02852)  , 19 },
     { MAD_F(0x04c05d4b)  , 19 },
     { MAD_F(0x04c09243)  , 19 },

     { MAD_F(0x04c0c73d)  , 19 },
     { MAD_F(0x04c0fc37)  , 19 },
     { MAD_F(0x04c13131)  , 19 },
     { MAD_F(0x04c1662d)  , 19 },
     { MAD_F(0x04c19b28)  , 19 },
     { MAD_F(0x04c1d025)  , 19 },
     { MAD_F(0x04c20521)  , 19 },
     { MAD_F(0x04c23a1f)  , 19 },
     { MAD_F(0x04c26f1d)  , 19 },
     { MAD_F(0x04c2a41b)  , 19 },
     { MAD_F(0x04c2d91b)  , 19 },
     { MAD_F(0x04c30e1a)  , 19 },
     { MAD_F(0x04c3431b)  , 19 },
     { MAD_F(0x04c3781c)  , 19 },
     { MAD_F(0x04c3ad1d)  , 19 },
     { MAD_F(0x04c3e21f)  , 19 },

     { MAD_F(0x04c41722)  , 19 },
     { MAD_F(0x04c44c25)  , 19 },
     { MAD_F(0x04c48129)  , 19 },
     { MAD_F(0x04c4b62d)  , 19 },
     { MAD_F(0x04c4eb32)  , 19 },
     { MAD_F(0x04c52038)  , 19 },
     { MAD_F(0x04c5553e)  , 19 },
     { MAD_F(0x04c58a44)  , 19 },
     { MAD_F(0x04c5bf4c)  , 19 },
     { MAD_F(0x04c5f453)  , 19 },
     { MAD_F(0x04c6295c)  , 19 },
     { MAD_F(0x04c65e65)  , 19 },
     { MAD_F(0x04c6936e)  , 19 },
     { MAD_F(0x04c6c878)  , 19 },
     { MAD_F(0x04c6fd83)  , 19 },
     { MAD_F(0x04c7328e)  , 19 },

     { MAD_F(0x04c7679a)  , 19 },
     { MAD_F(0x04c79ca7)  , 19 },
     { MAD_F(0x04c7d1b4)  , 19 },
     { MAD_F(0x04c806c1)  , 19 },
     { MAD_F(0x04c83bcf)  , 19 },
     { MAD_F(0x04c870de)  , 19 },
     { MAD_F(0x04c8a5ed)  , 19 },
     { MAD_F(0x04c8dafd)  , 19 },
     { MAD_F(0x04c9100d)  , 19 },
     { MAD_F(0x04c9451e)  , 19 },
     { MAD_F(0x04c97a30)  , 19 },
     { MAD_F(0x04c9af42)  , 19 },
     { MAD_F(0x04c9e455)  , 19 },
     { MAD_F(0x04ca1968)  , 19 },
     { MAD_F(0x04ca4e7c)  , 19 },
     { MAD_F(0x04ca8391)  , 19 },

     { MAD_F(0x04cab8a6)  , 19 },
     { MAD_F(0x04caedbb)  , 19 },
     { MAD_F(0x04cb22d1)  , 19 },
     { MAD_F(0x04cb57e8)  , 19 },
     { MAD_F(0x04cb8d00)  , 19 },
     { MAD_F(0x04cbc217)  , 19 },
     { MAD_F(0x04cbf730)  , 19 },
     { MAD_F(0x04cc2c49)  , 19 },
     { MAD_F(0x04cc6163)  , 19 },
     { MAD_F(0x04cc967d)  , 19 },
     { MAD_F(0x04cccb98)  , 19 },
     { MAD_F(0x04cd00b3)  , 19 },
     { MAD_F(0x04cd35cf)  , 19 },
     { MAD_F(0x04cd6aeb)  , 19 },
     { MAD_F(0x04cda008)  , 19 },
     { MAD_F(0x04cdd526)  , 19 },

     { MAD_F(0x04ce0a44)  , 19 },
     { MAD_F(0x04ce3f63)  , 19 },
     { MAD_F(0x04ce7482)  , 19 },
     { MAD_F(0x04cea9a2)  , 19 },
     { MAD_F(0x04cedec3)  , 19 },
     { MAD_F(0x04cf13e4)  , 19 },
     { MAD_F(0x04cf4906)  , 19 },
     { MAD_F(0x04cf7e28)  , 19 },
     { MAD_F(0x04cfb34b)  , 19 },
     { MAD_F(0x04cfe86e)  , 19 },
     { MAD_F(0x04d01d92)  , 19 },
     { MAD_F(0x04d052b6)  , 19 },
     { MAD_F(0x04d087db)  , 19 },
     { MAD_F(0x04d0bd01)  , 19 },
     { MAD_F(0x04d0f227)  , 19 },
     { MAD_F(0x04d1274e)  , 19 },

     { MAD_F(0x04d15c76)  , 19 },
     { MAD_F(0x04d1919e)  , 19 },
     { MAD_F(0x04d1c6c6)  , 19 },
     { MAD_F(0x04d1fbef)  , 19 },
     { MAD_F(0x04d23119)  , 19 },
     { MAD_F(0x04d26643)  , 19 },
     { MAD_F(0x04d29b6e)  , 19 },
     { MAD_F(0x04d2d099)  , 19 },
     { MAD_F(0x04d305c5)  , 19 },
     { MAD_F(0x04d33af2)  , 19 },
     { MAD_F(0x04d3701f)  , 19 },
     { MAD_F(0x04d3a54d)  , 19 },
     { MAD_F(0x04d3da7b)  , 19 },
     { MAD_F(0x04d40faa)  , 19 },
     { MAD_F(0x04d444d9)  , 19 },
     { MAD_F(0x04d47a09)  , 19 },

     { MAD_F(0x04d4af3a)  , 19 },
     { MAD_F(0x04d4e46b)  , 19 },
     { MAD_F(0x04d5199c)  , 19 },
     { MAD_F(0x04d54ecf)  , 19 },
     { MAD_F(0x04d58401)  , 19 },
     { MAD_F(0x04d5b935)  , 19 },
     { MAD_F(0x04d5ee69)  , 19 },
     { MAD_F(0x04d6239d)  , 19 },
     { MAD_F(0x04d658d2)  , 19 },
     { MAD_F(0x04d68e08)  , 19 },
     { MAD_F(0x04d6c33e)  , 19 },
     { MAD_F(0x04d6f875)  , 19 },
     { MAD_F(0x04d72dad)  , 19 },
     { MAD_F(0x04d762e5)  , 19 },
     { MAD_F(0x04d7981d)  , 19 },
     { MAD_F(0x04d7cd56)  , 19 },

     { MAD_F(0x04d80290)  , 19 },
     { MAD_F(0x04d837ca)  , 19 },
     { MAD_F(0x04d86d05)  , 19 },
     { MAD_F(0x04d8a240)  , 19 },
     { MAD_F(0x04d8d77c)  , 19 },
     { MAD_F(0x04d90cb9)  , 19 },
     { MAD_F(0x04d941f6)  , 19 },
     { MAD_F(0x04d97734)  , 19 },
     { MAD_F(0x04d9ac72)  , 19 },
     { MAD_F(0x04d9e1b1)  , 19 },
     { MAD_F(0x04da16f0)  , 19 },
     { MAD_F(0x04da4c30)  , 19 },
     { MAD_F(0x04da8171)  , 19 },
     { MAD_F(0x04dab6b2)  , 19 },
     { MAD_F(0x04daebf4)  , 19 },
     { MAD_F(0x04db2136)  , 19 },

     { MAD_F(0x04db5679)  , 19 },
     { MAD_F(0x04db8bbc)  , 19 },
     { MAD_F(0x04dbc100)  , 19 },
     { MAD_F(0x04dbf644)  , 19 },
     { MAD_F(0x04dc2b8a)  , 19 },
     { MAD_F(0x04dc60cf)  , 19 },
     { MAD_F(0x04dc9616)  , 19 },
     { MAD_F(0x04dccb5c)  , 19 },
     { MAD_F(0x04dd00a4)  , 19 },
     { MAD_F(0x04dd35ec)  , 19 },
     { MAD_F(0x04dd6b34)  , 19 },
     { MAD_F(0x04dda07d)  , 19 },
     { MAD_F(0x04ddd5c7)  , 19 },
     { MAD_F(0x04de0b11)  , 19 },
     { MAD_F(0x04de405c)  , 19 },
     { MAD_F(0x04de75a7)  , 19 },

     { MAD_F(0x04deaaf3)  , 19 },
     { MAD_F(0x04dee040)  , 19 },
     { MAD_F(0x04df158d)  , 19 },
     { MAD_F(0x04df4adb)  , 19 },
     { MAD_F(0x04df8029)  , 19 },
     { MAD_F(0x04dfb578)  , 19 },
     { MAD_F(0x04dfeac7)  , 19 },
     { MAD_F(0x04e02017)  , 19 },
     { MAD_F(0x04e05567)  , 19 },
     { MAD_F(0x04e08ab8)  , 19 },
     { MAD_F(0x04e0c00a)  , 19 },
     { MAD_F(0x04e0f55c)  , 19 },
     { MAD_F(0x04e12aaf)  , 19 },
     { MAD_F(0x04e16002)  , 19 },
     { MAD_F(0x04e19556)  , 19 },
     { MAD_F(0x04e1caab)  , 19 },

     { MAD_F(0x04e20000)  , 19 },
     { MAD_F(0x04e23555)  , 19 },
     { MAD_F(0x04e26aac)  , 19 },
     { MAD_F(0x04e2a002)  , 19 },
     { MAD_F(0x04e2d55a)  , 19 },
     { MAD_F(0x04e30ab2)  , 19 },
     { MAD_F(0x04e3400a)  , 19 },
     { MAD_F(0x04e37563)  , 19 },
     { MAD_F(0x04e3aabd)  , 19 },
     { MAD_F(0x04e3e017)  , 19 },
     { MAD_F(0x04e41572)  , 19 },
     { MAD_F(0x04e44acd)  , 19 },
     { MAD_F(0x04e48029)  , 19 },
     { MAD_F(0x04e4b585)  , 19 },
     { MAD_F(0x04e4eae2)  , 19 },
     { MAD_F(0x04e52040)  , 19 },

     { MAD_F(0x04e5559e)  , 19 },
     { MAD_F(0x04e58afd)  , 19 },
     { MAD_F(0x04e5c05c)  , 19 },
     { MAD_F(0x04e5f5bc)  , 19 },
     { MAD_F(0x04e62b1c)  , 19 },
     { MAD_F(0x04e6607d)  , 19 },
     { MAD_F(0x04e695df)  , 19 },
     { MAD_F(0x04e6cb41)  , 19 },
     { MAD_F(0x04e700a3)  , 19 },
     { MAD_F(0x04e73607)  , 19 },
     { MAD_F(0x04e76b6b)  , 19 },
     { MAD_F(0x04e7a0cf)  , 19 },
     { MAD_F(0x04e7d634)  , 19 },
     { MAD_F(0x04e80b99)  , 19 },
     { MAD_F(0x04e84100)  , 19 },
     { MAD_F(0x04e87666)  , 19 },

     { MAD_F(0x04e8abcd)  , 19 },
     { MAD_F(0x04e8e135)  , 19 },
     { MAD_F(0x04e9169e)  , 19 },
     { MAD_F(0x04e94c07)  , 19 },
     { MAD_F(0x04e98170)  , 19 },
     { MAD_F(0x04e9b6da)  , 19 },
     { MAD_F(0x04e9ec45)  , 19 },
     { MAD_F(0x04ea21b0)  , 19 },
     { MAD_F(0x04ea571c)  , 19 },
     { MAD_F(0x04ea8c88)  , 19 },
     { MAD_F(0x04eac1f5)  , 19 },
     { MAD_F(0x04eaf762)  , 19 },
     { MAD_F(0x04eb2cd0)  , 19 },
     { MAD_F(0x04eb623f)  , 19 },
     { MAD_F(0x04eb97ae)  , 19 },
     { MAD_F(0x04ebcd1e)  , 19 },

     { MAD_F(0x04ec028e)  , 19 },
     { MAD_F(0x04ec37ff)  , 19 },
     { MAD_F(0x04ec6d71)  , 19 },
     { MAD_F(0x04eca2e3)  , 19 },
     { MAD_F(0x04ecd855)  , 19 },
     { MAD_F(0x04ed0dc8)  , 19 },
     { MAD_F(0x04ed433c)  , 19 },
     { MAD_F(0x04ed78b0)  , 19 },
     { MAD_F(0x04edae25)  , 19 },
     { MAD_F(0x04ede39a)  , 19 },
     { MAD_F(0x04ee1910)  , 19 },
     { MAD_F(0x04ee4e87)  , 19 },
     { MAD_F(0x04ee83fe)  , 19 },
     { MAD_F(0x04eeb976)  , 19 },
     { MAD_F(0x04eeeeee)  , 19 },
     { MAD_F(0x04ef2467)  , 19 },

     { MAD_F(0x04ef59e0)  , 19 },
     { MAD_F(0x04ef8f5a)  , 19 },
     { MAD_F(0x04efc4d5)  , 19 },
     { MAD_F(0x04effa50)  , 19 },
     { MAD_F(0x04f02fcb)  , 19 },
     { MAD_F(0x04f06547)  , 19 },
     { MAD_F(0x04f09ac4)  , 19 },
     { MAD_F(0x04f0d041)  , 19 },
     { MAD_F(0x04f105bf)  , 19 },
     { MAD_F(0x04f13b3e)  , 19 },
     { MAD_F(0x04f170bd)  , 19 },
     { MAD_F(0x04f1a63c)  , 19 },
     { MAD_F(0x04f1dbbd)  , 19 },
     { MAD_F(0x04f2113d)  , 19 },
     { MAD_F(0x04f246bf)  , 19 },
     { MAD_F(0x04f27c40)  , 19 },


     { MAD_F(0x04f2b1c3)  , 19 },
     { MAD_F(0x04f2e746)  , 19 },
     { MAD_F(0x04f31cc9)  , 19 },
     { MAD_F(0x04f3524d)  , 19 },
     { MAD_F(0x04f387d2)  , 19 },
     { MAD_F(0x04f3bd57)  , 19 },
     { MAD_F(0x04f3f2dd)  , 19 },
     { MAD_F(0x04f42864)  , 19 },
     { MAD_F(0x04f45dea)  , 19 },
     { MAD_F(0x04f49372)  , 19 },
     { MAD_F(0x04f4c8fa)  , 19 },
     { MAD_F(0x04f4fe83)  , 19 },
     { MAD_F(0x04f5340c)  , 19 },
     { MAD_F(0x04f56996)  , 19 },
     { MAD_F(0x04f59f20)  , 19 },
     { MAD_F(0x04f5d4ab)  , 19 },

     { MAD_F(0x04f60a36)  , 19 },
     { MAD_F(0x04f63fc2)  , 19 },
     { MAD_F(0x04f6754f)  , 19 },
     { MAD_F(0x04f6aadc)  , 19 },
     { MAD_F(0x04f6e06a)  , 19 },
     { MAD_F(0x04f715f8)  , 19 },
     { MAD_F(0x04f74b87)  , 19 },
     { MAD_F(0x04f78116)  , 19 },
     { MAD_F(0x04f7b6a6)  , 19 },
     { MAD_F(0x04f7ec37)  , 19 },
     { MAD_F(0x04f821c8)  , 19 },
     { MAD_F(0x04f85759)  , 19 },
     { MAD_F(0x04f88cec)  , 19 },
     { MAD_F(0x04f8c27e)  , 19 },
     { MAD_F(0x04f8f812)  , 19 },
     { MAD_F(0x04f92da6)  , 19 },

     { MAD_F(0x04f9633a)  , 19 },
     { MAD_F(0x04f998cf)  , 19 },
     { MAD_F(0x04f9ce65)  , 19 },
     { MAD_F(0x04fa03fb)  , 19 },
     { MAD_F(0x04fa3992)  , 19 },
     { MAD_F(0x04fa6f29)  , 19 },
     { MAD_F(0x04faa4c1)  , 19 },
     { MAD_F(0x04fada59)  , 19 },
     { MAD_F(0x04fb0ff2)  , 19 },
     { MAD_F(0x04fb458c)  , 19 },
     { MAD_F(0x04fb7b26)  , 19 },
     { MAD_F(0x04fbb0c1)  , 19 },
     { MAD_F(0x04fbe65c)  , 19 },
     { MAD_F(0x04fc1bf8)  , 19 },
     { MAD_F(0x04fc5194)  , 19 },
     { MAD_F(0x04fc8731)  , 19 },

     { MAD_F(0x04fcbcce)  , 19 },
     { MAD_F(0x04fcf26c)  , 19 },
     { MAD_F(0x04fd280b)  , 19 },
     { MAD_F(0x04fd5daa)  , 19 },
     { MAD_F(0x04fd934a)  , 19 },
     { MAD_F(0x04fdc8ea)  , 19 },
     { MAD_F(0x04fdfe8b)  , 19 },
     { MAD_F(0x04fe342c)  , 19 },
     { MAD_F(0x04fe69ce)  , 19 },
     { MAD_F(0x04fe9f71)  , 19 },
     { MAD_F(0x04fed514)  , 19 },
     { MAD_F(0x04ff0ab8)  , 19 },
     { MAD_F(0x04ff405c)  , 19 },
     { MAD_F(0x04ff7601)  , 19 },
     { MAD_F(0x04ffaba6)  , 19 },
     { MAD_F(0x04ffe14c)  , 19 },

     { MAD_F(0x050016f3)  , 19 },
     { MAD_F(0x05004c9a)  , 19 },
     { MAD_F(0x05008241)  , 19 },
     { MAD_F(0x0500b7e9)  , 19 },
     { MAD_F(0x0500ed92)  , 19 },
     { MAD_F(0x0501233b)  , 19 },
     { MAD_F(0x050158e5)  , 19 },
     { MAD_F(0x05018e90)  , 19 },
     { MAD_F(0x0501c43b)  , 19 },
     { MAD_F(0x0501f9e6)  , 19 },
     { MAD_F(0x05022f92)  , 19 },
     { MAD_F(0x0502653f)  , 19 },
     { MAD_F(0x05029aec)  , 19 },
     { MAD_F(0x0502d09a)  , 19 },
     { MAD_F(0x05030648)  , 19 },
     { MAD_F(0x05033bf7)  , 19 },

     { MAD_F(0x050371a7)  , 19 },
     { MAD_F(0x0503a757)  , 19 },
     { MAD_F(0x0503dd07)  , 19 },
     { MAD_F(0x050412b9)  , 19 },
     { MAD_F(0x0504486a)  , 19 },
     { MAD_F(0x05047e1d)  , 19 },
     { MAD_F(0x0504b3cf)  , 19 },
     { MAD_F(0x0504e983)  , 19 },
     { MAD_F(0x05051f37)  , 19 },
     { MAD_F(0x050554eb)  , 19 },
     { MAD_F(0x05058aa0)  , 19 },
     { MAD_F(0x0505c056)  , 19 },
     { MAD_F(0x0505f60c)  , 19 },
     { MAD_F(0x05062bc3)  , 19 },
     { MAD_F(0x0506617a)  , 19 },
     { MAD_F(0x05069732)  , 19 },

     { MAD_F(0x0506cceb)  , 19 },
     { MAD_F(0x050702a4)  , 19 },
     { MAD_F(0x0507385d)  , 19 },
     { MAD_F(0x05076e17)  , 19 },
     { MAD_F(0x0507a3d2)  , 19 },
     { MAD_F(0x0507d98d)  , 19 },
     { MAD_F(0x05080f49)  , 19 },
     { MAD_F(0x05084506)  , 19 },
     { MAD_F(0x05087ac2)  , 19 },
     { MAD_F(0x0508b080)  , 19 },
     { MAD_F(0x0508e63e)  , 19 },
     { MAD_F(0x05091bfd)  , 19 },
     { MAD_F(0x050951bc)  , 19 },
     { MAD_F(0x0509877c)  , 19 },
     { MAD_F(0x0509bd3c)  , 19 },
     { MAD_F(0x0509f2fd)  , 19 },

     { MAD_F(0x050a28be)  , 19 },
     { MAD_F(0x050a5e80)  , 19 },
     { MAD_F(0x050a9443)  , 19 },
     { MAD_F(0x050aca06)  , 19 },
     { MAD_F(0x050affc9)  , 19 },
     { MAD_F(0x050b358e)  , 19 },
     { MAD_F(0x050b6b52)  , 19 },
     { MAD_F(0x050ba118)  , 19 },
     { MAD_F(0x050bd6de)  , 19 },
     { MAD_F(0x050c0ca4)  , 19 },
     { MAD_F(0x050c426b)  , 19 },
     { MAD_F(0x050c7833)  , 19 },
     { MAD_F(0x050cadfb)  , 19 },
     { MAD_F(0x050ce3c4)  , 19 },
     { MAD_F(0x050d198d)  , 19 }

};

 
static
mad_fixed_t const root_table[7] = {
  MAD_F(0x09837f05)  ,
  MAD_F(0x0b504f33)  ,
  MAD_F(0x0d744fcd)  ,
  MAD_F(0x10000000)  ,
  MAD_F(0x1306fe0a)  ,
  MAD_F(0x16a09e66)  ,
  MAD_F(0x1ae89f99)  
};

 
static
mad_fixed_t const cs[8] = {
  +MAD_F(0x0db84a81)  , +MAD_F(0x0e1b9d7f)  ,
  +MAD_F(0x0f31adcf)  , +MAD_F(0x0fbba815)  ,
  +MAD_F(0x0feda417)  , +MAD_F(0x0ffc8fc8)  ,
  +MAD_F(0x0fff964c)  , +MAD_F(0x0ffff8d3)  
};

static
mad_fixed_t const ca[8] = {
  -MAD_F(0x083b5fe7)  , -MAD_F(0x078c36d2)  ,
  -MAD_F(0x05039814)  , -MAD_F(0x02e91dd1)  ,
  -MAD_F(0x0183603a)  , -MAD_F(0x00a7cb87)  ,
  -MAD_F(0x003a2847)  , -MAD_F(0x000f27b4)  
};

 
static
mad_fixed_t const imdct_s[6][6] = {

     {  MAD_F(0x09bd7ca0)  ,
	      -MAD_F(0x0ec835e8)  ,
	      -MAD_F(0x0216a2a2)  ,
	       MAD_F(0x0fdcf549)  ,
	      -MAD_F(0x061f78aa)  ,
	      -MAD_F(0x0cb19346)   },

     { -MAD_F(0x0cb19346)  ,
	       MAD_F(0x061f78aa)  ,
	       MAD_F(0x0fdcf549)  ,
	       MAD_F(0x0216a2a2)  ,
	      -MAD_F(0x0ec835e8)  ,
	      -MAD_F(0x09bd7ca0)   },

     {  MAD_F(0x061f78aa)  ,
	      -MAD_F(0x0ec835e8)  ,
	       MAD_F(0x0ec835e8)  ,
	      -MAD_F(0x061f78aa)  ,
	      -MAD_F(0x061f78aa)  ,
	       MAD_F(0x0ec835e8)   },

     { -MAD_F(0x0ec835e8)  ,
	      -MAD_F(0x061f78aa)  ,
	       MAD_F(0x061f78aa)  ,
	       MAD_F(0x0ec835e8)  ,
	       MAD_F(0x0ec835e8)  ,
	       MAD_F(0x061f78aa)   },

     {  MAD_F(0x0216a2a2)  ,
	      -MAD_F(0x061f78aa)  ,
	       MAD_F(0x09bd7ca0)  ,
	      -MAD_F(0x0cb19346)  ,
	       MAD_F(0x0ec835e8)  ,
	      -MAD_F(0x0fdcf549)   },

     { -MAD_F(0x0fdcf549)  ,
	      -MAD_F(0x0ec835e8)  ,
	      -MAD_F(0x0cb19346)  ,
	      -MAD_F(0x09bd7ca0)  ,
	      -MAD_F(0x061f78aa)  ,
	      -MAD_F(0x0216a2a2)   }

};

# if !defined(ASO_IMDCT)
 
static
mad_fixed_t const window_l[36] = {
  MAD_F(0x00b2aa3e)  , MAD_F(0x0216a2a2)  ,
  MAD_F(0x03768962)  , MAD_F(0x04cfb0e2)  ,
  MAD_F(0x061f78aa)  , MAD_F(0x07635284)  ,
  MAD_F(0x0898c779)  , MAD_F(0x09bd7ca0)  ,
  MAD_F(0x0acf37ad)  , MAD_F(0x0bcbe352)  ,
  MAD_F(0x0cb19346)  , MAD_F(0x0d7e8807)  ,

  MAD_F(0x0e313245)  , MAD_F(0x0ec835e8)  ,
  MAD_F(0x0f426cb5)  , MAD_F(0x0f9ee890)  ,
  MAD_F(0x0fdcf549)  , MAD_F(0x0ffc19fd)  ,
  MAD_F(0x0ffc19fd)  , MAD_F(0x0fdcf549)  ,
  MAD_F(0x0f9ee890)  , MAD_F(0x0f426cb5)  ,
  MAD_F(0x0ec835e8)  , MAD_F(0x0e313245)  ,

  MAD_F(0x0d7e8807)  , MAD_F(0x0cb19346)  ,
  MAD_F(0x0bcbe352)  , MAD_F(0x0acf37ad)  ,
  MAD_F(0x09bd7ca0)  , MAD_F(0x0898c779)  ,
  MAD_F(0x07635284)  , MAD_F(0x061f78aa)  ,
  MAD_F(0x04cfb0e2)  , MAD_F(0x03768962)  ,
  MAD_F(0x0216a2a2)  , MAD_F(0x00b2aa3e)  ,
};
# endif   

 
static
mad_fixed_t const window_s[12] = {
  MAD_F(0x0216a2a2)  , MAD_F(0x061f78aa)  ,
  MAD_F(0x09bd7ca0)  , MAD_F(0x0cb19346)  ,
  MAD_F(0x0ec835e8)  , MAD_F(0x0fdcf549)  ,
  MAD_F(0x0fdcf549)  , MAD_F(0x0ec835e8)  ,
  MAD_F(0x0cb19346)  , MAD_F(0x09bd7ca0)  ,
  MAD_F(0x061f78aa)  , MAD_F(0x0216a2a2)  ,
};

 
static
mad_fixed_t const is_table[7] = {
  MAD_F(0x00000000)  ,
  MAD_F(0x0361962f)  ,
  MAD_F(0x05db3d74)  ,
  MAD_F(0x08000000)  ,
  MAD_F(0x0a24c28c)  ,
  MAD_F(0x0c9e69d1)  ,
  MAD_F(0x10000000)  
};

 
static
mad_fixed_t const is_lsf_table[2][15] = {
  {
    MAD_F(0x0d744fcd)  ,
    MAD_F(0x0b504f33)  ,
    MAD_F(0x09837f05)  ,
    MAD_F(0x08000000)  ,
    MAD_F(0x06ba27e6)  ,
    MAD_F(0x05a8279a)  ,
    MAD_F(0x04c1bf83)  ,
    MAD_F(0x04000000)  ,
    MAD_F(0x035d13f3)  ,
    MAD_F(0x02d413cd)  ,
    MAD_F(0x0260dfc1)  ,
    MAD_F(0x02000000)  ,
    MAD_F(0x01ae89fa)  ,
    MAD_F(0x016a09e6)  ,
    MAD_F(0x01306fe1)  
  }, {
    MAD_F(0x0b504f33)  ,
    MAD_F(0x08000000)  ,
    MAD_F(0x05a8279a)  ,
    MAD_F(0x04000000)  ,
    MAD_F(0x02d413cd)  ,
    MAD_F(0x02000000)  ,
    MAD_F(0x016a09e6)  ,
    MAD_F(0x01000000)  ,
    MAD_F(0x00b504f3)  ,
    MAD_F(0x00800000)  ,
    MAD_F(0x005a827a)  ,
    MAD_F(0x00400000)  ,
    MAD_F(0x002d413d)  ,
    MAD_F(0x00200000)  ,
    MAD_F(0x0016a09e)  
  }
};

 
static
enum mad_error III_sideinfo(struct mad_bitptr *ptr, unsigned int nch,
			    int lsf, struct sideinfo *si,
			    unsigned int *data_bitlen,
			    unsigned int *priv_bitlen)
{
  unsigned int ngr, gr, ch, i;
  enum mad_error result = MAD_ERROR_NONE;

  *data_bitlen = 0;
  *priv_bitlen = lsf ? ((nch == 1) ? 1 : 2) : ((nch == 1) ? 5 : 3);

  si->main_data_begin = mad_bit_read(ptr, lsf ? 8 : 9);
  si->private_bits    = mad_bit_read(ptr, *priv_bitlen);

  ngr = 1;
  if (!lsf) {
    ngr = 2;

    for (ch = 0; ch < nch; ++ch)
      si->scfsi[ch] = mad_bit_read(ptr, 4);
  }

  for (gr = 0; gr < ngr; ++gr) {
    struct granule * granule = (struct granule *)  &si->gr[gr];

    for (ch = 0; ch < nch; ++ch) {
      struct channel *channel = &granule->ch[ch];

      channel->part2_3_length    = mad_bit_read(ptr, 12);
      channel->big_values        = mad_bit_read(ptr, 9);
      channel->global_gain       = mad_bit_read(ptr, 8);
      channel->scalefac_compress = mad_bit_read(ptr, lsf ? 9 : 4);

      *data_bitlen += channel->part2_3_length;

      if (channel->big_values > 288 && result == 0)
	result = MAD_ERROR_BADBIGVALUES;

      channel->flags = 0;

       
      if (mad_bit_read(ptr, 1)) {
	channel->block_type = mad_bit_read(ptr, 2);

	if (channel->block_type == 0 && result == 0)
	  result = MAD_ERROR_BADBLOCKTYPE;

	if (!lsf && channel->block_type == 2 && si->scfsi[ch] && result == 0)
	  result = MAD_ERROR_BADSCFSI;

	channel->region0_count = 7;
	channel->region1_count = 36;

	if (mad_bit_read(ptr, 1))
	  channel->flags |= mixed_block_flag;
	else if (channel->block_type == 2)
	  channel->region0_count = 8;

	for (i = 0; i < 2; ++i)
	  channel->table_select[i] = mad_bit_read(ptr, 5);

# if defined(DEBUG)
	channel->table_select[2] = 4;   
# endif

	for (i = 0; i < 3; ++i)
	  channel->subblock_gain[i] = mad_bit_read(ptr, 3);
      }
      else {
	channel->block_type = 0;

	for (i = 0; i < 3; ++i)
	  channel->table_select[i] = mad_bit_read(ptr, 5);

	channel->region0_count = mad_bit_read(ptr, 4);
	channel->region1_count = mad_bit_read(ptr, 3);
      }

       
      channel->flags |= mad_bit_read(ptr, lsf ? 2 : 3);
    }
  }

  return result;
}

 
static
unsigned int III_scalefactors_lsf(struct mad_bitptr *ptr,
				  struct channel *channel,
				  struct channel *gr1ch, int mode_extension)
{
  struct mad_bitptr start;
  unsigned int scalefac_compress, index, slen[4], part, n, i;
  unsigned char const *nsfb;

  start = *ptr;

  scalefac_compress = channel->scalefac_compress;
  index = (channel->block_type == 2) ?
    ((channel->flags & mixed_block_flag) ? 2 : 1) : 0;

  if (!((mode_extension & I_STEREO) && gr1ch)) {
    if (scalefac_compress < 400) {
      slen[0] = (scalefac_compress >> 4) / 5;
      slen[1] = (scalefac_compress >> 4) % 5;
      slen[2] = (scalefac_compress % 16) >> 2;
      slen[3] =  scalefac_compress %  4;

      nsfb = nsfb_table[0][index];
    }
    else if (scalefac_compress < 500) {
      scalefac_compress -= 400;

      slen[0] = (scalefac_compress >> 2) / 5;
      slen[1] = (scalefac_compress >> 2) % 5;
      slen[2] =  scalefac_compress %  4;
      slen[3] = 0;

      nsfb = nsfb_table[1][index];
    }
    else {
      scalefac_compress -= 500;

      slen[0] = scalefac_compress / 3;
      slen[1] = scalefac_compress % 3;
      slen[2] = 0;
      slen[3] = 0;

      channel->flags |= preflag;

      nsfb = nsfb_table[2][index];
    }

    n = 0;
    for (part = 0; part < 4; ++part) {
      for (i = 0; i < nsfb[part]; ++i)
	channel->scalefac[n++] = mad_bit_read(ptr, slen[part]);
    }

    while (n < 39)
      channel->scalefac[n++] = 0;
  }
  else {   
    scalefac_compress >>= 1;

    if (scalefac_compress < 180) {
      slen[0] =  scalefac_compress / 36;
      slen[1] = (scalefac_compress % 36) / 6;
      slen[2] = (scalefac_compress % 36) % 6;
      slen[3] = 0;

      nsfb = nsfb_table[3][index];
    }
    else if (scalefac_compress < 244) {
      scalefac_compress -= 180;

      slen[0] = (scalefac_compress % 64) >> 4;
      slen[1] = (scalefac_compress % 16) >> 2;
      slen[2] =  scalefac_compress %  4;
      slen[3] = 0;

      nsfb = nsfb_table[4][index];
    }
    else {
      scalefac_compress -= 244;

      slen[0] = scalefac_compress / 3;
      slen[1] = scalefac_compress % 3;
      slen[2] = 0;
      slen[3] = 0;

      nsfb = nsfb_table[5][index];
    }

    n = 0;
    for (part = 0; part < 4; ++part) {
      unsigned int max, is_pos;

      max = (1 << slen[part]) - 1;

      for (i = 0; i < nsfb[part]; ++i) {
	is_pos = mad_bit_read(ptr, slen[part]);

	channel->scalefac[n] = is_pos;
	gr1ch->scalefac[n++] = (is_pos == max);
      }
    }

    while (n < 39) {
      channel->scalefac[n] = 0;
      gr1ch->scalefac[n++] = 0;   
    }
  }

  return mad_bit_length(&start, ptr);
}

 
static
unsigned int III_scalefactors(struct mad_bitptr *ptr, struct channel *channel,
			      struct channel const *gr0ch, unsigned int scfsi)
{
  struct mad_bitptr start;
  unsigned int slen1, slen2, sfbi;

  start = *ptr;

  slen1 = sflen_table[channel->scalefac_compress].slen1;
  slen2 = sflen_table[channel->scalefac_compress].slen2;

  if (channel->block_type == 2) {
    unsigned int nsfb;

    sfbi = 0;

    nsfb = (channel->flags & mixed_block_flag) ? 8 + 3 * 3 : 6 * 3;
    while (nsfb--)
      channel->scalefac[sfbi++] = mad_bit_read(ptr, slen1);

    nsfb = 6 * 3;
    while (nsfb--)
      channel->scalefac[sfbi++] = mad_bit_read(ptr, slen2);

    nsfb = 1 * 3;
    while (nsfb--)
      channel->scalefac[sfbi++] = 0;
  }
  else {   
    if (scfsi & 0x8) {
      for (sfbi = 0; sfbi < 6; ++sfbi)
	channel->scalefac[sfbi] = gr0ch->scalefac[sfbi];
    }
    else {
      for (sfbi = 0; sfbi < 6; ++sfbi)
	channel->scalefac[sfbi] = mad_bit_read(ptr, slen1);
    }

    if (scfsi & 0x4) {
      for (sfbi = 6; sfbi < 11; ++sfbi)
	channel->scalefac[sfbi] = gr0ch->scalefac[sfbi];
    }
    else {
      for (sfbi = 6; sfbi < 11; ++sfbi)
	channel->scalefac[sfbi] = mad_bit_read(ptr, slen1);
    }

    if (scfsi & 0x2) {
      for (sfbi = 11; sfbi < 16; ++sfbi)
	channel->scalefac[sfbi] = gr0ch->scalefac[sfbi];
    }
    else {
      for (sfbi = 11; sfbi < 16; ++sfbi)
	channel->scalefac[sfbi] = mad_bit_read(ptr, slen2);
    }

    if (scfsi & 0x1) {
      for (sfbi = 16; sfbi < 21; ++sfbi)
	channel->scalefac[sfbi] = gr0ch->scalefac[sfbi];
    }
    else {
      for (sfbi = 16; sfbi < 21; ++sfbi)
	channel->scalefac[sfbi] = mad_bit_read(ptr, slen2);
    }

    channel->scalefac[21] = 0;
  }

  return mad_bit_length(&start, ptr);
}

 

 
static
void III_exponents(struct channel const *channel,
		   unsigned char const *sfbwidth, signed int exponents[39])
{
  signed int gain;
  unsigned int scalefac_multiplier, sfbi;

  gain = (signed int) channel->global_gain - 210;
  scalefac_multiplier = (channel->flags & scalefac_scale) ? 2 : 1;

  if (channel->block_type == 2) {
    unsigned int l;
    signed int gain0, gain1, gain2;

    sfbi = l = 0;

    if (channel->flags & mixed_block_flag) {
      unsigned int premask;

      premask = (channel->flags & preflag) ? ~0 : 0;

       

      while (l < 36) {
	exponents[sfbi] = gain -
	  (signed int) ((channel->scalefac[sfbi] + (pretab[sfbi] & premask)) <<
			scalefac_multiplier);

	l += sfbwidth[sfbi++];
      }
    }

     

    gain0 = gain - 8 * (signed int) channel->subblock_gain[0];
    gain1 = gain - 8 * (signed int) channel->subblock_gain[1];
    gain2 = gain - 8 * (signed int) channel->subblock_gain[2];

    while (l < 576) {
      exponents[sfbi + 0] = gain0 -
	(signed int) (channel->scalefac[sfbi + 0] << scalefac_multiplier);
      exponents[sfbi + 1] = gain1 -
	(signed int) (channel->scalefac[sfbi + 1] << scalefac_multiplier);
      exponents[sfbi + 2] = gain2 -
	(signed int) (channel->scalefac[sfbi + 2] << scalefac_multiplier);

      l    += 3 * sfbwidth[sfbi];
      sfbi += 3;
    }
  }
  else {   
    if (channel->flags & preflag) {
      for (sfbi = 0; sfbi < 22; ++sfbi) {
	exponents[sfbi] = gain -
	  (signed int) ((channel->scalefac[sfbi] + pretab[sfbi]) <<
			scalefac_multiplier);
      }
    }
    else {
      for (sfbi = 0; sfbi < 22; ++sfbi) {
	exponents[sfbi] = gain -
	  (signed int) (channel->scalefac[sfbi] << scalefac_multiplier);
      }
    }
  }
}

 
static
mad_fixed_t III_requantize(unsigned int value, signed int exp)
{
  mad_fixed_t requantized;
  signed int frac;
  struct fixedfloat const *power;

  frac = exp % 4;   
  exp /= 4;

  power = &rq_table[value];
  requantized = power->mantissa;
  exp += power->exponent;

  if (exp < 0) {
    if (-exp >= sizeof(mad_fixed_t) * CHAR_BIT) {
       
      requantized = 0;
    }
    else {
      requantized += 1L << (-exp - 1);
      requantized >>= -exp;
    }
  }
  else {
    if (exp >= 5) {
       
# if defined(DEBUG)
      fprintf(stderr, "requantize overflow (%f * 2^%d)\n",
	      mad_f_todouble(requantized), exp);
# endif
      requantized = MAD_F_MAX;
    }
    else
      requantized <<= exp;
  }
  


  return frac ? mad_f_mul(requantized, root_table[3 + frac]) : requantized;
}

 
# define MASK(cache, sz, bits)	\
    (((cache) >> ((sz) - (bits))) & ((1 << (bits)) - 1))
# define MASK1BIT(cache, sz)  \
    ((cache) & (1 << ((sz) - 1)))

 
static
enum mad_error III_huffdecode(struct mad_bitptr *ptr, mad_fixed_t xr[576],
			      struct channel *channel,
			      unsigned char const *sfbwidth,
			      unsigned int part2_length)
{
  signed int exponents[39], exp;
  signed int const *expptr;
  struct mad_bitptr peek;
  signed int bits_left, cachesz;
  register mad_fixed_t *xrptr;
  mad_fixed_t const *sfbound;
  register unsigned long bitcache;

  bits_left = (signed) channel->part2_3_length - (signed) part2_length;
  if (bits_left < 0)
    return MAD_ERROR_BADPART3LEN;

  III_exponents(channel, sfbwidth, exponents);

  peek = *ptr;
  mad_bit_skip(ptr, bits_left);

   
  cachesz  = mad_bit_bitsleft(&peek);
  cachesz += ((32 - 1 - 24) + (24 - cachesz)) & ~7;

  bitcache   = mad_bit_read(&peek, cachesz);
  bits_left -= cachesz;

  xrptr = &xr[0];

   
  {
    unsigned int region, rcount;
    struct hufftable const *entry;
    union huffpair const *table;
    unsigned int linbits, startbits, big_values, reqhits;
    mad_fixed_t reqcache[16];

    sfbound = xrptr + *sfbwidth++;
    rcount  = channel->region0_count + 1;

    entry     = &mad_huff_pair_table[channel->table_select[region = 0]];
    table     = entry->table;
    linbits   = entry->linbits;
    startbits = entry->startbits;

    if (table == 0)
      return MAD_ERROR_BADHUFFTABLE;

    expptr  = &exponents[0];
    exp     = *expptr++;
    reqhits = 0;

    big_values = channel->big_values;

    while (big_values-- && cachesz + bits_left > 0) {
      union huffpair const *pair;
      unsigned int clumpsz, value;
      register mad_fixed_t requantized;

      if (xrptr == sfbound) {
	sfbound += *sfbwidth++;

	 

	if (--rcount == 0) {
	  if (region == 0)
	    rcount = channel->region1_count + 1;
	  else
	    rcount = 0;   

	  entry     = &mad_huff_pair_table[channel->table_select[++region]];
	  table     = entry->table;
	  linbits   = entry->linbits;
	  startbits = entry->startbits;

	  if (table == 0)
	    return MAD_ERROR_BADHUFFTABLE;
	}

	if (exp != *expptr) {
	  exp = *expptr;
	  reqhits = 0;
	}

	++expptr;
      }

      if (cachesz < 21) {
	unsigned int bits;

	bits       = ((32 - 1 - 21) + (21 - cachesz)) & ~7;
	bitcache   = (bitcache << bits) | mad_bit_read(&peek, bits);
	cachesz   += bits;
	bits_left -= bits;
      }

       

      clumpsz = startbits;
      pair    = &table[MASK(bitcache, cachesz, clumpsz)];

      while (!pair->final) {
	cachesz -= clumpsz;

	clumpsz = pair->ptr.bits;
	pair    = &table[pair->ptr.offset + MASK(bitcache, cachesz, clumpsz)];
      }

      cachesz -= pair->value.hlen;

      if (linbits) {
	 

	value = pair->value.x;

	switch (value) {
	case 0:
	  xrptr[0] = 0;
	  break;

	case 15:
	  if (cachesz < linbits + 2) {
	    bitcache   = (bitcache << 16) | mad_bit_read(&peek, 16);
	    cachesz   += 16;
	    bits_left -= 16;
	  }

	  value += MASK(bitcache, cachesz, linbits);
	  cachesz -= linbits;

	  requantized = III_requantize(value, exp);
	  goto x_final;

	default:
	  if (reqhits & (1 << value))
	    requantized = reqcache[value];
	  else {
	    reqhits |= (1 << value);
	    requantized = reqcache[value] = III_requantize(value, exp);
	  }

	x_final:
	  xrptr[0] = MASK1BIT(bitcache, cachesz--) ?
	    -requantized : requantized;
	}

	 

	value = pair->value.y;

	switch (value) {
	case 0:
	  xrptr[1] = 0;
	  break;

	case 15:
	  if (cachesz < linbits + 1) {
	    bitcache   = (bitcache << 16) | mad_bit_read(&peek, 16);
	    cachesz   += 16;
	    bits_left -= 16;
	  }

	  value += MASK(bitcache, cachesz, linbits);
	  cachesz -= linbits;

	  requantized = III_requantize(value, exp);
	  goto y_final;

	default:
	  if (reqhits & (1 << value))
	    requantized = reqcache[value];
	  else {
	    reqhits |= (1 << value);
	    requantized = reqcache[value] = III_requantize(value, exp);
	  }

	y_final:
	  xrptr[1] = MASK1BIT(bitcache, cachesz--) ?
	    -requantized : requantized;
	}
      }
      else {
	 

	value = pair->value.x;

	if (value == 0)
	  xrptr[0] = 0;
	else {
	  if (reqhits & (1 << value))
	    requantized = reqcache[value];
	  else {
	    reqhits |= (1 << value);
	    requantized = reqcache[value] = III_requantize(value, exp);
	  }

	  xrptr[0] = MASK1BIT(bitcache, cachesz--) ?
	    -requantized : requantized;
	}

	 

	value = pair->value.y;

	if (value == 0)
	  xrptr[1] = 0;
	else {
	  if (reqhits & (1 << value))
	    requantized = reqcache[value];
	  else {
	    reqhits |= (1 << value);
	    requantized = reqcache[value] = III_requantize(value, exp);
	  }

	  xrptr[1] = MASK1BIT(bitcache, cachesz--) ?
	    -requantized : requantized;
	}
      }

      xrptr += 2;
    }
  }

  if (cachesz + bits_left < 0)
    return MAD_ERROR_BADHUFFDATA;   

   
  {
    union huffquad const *table;
    register mad_fixed_t requantized;

    table = mad_huff_quad_table[channel->flags & count1table_select];

    requantized = III_requantize(1, exp);

    while (cachesz + bits_left > 0 && xrptr <= &xr[572]) {
      union huffquad const *quad;

       

      if (cachesz < 10) {
	bitcache   = (bitcache << 16) | mad_bit_read(&peek, 16);
	cachesz   += 16;
	bits_left -= 16;
      }

      quad = &table[MASK(bitcache, cachesz, 4)];

       
      if (!quad->final) {
	cachesz -= 4;

	quad = &table[quad->ptr.offset +
		      MASK(bitcache, cachesz, quad->ptr.bits)];
      }

      cachesz -= quad->value.hlen;

      if (xrptr == sfbound) {
	sfbound += *sfbwidth++;

	if (exp != *expptr) {
	  exp = *expptr;
	  requantized = III_requantize(1, exp);
	}

	++expptr;
      }

       

      xrptr[0] = quad->value.v ?
	(MASK1BIT(bitcache, cachesz--) ? -requantized : requantized) : 0;

       

      xrptr[1] = quad->value.w ?
	(MASK1BIT(bitcache, cachesz--) ? -requantized : requantized) : 0;

      xrptr += 2;

      if (xrptr == sfbound) {
	sfbound += *sfbwidth++;

	if (exp != *expptr) {
	  exp = *expptr;
	  requantized = III_requantize(1, exp);
	}

	++expptr;
      }

       

      xrptr[0] = quad->value.x ?
	(MASK1BIT(bitcache, cachesz--) ? -requantized : requantized) : 0;

       

      xrptr[1] = quad->value.y ?
	(MASK1BIT(bitcache, cachesz--) ? -requantized : requantized) : 0;

      xrptr += 2;
    }

    if (cachesz + bits_left < 0) {
# if 0 && defined(DEBUG)
      fprintf(stderr, "huffman count1 overrun (%d bits)\n",
	      -(cachesz + bits_left));
# endif

       

      xrptr -= 4;
    }
  }


 

# if 0 && defined(DEBUG)
  if (bits_left < 0)
    fprintf(stderr, "read %d bits too many\n", -bits_left);
  else if (cachesz + bits_left > 0)
    fprintf(stderr, "%d stuffing bits\n", cachesz + bits_left);
# endif

   
  while (xrptr < &xr[576]) {
    xrptr[0] = 0;
    xrptr[1] = 0;

    xrptr += 2;
  }

  return MAD_ERROR_NONE;
}

# undef MASK
# undef MASK1BIT

 
static
void III_reorder(mad_fixed_t xr[576], struct channel const *channel,
		 unsigned char  sfbwidth[39])
{
  mad_fixed_t tmp[32][3][6];
  unsigned int sb, l, f, w, sbw[3], sw[3];

   

  sb = 0;
  if (channel->flags & mixed_block_flag) {
    sb = 2;

    l = 0;
    while (l < 36)
      l += *sfbwidth++;
  }

  for (w = 0; w < 3; ++w) {
    sbw[w] = sb;
    sw[w]  = 0;
  }

  f = *sfbwidth++;
  w = 0;

  for (l = 18 * sb; l < 576; ++l) {
    if (f-- == 0) {
      f = *sfbwidth++ - 1;
      w = (w + 1) % 3;
    }

    tmp[sbw[w]][w][sw[w]++] = xr[l];

    if (sw[w] == 6) {
      sw[w] = 0;
      ++sbw[w];
    }
  }

  CopyMemory(&xr[18 * sb], &tmp[sb], (576 - 18 * sb) * sizeof(mad_fixed_t));
}

 
static
enum mad_error III_stereo(mad_fixed_t xr[2][576],
			  struct granule const *granule,
			  struct mad_header *header,
			  unsigned char const *sfbwidth)
{
  short modes[39];
  unsigned int sfbi, l, n, i;

  if (granule->ch[0].block_type !=
      granule->ch[1].block_type ||
      (granule->ch[0].flags & mixed_block_flag) !=
      (granule->ch[1].flags & mixed_block_flag))
    return MAD_ERROR_BADSTEREO;

  for (i = 0; i < 39; ++i)
    modes[i] = header->mode_extension;

   

  if (header->mode_extension & I_STEREO) {
    struct channel const *right_ch = &granule->ch[1];
    mad_fixed_t const *right_xr = xr[1];
    unsigned int is_pos;

    header->flags |= MAD_FLAG_I_STEREO;

     

    if (right_ch->block_type == 2) {
      unsigned int lower, start, max, bound[3], w;

      lower = start = max = bound[0] = bound[1] = bound[2] = 0;

      sfbi = l = 0;

      if (right_ch->flags & mixed_block_flag) {
	while (l < 36) {
	  n = sfbwidth[sfbi++];

	  for (i = 0; i < n; ++i) {
	    if (right_xr[i]) {
	      lower = sfbi;
	      break;
	    }
	  }

	  right_xr += n;
	  l += n;
	}

	start = sfbi;
      }

      w = 0;
      while (l < 576) {
	n = sfbwidth[sfbi++];

	for (i = 0; i < n; ++i) {
	  if (right_xr[i]) {
	    max = bound[w] = sfbi;
	    break;
	  }
	}

	right_xr += n;
	l += n;
	w = (w + 1) % 3;
      }

      if (max)
	lower = start;

       

      for (i = 0; i < lower; ++i)
	modes[i] = header->mode_extension & ~I_STEREO;

       

      w = 0;
      for (i = start; i < max; ++i) {
	if (i < bound[w])
	  modes[i] = header->mode_extension & ~I_STEREO;

	w = (w + 1) % 3;
      }
    }
    else {   
      unsigned int bound;

      bound = 0;
      for (sfbi = l = 0; l < 576; l += n) {
	n = sfbwidth[sfbi++];

	for (i = 0; i < n; ++i) {
	  if (right_xr[i]) {
	    bound = sfbi;
	    break;
	  }
	}

	right_xr += n;
      }

      for (i = 0; i < bound; ++i)
	modes[i] = header->mode_extension & ~I_STEREO;
    }

     

    if (header->flags & MAD_FLAG_LSF_EXT) {
      unsigned char const *illegal_pos = granule[1].ch[1].scalefac;
      mad_fixed_t const *lsf_scale;

       
      lsf_scale = is_lsf_table[right_ch->scalefac_compress & 0x1];

      for (sfbi = l = 0; l < 576; ++sfbi, l += n) {
	n = sfbwidth[sfbi];

	if (!(modes[sfbi] & I_STEREO))
	  continue;

	if (illegal_pos[sfbi]) {
	  modes[sfbi] &= ~I_STEREO;
	  continue;
	}

	is_pos = right_ch->scalefac[sfbi];

	for (i = 0; i < n; ++i) {
	  register mad_fixed_t left;

	  left = xr[0][l + i];

	  if (is_pos == 0)
	    xr[1][l + i] = left;
	  else {
	    register mad_fixed_t opposite;

	    opposite = mad_f_mul(left, lsf_scale[(is_pos - 1) / 2]);

	    if (is_pos & 1) {
	      xr[0][l + i] = opposite;
	      xr[1][l + i] = left;
	    }
	    else
	      xr[1][l + i] = opposite;
	  }
	}
      }
    }
    else {   
      for (sfbi = l = 0; l < 576; ++sfbi, l += n) {
	n = sfbwidth[sfbi];

	if (!(modes[sfbi] & I_STEREO))
	  continue;

	is_pos = right_ch->scalefac[sfbi];

	if (is_pos >= 7) {   
	  modes[sfbi] &= ~I_STEREO;
	  continue;
	}

	for (i = 0; i < n; ++i) {
	  register mad_fixed_t left;

	  left = xr[0][l + i];

	  xr[0][l + i] = mad_f_mul(left, is_table[    is_pos]);
	  xr[1][l + i] = mad_f_mul(left, is_table[6 - is_pos]);
	}
      }
    }
  }

   

  if (header->mode_extension & MS_STEREO) {
    register mad_fixed_t invsqrt2;

    header->flags |= MAD_FLAG_MS_STEREO;

    invsqrt2 = root_table[3 + -2];

    for (sfbi = l = 0; l < 576; ++sfbi, l += n) {
      n = sfbwidth[sfbi];

      if (modes[sfbi] != MS_STEREO)
	continue;

      for (i = 0; i < n; ++i) {
	register mad_fixed_t m, s;

	m = xr[0][l + i];
	s = xr[1][l + i];

	xr[0][l + i] = mad_f_mul(m + s, invsqrt2);   
	xr[1][l + i] = mad_f_mul(m - s, invsqrt2);   
      }
    }
  }

  return MAD_ERROR_NONE;
}

 
static
void III_aliasreduce(mad_fixed_t xr[576], int lines)
{
  mad_fixed_t const *bound;
  int i;

  bound = &xr[lines];
  for (xr += 18; xr < bound; xr += 18) {
    for (i = 0; i < 8; ++i) {
      register mad_fixed_t a, b;
      register mad_fixed64hi_t hi;
      register mad_fixed64lo_t lo;

      a = xr[-1 - i];
      b = xr[     i];

# if defined(ASO_ZEROCHECK)
      if (a | b) {
# endif
	MAD_F_ML0(hi, lo,  a, cs[i]);
	MAD_F_MLA(hi, lo, -b, ca[i]);

	xr[-1 - i] = MAD_F_MLZ(hi, lo);

	MAD_F_ML0(hi, lo,  b, cs[i]);
	MAD_F_MLA(hi, lo,  a, ca[i]);

	xr[     i] = MAD_F_MLZ(hi, lo);
# if defined(ASO_ZEROCHECK)
      }
# endif
    }
  }
}

# if defined(ASO_IMDCT)
void III_imdct_l(mad_fixed_t const [18], mad_fixed_t [36], unsigned int);
# else
#  if 1
static
void fastsdct(mad_fixed_t const x[9], mad_fixed_t y[18])
{
  mad_fixed_t a0,  a1,  a2,  a3,  a4,  a5,  a6,  a7,  a8,  a9,  a10, a11, a12;
  mad_fixed_t a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25;
  mad_fixed_t m0,  m1,  m2,  m3,  m4,  m5,  m6,  m7;

  enum {
    c0 =  MAD_F(0x1f838b8d),   
    c1 =  MAD_F(0x1bb67ae8),   
    c2 =  MAD_F(0x18836fa3),   
    c3 =  MAD_F(0x1491b752),   
    c4 =  MAD_F(0x0af1d43a),   
    c5 =  MAD_F(0x058e86a0),   
    c6 = -MAD_F(0x1e11f642)    
  };

  a0 = x[3] + x[5];
  a1 = x[3] - x[5];
  a2 = x[6] + x[2];
  a3 = x[6] - x[2];
  a4 = x[1] + x[7];
  a5 = x[1] - x[7];
  a6 = x[8] + x[0];
  a7 = x[8] - x[0];

  a8  = a0  + a2;
  a9  = a0  - a2;
  a10 = a0  - a6;
  a11 = a2  - a6;
  a12 = a8  + a6;
  a13 = a1  - a3;
  a14 = a13 + a7;
  a15 = a3  + a7;
  a16 = a1  - a7;
  a17 = a1  + a3;

  m0 = mad_f_mul(a17, -c3);
  m1 = mad_f_mul(a16, -c0);
  m2 = mad_f_mul(a15, -c4);
  m3 = mad_f_mul(a14, -c1);
  m4 = mad_f_mul(a5,  -c1);
  m5 = mad_f_mul(a11, -c6);
  m6 = mad_f_mul(a10, -c5);
  m7 = mad_f_mul(a9,  -c2);

  a18 =     x[4] + a4;
  a19 = 2 * x[4] - a4;
  a20 = a19 + m5;
  a21 = a19 - m5;
  a22 = a19 + m6;
  a23 = m4  + m2;
  a24 = m4  - m2;
  a25 = m4  + m1;

   

  y[ 0] = a18 + a12;
  y[ 2] = m0  - a25;
  y[ 4] = m7  - a20;
  y[ 6] = m3;
  y[ 8] = a21 - m6;
  y[10] = a24 - m1;
  y[12] = a12 - 2 * a18;
  y[14] = a23 + m0;
  y[16] = a22 + m7;
}


 
   
static inline void sdctII(mad_fixed_t const x[18], mad_fixed_t X[18])
{
  mad_fixed_t tmp[9];
  int i;

   
  static mad_fixed_t const scale[9] = {
    MAD_F(0x1fe0d3b4), MAD_F(0x1ee8dd47), MAD_F(0x1d007930),
    MAD_F(0x1a367e59), MAD_F(0x16a09e66), MAD_F(0x125abcf8),
    MAD_F(0x0d8616bc), MAD_F(0x08483ee1), MAD_F(0x02c9fad7)
  };

   

   

  for (i = 0; i < 9; i += 3) {
    tmp[i + 0] = x[i + 0] + x[18 - (i + 0) - 1];
    tmp[i + 1] = x[i + 1] + x[18 - (i + 1) - 1];
    tmp[i + 2] = x[i + 2] + x[18 - (i + 2) - 1];
  }

  fastsdct(tmp, &X[0]);

   

  for (i = 0; i < 9; i += 3) {
    tmp[i + 0] = mad_f_mul(x[i + 0] - x[18 - (i + 0) - 1], scale[i + 0]);
    tmp[i + 1] = mad_f_mul(x[i + 1] - x[18 - (i + 1) - 1], scale[i + 1]);
    tmp[i + 2] = mad_f_mul(x[i + 2] - x[18 - (i + 2) - 1], scale[i + 2]);
  }

  fastsdct(tmp, &X[1]);

   

  for (i = 3; i < 18; i += 8) {
    X[i + 0] -= X[(i + 0) - 2];
    X[i + 2] -= X[(i + 2) - 2];
    X[i + 4] -= X[(i + 4) - 2];
    X[i + 6] -= X[(i + 6) - 2];
  }
}

static inline
void dctIV(mad_fixed_t const y[18], mad_fixed_t X[18])
{
  mad_fixed_t tmp[18];
  int i;

   
  static mad_fixed_t const scale[18] = {
    MAD_F(0x1ff833fa), MAD_F(0x1fb9ea93), MAD_F(0x1f3dd120),
    MAD_F(0x1e84d969), MAD_F(0x1d906bcf), MAD_F(0x1c62648b),
    MAD_F(0x1afd100f), MAD_F(0x1963268b), MAD_F(0x1797c6a4),
    MAD_F(0x159e6f5b), MAD_F(0x137af940), MAD_F(0x11318ef3),
    MAD_F(0x0ec6a507), MAD_F(0x0c3ef153), MAD_F(0x099f61c5),
    MAD_F(0x06ed12c5), MAD_F(0x042d4544), MAD_F(0x0165547c)
  };

   

  for (i = 0; i < 18; i += 3) {
    tmp[i + 0] = mad_f_mul(y[i + 0], scale[i + 0]);
    tmp[i + 1] = mad_f_mul(y[i + 1], scale[i + 1]);
    tmp[i + 2] = mad_f_mul(y[i + 2], scale[i + 2]);
  }

   

  sdctII(tmp, X);

   

  X[0] /= 2;
  for (i = 1; i < 17; i += 4) {
    X[i + 0] = X[i + 0] / 2 - X[(i + 0) - 1];
    X[i + 1] = X[i + 1] / 2 - X[(i + 1) - 1];
    X[i + 2] = X[i + 2] / 2 - X[(i + 2) - 1];
    X[i + 3] = X[i + 3] / 2 - X[(i + 3) - 1];
  }
  X[17] = X[17] / 2 - X[16];
}

 
static inline
void imdct36(mad_fixed_t const x[18], mad_fixed_t y[36])
{
  mad_fixed_t tmp[18];
  int i;

   

  dctIV(x, tmp);

   

  for (i =  0; i <  9; i += 3) {
    y[i + 0] =  tmp[9 + (i + 0)];
    y[i + 1] =  tmp[9 + (i + 1)];
    y[i + 2] =  tmp[9 + (i + 2)];
  }
  for (i =  9; i < 27; i += 3) {
    y[i + 0] = -tmp[36 - (9 + (i + 0)) - 1];
    y[i + 1] = -tmp[36 - (9 + (i + 1)) - 1];
    y[i + 2] = -tmp[36 - (9 + (i + 2)) - 1];
  }
  for (i = 27; i < 36; i += 3) {
    y[i + 0] = -tmp[(i + 0) - 27];
    y[i + 1] = -tmp[(i + 1) - 27];
    y[i + 2] = -tmp[(i + 2) - 27];
  }
}
#  else
 
static inline
void imdct36(mad_fixed_t const X[18], mad_fixed_t x[36])
{
  mad_fixed_t t0, t1, t2,  t3,  t4,  t5,  t6,  t7;
  mad_fixed_t t8, t9, t10, t11, t12, t13, t14, t15;
  register mad_fixed64hi_t hi;
  register mad_fixed64lo_t lo;

  MAD_F_ML0(hi, lo, X[4],  MAD_F(0x0ec835e8));
  MAD_F_MLA(hi, lo, X[13], MAD_F(0x061f78aa));

  t6 = MAD_F_MLZ(hi, lo);

  MAD_F_MLA(hi, lo, (t14 = X[1] - X[10]), -MAD_F(0x061f78aa));
  MAD_F_MLA(hi, lo, (t15 = X[7] + X[16]), -MAD_F(0x0ec835e8));

  t0 = MAD_F_MLZ(hi, lo);

  MAD_F_MLA(hi, lo, (t8  = X[0] - X[11] - X[12]),  MAD_F(0x0216a2a2));
  MAD_F_MLA(hi, lo, (t9  = X[2] - X[9]  - X[14]),  MAD_F(0x09bd7ca0));
  MAD_F_MLA(hi, lo, (t10 = X[3] - X[8]  - X[15]), -MAD_F(0x0cb19346));
  MAD_F_MLA(hi, lo, (t11 = X[5] - X[6]  - X[17]), -MAD_F(0x0fdcf549));

  x[7]  = MAD_F_MLZ(hi, lo);
  x[10] = -x[7];

  MAD_F_ML0(hi, lo, t8,  -MAD_F(0x0cb19346));
  MAD_F_MLA(hi, lo, t9,   MAD_F(0x0fdcf549));
  MAD_F_MLA(hi, lo, t10,  MAD_F(0x0216a2a2));
  MAD_F_MLA(hi, lo, t11, -MAD_F(0x09bd7ca0));

  x[19] = x[34] = MAD_F_MLZ(hi, lo) - t0;

  t12 = X[0] - X[3] + X[8] - X[11] - X[12] + X[15];
  t13 = X[2] + X[5] - X[6] - X[9]  - X[14] - X[17];

  MAD_F_ML0(hi, lo, t12, -MAD_F(0x0ec835e8));
  MAD_F_MLA(hi, lo, t13,  MAD_F(0x061f78aa));

  x[22] = x[31] = MAD_F_MLZ(hi, lo) + t0;

  MAD_F_ML0(hi, lo, X[1],  -MAD_F(0x09bd7ca0));
  MAD_F_MLA(hi, lo, X[7],   MAD_F(0x0216a2a2));
  MAD_F_MLA(hi, lo, X[10], -MAD_F(0x0fdcf549));
  MAD_F_MLA(hi, lo, X[16],  MAD_F(0x0cb19346));

  t1 = MAD_F_MLZ(hi, lo) + t6;

  MAD_F_ML0(hi, lo, X[0],   MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[2],   MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[3],  -MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[5],  -MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[6],   MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[8],  -MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[9],   MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[11],  MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[12], -MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[14],  MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[15], -MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[17], -MAD_F(0x0f9ee890));

  x[6]  = MAD_F_MLZ(hi, lo) + t1;
  x[11] = -x[6];

  MAD_F_ML0(hi, lo, X[0],  -MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[2],  -MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[3],   MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[5],   MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[6],   MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[8],  -MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[9],  -MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[11], -MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[12], -MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[14],  MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[15],  MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[17],  MAD_F(0x04cfb0e2));

  x[23] = x[30] = MAD_F_MLZ(hi, lo) + t1;

  MAD_F_ML0(hi, lo, X[0],  -MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[2],   MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[3],  -MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[5],   MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[6],   MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[8],  -MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[9],  -MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[11],  MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[12], -MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[14],  MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[15],  MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[17], -MAD_F(0x0acf37ad));

  x[18] = x[35] = MAD_F_MLZ(hi, lo) - t1;

  MAD_F_ML0(hi, lo, X[4],   MAD_F(0x061f78aa));
  MAD_F_MLA(hi, lo, X[13], -MAD_F(0x0ec835e8));

  t7 = MAD_F_MLZ(hi, lo);

  MAD_F_MLA(hi, lo, X[1],  -MAD_F(0x0cb19346));
  MAD_F_MLA(hi, lo, X[7],   MAD_F(0x0fdcf549));
  MAD_F_MLA(hi, lo, X[10],  MAD_F(0x0216a2a2));
  MAD_F_MLA(hi, lo, X[16], -MAD_F(0x09bd7ca0));

  t2 = MAD_F_MLZ(hi, lo);

  MAD_F_MLA(hi, lo, X[0],   MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[2],   MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[3],  -MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[5],   MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[6],  -MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[8],  -MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[9],   MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[11], -MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[12],  MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[14],  MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[15],  MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[17],  MAD_F(0x0f426cb5));

  x[5]  = MAD_F_MLZ(hi, lo);
  x[12] = -x[5];

  MAD_F_ML0(hi, lo, X[0],   MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[2],  -MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[3],   MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[5],  -MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[6],  -MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[8],   MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[9],  -MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[11],  MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[12], -MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[14],  MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[15],  MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[17], -MAD_F(0x0bcbe352));

  x[0]  = MAD_F_MLZ(hi, lo) + t2;
  x[17] = -x[0];

  MAD_F_ML0(hi, lo, X[0],  -MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[2],  -MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[3],  -MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[5],   MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[6],   MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[8],   MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[9],   MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[11], -MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[12], -MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[14], -MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[15], -MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[17], -MAD_F(0x03768962));

  x[24] = x[29] = MAD_F_MLZ(hi, lo) + t2;

  MAD_F_ML0(hi, lo, X[1],  -MAD_F(0x0216a2a2));
  MAD_F_MLA(hi, lo, X[7],  -MAD_F(0x09bd7ca0));
  MAD_F_MLA(hi, lo, X[10],  MAD_F(0x0cb19346));
  MAD_F_MLA(hi, lo, X[16],  MAD_F(0x0fdcf549));

  t3 = MAD_F_MLZ(hi, lo) + t7;

  MAD_F_ML0(hi, lo, X[0],   MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[2],   MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[3],  -MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[5],  -MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[6],   MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[8],   MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[9],  -MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[11], -MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[12],  MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[14],  MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[15], -MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[17], -MAD_F(0x0ffc19fd));

  x[8] = MAD_F_MLZ(hi, lo) + t3;
  x[9] = -x[8];

  MAD_F_ML0(hi, lo, X[0],  -MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[2],   MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[3],   MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[5],  -MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[6],  -MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[8],   MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[9],   MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[11], -MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[12], -MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[14], -MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[15],  MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[17],  MAD_F(0x07635284));

  x[21] = x[32] = MAD_F_MLZ(hi, lo) + t3;

  MAD_F_ML0(hi, lo, X[0],  -MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[2],   MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[3],   MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[5],  -MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[6],  -MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[8],   MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[9],   MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[11], -MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[12],  MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[14],  MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[15], -MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[17], -MAD_F(0x0898c779));

  x[20] = x[33] = MAD_F_MLZ(hi, lo) - t3;

  MAD_F_ML0(hi, lo, t14, -MAD_F(0x0ec835e8));
  MAD_F_MLA(hi, lo, t15,  MAD_F(0x061f78aa));

  t4 = MAD_F_MLZ(hi, lo) - t7;

  MAD_F_ML0(hi, lo, t12, MAD_F(0x061f78aa));
  MAD_F_MLA(hi, lo, t13, MAD_F(0x0ec835e8));

  x[4]  = MAD_F_MLZ(hi, lo) + t4;
  x[13] = -x[4];

  MAD_F_ML0(hi, lo, t8,   MAD_F(0x09bd7ca0));
  MAD_F_MLA(hi, lo, t9,  -MAD_F(0x0216a2a2));
  MAD_F_MLA(hi, lo, t10,  MAD_F(0x0fdcf549));
  MAD_F_MLA(hi, lo, t11, -MAD_F(0x0cb19346));

  x[1]  = MAD_F_MLZ(hi, lo) + t4;
  x[16] = -x[1];

  MAD_F_ML0(hi, lo, t8,  -MAD_F(0x0fdcf549));
  MAD_F_MLA(hi, lo, t9,  -MAD_F(0x0cb19346));
  MAD_F_MLA(hi, lo, t10, -MAD_F(0x09bd7ca0));
  MAD_F_MLA(hi, lo, t11, -MAD_F(0x0216a2a2));

  x[25] = x[28] = MAD_F_MLZ(hi, lo) + t4;

  MAD_F_ML0(hi, lo, X[1],  -MAD_F(0x0fdcf549));
  MAD_F_MLA(hi, lo, X[7],  -MAD_F(0x0cb19346));
  MAD_F_MLA(hi, lo, X[10], -MAD_F(0x09bd7ca0));
  MAD_F_MLA(hi, lo, X[16], -MAD_F(0x0216a2a2));

  t5 = MAD_F_MLZ(hi, lo) - t6;

  MAD_F_ML0(hi, lo, X[0],   MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[2],   MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[3],   MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[5],   MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[6],   MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[8],  -MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[9],   MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[11], -MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[12],  MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[14], -MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[15],  MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[17], -MAD_F(0x0d7e8807));

  x[2]  = MAD_F_MLZ(hi, lo) + t5;
  x[15] = -x[2];

  MAD_F_ML0(hi, lo, X[0],   MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[2],   MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[3],   MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[5],   MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[6],  -MAD_F(0x00b2aa3e));
  MAD_F_MLA(hi, lo, X[8],   MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[9],  -MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[11],  MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[12], -MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[14],  MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[15], -MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[17],  MAD_F(0x0e313245));

  x[3]  = MAD_F_MLZ(hi, lo) + t5;
  x[14] = -x[3];

  MAD_F_ML0(hi, lo, X[0],  -MAD_F(0x0ffc19fd));
  MAD_F_MLA(hi, lo, X[2],  -MAD_F(0x0f9ee890));
  MAD_F_MLA(hi, lo, X[3],  -MAD_F(0x0f426cb5));
  MAD_F_MLA(hi, lo, X[5],  -MAD_F(0x0e313245));
  MAD_F_MLA(hi, lo, X[6],  -MAD_F(0x0d7e8807));
  MAD_F_MLA(hi, lo, X[8],  -MAD_F(0x0bcbe352));
  MAD_F_MLA(hi, lo, X[9],  -MAD_F(0x0acf37ad));
  MAD_F_MLA(hi, lo, X[11], -MAD_F(0x0898c779));
  MAD_F_MLA(hi, lo, X[12], -MAD_F(0x07635284));
  MAD_F_MLA(hi, lo, X[14], -MAD_F(0x04cfb0e2));
  MAD_F_MLA(hi, lo, X[15], -MAD_F(0x03768962));
  MAD_F_MLA(hi, lo, X[17], -MAD_F(0x00b2aa3e));

  x[26] = x[27] = MAD_F_MLZ(hi, lo) + t5;
}
#  endif

 
static
void III_imdct_l(mad_fixed_t const X[18], mad_fixed_t z[36],
		 unsigned int block_type)
{
  unsigned int i;

   

  imdct36(X, z);

   

  switch (block_type) {
  case 0:   
# if defined(ASO_INTERLEAVE1)
    {
      register mad_fixed_t tmp1, tmp2;

      tmp1 = window_l[0];
      tmp2 = window_l[1];

      for (i = 0; i < 34; i += 2) {
	z[i + 0] = mad_f_mul(z[i + 0], tmp1);
	tmp1 = window_l[i + 2];
	z[i + 1] = mad_f_mul(z[i + 1], tmp2);
	tmp2 = window_l[i + 3];
      }

      z[34] = mad_f_mul(z[34], tmp1);
      z[35] = mad_f_mul(z[35], tmp2);
    }
# elif defined(ASO_INTERLEAVE2)
    {
      register mad_fixed_t tmp1, tmp2;

      tmp1 = z[0];
      tmp2 = window_l[0];

      for (i = 0; i < 35; ++i) {
	z[i] = mad_f_mul(tmp1, tmp2);
	tmp1 = z[i + 1];
	tmp2 = window_l[i + 1];
      }

      z[35] = mad_f_mul(tmp1, tmp2);
    }
# elif 1
    for (i = 0; i < 36; i += 4) {
      z[i + 0] = mad_f_mul(z[i + 0], window_l[i + 0]);
      z[i + 1] = mad_f_mul(z[i + 1], window_l[i + 1]);
      z[i + 2] = mad_f_mul(z[i + 2], window_l[i + 2]);
      z[i + 3] = mad_f_mul(z[i + 3], window_l[i + 3]);
    }
# else
    for (i =  0; i < 36; ++i) z[i] = mad_f_mul(z[i], window_l[i]);
# endif
    break;

  case 1:   
    for (i =  0; i < 18; i += 3) {
      z[i + 0] = mad_f_mul(z[i + 0], window_l[i + 0]);
      z[i + 1] = mad_f_mul(z[i + 1], window_l[i + 1]);
      z[i + 2] = mad_f_mul(z[i + 2], window_l[i + 2]);
    }
     
    for (i = 24; i < 30; ++i) z[i] = mad_f_mul(z[i], window_s[i - 18]);
    for (i = 30; i < 36; ++i) z[i] = 0;
    break;

  case 3:   
    for (i =  0; i <  6; ++i) z[i] = 0;
    for (i =  6; i < 12; ++i) z[i] = mad_f_mul(z[i], window_s[i - 6]);
     
    for (i = 18; i < 36; i += 3) {
      z[i + 0] = mad_f_mul(z[i + 0], window_l[i + 0]);
      z[i + 1] = mad_f_mul(z[i + 1], window_l[i + 1]);
      z[i + 2] = mad_f_mul(z[i + 2], window_l[i + 2]);
    }
    break;
  }
}
# endif   

 
static
void III_imdct_s(mad_fixed_t  X[18], mad_fixed_t z[36])
{
  mad_fixed_t y[36], *yptr;
  mad_fixed_t const *wptr;
  int w, i;
  register mad_fixed64hi_t hi;
  register mad_fixed64lo_t lo;

   

  yptr = &y[0];

  for (w = 0; w < 3; ++w) {
    register mad_fixed_t const (*s)[6];

    s = imdct_s;

    for (i = 0; i < 3; ++i) {
      MAD_F_ML0(hi, lo, X[0], (*s)[0]);
      MAD_F_MLA(hi, lo, X[1], (*s)[1]);
      MAD_F_MLA(hi, lo, X[2], (*s)[2]);
      MAD_F_MLA(hi, lo, X[3], (*s)[3]);
      MAD_F_MLA(hi, lo, X[4], (*s)[4]);
      MAD_F_MLA(hi, lo, X[5], (*s)[5]);

      yptr[i + 0] = MAD_F_MLZ(hi, lo);
      yptr[5 - i] = -yptr[i + 0];

      ++s;

      MAD_F_ML0(hi, lo, X[0], (*s)[0]);
      MAD_F_MLA(hi, lo, X[1], (*s)[1]);
      MAD_F_MLA(hi, lo, X[2], (*s)[2]);
      MAD_F_MLA(hi, lo, X[3], (*s)[3]);
      MAD_F_MLA(hi, lo, X[4], (*s)[4]);
      MAD_F_MLA(hi, lo, X[5], (*s)[5]);

      yptr[ i + 6] = MAD_F_MLZ(hi, lo);
      yptr[11 - i] = yptr[i + 6];

      ++s;
    }

    yptr += 12;
    X    += 6;
  }

   

  yptr = &y[0];
  wptr = &window_s[0];

  for (i = 0; i < 6; ++i) {
    z[i +  0] = 0;
    z[i +  6] = mad_f_mul(yptr[ 0 + 0], wptr[0]);

    MAD_F_ML0(hi, lo, yptr[ 0 + 6], wptr[6]);
    MAD_F_MLA(hi, lo, yptr[12 + 0], wptr[0]);

    z[i + 12] = MAD_F_MLZ(hi, lo);

    MAD_F_ML0(hi, lo, yptr[12 + 6], wptr[6]);
    MAD_F_MLA(hi, lo, yptr[24 + 0], wptr[0]);

    z[i + 18] = MAD_F_MLZ(hi, lo);

    z[i + 24] = mad_f_mul(yptr[24 + 6], wptr[6]);
    z[i + 30] = 0;

    ++yptr;
    ++wptr;
  }
}

 
static
void III_overlap(mad_fixed_t const output[36], mad_fixed_t overlap[18],
		 mad_fixed_t sample[18][32], unsigned int sb)
{
  unsigned int i;

# if defined(ASO_INTERLEAVE2)
  {
    register mad_fixed_t tmp1, tmp2;

    tmp1 = overlap[0];
    tmp2 = overlap[1];

    for (i = 0; i < 16; i += 2) {
      sample[i + 0][sb] = output[i + 0 +  0] + tmp1;
      overlap[i + 0]    = output[i + 0 + 18];
      tmp1 = overlap[i + 2];

      sample[i + 1][sb] = output[i + 1 +  0] + tmp2;
      overlap[i + 1]    = output[i + 1 + 18];
      tmp2 = overlap[i + 3];
    }

    sample[16][sb] = output[16 +  0] + tmp1;
    overlap[16]    = output[16 + 18];
    sample[17][sb] = output[17 +  0] + tmp2;
    overlap[17]    = output[17 + 18];
  }
# elif 0
  for (i = 0; i < 18; i += 2) {
    sample[i + 0][sb] = output[i + 0 +  0] + overlap[i + 0];
    overlap[i + 0]    = output[i + 0 + 18];

    sample[i + 1][sb] = output[i + 1 +  0] + overlap[i + 1];
    overlap[i + 1]    = output[i + 1 + 18];
  }
# else
  for (i = 0; i < 18; ++i) {
    sample[i][sb] = output[i +  0] + overlap[i];
    overlap[i]    = output[i + 18];
  }
# endif
}

 
static inline
void III_overlap_z(mad_fixed_t overlap[18],
		   mad_fixed_t sample[18][32], unsigned int sb)
{
  unsigned int i;

# if defined(ASO_INTERLEAVE2)
  {
    register mad_fixed_t tmp1, tmp2;

    tmp1 = overlap[0];
    tmp2 = overlap[1];

    for (i = 0; i < 16; i += 2) {
      sample[i + 0][sb] = tmp1;
      overlap[i + 0]    = 0;
      tmp1 = overlap[i + 2];

      sample[i + 1][sb] = tmp2;
      overlap[i + 1]    = 0;
      tmp2 = overlap[i + 3];
    }

    sample[16][sb] = tmp1;
    overlap[16]    = 0;
    sample[17][sb] = tmp2;
    overlap[17]    = 0;
  }
# else
  for (i = 0; i < 18; ++i) {
    sample[i][sb] = overlap[i];
    overlap[i]    = 0;
  }
# endif
}

 
static
void III_freqinver(mad_fixed_t sample[18][32], unsigned int sb)
{
  unsigned int i;

# if 1 || defined(ASO_INTERLEAVE1) || defined(ASO_INTERLEAVE2)
  {
    register mad_fixed_t tmp1, tmp2;

    tmp1 = sample[1][sb];
    tmp2 = sample[3][sb];

    for (i = 1; i < 13; i += 4) {
      sample[i + 0][sb] = -tmp1;
      tmp1 = sample[i + 4][sb];
      sample[i + 2][sb] = -tmp2;
      tmp2 = sample[i + 6][sb];
    }

    sample[13][sb] = -tmp1;
    tmp1 = sample[17][sb];
    sample[15][sb] = -tmp2;
    sample[17][sb] = -tmp1;
  }
# else
  for (i = 1; i < 18; i += 2)
    sample[i][sb] = -sample[i][sb];
# endif
}

 
static
enum mad_error III_decode(struct mad_bitptr *ptr, struct mad_frame *frame,
			  struct sideinfo *si, unsigned int nch)
{
  struct mad_header *header = &frame->header;
  unsigned int sfreqi, ngr, gr;

  {
    unsigned int sfreq;

    sfreq = header->samplerate;
    if (header->flags & MAD_FLAG_MPEG_2_5_EXT)
      sfreq *= 2;

     
    sfreqi = ((sfreq >>  7) & 0x000f) +
             ((sfreq >> 15) & 0x0001) - 8;

    if (header->flags & MAD_FLAG_MPEG_2_5_EXT)
      sfreqi += 3;
  }

   

  ngr = (header->flags & MAD_FLAG_LSF_EXT) ? 1 : 2;

  for (gr = 0; gr < ngr; ++gr) {
    struct granule *granule = &si->gr[gr];
    unsigned char const *sfbwidth[2];
    mad_fixed_t xr[2][576];
    unsigned int ch;
    enum mad_error error;

    for (ch = 0; ch < nch; ++ch) {
      struct channel *channel = &granule->ch[ch];
      unsigned int part2_length;

      sfbwidth[ch] = sfbwidth_table[sfreqi].l;
      if (channel->block_type == 2) {
	sfbwidth[ch] = (channel->flags & mixed_block_flag) ?
	  sfbwidth_table[sfreqi].m : sfbwidth_table[sfreqi].s;
      }

      if (header->flags & MAD_FLAG_LSF_EXT) {
	part2_length = III_scalefactors_lsf(ptr, channel,
					    ch == 0 ? 0 : &si->gr[1].ch[1],
					    header->mode_extension);
      }
      else {
	part2_length = III_scalefactors(ptr, channel, &si->gr[0].ch[ch],
					gr == 0 ? 0 : si->scfsi[ch]);
      }

      error = III_huffdecode(ptr, xr[ch], channel, sfbwidth[ch], part2_length);
      if (error)
	return error;
    }

     

    if (header->mode == MAD_MODE_JOINT_STEREO && header->mode_extension) {
      error = III_stereo(xr, granule, header, sfbwidth[0]);
      if (error)
	return error;
    }

     

    for (ch = 0; ch < nch; ++ch) {
      struct channel const *channel = &granule->ch[ch];
      mad_fixed_t (*sample)[32] = &frame->sbsample[ch][18 * gr];
      unsigned int sb, l, i, sblimit;
      mad_fixed_t output[36];

      if (channel->block_type == 2) {
	III_reorder(xr[ch], channel, (unsigned char*) sfbwidth[ch]);

# if !defined(OPT_STRICT)
	 
	if (channel->flags & mixed_block_flag)
	  III_aliasreduce(xr[ch], 36);
# endif
      }
      else
	III_aliasreduce(xr[ch], 576);

      l = 0;

       

      if (channel->block_type != 2 || (channel->flags & mixed_block_flag)) {
	unsigned int block_type;

	block_type = channel->block_type;
	if (channel->flags & mixed_block_flag)
	  block_type = 0;

	 
	for (sb = 0; sb < 2; ++sb, l += 18) {
	  III_imdct_l(&xr[ch][l], output, block_type);
	  III_overlap(output, (*frame->overlap)[ch][sb], sample, sb);
	}
      }
      else {
	 
	for (sb = 0; sb < 2; ++sb, l += 18) {
	  III_imdct_s(&xr[ch][l], output);
	  III_overlap(output, (*frame->overlap)[ch][sb], sample, sb);
	}
      }

      III_freqinver(sample, 1);

       

      i = 576;
      while (i > 36 && xr[ch][i - 1] == 0)
	--i;

      sblimit = 32 - (576 - i) / 18;

      if (channel->block_type != 2) {
	 
	for (sb = 2; sb < sblimit; ++sb, l += 18) {
	  III_imdct_l(&xr[ch][l], output, channel->block_type);
	  III_overlap(output, (*frame->overlap)[ch][sb], sample, sb);

	  if (sb & 1)
	    III_freqinver(sample, sb);
	}
      }
      else {
	 
	for (sb = 2; sb < sblimit; ++sb, l += 18) {
	  III_imdct_s(&xr[ch][l], output);
	  III_overlap(output, (*frame->overlap)[ch][sb], sample, sb);

	  if (sb & 1)
	    III_freqinver(sample, sb);
	}
      }

       

      for (sb = sblimit; sb < 32; ++sb) {
	III_overlap_z((*frame->overlap)[ch][sb], sample, sb);

	if (sb & 1)
	  III_freqinver(sample, sb);
      }
    }
  }

  return MAD_ERROR_NONE;
}

 
int mad_layer_III(struct mad_stream *stream, struct mad_frame *frame)
{
  struct mad_header *header = &frame->header;
  unsigned int nch, priv_bitlen, next_md_begin = 0;
  unsigned int si_len, data_bitlen, md_len;
  unsigned int frame_space, frame_used, frame_free;
  struct mad_bitptr ptr;
  struct sideinfo si;
  enum mad_error error;
  int result = 0;

   

  if (stream->main_data == 0) {
    stream->main_data = (unsigned char (*) [MAD_BUFFER_MDLEN]) LocalAlloc(0, MAD_BUFFER_MDLEN);
    if (stream->main_data == 0) {
      stream->error = MAD_ERROR_NOMEM;
      return -1;
    }
  }

  if (frame->overlap == 0) {
    frame->overlap = (mad_fixed_t (*)[2][32][18]) LocalAlloc(LPTR, 2 * 32 * 18 * sizeof(mad_fixed_t));
    if (frame->overlap == 0) {
      stream->error = MAD_ERROR_NOMEM;
      return -1;
    }
  }

  nch = MAD_NCHANNELS(header);
  si_len = (header->flags & MAD_FLAG_LSF_EXT) ?
    (nch == 1 ? 9 : 17) : (nch == 1 ? 17 : 32);

   

  if (stream->next_frame - mad_bit_nextbyte(&stream->ptr) <
      (signed int) si_len) {
    stream->error = MAD_ERROR_BADFRAMELEN;
    stream->md_len = 0;
    return -1;
  }

   

  if (header->flags & MAD_FLAG_PROTECTION) {
    header->crc_check =
      mad_bit_crc(stream->ptr, si_len * CHAR_BIT, header->crc_check);

    if (header->crc_check != header->crc_target &&
	!(frame->options & MAD_OPTION_IGNORECRC)) {
      stream->error = MAD_ERROR_BADCRC;
      result = -1;
    }
  }

   

  error = III_sideinfo(&stream->ptr, nch, header->flags & MAD_FLAG_LSF_EXT,
		       &si, &data_bitlen, &priv_bitlen);
  if (error && result == 0) {
    stream->error = error;
    result = -1;
  }

  header->flags        |= priv_bitlen;
  header->private_bits |= si.private_bits;

   

  {
    struct mad_bitptr peek;
    unsigned long header;

    mad_bit_init(&peek, stream->next_frame);

    header = mad_bit_read(&peek, 32);
    if ((header & 0xffe60000L)   == 0xffe20000L) {
      if (!(header & 0x00010000L))   
	mad_bit_skip(&peek, 16);   

      next_md_begin =
	mad_bit_read(&peek, (header & 0x00080000L)   ? 9 : 8);
    }

    mad_bit_finish(&peek);
  }

   

  frame_space = stream->next_frame - mad_bit_nextbyte(&stream->ptr);

  if (next_md_begin > si.main_data_begin + frame_space)
    next_md_begin = 0;

  md_len = si.main_data_begin + frame_space - next_md_begin;

  frame_used = 0;

  if (si.main_data_begin == 0) {
    ptr = stream->ptr;
    stream->md_len = 0;

    frame_used = md_len;
  }
  else {
    if (si.main_data_begin > stream->md_len) {
      if (result == 0) {
	stream->error = MAD_ERROR_BADDATAPTR;
	result = -1;
      }
    }
    else {
      mad_bit_init(&ptr,
		   *stream->main_data + stream->md_len - si.main_data_begin);

      if (md_len > si.main_data_begin) {

 

	CopyMemory(*stream->main_data + stream->md_len,
	       mad_bit_nextbyte(&stream->ptr),
	       frame_used = md_len - si.main_data_begin);
	stream->md_len += frame_used;
      }
    }
  }

  frame_free = frame_space - frame_used;

   

  if (result == 0) {
    error = III_decode(&ptr, frame, &si, nch);
    if (error) {
      stream->error = error;
      result = -1;
    }

     

    stream->anc_ptr    = ptr;
    stream->anc_bitlen = md_len * CHAR_BIT - data_bitlen;
  }

# if 0 && defined(DEBUG)
  fprintf(stderr,
	  "main_data_begin:%u, md_len:%u, frame_free:%u, "
	  "data_bitlen:%u, anc_bitlen: %u\n",
	  si.main_data_begin, md_len, frame_free,
	  data_bitlen, stream->anc_bitlen);
# endif

   

  if (frame_free >= next_md_begin) {
    CopyMemory(*stream->main_data,
	   stream->next_frame - next_md_begin, next_md_begin);
    stream->md_len = next_md_begin;
  }
  else {
    if (md_len < si.main_data_begin) {
      unsigned int extra;

      extra = si.main_data_begin - md_len;
      if (extra + frame_free > next_md_begin)
	extra = next_md_begin - frame_free;

      if (extra < stream->md_len) {
	MoveMemory(*stream->main_data,
		*stream->main_data + stream->md_len - extra, extra);
	stream->md_len = extra;
      }
    }
    else
      stream->md_len = 0;

    CopyMemory(*stream->main_data + stream->md_len,
	   stream->next_frame - frame_free, frame_free);
    stream->md_len += frame_free;
  }

  return result;
}



 


 

 
mad_fixed_t mad_f_abs(mad_fixed_t x)
{
  return x < 0 ? -x : x;
}

 
mad_fixed_t mad_f_div(mad_fixed_t x, mad_fixed_t y)
{
  mad_fixed_t q, r;
  unsigned int bits;

  q = mad_f_abs(x / y);

  if (x < 0) {
    x = -x;
    y = -y;
  }

  r = x % y;

  if (y < 0) {
    x = -x;
    y = -y;
  }

  if (q > mad_f_intpart(MAD_F_MAX) &&
      !(q == -mad_f_intpart(MAD_F_MIN) && r == 0 && (x < 0) != (y < 0)))
    return 0;

  for (bits = MAD_F_FRACBITS; bits && r; --bits) {
    q <<= 1, r <<= 1;
    if (r >= y)
      r -= y, ++q;
  }

   
  if (2 * r >= y)
    ++q;

   
  if ((x < 0) != (y < 0))
    q = -q;

  return q << bits;
}


 


 

static
unsigned long const bitrate_table[5][15] = {
   
  { 0,  32000,  64000,  96000, 128000, 160000, 192000, 224000,   
       256000, 288000, 320000, 352000, 384000, 416000, 448000 },
  { 0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,   
       128000, 160000, 192000, 224000, 256000, 320000, 384000 },
  { 0,  32000,  40000,  48000,  56000,  64000,  80000,  96000,   
       112000, 128000, 160000, 192000, 224000, 256000, 320000 },

   
  { 0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,   
       128000, 144000, 160000, 176000, 192000, 224000, 256000 },
  { 0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,   
        64000,  80000,  96000, 112000, 128000, 144000, 160000 }  
};

static
unsigned int const samplerate_table[3] = { 44100, 48000, 32000 };

static
int (*const decoder_table[3])(struct mad_stream *, struct mad_frame *) = {
  mad_layer_I,
  mad_layer_II,
  mad_layer_III
};

 
void mad_header_init(struct mad_header *header)
{

  header->layer          = 0;
  header->mode           = 0;
  header->mode_extension = 0;
  header->emphasis       = 0;

  header->bitrate        = 0;
  header->samplerate     = 0;

  header->crc_check      = 0;
  header->crc_target     = 0;

  header->flags          = 0;
  header->private_bits   = 0;

  

  
  header->duration       = mad_timer_zero;
}

 
void mad_frame_init(struct mad_frame *frame)
{
  mad_header_init(&frame->header);

  frame->options = 0;

  frame->overlap = 0;
  mad_frame_mute(frame);
}

 
void mad_frame_finish(struct mad_frame *frame)
{
  mad_header_finish(&frame->header);

  if (frame->overlap) {
    LocalFree(frame->overlap);
    frame->overlap = 0;
  }
}

 
static
int decode_header(struct mad_header *header, struct mad_stream *stream)
{
  unsigned int index;

  header->flags        = 0;
  header->private_bits = 0;

   

   
  mad_bit_skip(&stream->ptr, 11);

   
  if (mad_bit_read(&stream->ptr, 1) == 0)
    header->flags |= MAD_FLAG_MPEG_2_5_EXT;

   
  if (mad_bit_read(&stream->ptr, 1) == 0)
    header->flags |= MAD_FLAG_LSF_EXT;
  else if (header->flags & MAD_FLAG_MPEG_2_5_EXT) {
    stream->error = MAD_ERROR_LOSTSYNC;
    return -1;
  }

   
  header->layer = 4 - mad_bit_read(&stream->ptr, 2);

  if (header->layer == 4) {
    stream->error = MAD_ERROR_BADLAYER;
    return -1;
  }

   
  if (mad_bit_read(&stream->ptr, 1) == 0) {
    header->flags    |= MAD_FLAG_PROTECTION;
    header->crc_check = mad_bit_crc(stream->ptr, 16, 0xffff);
  }

   
  index = mad_bit_read(&stream->ptr, 4);

  if (index == 15) {
    stream->error = MAD_ERROR_BADBITRATE;

    return -1;
  }

  if (header->flags & MAD_FLAG_LSF_EXT)
    header->bitrate = bitrate_table[3 + (header->layer >> 1)][index];
  else
    header->bitrate = bitrate_table[header->layer - 1][index];

   
  index = mad_bit_read(&stream->ptr, 2);

  if (index == 3) {
    stream->error = MAD_ERROR_BADSAMPLERATE;
    return -1;
  }

  header->samplerate = samplerate_table[index];

  if (header->flags & MAD_FLAG_LSF_EXT) {
    header->samplerate /= 2;

    if (header->flags & MAD_FLAG_MPEG_2_5_EXT)
      header->samplerate /= 2;
  }

   
  if (mad_bit_read(&stream->ptr, 1))
    header->flags |= MAD_FLAG_PADDING;

   
  if (mad_bit_read(&stream->ptr, 1))
    header->private_bits |= MAD_PRIVATE_HEADER;

   
  header->mode = 3 - mad_bit_read(&stream->ptr, 2);

   
  header->mode_extension = mad_bit_read(&stream->ptr, 2);

   
  if (mad_bit_read(&stream->ptr, 1))
    header->flags |= MAD_FLAG_COPYRIGHT;

   
  if (mad_bit_read(&stream->ptr, 1))
    header->flags |= MAD_FLAG_ORIGINAL;

   
  header->emphasis = mad_bit_read(&stream->ptr, 2);

# if defined(OPT_STRICT)
   
  if (header->emphasis == MAD_EMPHASIS_RESERVED) {
    stream->error = MAD_ERROR_BADEMPHASIS;
    return -1;
  }
# endif

   

   
  if (header->flags & MAD_FLAG_PROTECTION)
    header->crc_target = mad_bit_read(&stream->ptr, 16);



  return 0;
}

 
static
int free_bitrate(struct mad_stream *stream, struct mad_header const *header)
{
  struct mad_bitptr keep_ptr;
  unsigned long rate = 0;
  unsigned int pad_slot, slots_per_frame;
  unsigned char const *ptr = 0;

  keep_ptr = stream->ptr;

  pad_slot = (header->flags & MAD_FLAG_PADDING) ? 1 : 0;
  slots_per_frame = (header->layer == MAD_LAYER_III &&
		     (header->flags & MAD_FLAG_LSF_EXT)) ? 72 : 144;

  while (mad_stream_sync(stream) == 0) {
    struct mad_stream peek_stream;
    struct mad_header peek_header;

    peek_stream = *stream;
    peek_header = *header;

    if (decode_header(&peek_header, &peek_stream) == 0 &&
	peek_header.layer == header->layer &&
	peek_header.samplerate == header->samplerate) {
      unsigned int N;

      ptr = mad_bit_nextbyte(&stream->ptr);

      N = ptr - stream->this_frame;

      if (header->layer == MAD_LAYER_I) {
	rate = (unsigned long) header->samplerate *
	  (N - 4 * pad_slot + 4) / 48 / 1000;
      }
      else {
	rate = (unsigned long) header->samplerate *
	  (N - pad_slot + 1) / slots_per_frame / 1000;
      }

      if (rate >= 8)
	break;
    }

    mad_bit_skip(&stream->ptr, 8);
  }

  stream->ptr = keep_ptr;

  if (rate < 8 || (header->layer == MAD_LAYER_III && rate > 640)) {
    stream->error = MAD_ERROR_LOSTSYNC;
    return -1;
  }

  stream->freerate = rate * 1000;

  return 0;
}

 
int mad_header_decode(struct mad_header *header, struct mad_stream *stream)
{
  register unsigned char const *ptr, *end;
  unsigned int pad_slot, N;
 

  ptr = stream->next_frame;
  end = stream->bufend;

  if (ptr == 0) {
    stream->error = MAD_ERROR_BUFPTR;
    goto fail;
  }

   
  if (stream->skiplen) {
    if (!stream->sync)
      ptr = stream->this_frame;

    if (end - ptr < stream->skiplen) {
      stream->skiplen   -= end - ptr;
      stream->next_frame = end;

      stream->error = MAD_ERROR_BUFLEN;
      goto fail;
    }

    ptr += stream->skiplen;
    stream->skiplen = 0;

    stream->sync = 1;
  }

 sync:
   
  if (stream->sync) {
    if (end - ptr < MAD_BUFFER_GUARD) {
      stream->next_frame = ptr;

      stream->error = MAD_ERROR_BUFLEN;
      goto fail;
    }
    else if (!(ptr[0] == 0xff && (ptr[1] & 0xe0) == 0xe0)) {
       
      stream->this_frame = ptr;
      stream->next_frame = ptr + 1;

      stream->error = MAD_ERROR_LOSTSYNC;
      goto fail;
    }
  }
  else {
    mad_bit_init(&stream->ptr, ptr);

    if (mad_stream_sync(stream) == -1) {
      if (end - stream->next_frame >= MAD_BUFFER_GUARD)
	stream->next_frame = end - MAD_BUFFER_GUARD;

      stream->error = MAD_ERROR_BUFLEN;
      goto fail;
    }

    ptr = mad_bit_nextbyte(&stream->ptr);
  }

   
  stream->this_frame = ptr;
  stream->next_frame = ptr + 1;   

  mad_bit_init(&stream->ptr, stream->this_frame);

  if (decode_header(header, stream) == -1)
    goto fail;

   
  mad_timer_set(&header->duration, 0,
		32 * MAD_NSBSAMPLES(header), header->samplerate);

   
  if (header->bitrate == 0) {
    if ((stream->freerate == 0 || !stream->sync ||
	 (header->layer == MAD_LAYER_III && stream->freerate > 640000)) &&
	free_bitrate(stream, header) == -1)
      goto fail;

    header->bitrate = stream->freerate;
    header->flags  |= MAD_FLAG_FREEFORMAT;
  }

   
  pad_slot = (header->flags & MAD_FLAG_PADDING) ? 1 : 0;

  if (header->layer == MAD_LAYER_I)
    N = ((12 * header->bitrate / header->samplerate) + pad_slot) * 4;
  else {
    unsigned int slots_per_frame;

    slots_per_frame = (header->layer == MAD_LAYER_III &&
		       (header->flags & MAD_FLAG_LSF_EXT)) ? 72 : 144;

	N = (slots_per_frame * header->bitrate / header->samplerate) + pad_slot;
	
  }

  header->size = N;

   
  if (N + MAD_BUFFER_GUARD > end - stream->this_frame) {
    stream->next_frame = stream->this_frame;

    stream->error = MAD_ERROR_BUFLEN;
    goto fail;
  }

  stream->next_frame = stream->this_frame + N;

  if (!stream->sync) {
     

    ptr = stream->next_frame;
    if (!(ptr[0] == 0xff && (ptr[1] & 0xe0) == 0xe0)) {
      ptr = stream->next_frame = stream->this_frame + 1;
      goto sync;
    }

    stream->sync = 1;
  }

  header->flags |= MAD_FLAG_INCOMPLETE;

  

  return 0;

 fail:
  stream->sync = 0;

  return -1;
}

 
int mad_frame_decode(struct mad_frame *frame, struct mad_stream *stream)
{
  frame->options = stream->options;

   
   

  if (!(frame->header.flags & MAD_FLAG_INCOMPLETE) &&
      mad_header_decode(&frame->header, stream) == -1)
    goto fail;

   

  frame->header.flags &= ~MAD_FLAG_INCOMPLETE;

  if (decoder_table[frame->header.layer - 1](stream, frame) == -1) {
    if (!MAD_RECOVERABLE(stream->error))
      stream->next_frame = stream->this_frame;

    goto fail;
  }

   

  if (frame->header.layer != MAD_LAYER_III) {
    struct mad_bitptr next_frame;

    mad_bit_init(&next_frame, stream->next_frame);

    stream->anc_ptr    = stream->ptr;
    stream->anc_bitlen = mad_bit_length(&stream->ptr, &next_frame);

    mad_bit_finish(&next_frame);
  }

  return 0;

 fail:
  stream->anc_bitlen = 0;
  return -1;
}

 
void mad_frame_mute(struct mad_frame *frame)
{
  unsigned int s, sb;

  for (s = 0; s < 36; ++s) {
    for (sb = 0; sb < 32; ++sb) {
      frame->sbsample[0][s][sb] =
      frame->sbsample[1][s][sb] = 0;
    }
  }

  if (frame->overlap) {
    for (s = 0; s < 18; ++s) {
      for (sb = 0; sb < 32; ++sb) {
	(*frame->overlap)[0][sb][s] =
	(*frame->overlap)[1][sb][s] = 0;
      }
    }
  }
}


 
 

# ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif

# ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
# endif

# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif

# ifdef HAVE_FCNTL_H
#  include <fcntl.h>
# endif

# include <stdlib.h>

# ifdef HAVE_ERRNO_H
#  include <errno.h>
# endif



 
void mad_decoder_init(struct mad_decoder *decoder, void *data,
		      enum mad_flow (*input_func)(void *,
						  struct mad_stream *),
		      enum mad_flow (*header_func)(void *,
						   struct mad_header const *),
		      enum mad_flow (*filter_func)(void *,
						   struct mad_stream const *,
						   struct mad_frame *),
		      enum mad_flow (*output_func)(void *,
						   struct mad_header const *,
						   struct mad_pcm *),
		      enum mad_flow (*error_func)(void *,
						  struct mad_stream *,
						  struct mad_frame *),
		      enum mad_flow (*message_func)(void *,
						    void *, unsigned int *))
{
  decoder->mode         = -1;

  decoder->options      = 0;

  decoder->async.pid    = 0;
  decoder->async.in     = -1;
  decoder->async.out    = -1;

  decoder->sync         = 0;

  decoder->cb_data      = data;

  decoder->input_func   = input_func;
  decoder->header_func  = header_func;
  decoder->filter_func  = filter_func;
  decoder->output_func  = output_func;
  decoder->error_func   = error_func;
  decoder->message_func = message_func;
}


int mad_decoder_finish(struct mad_decoder *decoder)
{
# if defined(USE_ASYNC)
  if (decoder->mode == MAD_DECODER_MODE_ASYNC && decoder->async.pid) {
    pid_t pid;
    int status;

    close(decoder->async.in);

    do
      pid = waitpid(decoder->async.pid, &status, 0);
    while (pid == -1 && errno == EINTR);

    decoder->mode = -1;

    close(decoder->async.out);

    decoder->async.pid = 0;
    decoder->async.in  = -1;
    decoder->async.out = -1;

    if (pid == -1)
      return -1;

    return (!WIFEXITED(status) || WEXITSTATUS(status)) ? -1 : 0;
  }
# endif

  return 0;
}

# if defined(USE_ASYNC)
static
enum mad_flow send_io(int fd, void const *data, size_t len)
{
  char const *ptr = data;
  ssize_t count;

  while (len) {
    do
      count = write(fd, ptr, len);
    while (count == -1 && errno == EINTR);

    if (count == -1)
      return MAD_FLOW_BREAK;

    len -= count;
    ptr += count;
  }

  return MAD_FLOW_CONTINUE;
}

static
enum mad_flow receive_io(int fd, void *buffer, size_t len)
{
  char *ptr = buffer;
  ssize_t count;

  while (len) {
    do
      count = read(fd, ptr, len);
    while (count == -1 && errno == EINTR);

    if (count == -1)
      return (errno == EAGAIN) ? MAD_FLOW_IGNORE : MAD_FLOW_BREAK;
    else if (count == 0)
      return MAD_FLOW_STOP;

    len -= count;
    ptr += count;
  }

  return MAD_FLOW_CONTINUE;
}

static
enum mad_flow receive_io_blocking(int fd, void *buffer, size_t len)
{
  int flags, blocking;
  enum mad_flow result;

  flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    return MAD_FLOW_BREAK;

  blocking = flags & ~O_NONBLOCK;

  if (blocking != flags &&
      fcntl(fd, F_SETFL, blocking) == -1)
    return MAD_FLOW_BREAK;

  result = receive_io(fd, buffer, len);

  if (flags != blocking &&
      fcntl(fd, F_SETFL, flags) == -1)
    return MAD_FLOW_BREAK;

  return result;
}

static
enum mad_flow send(int fd, void const *message, unsigned int size)
{
  enum mad_flow result;

   

  result = send_io(fd, &size, sizeof(size));

   

  if (result == MAD_FLOW_CONTINUE)
    result = send_io(fd, message, size);

  return result;
}

static
enum mad_flow receive(int fd, void **message, unsigned int *size)
{
  enum mad_flow result;
  unsigned int actual;

  if (*message == 0)
    *size = 0;

   

  result = receive_io(fd, &actual, sizeof(actual));

   

  if (result == MAD_FLOW_CONTINUE) {
    if (actual > *size)
      actual -= *size;
    else {
      *size  = actual;
      actual = 0;
    }

    if (*size > 0) {
      if (*message == 0) {
	*message = LocalAlloc(0, *size);
	if (*message == 0)
	  return MAD_FLOW_BREAK;
      }

      result = receive_io_blocking(fd, *message, *size);
    }

     

    while (actual && result == MAD_FLOW_CONTINUE) {
      char sink[256];
      unsigned int len;

      len = actual > sizeof(sink) ? sizeof(sink) : actual;

      result = receive_io_blocking(fd, sink, len);

      actual -= len;
    }
  }

  return result;
}


static
enum mad_flow check_message(struct mad_decoder *decoder)
{
  enum mad_flow result;
  void *message = 0;
  unsigned int size;

  result = receive(decoder->async.in, &message, &size);

  if (result == MAD_FLOW_CONTINUE) {
    if (decoder->message_func == 0)
      size = 0;
    else {
      result = decoder->message_func(decoder->cb_data, message, &size);

      if (result == MAD_FLOW_IGNORE ||
	  result == MAD_FLOW_BREAK)
	size = 0;
    }

    if (send(decoder->async.out, message, size) != MAD_FLOW_CONTINUE)
      result = MAD_FLOW_BREAK;
  }

  if (message)
    LocalFree(message);

  return result;
}
# endif

static
enum mad_flow error_default(void *data, struct mad_stream *stream,
			    struct mad_frame *frame)
{
  int *bad_last_frame = (int*) data;

  switch (stream->error) {
  case MAD_ERROR_BADCRC:
    if (*bad_last_frame)
      mad_frame_mute(frame);
    else
      *bad_last_frame = 1;

    return MAD_FLOW_IGNORE;

  default:
    return MAD_FLOW_CONTINUE;
  }
}

static
int run_sync(struct mad_decoder *decoder)
{
  enum mad_flow (*error_func)(void *, struct mad_stream *, struct mad_frame *);
  void *error_data;
  int bad_last_frame = 0;
  struct mad_stream *stream;
  struct mad_frame *frame;
  struct mad_synth *synth;
  int result = 0;

  if (decoder->input_func == 0)
    return 0;

  if (decoder->error_func) {
    error_func = decoder->error_func;
    error_data = decoder->cb_data;
  }
  else {
    error_func = error_default;
    error_data = &bad_last_frame;
  }

  stream = &decoder->sync->stream;
  frame  = &decoder->sync->frame;
  synth  = &decoder->sync->synth;

  mad_stream_init(stream);
  mad_frame_init(frame);
  mad_synth_init(synth);

  mad_stream_options(stream, decoder->options);

  do {
    switch (decoder->input_func(decoder->cb_data, stream)) {
    case MAD_FLOW_STOP:
      goto done;
    case MAD_FLOW_BREAK:
      goto fail;
    case MAD_FLOW_IGNORE:
      continue;
    case MAD_FLOW_CONTINUE:
      break;
    }

    while (1) {
# if defined(USE_ASYNC)
      if (decoder->mode == MAD_DECODER_MODE_ASYNC) {
	switch (check_message(decoder)) {
	case MAD_FLOW_IGNORE:
	case MAD_FLOW_CONTINUE:
	  break;
	case MAD_FLOW_BREAK:
	  goto fail;
	case MAD_FLOW_STOP:
	  goto done;
	}
      }
# endif

      if (decoder->header_func) {
	if (mad_header_decode(&frame->header, stream) == -1) {
	  if (!MAD_RECOVERABLE(stream->error))
	    break;

	  switch (error_func(error_data, stream, frame)) {
	  case MAD_FLOW_STOP:
	    goto done;
	  case MAD_FLOW_BREAK:
	    goto fail;
	  case MAD_FLOW_IGNORE:
	  case MAD_FLOW_CONTINUE:
	  default:
	    continue;
	  }
	}

	switch (decoder->header_func(decoder->cb_data, &frame->header)) {
	case MAD_FLOW_STOP:
	  goto done;
	case MAD_FLOW_BREAK:
	  goto fail;
	case MAD_FLOW_IGNORE:
	  continue;
	case MAD_FLOW_CONTINUE:
	  break;
	}
      }

      if (mad_frame_decode(frame, stream) == -1) {
	if (!MAD_RECOVERABLE(stream->error))
	  break;

	switch (error_func(error_data, stream, frame)) {
	case MAD_FLOW_STOP:
	  goto done;
	case MAD_FLOW_BREAK:
	  goto fail;
	case MAD_FLOW_IGNORE:
	  break;
	case MAD_FLOW_CONTINUE:
	default:
	  continue;
	}
      }
      else
	bad_last_frame = 0;

      if (decoder->filter_func) {
	switch (decoder->filter_func(decoder->cb_data, stream, frame)) {
	case MAD_FLOW_STOP:
	  goto done;
	case MAD_FLOW_BREAK:
	  goto fail;
	case MAD_FLOW_IGNORE:
	  continue;
	case MAD_FLOW_CONTINUE:
	  break;
	}
      }

      mad_synth_frame(synth, frame);

      if (decoder->output_func) {
	switch (decoder->output_func(decoder->cb_data,
				     &frame->header, &synth->pcm)) {
	case MAD_FLOW_STOP:
	  goto done;
	case MAD_FLOW_BREAK:
	  goto fail;
	case MAD_FLOW_IGNORE:
	case MAD_FLOW_CONTINUE:
	  break;
	}
      }
    }
  }
  while (stream->error == MAD_ERROR_BUFLEN);

 fail:
  result = -1;

 done:
  mad_synth_finish(synth);
  mad_frame_finish(frame);
  mad_stream_finish(stream);

  return result;
}

# if defined(USE_ASYNC)
static
int run_async(struct mad_decoder *decoder)
{
  pid_t pid;
  int ptoc[2], ctop[2], flags;

  if (pipe(ptoc) == -1)
    return -1;

  if (pipe(ctop) == -1) {
    close(ptoc[0]);
    close(ptoc[1]);
    return -1;
  }

  flags = fcntl(ptoc[0], F_GETFL);
  if (flags == -1 ||
      fcntl(ptoc[0], F_SETFL, flags | O_NONBLOCK) == -1) {
    close(ctop[0]);
    close(ctop[1]);
    close(ptoc[0]);
    close(ptoc[1]);
    return -1;
  }

  pid = fork();
  if (pid == -1) {
    close(ctop[0]);
    close(ctop[1]);
    close(ptoc[0]);
    close(ptoc[1]);
    return -1;
  }

  decoder->async.pid = pid;

  if (pid) {
     

    close(ptoc[0]);
    close(ctop[1]);

    decoder->async.in  = ctop[0];
    decoder->async.out = ptoc[1];

    return 0;
  }

   

  close(ptoc[1]);
  close(ctop[0]);

  decoder->async.in  = ptoc[0];
  decoder->async.out = ctop[1];

  _exit(run_sync(decoder));

   
  return -1;
}
# endif

 
int mad_decoder_run(struct mad_decoder *decoder, enum mad_decoder_mode mode)
{
  int result;
  int (*run)(struct mad_decoder *) = 0;

  switch (decoder->mode = mode) {
  case MAD_DECODER_MODE_SYNC:
    run = run_sync;
    break;

  case MAD_DECODER_MODE_ASYNC:
# if defined(USE_ASYNC)
    run = run_async;
# endif
    break;
  }

  if (run == 0)
    return -1;

  decoder->sync = (SYNC*) LocalAlloc(0, sizeof(*decoder->sync));
  if (decoder->sync == 0)
    return -1;

  result = run(decoder);

  LocalFree(decoder->sync);
  decoder->sync = 0;

  return result;
}

 
int mad_decoder_message(struct mad_decoder *decoder,
			void *message, unsigned int *len)
{
# if defined(USE_ASYNC)
  if (decoder->mode != MAD_DECODER_MODE_ASYNC ||
      send(decoder->async.out, message, *len) != MAD_FLOW_CONTINUE ||
      receive(decoder->async.in, &message, len) != MAD_FLOW_CONTINUE)
    return -1;

  return 0;
# else
  return -1;
# endif
}


 

 
 

 

 static
mad_fixed_t const sf_table[64] = {

  MAD_F(0x20000000),   
  MAD_F(0x1965fea5),   
  MAD_F(0x1428a2fa),   
  MAD_F(0x10000000),   
  MAD_F(0x0cb2ff53),   
  MAD_F(0x0a14517d),   
  MAD_F(0x08000000),   
  MAD_F(0x06597fa9),   

  MAD_F(0x050a28be),   
  MAD_F(0x04000000),   
  MAD_F(0x032cbfd5),   
  MAD_F(0x0285145f),   
  MAD_F(0x02000000),   
  MAD_F(0x01965fea),   
  MAD_F(0x01428a30),   
  MAD_F(0x01000000),   

  MAD_F(0x00cb2ff5),   
  MAD_F(0x00a14518),   
  MAD_F(0x00800000),   
  MAD_F(0x006597fb),   
  MAD_F(0x0050a28c),   
  MAD_F(0x00400000),   
  MAD_F(0x0032cbfd),   
  MAD_F(0x00285146),   

  MAD_F(0x00200000),   
  MAD_F(0x001965ff),   
  MAD_F(0x001428a3),   
  MAD_F(0x00100000),   
  MAD_F(0x000cb2ff),   
  MAD_F(0x000a1451),   
  MAD_F(0x00080000),   
  MAD_F(0x00065980),   

  MAD_F(0x00050a29),   
  MAD_F(0x00040000),   
  MAD_F(0x00032cc0),   
  MAD_F(0x00028514),   
  MAD_F(0x00020000),   
  MAD_F(0x00019660),   
  MAD_F(0x0001428a),   
  MAD_F(0x00010000),   

  MAD_F(0x0000cb30),   
  MAD_F(0x0000a145),   
  MAD_F(0x00008000),   
  MAD_F(0x00006598),   
  MAD_F(0x000050a3),   
  MAD_F(0x00004000),   
  MAD_F(0x000032cc),   
  MAD_F(0x00002851),   

  MAD_F(0x00002000),   
  MAD_F(0x00001966),   
  MAD_F(0x00001429),   
  MAD_F(0x00001000),   
  MAD_F(0x00000cb3),   
  MAD_F(0x00000a14),   
  MAD_F(0x00000800),   
  MAD_F(0x00000659),   

  MAD_F(0x0000050a),   
  MAD_F(0x00000400),   
  MAD_F(0x0000032d),   
  MAD_F(0x00000285),   
  MAD_F(0x00000200),   
  MAD_F(0x00000196),   
  MAD_F(0x00000143),   
  MAD_F(0x00000000)    

};
 

 
 
static
mad_fixed_t const linear_table[14] = {
  MAD_F(0x15555555),   
  MAD_F(0x12492492),   
  MAD_F(0x11111111),   
  MAD_F(0x10842108),   
  MAD_F(0x10410410),   
  MAD_F(0x10204081),   
  MAD_F(0x10101010),   
  MAD_F(0x10080402),   
  MAD_F(0x10040100),   
  MAD_F(0x10020040),   
  MAD_F(0x10010010),   
  MAD_F(0x10008004),   
  MAD_F(0x10004001),   
  MAD_F(0x10002000)    
};

 
static
mad_fixed_t I_sample(struct mad_bitptr *ptr, unsigned int nb)
{
  mad_fixed_t sample;

  sample = mad_bit_read(ptr, nb);

   

  sample ^= 1 << (nb - 1);
  sample |= -(sample & (1 << (nb - 1)));

  sample <<= MAD_F_FRACBITS - (nb - 1);

   

   

  sample += MAD_F_ONE >> (nb - 1);

  return mad_f_mul(sample, linear_table[nb - 2]);

   
   
}

 
int mad_layer_I(struct mad_stream *stream, struct mad_frame *frame)
{
  struct mad_header *header = &frame->header;
  unsigned int nch, bound, ch, s, sb, nb;
  unsigned char allocation[2][32], scalefactor[2][32];

  nch = MAD_NCHANNELS(header);

  bound = 32;
  if (header->mode == MAD_MODE_JOINT_STEREO) {
    header->flags |= MAD_FLAG_I_STEREO;
    bound = 4 + header->mode_extension * 4;
  }

   

  if (header->flags & MAD_FLAG_PROTECTION) {
    header->crc_check =
      mad_bit_crc(stream->ptr, 4 * (bound * nch + (32 - bound)),
		  header->crc_check);

    if (header->crc_check != header->crc_target &&
	!(frame->options & MAD_OPTION_IGNORECRC)) {
      stream->error = MAD_ERROR_BADCRC;
      return -1;
    }
  }

   

  for (sb = 0; sb < bound; ++sb) {
    for (ch = 0; ch < nch; ++ch) {
      nb = mad_bit_read(&stream->ptr, 4);

      if (nb == 15) {
	stream->error = MAD_ERROR_BADBITALLOC;
	return -1;
      }

      allocation[ch][sb] = nb ? nb + 1 : 0;
    }
  }

  for (sb = bound; sb < 32; ++sb) {
    nb = mad_bit_read(&stream->ptr, 4);

    if (nb == 15) {
      stream->error = MAD_ERROR_BADBITALLOC;
      return -1;
    }

    allocation[0][sb] =
    allocation[1][sb] = nb ? nb + 1 : 0;
  }

   

  for (sb = 0; sb < 32; ++sb) {
    for (ch = 0; ch < nch; ++ch) {
      if (allocation[ch][sb]) {
	scalefactor[ch][sb] = mad_bit_read(&stream->ptr, 6);

# if defined(OPT_STRICT)
	 
	if (scalefactor[ch][sb] == 63) {
	  stream->error = MAD_ERROR_BADSCALEFACTOR;
	  return -1;
	}
# endif
      }
    }
  }

   

  for (s = 0; s < 12; ++s) {
    for (sb = 0; sb < bound; ++sb) {
      for (ch = 0; ch < nch; ++ch) {
	nb = allocation[ch][sb];
	frame->sbsample[ch][s][sb] = nb ?
	  mad_f_mul(I_sample(&stream->ptr, nb),
		    sf_table[scalefactor[ch][sb]]) : 0;
      }
    }

    for (sb = bound; sb < 32; ++sb) {
      if ((nb = allocation[0][sb])) {
	mad_fixed_t sample;

	sample = I_sample(&stream->ptr, nb);

	for (ch = 0; ch < nch; ++ch) {
	  frame->sbsample[ch][s][sb] =
	    mad_f_mul(sample, sf_table[scalefactor[ch][sb]]);
	}
      }
      else {
	for (ch = 0; ch < nch; ++ch)
	  frame->sbsample[ch][s][sb] = 0;
      }
    }
  }

  return 0;
}

 

 
static
struct {
  unsigned int sblimit;
  unsigned char  offsets[30];
}  sbquant_table[5] = {
   
  { 27, { 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3,	 
	  3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0 } },
   
  { 30, { 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3,	 
	  3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 } },
   
  {  8, { 5, 5, 2, 2, 2, 2, 2, 2 } },				 
   
  { 12, { 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 } },		 
   
  { 30, { 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,	 
	  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }
};

 
static
struct {
  unsigned short nbal;
  unsigned short offset;
} const bitalloc_table[8] = {
  { 2, 0 },   
  { 2, 3 },   
  { 3, 3 },   
  { 3, 1 },   
  { 4, 2 },   
  { 4, 3 },   
  { 4, 4 },   
  { 4, 5 }    
};

 
static
unsigned char const offset_table[6][15] = {
  { 0, 1, 16                                             },   
  { 0, 1,  2, 3, 4, 5, 16                                },   
  { 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 14 },   
  { 0, 1,  3, 4, 5, 6,  7, 8,  9, 10, 11, 12, 13, 14, 15 },   
  { 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },   
  { 0, 2,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16 }    
};


static
struct quantclass {
	unsigned short nlevels;
	unsigned char group;
	unsigned char bits;
	mad_fixed_t C;
	mad_fixed_t D;
} const qc_table[17] = {



	{     3, 2,  5,
	MAD_F(0x15555555)  ,
	MAD_F(0x08000000)   },
	{     5, 3,  7,
	MAD_F(0x1999999a)  ,
	MAD_F(0x08000000)   },
	{     7, 0,  3,
	MAD_F(0x12492492)  ,
	MAD_F(0x04000000)   },
	{     9, 4, 10,
	MAD_F(0x1c71c71c)  ,
	MAD_F(0x08000000)   },
	{    15, 0,  4,
	MAD_F(0x11111111)  ,
	MAD_F(0x02000000)   },
	{    31, 0,  5,
	MAD_F(0x10842108)  ,
	MAD_F(0x01000000)   },
	{    63, 0,  6,
	MAD_F(0x10410410)  ,
	MAD_F(0x00800000)   },
	{   127, 0,  7,
	MAD_F(0x10204081)  ,
	MAD_F(0x00400000)   },
	{   255, 0,  8,
	MAD_F(0x10101010)  ,
	MAD_F(0x00200000)   },
	{   511, 0,  9,
	MAD_F(0x10080402)  ,
	MAD_F(0x00100000)   },
	{  1023, 0, 10,
	MAD_F(0x10040100)  ,
	MAD_F(0x00080000)   },
	{  2047, 0, 11,
	MAD_F(0x10020040)  ,
	MAD_F(0x00040000)   },
	{  4095, 0, 12,
	MAD_F(0x10010010)  ,
	MAD_F(0x00020000)   },
	{  8191, 0, 13,
	MAD_F(0x10008004)  ,
	MAD_F(0x00010000)   },
	{ 16383, 0, 14,
	MAD_F(0x10004001)  ,
	MAD_F(0x00008000)   },
	{ 32767, 0, 15,
	MAD_F(0x10002000)  ,
	MAD_F(0x00004000)   },
	{ 65535, 0, 16,
	MAD_F(0x10001000)  ,
	MAD_F(0x00002000)   }

};


static
void II_samples(struct mad_bitptr *ptr,
struct quantclass const *quantclass,
	mad_fixed_t output[3])
{
	unsigned int nb, s, sample[3];

	if ((nb = quantclass->group)) {
		unsigned int c, nlevels;


		c = mad_bit_read(ptr, quantclass->bits);
		nlevels = quantclass->nlevels;

		for (s = 0; s < 3; ++s) {
			sample[s] = c % nlevels;
			c /= nlevels;
		}
	}
	else {
		nb = quantclass->bits;

		for (s = 0; s < 3; ++s)
			sample[s] = mad_bit_read(ptr, nb);
	}

	for (s = 0; s < 3; ++s) {
		mad_fixed_t requantized;



		requantized  = sample[s] ^ (1 << (nb - 1));
		requantized |= -(requantized & (1 << (nb - 1)));

		requantized <<= MAD_F_FRACBITS - (nb - 1);





		output[s] = mad_f_mul(requantized + quantclass->D, quantclass->C);



	}
}


int mad_layer_II(struct mad_stream *stream, struct mad_frame *frame)
{
	struct mad_header *header = &frame->header;
	struct mad_bitptr start;
	unsigned int index, sblimit, nbal, nch, bound, gr, ch, s, sb;
	unsigned char const *offsets;
	unsigned char allocation[2][32], scfsi[2][32], scalefactor[2][32][3];
	mad_fixed_t samples[3];

	nch = MAD_NCHANNELS(header);

	if (header->flags & MAD_FLAG_LSF_EXT)
		index = 4;
	else if (header->flags & MAD_FLAG_FREEFORMAT)
		goto freeformat;
	else {
		unsigned long bitrate_per_channel;

		bitrate_per_channel = header->bitrate;
		if (nch == 2) {
			bitrate_per_channel /= 2;

# if defined(OPT_STRICT)

			if (bitrate_per_channel <= 28000 || bitrate_per_channel == 40000) {
				stream->error = MAD_ERROR_BADMODE;
				return -1;
			}
# endif
		}
		else {   
			if (bitrate_per_channel > 192000) {

				stream->error = MAD_ERROR_BADMODE;
				return -1;
			}
		}

		if (bitrate_per_channel <= 48000)
			index = (header->samplerate == 32000) ? 3 : 2;
		else if (bitrate_per_channel <= 80000)
			index = 0;
		else {
freeformat:
			index = (header->samplerate == 48000) ? 0 : 1;
		}
	}

	sblimit = sbquant_table[index].sblimit;
	offsets = sbquant_table[index].offsets;

	bound = 32;
	if (header->mode == MAD_MODE_JOINT_STEREO) {
		header->flags |= MAD_FLAG_I_STEREO;
		bound = 4 + header->mode_extension * 4;
	}

	if (bound > sblimit)
		bound = sblimit;

	start = stream->ptr;



	for (sb = 0; sb < bound; ++sb) {
		nbal = bitalloc_table[offsets[sb]].nbal;

		for (ch = 0; ch < nch; ++ch)
			allocation[ch][sb] = mad_bit_read(&stream->ptr, nbal);
	}

	for (sb = bound; sb < sblimit; ++sb) {
		nbal = bitalloc_table[offsets[sb]].nbal;

		allocation[0][sb] =
			allocation[1][sb] = mad_bit_read(&stream->ptr, nbal);
	}



	for (sb = 0; sb < sblimit; ++sb) {
		for (ch = 0; ch < nch; ++ch) {
			if (allocation[ch][sb])
				scfsi[ch][sb] = mad_bit_read(&stream->ptr, 2);
		}
	}



	if (header->flags & MAD_FLAG_PROTECTION) {
		header->crc_check =
			mad_bit_crc(start, mad_bit_length(&start, &stream->ptr),
			header->crc_check);

		if (header->crc_check != header->crc_target &&
			!(frame->options & MAD_OPTION_IGNORECRC)) {
				stream->error = MAD_ERROR_BADCRC;
				return -1;
		}
	}



	for (sb = 0; sb < sblimit; ++sb) {
		for (ch = 0; ch < nch; ++ch) {
			if (allocation[ch][sb]) {
				scalefactor[ch][sb][0] = mad_bit_read(&stream->ptr, 6);

				switch (scfsi[ch][sb]) {
case 2:
	scalefactor[ch][sb][2] =
		scalefactor[ch][sb][1] =
		scalefactor[ch][sb][0];
	break;

case 0:
	scalefactor[ch][sb][1] = mad_bit_read(&stream->ptr, 6);


case 1:
case 3:
	scalefactor[ch][sb][2] = mad_bit_read(&stream->ptr, 6);
				}

				if (scfsi[ch][sb] & 1)
					scalefactor[ch][sb][1] = scalefactor[ch][sb][scfsi[ch][sb] - 1];

# if defined(OPT_STRICT)

				if (scalefactor[ch][sb][0] == 63 ||
					scalefactor[ch][sb][1] == 63 ||
					scalefactor[ch][sb][2] == 63) {
						stream->error = MAD_ERROR_BADSCALEFACTOR;
						return -1;
				}
# endif
			}
		}
	}



	for (gr = 0; gr < 12; ++gr) {
		for (sb = 0; sb < bound; ++sb) {
			for (ch = 0; ch < nch; ++ch) {
				if ((index = allocation[ch][sb])) {
					index = offset_table[bitalloc_table[offsets[sb]].offset][index - 1];

					II_samples(&stream->ptr, &qc_table[index], samples);

					for (s = 0; s < 3; ++s) {
						frame->sbsample[ch][3 * gr + s][sb] =
							mad_f_mul(samples[s], sf_table[scalefactor[ch][sb][gr / 4]]);
					}
				}
				else {
					for (s = 0; s < 3; ++s)
						frame->sbsample[ch][3 * gr + s][sb] = 0;
				}
			}
		}

		for (sb = bound; sb < sblimit; ++sb) {
			if ((index = allocation[0][sb])) {
				index = offset_table[bitalloc_table[offsets[sb]].offset][index - 1];

				II_samples(&stream->ptr, &qc_table[index], samples);

				for (ch = 0; ch < nch; ++ch) {
					for (s = 0; s < 3; ++s) {
						frame->sbsample[ch][3 * gr + s][sb] =
							mad_f_mul(samples[s], sf_table[scalefactor[ch][sb][gr / 4]]);
					}
				}
			}
			else {
				for (ch = 0; ch < nch; ++ch) {
					for (s = 0; s < 3; ++s)
						frame->sbsample[ch][3 * gr + s][sb] = 0;
				}
			}
		}

		for (ch = 0; ch < nch; ++ch) {
			for (s = 0; s < 3; ++s) {
				for (sb = sblimit; sb < 32; ++sb)
					frame->sbsample[ch][3 * gr + s][sb] = 0;
			}
		}
	}

	return 0;
}






mad_timer_t const mad_timer_zero = { 0, 0 };


int mad_timer_compare(mad_timer_t timer1, mad_timer_t timer2)
{
	signed long diff;

	diff = timer1.seconds - timer2.seconds;
	if (diff < 0)
		return -1;
	else if (diff > 0)
		return +1;

	diff = timer1.fraction - timer2.fraction;
	if (diff < 0)
		return -1;
	else if (diff > 0)
		return +1;

	return 0;
}


void mad_timer_negate(mad_timer_t *timer)
{
	timer->seconds = -timer->seconds;

	if (timer->fraction) {
		timer->seconds -= 1;
		timer->fraction = MAD_TIMER_RESOLUTION - timer->fraction;
	}
}


mad_timer_t mad_timer_abs(mad_timer_t timer)
{
	if (timer.seconds < 0)
		mad_timer_negate(&timer);

	return timer;
}


static
void reduce_timer(mad_timer_t *timer)
{
	timer->seconds  += timer->fraction / MAD_TIMER_RESOLUTION;
	timer->fraction %= MAD_TIMER_RESOLUTION;
}


static
unsigned long gcd(unsigned long num1, unsigned long num2)
{
	unsigned long tmp;

	while (num2) {
		tmp  = num2;
		num2 = num1 % num2;
		num1 = tmp;
	}

	return num1;
}


static
void reduce_rational(unsigned long *numer, unsigned long *denom)
{
	unsigned long factor;

	factor = gcd(*numer, *denom);



	*numer /= factor;
	*denom /= factor;
}


static
unsigned long scale_rational(unsigned long numer, unsigned long denom,
							 unsigned long scale)
{
	reduce_rational(&numer, &denom);
	reduce_rational(&scale, &denom);



	if (denom < scale)
		return numer * (scale / denom) + numer * (scale % denom) / denom;
	if (denom < numer)
		return scale * (numer / denom) + scale * (numer % denom) / denom;

	return numer * scale / denom;
}


void mad_timer_set(mad_timer_t *timer, unsigned long seconds,
				   unsigned long numer, unsigned long denom)
{
	timer->seconds = seconds;
	if (numer >= denom && denom > 0) {
		timer->seconds += numer / denom;
		numer %= denom;
	}

	switch (denom) {
case 0:
case 1:
	timer->fraction = 0;
	break;

case MAD_TIMER_RESOLUTION:
	timer->fraction = numer;
	break;

case 1000:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION /  1000);
	break;

case 8000:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION /  8000);
	break;

case 11025:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION / 11025);
	break;

case 12000:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION / 12000);
	break;

case 16000:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION / 16000);
	break;

case 22050:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION / 22050);
	break;

case 24000:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION / 24000);
	break;

case 32000:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION / 32000);
	break;

case 44100:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION / 44100);
	break;

case 48000:
	timer->fraction = numer * (MAD_TIMER_RESOLUTION / 48000);
	break;

default:
	timer->fraction = scale_rational(numer, denom, MAD_TIMER_RESOLUTION);
	break;
	}

	if (timer->fraction >= MAD_TIMER_RESOLUTION)
		reduce_timer(timer);
}


void mad_timer_add(mad_timer_t *timer, mad_timer_t incr)
{
	timer->seconds  += incr.seconds;
	timer->fraction += incr.fraction;

	if (timer->fraction >= MAD_TIMER_RESOLUTION)
		reduce_timer(timer);
}


void mad_timer_multiply(mad_timer_t *timer, signed long scalar)
{
	mad_timer_t addend;
	unsigned long factor;

	factor = scalar;
	if (scalar < 0) {
		factor = -scalar;
		mad_timer_negate(timer);
	}

	addend = *timer;
	*timer = mad_timer_zero;

	while (factor) {
		if (factor & 1)
			mad_timer_add(timer, addend);

		mad_timer_add(&addend, addend);
		factor >>= 1;
	}
}


signed long mad_timer_count(mad_timer_t timer, enum mad_units units)
{
	switch (units) {
case MAD_UNITS_HOURS:
	return timer.seconds / 60 / 60;

case MAD_UNITS_MINUTES:
	return timer.seconds / 60;

case MAD_UNITS_SECONDS:
	return timer.seconds;

case MAD_UNITS_DECISECONDS:
case MAD_UNITS_CENTISECONDS:
case MAD_UNITS_MILLISECONDS:

case MAD_UNITS_8000_HZ:
case MAD_UNITS_11025_HZ:
case MAD_UNITS_12000_HZ:
case MAD_UNITS_16000_HZ:
case MAD_UNITS_22050_HZ:
case MAD_UNITS_24000_HZ:
case MAD_UNITS_32000_HZ:
case MAD_UNITS_44100_HZ:
case MAD_UNITS_48000_HZ:

case MAD_UNITS_24_FPS:
case MAD_UNITS_25_FPS:
case MAD_UNITS_30_FPS:
case MAD_UNITS_48_FPS:
case MAD_UNITS_50_FPS:
case MAD_UNITS_60_FPS:
case MAD_UNITS_75_FPS:
	return timer.seconds * (signed long) units +
		(signed long) scale_rational(timer.fraction, MAD_TIMER_RESOLUTION,
		units);

case MAD_UNITS_23_976_FPS:
case MAD_UNITS_24_975_FPS:
case MAD_UNITS_29_97_FPS:
case MAD_UNITS_47_952_FPS:
case MAD_UNITS_49_95_FPS:
case MAD_UNITS_59_94_FPS:
	return (mad_timer_count(timer, (enum mad_units) -units) + 1) * 1000 / 1001;
	}


	return 0;
}


unsigned long mad_timer_fraction(mad_timer_t timer, unsigned long denom)
{
	timer = mad_timer_abs(timer);

	switch (denom) {
case 0:
	return timer.fraction ?
		MAD_TIMER_RESOLUTION / timer.fraction : MAD_TIMER_RESOLUTION + 1;

case MAD_TIMER_RESOLUTION:
	return timer.fraction;

default:
	return scale_rational(timer.fraction, MAD_TIMER_RESOLUTION, denom);
	}
}


void mad_timer_string(mad_timer_t timer,
					  char *dest, char const *format, enum mad_units units,
					  enum mad_units fracunits, unsigned long subparts)
{


	unsigned long hours, minutes, seconds, sub;
	unsigned int frac;

	timer = mad_timer_abs(timer);

	seconds = timer.seconds;
	frac = sub = 0;

	switch (fracunits) {
case MAD_UNITS_HOURS:
case MAD_UNITS_MINUTES:
case MAD_UNITS_SECONDS:
	break;

case MAD_UNITS_DECISECONDS:
case MAD_UNITS_CENTISECONDS:
case MAD_UNITS_MILLISECONDS:

case MAD_UNITS_8000_HZ:
case MAD_UNITS_11025_HZ:
case MAD_UNITS_12000_HZ:
case MAD_UNITS_16000_HZ:
case MAD_UNITS_22050_HZ:
case MAD_UNITS_24000_HZ:
case MAD_UNITS_32000_HZ:
case MAD_UNITS_44100_HZ:
case MAD_UNITS_48000_HZ:

case MAD_UNITS_24_FPS:
case MAD_UNITS_25_FPS:
case MAD_UNITS_30_FPS:
case MAD_UNITS_48_FPS:
case MAD_UNITS_50_FPS:
case MAD_UNITS_60_FPS:
case MAD_UNITS_75_FPS:
	{
		unsigned long denom;

		denom = MAD_TIMER_RESOLUTION / fracunits;

		frac = timer.fraction / denom;
		sub  = scale_rational(timer.fraction % denom, denom, subparts);
	}
	break;

case MAD_UNITS_23_976_FPS:
case MAD_UNITS_24_975_FPS:
case MAD_UNITS_29_97_FPS:
case MAD_UNITS_47_952_FPS:
case MAD_UNITS_49_95_FPS:
case MAD_UNITS_59_94_FPS:


	{
		unsigned long frame, cycle, d, m;

		frame = mad_timer_count(timer, fracunits);

		cycle = -fracunits * 60 * 10 - (10 - 1) * 2;

		d = frame / cycle;
		m = frame % cycle;
		frame += (10 - 1) * 2 * d;
		if (m > 2)
			frame += 2 * ((m - 2) / (cycle / 10));

		frac    = frame % -fracunits;
		seconds = frame / -fracunits;
	}
	break;
	}

	switch (units) {
case MAD_UNITS_HOURS:
	minutes = seconds / 60;
	hours   = minutes / 60;

	sprintf(dest, format,
		hours,
		(unsigned int) (minutes % 60),
		(unsigned int) (seconds % 60),
		frac, sub);
	break;

case MAD_UNITS_MINUTES:
	minutes = seconds / 60;

	sprintf(dest, format,
		minutes,
		(unsigned int) (seconds % 60),
		frac, sub);
	break;

case MAD_UNITS_SECONDS:
	sprintf(dest, format,
		seconds,
		frac, sub);
	break;

case MAD_UNITS_23_976_FPS:
case MAD_UNITS_24_975_FPS:
case MAD_UNITS_29_97_FPS:
case MAD_UNITS_47_952_FPS:
case MAD_UNITS_49_95_FPS:
case MAD_UNITS_59_94_FPS:
	if (fracunits < 0) {

		sub = 0;
	}



case MAD_UNITS_DECISECONDS:
case MAD_UNITS_CENTISECONDS:
case MAD_UNITS_MILLISECONDS:

case MAD_UNITS_8000_HZ:
case MAD_UNITS_11025_HZ:
case MAD_UNITS_12000_HZ:
case MAD_UNITS_16000_HZ:
case MAD_UNITS_22050_HZ:
case MAD_UNITS_24000_HZ:
case MAD_UNITS_32000_HZ:
case MAD_UNITS_44100_HZ:
case MAD_UNITS_48000_HZ:

case MAD_UNITS_24_FPS:
case MAD_UNITS_25_FPS:
case MAD_UNITS_30_FPS:
case MAD_UNITS_48_FPS:
case MAD_UNITS_50_FPS:
case MAD_UNITS_60_FPS:
case MAD_UNITS_75_FPS:
	sprintf(dest, format, mad_timer_count(timer, units), sub);
	break;
	}
}






void mad_stream_init(struct mad_stream *stream)
{
	stream->buffer     = 0;
	stream->bufend     = 0;
	stream->skiplen    = 0;

	stream->sync       = 0;
	stream->freerate   = 0;

	stream->this_frame = 0;
	stream->next_frame = 0;
	mad_bit_init(&stream->ptr, 0);

	mad_bit_init(&stream->anc_ptr, 0);
	stream->anc_bitlen = 0;

	stream->main_data  = 0;
	stream->md_len     = 0;

	stream->options    = 0;
	stream->error      = MAD_ERROR_NONE;
}


void mad_stream_finish(struct mad_stream *stream)
{
	if (stream->main_data) {
		LocalFree(stream->main_data);
		stream->main_data = 0;
	}

	mad_bit_finish(&stream->anc_ptr);
	mad_bit_finish(&stream->ptr);
}


void mad_stream_buffer(struct mad_stream *stream,
					   unsigned char const *buffer, unsigned long length)
{
	stream->buffer = buffer;
	stream->bufend = buffer + length;

	stream->this_frame = buffer;
	stream->next_frame = buffer;

	stream->sync = 1;

	mad_bit_init(&stream->ptr, buffer);
}


void mad_stream_skip(struct mad_stream *stream, unsigned long length)
{
	stream->skiplen += length;
}


int mad_stream_sync(struct mad_stream *stream)
{
	register unsigned char const *ptr, *end;

	ptr = mad_bit_nextbyte(&stream->ptr);
	end = stream->bufend;

	while (ptr < end - 1 &&
		!(ptr[0] == 0xff && (ptr[1] & 0xe0) == 0xe0))
		++ptr;

	if (end - ptr < MAD_BUFFER_GUARD)
		return -1;

	mad_bit_init(&stream->ptr, ptr);

	return 0;
}


char const *mad_stream_errorstr(struct mad_stream const *stream)
{
	switch (stream->error) {
case MAD_ERROR_NONE:		 return "no error";

case MAD_ERROR_BUFLEN:	 return "input buffer too small (or EOF)";
case MAD_ERROR_BUFPTR:	 return "invalid (null) buffer pointer";

case MAD_ERROR_NOMEM:		 return "not enough memory";

case MAD_ERROR_LOSTSYNC:	 return "lost synchronization";
case MAD_ERROR_BADLAYER:	 return "reserved header layer value";
case MAD_ERROR_BADBITRATE:	 return "forbidden bitrate value";
case MAD_ERROR_BADSAMPLERATE:	 return "reserved sample frequency value";
case MAD_ERROR_BADEMPHASIS:	 return "reserved emphasis value";

case MAD_ERROR_BADCRC:	 return "CRC check failed";
case MAD_ERROR_BADBITALLOC:	 return "forbidden bit allocation value";
case MAD_ERROR_BADSCALEFACTOR: return "bad scalefactor index";
case MAD_ERROR_BADMODE:	 return "bad bitrate/mode combination";
case MAD_ERROR_BADFRAMELEN:	 return "bad frame length";
case MAD_ERROR_BADBIGVALUES:	 return "bad big_values count";
case MAD_ERROR_BADBLOCKTYPE:	 return "reserved block_type";
case MAD_ERROR_BADSCFSI:	 return "bad scalefactor selection info";
case MAD_ERROR_BADDATAPTR:	 return "bad main_data_begin pointer";
case MAD_ERROR_BADPART3LEN:	 return "bad audio data length";
case MAD_ERROR_BADHUFFTABLE:	 return "bad Huffman table select";
case MAD_ERROR_BADHUFFDATA:	 return "Huffman data overrun";
case MAD_ERROR_BADSTEREO:	 return "incompatible block_type for JS";
	}

	return 0;
}








void mad_synth_init(struct mad_synth *synth)
{
	mad_synth_mute(synth);

	synth->phase = 0;

	synth->pcm.samplerate = 0;
	synth->pcm.channels   = 0;
	synth->pcm.length     = 0;
}


void mad_synth_mute(struct mad_synth *synth)
{
	unsigned int ch, s, v;

	for (ch = 0; ch < 2; ++ch) {
		for (s = 0; s < 16; ++s) {
			for (v = 0; v < 8; ++v) {
				synth->filter[ch][0][0][s][v] = synth->filter[ch][0][1][s][v] =
					synth->filter[ch][1][0][s][v] = synth->filter[ch][1][1][s][v] = 0;
			}
		}
	}
}





# if defined(FPM_DEFAULT) && !defined(OPT_SSO)
#  define OPT_SSO
# endif



# if defined(OPT_SSO)
#  define SHIFT(x)  (((x) + (1L << 11)) >> 12)
# else
#  define SHIFT(x)  (x)
# endif



# if defined(OPT_SPEED) && defined(MAD_F_MLX)
#  define OPT_DCTO
#  define MUL(x, y)  \
	({ mad_fixed64hi_t hi;  \
	mad_fixed64lo_t lo;  \
	MAD_F_MLX(hi, lo, (x), (y));  \
	hi << (32 - MAD_F_SCALEBITS - 3);  \
})
# else
#  undef OPT_DCTO
#  define MUL(x, y)  mad_f_mul((x), (y))
# endif


static
void dct32(mad_fixed_t const in[32], unsigned int slot,
		   mad_fixed_t lo[16][8], mad_fixed_t hi[16][8])
{
	mad_fixed_t t0,   t1,   t2,   t3,   t4,   t5,   t6,   t7;
	mad_fixed_t t8,   t9,   t10,  t11,  t12,  t13,  t14,  t15;
	mad_fixed_t t16,  t17,  t18,  t19,  t20,  t21,  t22,  t23;
	mad_fixed_t t24,  t25,  t26,  t27,  t28,  t29,  t30,  t31;
	mad_fixed_t t32,  t33,  t34,  t35,  t36,  t37,  t38,  t39;
	mad_fixed_t t40,  t41,  t42,  t43,  t44,  t45,  t46,  t47;
	mad_fixed_t t48,  t49,  t50,  t51,  t52,  t53,  t54,  t55;
	mad_fixed_t t56,  t57,  t58,  t59,  t60,  t61,  t62,  t63;
	mad_fixed_t t64,  t65,  t66,  t67,  t68,  t69,  t70,  t71;
	mad_fixed_t t72,  t73,  t74,  t75,  t76,  t77,  t78,  t79;
	mad_fixed_t t80,  t81,  t82,  t83,  t84,  t85,  t86,  t87;
	mad_fixed_t t88,  t89,  t90,  t91,  t92,  t93,  t94,  t95;
	mad_fixed_t t96,  t97,  t98,  t99,  t100, t101, t102, t103;
	mad_fixed_t t104, t105, t106, t107, t108, t109, t110, t111;
	mad_fixed_t t112, t113, t114, t115, t116, t117, t118, t119;
	mad_fixed_t t120, t121, t122, t123, t124, t125, t126, t127;
	mad_fixed_t t128, t129, t130, t131, t132, t133, t134, t135;
	mad_fixed_t t136, t137, t138, t139, t140, t141, t142, t143;
	mad_fixed_t t144, t145, t146, t147, t148, t149, t150, t151;
	mad_fixed_t t152, t153, t154, t155, t156, t157, t158, t159;
	mad_fixed_t t160, t161, t162, t163, t164, t165, t166, t167;
	mad_fixed_t t168, t169, t170, t171, t172, t173, t174, t175;
	mad_fixed_t t176;



# if defined(OPT_DCTO)
#  define costab1	MAD_F(0x7fd8878e)
#  define costab2	MAD_F(0x7f62368f)
#  define costab3	MAD_F(0x7e9d55fc)
#  define costab4	MAD_F(0x7d8a5f40)
#  define costab5	MAD_F(0x7c29fbee)
#  define costab6	MAD_F(0x7a7d055b)
#  define costab7	MAD_F(0x78848414)
#  define costab8	MAD_F(0x7641af3d)
#  define costab9	MAD_F(0x73b5ebd1)
#  define costab10	MAD_F(0x70e2cbc6)
#  define costab11	MAD_F(0x6dca0d14)
#  define costab12	MAD_F(0x6a6d98a4)
#  define costab13	MAD_F(0x66cf8120)
#  define costab14	MAD_F(0x62f201ac)
#  define costab15	MAD_F(0x5ed77c8a)
#  define costab16	MAD_F(0x5a82799a)
#  define costab17	MAD_F(0x55f5a4d2)
#  define costab18	MAD_F(0x5133cc94)
#  define costab19	MAD_F(0x4c3fdff4)
#  define costab20	MAD_F(0x471cece7)
#  define costab21	MAD_F(0x41ce1e65)
#  define costab22	MAD_F(0x3c56ba70)
#  define costab23	MAD_F(0x36ba2014)
#  define costab24	MAD_F(0x30fbc54d)
#  define costab25	MAD_F(0x2b1f34eb)
#  define costab26	MAD_F(0x25280c5e)
#  define costab27	MAD_F(0x1f19f97b)
#  define costab28	MAD_F(0x18f8b83c)
#  define costab29	MAD_F(0x12c8106f)
#  define costab30	MAD_F(0x0c8bd35e)
#  define costab31	MAD_F(0x0647d97c)
# else
#  define costab1	MAD_F(0x0ffb10f2)   
#  define costab2	MAD_F(0x0fec46d2)   
#  define costab3	MAD_F(0x0fd3aac0)   
#  define costab4	MAD_F(0x0fb14be8)   
#  define costab5	MAD_F(0x0f853f7e)   
#  define costab6	MAD_F(0x0f4fa0ab)   
#  define costab7	MAD_F(0x0f109082)   
#  define costab8	MAD_F(0x0ec835e8)   
#  define costab9	MAD_F(0x0e76bd7a)   
#  define costab10	MAD_F(0x0e1c5979)   
#  define costab11	MAD_F(0x0db941a3)   
#  define costab12	MAD_F(0x0d4db315)   
#  define costab13	MAD_F(0x0cd9f024)   
#  define costab14	MAD_F(0x0c5e4036)   
#  define costab15	MAD_F(0x0bdaef91)   
#  define costab16	MAD_F(0x0b504f33)   
#  define costab17	MAD_F(0x0abeb49a)   
#  define costab18	MAD_F(0x0a267993)   
#  define costab19	MAD_F(0x0987fbfe)   
#  define costab20	MAD_F(0x08e39d9d)   
#  define costab21	MAD_F(0x0839c3cd)   
#  define costab22	MAD_F(0x078ad74e)   
#  define costab23	MAD_F(0x06d74402)   
#  define costab24	MAD_F(0x061f78aa)   
#  define costab25	MAD_F(0x0563e69d)   
#  define costab26	MAD_F(0x04a5018c)   
#  define costab27	MAD_F(0x03e33f2f)   
#  define costab28	MAD_F(0x031f1708)   
#  define costab29	MAD_F(0x0259020e)   
#  define costab30	MAD_F(0x01917a6c)   
#  define costab31	MAD_F(0x00c8fb30)   
# endif

	t0   = in[0]  + in[31];  t16  = MUL(in[0]  - in[31], costab1);
	t1   = in[15] + in[16];  t17  = MUL(in[15] - in[16], costab31);

	t41  = t16 + t17;
	t59  = MUL(t16 - t17, costab2);
	t33  = t0  + t1;
	t50  = MUL(t0  - t1,  costab2);

	t2   = in[7]  + in[24];  t18  = MUL(in[7]  - in[24], costab15);
	t3   = in[8]  + in[23];  t19  = MUL(in[8]  - in[23], costab17);

	t42  = t18 + t19;
	t60  = MUL(t18 - t19, costab30);
	t34  = t2  + t3;
	t51  = MUL(t2  - t3,  costab30);

	t4   = in[3]  + in[28];  t20  = MUL(in[3]  - in[28], costab7);
	t5   = in[12] + in[19];  t21  = MUL(in[12] - in[19], costab25);

	t43  = t20 + t21;
	t61  = MUL(t20 - t21, costab14);
	t35  = t4  + t5;
	t52  = MUL(t4  - t5,  costab14);

	t6   = in[4]  + in[27];  t22  = MUL(in[4]  - in[27], costab9);
	t7   = in[11] + in[20];  t23  = MUL(in[11] - in[20], costab23);

	t44  = t22 + t23;
	t62  = MUL(t22 - t23, costab18);
	t36  = t6  + t7;
	t53  = MUL(t6  - t7,  costab18);

	t8   = in[1]  + in[30];  t24  = MUL(in[1]  - in[30], costab3);
	t9   = in[14] + in[17];  t25  = MUL(in[14] - in[17], costab29);

	t45  = t24 + t25;
	t63  = MUL(t24 - t25, costab6);
	t37  = t8  + t9;
	t54  = MUL(t8  - t9,  costab6);

	t10  = in[6]  + in[25];  t26  = MUL(in[6]  - in[25], costab13);
	t11  = in[9]  + in[22];  t27  = MUL(in[9]  - in[22], costab19);

	t46  = t26 + t27;
	t64  = MUL(t26 - t27, costab26);
	t38  = t10 + t11;
	t55  = MUL(t10 - t11, costab26);

	t12  = in[2]  + in[29];  t28  = MUL(in[2]  - in[29], costab5);
	t13  = in[13] + in[18];  t29  = MUL(in[13] - in[18], costab27);

	t47  = t28 + t29;
	t65  = MUL(t28 - t29, costab10);
	t39  = t12 + t13;
	t56  = MUL(t12 - t13, costab10);

	t14  = in[5]  + in[26];  t30  = MUL(in[5]  - in[26], costab11);
	t15  = in[10] + in[21];  t31  = MUL(in[10] - in[21], costab21);

	t48  = t30 + t31;
	t66  = MUL(t30 - t31, costab22);
	t40  = t14 + t15;
	t57  = MUL(t14 - t15, costab22);

	t69  = t33 + t34;  t89  = MUL(t33 - t34, costab4);
	t70  = t35 + t36;  t90  = MUL(t35 - t36, costab28);
	t71  = t37 + t38;  t91  = MUL(t37 - t38, costab12);
	t72  = t39 + t40;  t92  = MUL(t39 - t40, costab20);
	t73  = t41 + t42;  t94  = MUL(t41 - t42, costab4);
	t74  = t43 + t44;  t95  = MUL(t43 - t44, costab28);
	t75  = t45 + t46;  t96  = MUL(t45 - t46, costab12);
	t76  = t47 + t48;  t97  = MUL(t47 - t48, costab20);

	t78  = t50 + t51;  t100 = MUL(t50 - t51, costab4);
	t79  = t52 + t53;  t101 = MUL(t52 - t53, costab28);
	t80  = t54 + t55;  t102 = MUL(t54 - t55, costab12);
	t81  = t56 + t57;  t103 = MUL(t56 - t57, costab20);

	t83  = t59 + t60;  t106 = MUL(t59 - t60, costab4);
	t84  = t61 + t62;  t107 = MUL(t61 - t62, costab28);
	t85  = t63 + t64;  t108 = MUL(t63 - t64, costab12);
	t86  = t65 + t66;  t109 = MUL(t65 - t66, costab20);

	t113 = t69  + t70;
	t114 = t71  + t72;

	hi[15][slot] = SHIFT(t113 + t114);
	lo[ 0][slot] = SHIFT(MUL(t113 - t114, costab16));

	t115 = t73  + t74;
	t116 = t75  + t76;

	t32  = t115 + t116;

	hi[14][slot] = SHIFT(t32);

	t118 = t78  + t79;
	t119 = t80  + t81;

	t58  = t118 + t119;

	hi[13][slot] = SHIFT(t58);

	t121 = t83  + t84;
	t122 = t85  + t86;

	t67  = t121 + t122;

	t49  = (t67 * 2) - t32;

	hi[12][slot] = SHIFT(t49);

	t125 = t89  + t90;
	t126 = t91  + t92;

	t93  = t125 + t126;

	hi[11][slot] = SHIFT(t93);

	t128 = t94  + t95;
	t129 = t96  + t97;

	t98  = t128 + t129;

	t68  = (t98 * 2) - t49;

	hi[10][slot] = SHIFT(t68);

	t132 = t100 + t101;
	t133 = t102 + t103;

	t104 = t132 + t133;

	t82  = (t104 * 2) - t58;

	hi[ 9][slot] = SHIFT(t82);

	t136 = t106 + t107;
	t137 = t108 + t109;

	t110 = t136 + t137;

	t87  = (t110 * 2) - t67;

	t77  = (t87 * 2) - t68;

	hi[ 8][slot] = SHIFT(t77);

	t141 = MUL(t69 - t70, costab8);
	t142 = MUL(t71 - t72, costab24);
	t143 = t141 + t142;

	hi[ 7][slot] = SHIFT(t143);
	lo[ 8][slot] =
		SHIFT((MUL(t141 - t142, costab16) * 2) - t143);

	t144 = MUL(t73 - t74, costab8);
	t145 = MUL(t75 - t76, costab24);
	t146 = t144 + t145;

	t88  = (t146 * 2) - t77;

	hi[ 6][slot] = SHIFT(t88);

	t148 = MUL(t78 - t79, costab8);
	t149 = MUL(t80 - t81, costab24);
	t150 = t148 + t149;

	t105 = (t150 * 2) - t82;

	hi[ 5][slot] = SHIFT(t105);

	t152 = MUL(t83 - t84, costab8);
	t153 = MUL(t85 - t86, costab24);
	t154 = t152 + t153;

	t111 = (t154 * 2) - t87;

	t99  = (t111 * 2) - t88;

	hi[ 4][slot] = SHIFT(t99);

	t157 = MUL(t89 - t90, costab8);
	t158 = MUL(t91 - t92, costab24);
	t159 = t157 + t158;

	t127 = (t159 * 2) - t93;

	hi[ 3][slot] = SHIFT(t127);

	t160 = (MUL(t125 - t126, costab16) * 2) - t127;

	lo[ 4][slot] = SHIFT(t160);
	lo[12][slot] =
		SHIFT((((MUL(t157 - t158, costab16) * 2) - t159) * 2) - t160);

	t161 = MUL(t94 - t95, costab8);
	t162 = MUL(t96 - t97, costab24);
	t163 = t161 + t162;

	t130 = (t163 * 2) - t98;

	t112 = (t130 * 2) - t99;

	hi[ 2][slot] = SHIFT(t112);

	t164 = (MUL(t128 - t129, costab16) * 2) - t130;

	t166 = MUL(t100 - t101, costab8);
	t167 = MUL(t102 - t103, costab24);
	t168 = t166 + t167;

	t134 = (t168 * 2) - t104;

	t120 = (t134 * 2) - t105;

	hi[ 1][slot] = SHIFT(t120);

	t135 = (MUL(t118 - t119, costab16) * 2) - t120;

	lo[ 2][slot] = SHIFT(t135);

	t169 = (MUL(t132 - t133, costab16) * 2) - t134;

	t151 = (t169 * 2) - t135;

	lo[ 6][slot] = SHIFT(t151);

	t170 = (((MUL(t148 - t149, costab16) * 2) - t150) * 2) - t151;

	lo[10][slot] = SHIFT(t170);
	lo[14][slot] =
		SHIFT((((((MUL(t166 - t167, costab16) * 2) -
		t168) * 2) - t169) * 2) - t170);

	t171 = MUL(t106 - t107, costab8);
	t172 = MUL(t108 - t109, costab24);
	t173 = t171 + t172;

	t138 = (t173 * 2) - t110;

	t123 = (t138 * 2) - t111;

	t139 = (MUL(t121 - t122, costab16) * 2) - t123;

	t117 = (t123 * 2) - t112;

	hi[ 0][slot] = SHIFT(t117);

	t124 = (MUL(t115 - t116, costab16) * 2) - t117;

	lo[ 1][slot] = SHIFT(t124);

	t131 = (t139 * 2) - t124;

	lo[ 3][slot] = SHIFT(t131);

	t140 = (t164 * 2) - t131;

	lo[ 5][slot] = SHIFT(t140);

	t174 = (MUL(t136 - t137, costab16) * 2) - t138;

	t155 = (t174 * 2) - t139;

	t147 = (t155 * 2) - t140;

	lo[ 7][slot] = SHIFT(t147);

	t156 = (((MUL(t144 - t145, costab16) * 2) - t146) * 2) - t147;

	lo[ 9][slot] = SHIFT(t156);

	t175 = (((MUL(t152 - t153, costab16) * 2) - t154) * 2) - t155;

	t165 = (t175 * 2) - t156;

	lo[11][slot] = SHIFT(t165);

	t176 = (((((MUL(t161 - t162, costab16) * 2) -
		t163) * 2) - t164) * 2) - t165;

	lo[13][slot] = SHIFT(t176);
	lo[15][slot] =
		SHIFT((((((((MUL(t171 - t172, costab16) * 2) -
		t173) * 2) - t174) * 2) - t175) * 2) - t176);


}

# undef MUL
# undef SHIFT



# if defined(OPT_SSO)
#  if MAD_F_FRACBITS != 28
#   error "MAD_F_FRACBITS must be 28 to use OPT_SSO"
#  endif
#  define ML0(hi, lo, x, y)	((lo)  = (x) * (y))
#  define MLA(hi, lo, x, y)	((lo) += (x) * (y))
#  define MLN(hi, lo)		((lo)  = -(lo))
#  define MLZ(hi, lo)		((void) (hi), (mad_fixed_t) (lo))
#  define SHIFT(x)		((x) >> 2)
#  define PRESHIFT(x)		((MAD_F(x) + (1L << 13)) >> 14)
# else
#  define ML0(hi, lo, x, y)	MAD_F_ML0((hi), (lo), (x), (y))
#  define MLA(hi, lo, x, y)	MAD_F_MLA((hi), (lo), (x), (y))
#  define MLN(hi, lo)		MAD_F_MLN((hi), (lo))
#  define MLZ(hi, lo)		MAD_F_MLZ((hi), (lo))
#  define SHIFT(x)		(x)
#  if defined(MAD_F_SCALEBITS)
#   undef  MAD_F_SCALEBITS
#   define MAD_F_SCALEBITS	(MAD_F_FRACBITS - 12)
#   define PRESHIFT(x)		(MAD_F(x) >> 12)
#  else
#   define PRESHIFT(x)		MAD_F(x)
#  endif
# endif

static
mad_fixed_t const D[17][32] =
{

  {  PRESHIFT(0x00000000)  ,	 
    -PRESHIFT(0x0001d000)  ,
     PRESHIFT(0x000d5000)  ,
    -PRESHIFT(0x001cb000)  ,
     PRESHIFT(0x007f5000)  ,
    -PRESHIFT(0x01421000)  ,
     PRESHIFT(0x019ae000)  ,
    -PRESHIFT(0x09271000)  ,
     PRESHIFT(0x1251e000)  ,
     PRESHIFT(0x09271000)  ,
     PRESHIFT(0x019ae000)  ,
     PRESHIFT(0x01421000)  ,
     PRESHIFT(0x007f5000)  ,
     PRESHIFT(0x001cb000)  ,
     PRESHIFT(0x000d5000)  ,
     PRESHIFT(0x0001d000)  ,

     PRESHIFT(0x00000000)  ,
    -PRESHIFT(0x0001d000)  ,
     PRESHIFT(0x000d5000)  ,
    -PRESHIFT(0x001cb000)  ,
     PRESHIFT(0x007f5000)  ,
    -PRESHIFT(0x01421000)  ,
     PRESHIFT(0x019ae000)  ,
    -PRESHIFT(0x09271000)  ,
     PRESHIFT(0x1251e000)  ,
     PRESHIFT(0x09271000)  ,
     PRESHIFT(0x019ae000)  ,
     PRESHIFT(0x01421000)  ,
     PRESHIFT(0x007f5000)  ,
     PRESHIFT(0x001cb000)  ,
     PRESHIFT(0x000d5000)  ,
     PRESHIFT(0x0001d000)   },

  { -PRESHIFT(0x00001000)  ,	 
    -PRESHIFT(0x0001f000)  ,
     PRESHIFT(0x000da000)  ,
    -PRESHIFT(0x00207000)  ,
     PRESHIFT(0x007d0000)  ,
    -PRESHIFT(0x0158d000)  ,
     PRESHIFT(0x01747000)  ,
    -PRESHIFT(0x099a8000)  ,
     PRESHIFT(0x124f0000)  ,
     PRESHIFT(0x08b38000)  ,
     PRESHIFT(0x01bde000)  ,
     PRESHIFT(0x012b4000)  ,
     PRESHIFT(0x0080f000)  ,
     PRESHIFT(0x00191000)  ,
     PRESHIFT(0x000d0000)  ,
     PRESHIFT(0x0001a000)  ,

    -PRESHIFT(0x00001000)  ,
    -PRESHIFT(0x0001f000)  ,
     PRESHIFT(0x000da000)  ,
    -PRESHIFT(0x00207000)  ,
     PRESHIFT(0x007d0000)  ,
    -PRESHIFT(0x0158d000)  ,
     PRESHIFT(0x01747000)  ,
    -PRESHIFT(0x099a8000)  ,
     PRESHIFT(0x124f0000)  ,
     PRESHIFT(0x08b38000)  ,
     PRESHIFT(0x01bde000)  ,
     PRESHIFT(0x012b4000)  ,
     PRESHIFT(0x0080f000)  ,
     PRESHIFT(0x00191000)  ,
     PRESHIFT(0x000d0000)  ,
     PRESHIFT(0x0001a000)   },

  { -PRESHIFT(0x00001000)  ,	 
    -PRESHIFT(0x00023000)  ,
     PRESHIFT(0x000de000)  ,
    -PRESHIFT(0x00245000)  ,
     PRESHIFT(0x007a0000)  ,
    -PRESHIFT(0x016f7000)  ,
     PRESHIFT(0x014a8000)  ,
    -PRESHIFT(0x0a0d8000)  ,
     PRESHIFT(0x12468000)  ,
     PRESHIFT(0x083ff000)  ,
     PRESHIFT(0x01dd8000)  ,
     PRESHIFT(0x01149000)  ,
     PRESHIFT(0x00820000)  ,
     PRESHIFT(0x0015b000)  ,
     PRESHIFT(0x000ca000)  ,
     PRESHIFT(0x00018000)  ,

    -PRESHIFT(0x00001000)  ,
    -PRESHIFT(0x00023000)  ,
     PRESHIFT(0x000de000)  ,
    -PRESHIFT(0x00245000)  ,
     PRESHIFT(0x007a0000)  ,
    -PRESHIFT(0x016f7000)  ,
     PRESHIFT(0x014a8000)  ,
    -PRESHIFT(0x0a0d8000)  ,
     PRESHIFT(0x12468000)  ,
     PRESHIFT(0x083ff000)  ,
     PRESHIFT(0x01dd8000)  ,
     PRESHIFT(0x01149000)  ,
     PRESHIFT(0x00820000)  ,
     PRESHIFT(0x0015b000)  ,
     PRESHIFT(0x000ca000)  ,
     PRESHIFT(0x00018000)   },

  { -PRESHIFT(0x00001000)  ,	 
    -PRESHIFT(0x00026000)  ,
     PRESHIFT(0x000e1000)  ,
    -PRESHIFT(0x00285000)  ,
     PRESHIFT(0x00765000)  ,
    -PRESHIFT(0x0185d000)  ,
     PRESHIFT(0x011d1000)  ,
    -PRESHIFT(0x0a7fe000)  ,
     PRESHIFT(0x12386000)  ,
     PRESHIFT(0x07ccb000)  ,
     PRESHIFT(0x01f9c000)  ,
     PRESHIFT(0x00fdf000)  ,
     PRESHIFT(0x00827000)  ,
     PRESHIFT(0x00126000)  ,
     PRESHIFT(0x000c4000)  ,
     PRESHIFT(0x00015000)  ,

    -PRESHIFT(0x00001000)  ,
    -PRESHIFT(0x00026000)  ,
     PRESHIFT(0x000e1000)  ,
    -PRESHIFT(0x00285000)  ,
     PRESHIFT(0x00765000)  ,
    -PRESHIFT(0x0185d000)  ,
     PRESHIFT(0x011d1000)  ,
    -PRESHIFT(0x0a7fe000)  ,
     PRESHIFT(0x12386000)  ,
     PRESHIFT(0x07ccb000)  ,
     PRESHIFT(0x01f9c000)  ,
     PRESHIFT(0x00fdf000)  ,
     PRESHIFT(0x00827000)  ,
     PRESHIFT(0x00126000)  ,
     PRESHIFT(0x000c4000)  ,
     PRESHIFT(0x00015000)   },

  { -PRESHIFT(0x00001000)  ,	 
    -PRESHIFT(0x00029000)  ,
     PRESHIFT(0x000e3000)  ,
    -PRESHIFT(0x002c7000)  ,
     PRESHIFT(0x0071e000)  ,
    -PRESHIFT(0x019bd000)  ,
     PRESHIFT(0x00ec0000)  ,
    -PRESHIFT(0x0af15000)  ,
     PRESHIFT(0x12249000)  ,
     PRESHIFT(0x075a0000)  ,
     PRESHIFT(0x0212c000)  ,
     PRESHIFT(0x00e79000)  ,
     PRESHIFT(0x00825000)  ,
     PRESHIFT(0x000f4000)  ,
     PRESHIFT(0x000be000)  ,
     PRESHIFT(0x00013000)  ,

    -PRESHIFT(0x00001000)  ,
    -PRESHIFT(0x00029000)  ,
     PRESHIFT(0x000e3000)  ,
    -PRESHIFT(0x002c7000)  ,
     PRESHIFT(0x0071e000)  ,
    -PRESHIFT(0x019bd000)  ,
     PRESHIFT(0x00ec0000)  ,
    -PRESHIFT(0x0af15000)  ,
     PRESHIFT(0x12249000)  ,
     PRESHIFT(0x075a0000)  ,
     PRESHIFT(0x0212c000)  ,
     PRESHIFT(0x00e79000)  ,
     PRESHIFT(0x00825000)  ,
     PRESHIFT(0x000f4000)  ,
     PRESHIFT(0x000be000)  ,
     PRESHIFT(0x00013000)   },

  { -PRESHIFT(0x00001000)  ,	 
    -PRESHIFT(0x0002d000)  ,
     PRESHIFT(0x000e4000)  ,
    -PRESHIFT(0x0030b000)  ,
     PRESHIFT(0x006cb000)  ,
    -PRESHIFT(0x01b17000)  ,
     PRESHIFT(0x00b77000)  ,
    -PRESHIFT(0x0b619000)  ,
     PRESHIFT(0x120b4000)  ,
     PRESHIFT(0x06e81000)  ,
     PRESHIFT(0x02288000)  ,
     PRESHIFT(0x00d17000)  ,
     PRESHIFT(0x0081b000)  ,
     PRESHIFT(0x000c5000)  ,
     PRESHIFT(0x000b7000)  ,
     PRESHIFT(0x00011000)  ,

    -PRESHIFT(0x00001000)  ,
    -PRESHIFT(0x0002d000)  ,
     PRESHIFT(0x000e4000)  ,
    -PRESHIFT(0x0030b000)  ,
     PRESHIFT(0x006cb000)  ,
    -PRESHIFT(0x01b17000)  ,
     PRESHIFT(0x00b77000)  ,
    -PRESHIFT(0x0b619000)  ,
     PRESHIFT(0x120b4000)  ,
     PRESHIFT(0x06e81000)  ,
     PRESHIFT(0x02288000)  ,
     PRESHIFT(0x00d17000)  ,
     PRESHIFT(0x0081b000)  ,
     PRESHIFT(0x000c5000)  ,
     PRESHIFT(0x000b7000)  ,
     PRESHIFT(0x00011000)   },

  { -PRESHIFT(0x00001000)  ,	 
    -PRESHIFT(0x00031000)  ,
     PRESHIFT(0x000e4000)  ,
    -PRESHIFT(0x00350000)  ,
     PRESHIFT(0x0066c000)  ,
    -PRESHIFT(0x01c67000)  ,
     PRESHIFT(0x007f5000)  ,
    -PRESHIFT(0x0bd06000)  ,
     PRESHIFT(0x11ec7000)  ,
     PRESHIFT(0x06772000)  ,
     PRESHIFT(0x023b3000)  ,
     PRESHIFT(0x00bbc000)  ,
     PRESHIFT(0x00809000)  ,
     PRESHIFT(0x00099000)  ,
     PRESHIFT(0x000b0000)  ,
     PRESHIFT(0x00010000)  ,

    -PRESHIFT(0x00001000)  ,
    -PRESHIFT(0x00031000)  ,
     PRESHIFT(0x000e4000)  ,
    -PRESHIFT(0x00350000)  ,
     PRESHIFT(0x0066c000)  ,
    -PRESHIFT(0x01c67000)  ,
     PRESHIFT(0x007f5000)  ,
    -PRESHIFT(0x0bd06000)  ,
     PRESHIFT(0x11ec7000)  ,
     PRESHIFT(0x06772000)  ,
     PRESHIFT(0x023b3000)  ,
     PRESHIFT(0x00bbc000)  ,
     PRESHIFT(0x00809000)  ,
     PRESHIFT(0x00099000)  ,
     PRESHIFT(0x000b0000)  ,
     PRESHIFT(0x00010000)   },

  { -PRESHIFT(0x00002000)  ,	 
    -PRESHIFT(0x00035000)  ,
     PRESHIFT(0x000e3000)  ,
    -PRESHIFT(0x00397000)  ,
     PRESHIFT(0x005ff000)  ,
    -PRESHIFT(0x01dad000)  ,
     PRESHIFT(0x0043a000)  ,
    -PRESHIFT(0x0c3d9000)  ,
     PRESHIFT(0x11c83000)  ,
     PRESHIFT(0x06076000)  ,
     PRESHIFT(0x024ad000)  ,
     PRESHIFT(0x00a67000)  ,
     PRESHIFT(0x007f0000)  ,
     PRESHIFT(0x0006f000)  ,
     PRESHIFT(0x000a9000)  ,
     PRESHIFT(0x0000e000)  ,

    -PRESHIFT(0x00002000)  ,
    -PRESHIFT(0x00035000)  ,
     PRESHIFT(0x000e3000)  ,
    -PRESHIFT(0x00397000)  ,
     PRESHIFT(0x005ff000)  ,
    -PRESHIFT(0x01dad000)  ,
     PRESHIFT(0x0043a000)  ,
    -PRESHIFT(0x0c3d9000)  ,
     PRESHIFT(0x11c83000)  ,
     PRESHIFT(0x06076000)  ,
     PRESHIFT(0x024ad000)  ,
     PRESHIFT(0x00a67000)  ,
     PRESHIFT(0x007f0000)  ,
     PRESHIFT(0x0006f000)  ,
     PRESHIFT(0x000a9000)  ,
     PRESHIFT(0x0000e000)   },

  { -PRESHIFT(0x00002000)  ,	 
    -PRESHIFT(0x0003a000)  ,
     PRESHIFT(0x000e0000)  ,
    -PRESHIFT(0x003df000)  ,
     PRESHIFT(0x00586000)  ,
    -PRESHIFT(0x01ee6000)  ,
     PRESHIFT(0x00046000)  ,
    -PRESHIFT(0x0ca8d000)  ,
     PRESHIFT(0x119e9000)  ,
     PRESHIFT(0x05991000)  ,
     PRESHIFT(0x02578000)  ,
     PRESHIFT(0x0091a000)  ,
     PRESHIFT(0x007d1000)  ,
     PRESHIFT(0x00048000)  ,
     PRESHIFT(0x000a1000)  ,
     PRESHIFT(0x0000d000)  ,

    -PRESHIFT(0x00002000)  ,
    -PRESHIFT(0x0003a000)  ,
     PRESHIFT(0x000e0000)  ,
    -PRESHIFT(0x003df000)  ,
     PRESHIFT(0x00586000)  ,
    -PRESHIFT(0x01ee6000)  ,
     PRESHIFT(0x00046000)  ,
    -PRESHIFT(0x0ca8d000)  ,
     PRESHIFT(0x119e9000)  ,
     PRESHIFT(0x05991000)  ,
     PRESHIFT(0x02578000)  ,
     PRESHIFT(0x0091a000)  ,
     PRESHIFT(0x007d1000)  ,
     PRESHIFT(0x00048000)  ,
     PRESHIFT(0x000a1000)  ,
     PRESHIFT(0x0000d000)   },

  { -PRESHIFT(0x00002000)  ,	 
    -PRESHIFT(0x0003f000)  ,
     PRESHIFT(0x000dd000)  ,
    -PRESHIFT(0x00428000)  ,
     PRESHIFT(0x00500000)  ,
    -PRESHIFT(0x02011000)  ,
    -PRESHIFT(0x003e6000)  ,
    -PRESHIFT(0x0d11e000)  ,
     PRESHIFT(0x116fc000)  ,
     PRESHIFT(0x052c5000)  ,
     PRESHIFT(0x02616000)  ,
     PRESHIFT(0x007d6000)  ,
     PRESHIFT(0x007aa000)  ,
     PRESHIFT(0x00024000)  ,
     PRESHIFT(0x0009a000)  ,
     PRESHIFT(0x0000b000)  ,

    -PRESHIFT(0x00002000)  ,
    -PRESHIFT(0x0003f000)  ,
     PRESHIFT(0x000dd000)  ,
    -PRESHIFT(0x00428000)  ,
     PRESHIFT(0x00500000)  ,
    -PRESHIFT(0x02011000)  ,
    -PRESHIFT(0x003e6000)  ,
    -PRESHIFT(0x0d11e000)  ,
     PRESHIFT(0x116fc000)  ,
     PRESHIFT(0x052c5000)  ,
     PRESHIFT(0x02616000)  ,
     PRESHIFT(0x007d6000)  ,
     PRESHIFT(0x007aa000)  ,
     PRESHIFT(0x00024000)  ,
     PRESHIFT(0x0009a000)  ,
     PRESHIFT(0x0000b000)   },

  { -PRESHIFT(0x00002000)  ,	 
    -PRESHIFT(0x00044000)  ,
     PRESHIFT(0x000d7000)  ,
    -PRESHIFT(0x00471000)  ,
     PRESHIFT(0x0046b000)  ,
    -PRESHIFT(0x0212b000)  ,
    -PRESHIFT(0x0084a000)  ,
    -PRESHIFT(0x0d78a000)  ,
     PRESHIFT(0x113be000)  ,
     PRESHIFT(0x04c16000)  ,
     PRESHIFT(0x02687000)  ,
     PRESHIFT(0x0069c000)  ,
     PRESHIFT(0x0077f000)  ,
     PRESHIFT(0x00002000)  ,
     PRESHIFT(0x00093000)  ,
     PRESHIFT(0x0000a000)  ,

    -PRESHIFT(0x00002000)  ,
    -PRESHIFT(0x00044000)  ,
     PRESHIFT(0x000d7000)  ,
    -PRESHIFT(0x00471000)  ,
     PRESHIFT(0x0046b000)  ,
    -PRESHIFT(0x0212b000)  ,
    -PRESHIFT(0x0084a000)  ,
    -PRESHIFT(0x0d78a000)  ,
     PRESHIFT(0x113be000)  ,
     PRESHIFT(0x04c16000)  ,
     PRESHIFT(0x02687000)  ,
     PRESHIFT(0x0069c000)  ,
     PRESHIFT(0x0077f000)  ,
     PRESHIFT(0x00002000)  ,
     PRESHIFT(0x00093000)  ,
     PRESHIFT(0x0000a000)   },

  { -PRESHIFT(0x00003000)  ,	 
    -PRESHIFT(0x00049000)  ,
     PRESHIFT(0x000d0000)  ,
    -PRESHIFT(0x004ba000)  ,
     PRESHIFT(0x003ca000)  ,
    -PRESHIFT(0x02233000)  ,
    -PRESHIFT(0x00ce4000)  ,
    -PRESHIFT(0x0ddca000)  ,
     PRESHIFT(0x1102f000)  ,
     PRESHIFT(0x04587000)  ,
     PRESHIFT(0x026cf000)  ,
     PRESHIFT(0x0056c000)  ,
     PRESHIFT(0x0074e000)  ,
    -PRESHIFT(0x0001d000)  ,
     PRESHIFT(0x0008b000)  ,
     PRESHIFT(0x00009000)  ,

    -PRESHIFT(0x00003000)  ,
    -PRESHIFT(0x00049000)  ,
     PRESHIFT(0x000d0000)  ,
    -PRESHIFT(0x004ba000)  ,
     PRESHIFT(0x003ca000)  ,
    -PRESHIFT(0x02233000)  ,
    -PRESHIFT(0x00ce4000)  ,
    -PRESHIFT(0x0ddca000)  ,
     PRESHIFT(0x1102f000)  ,
     PRESHIFT(0x04587000)  ,
     PRESHIFT(0x026cf000)  ,
     PRESHIFT(0x0056c000)  ,
     PRESHIFT(0x0074e000)  ,
    -PRESHIFT(0x0001d000)  ,
     PRESHIFT(0x0008b000)  ,
     PRESHIFT(0x00009000)   },

  { -PRESHIFT(0x00003000)  ,	 
    -PRESHIFT(0x0004f000)  ,
     PRESHIFT(0x000c8000)  ,
    -PRESHIFT(0x00503000)  ,
     PRESHIFT(0x0031a000)  ,
    -PRESHIFT(0x02326000)  ,
    -PRESHIFT(0x011b5000)  ,
    -PRESHIFT(0x0e3dd000)  ,
     PRESHIFT(0x10c54000)  ,
     PRESHIFT(0x03f1b000)  ,
     PRESHIFT(0x026ee000)  ,
     PRESHIFT(0x00447000)  ,
     PRESHIFT(0x00719000)  ,
    -PRESHIFT(0x00039000)  ,
     PRESHIFT(0x00084000)  ,
     PRESHIFT(0x00008000)  ,

    -PRESHIFT(0x00003000)  ,
    -PRESHIFT(0x0004f000)  ,
     PRESHIFT(0x000c8000)  ,
    -PRESHIFT(0x00503000)  ,
     PRESHIFT(0x0031a000)  ,
    -PRESHIFT(0x02326000)  ,
    -PRESHIFT(0x011b5000)  ,
    -PRESHIFT(0x0e3dd000)  ,
     PRESHIFT(0x10c54000)  ,
     PRESHIFT(0x03f1b000)  ,
     PRESHIFT(0x026ee000)  ,
     PRESHIFT(0x00447000)  ,
     PRESHIFT(0x00719000)  ,
    -PRESHIFT(0x00039000)  ,
     PRESHIFT(0x00084000)  ,
     PRESHIFT(0x00008000)   },

  { -PRESHIFT(0x00004000)  ,	 
    -PRESHIFT(0x00055000)  ,
     PRESHIFT(0x000bd000)  ,
    -PRESHIFT(0x0054c000)  ,
     PRESHIFT(0x0025d000)  ,
    -PRESHIFT(0x02403000)  ,
    -PRESHIFT(0x016ba000)  ,
    -PRESHIFT(0x0e9be000)  ,
     PRESHIFT(0x1082d000)  ,
     PRESHIFT(0x038d4000)  ,
     PRESHIFT(0x026e7000)  ,
     PRESHIFT(0x0032e000)  ,
     PRESHIFT(0x006df000)  ,
    -PRESHIFT(0x00053000)  ,
     PRESHIFT(0x0007d000)  ,
     PRESHIFT(0x00007000)  ,

    -PRESHIFT(0x00004000)  ,
    -PRESHIFT(0x00055000)  ,
     PRESHIFT(0x000bd000)  ,
    -PRESHIFT(0x0054c000)  ,
     PRESHIFT(0x0025d000)  ,
    -PRESHIFT(0x02403000)  ,
    -PRESHIFT(0x016ba000)  ,
    -PRESHIFT(0x0e9be000)  ,
     PRESHIFT(0x1082d000)  ,
     PRESHIFT(0x038d4000)  ,
     PRESHIFT(0x026e7000)  ,
     PRESHIFT(0x0032e000)  ,
     PRESHIFT(0x006df000)  ,
    -PRESHIFT(0x00053000)  ,
     PRESHIFT(0x0007d000)  ,
     PRESHIFT(0x00007000)   },

  { -PRESHIFT(0x00004000)  ,	 
    -PRESHIFT(0x0005b000)  ,
     PRESHIFT(0x000b1000)  ,
    -PRESHIFT(0x00594000)  ,
     PRESHIFT(0x00192000)  ,
    -PRESHIFT(0x024c8000)  ,
    -PRESHIFT(0x01bf2000)  ,
    -PRESHIFT(0x0ef69000)  ,
     PRESHIFT(0x103be000)  ,
     PRESHIFT(0x032b4000)  ,
     PRESHIFT(0x026bc000)  ,
     PRESHIFT(0x00221000)  ,
     PRESHIFT(0x006a2000)  ,
    -PRESHIFT(0x0006a000)  ,
     PRESHIFT(0x00075000)  ,
     PRESHIFT(0x00007000)  ,

    -PRESHIFT(0x00004000)  ,
    -PRESHIFT(0x0005b000)  ,
     PRESHIFT(0x000b1000)  ,
    -PRESHIFT(0x00594000)  ,
     PRESHIFT(0x00192000)  ,
    -PRESHIFT(0x024c8000)  ,
    -PRESHIFT(0x01bf2000)  ,
    -PRESHIFT(0x0ef69000)  ,
     PRESHIFT(0x103be000)  ,
     PRESHIFT(0x032b4000)  ,
     PRESHIFT(0x026bc000)  ,
     PRESHIFT(0x00221000)  ,
     PRESHIFT(0x006a2000)  ,
    -PRESHIFT(0x0006a000)  ,
     PRESHIFT(0x00075000)  ,
     PRESHIFT(0x00007000)   },

  { -PRESHIFT(0x00005000)  ,	 
    -PRESHIFT(0x00061000)  ,
     PRESHIFT(0x000a3000)  ,
    -PRESHIFT(0x005da000)  ,
     PRESHIFT(0x000b9000)  ,
    -PRESHIFT(0x02571000)  ,
    -PRESHIFT(0x0215c000)  ,
    -PRESHIFT(0x0f4dc000)  ,
     PRESHIFT(0x0ff0a000)  ,
     PRESHIFT(0x02cbf000)  ,
     PRESHIFT(0x0266e000)  ,
     PRESHIFT(0x00120000)  ,
     PRESHIFT(0x00662000)  ,
    -PRESHIFT(0x0007f000)  ,
     PRESHIFT(0x0006f000)  ,
     PRESHIFT(0x00006000)  ,

    -PRESHIFT(0x00005000)  ,
    -PRESHIFT(0x00061000)  ,
     PRESHIFT(0x000a3000)  ,
    -PRESHIFT(0x005da000)  ,
     PRESHIFT(0x000b9000)  ,
    -PRESHIFT(0x02571000)  ,
    -PRESHIFT(0x0215c000)  ,
    -PRESHIFT(0x0f4dc000)  ,
     PRESHIFT(0x0ff0a000)  ,
     PRESHIFT(0x02cbf000)  ,
     PRESHIFT(0x0266e000)  ,
     PRESHIFT(0x00120000)  ,
     PRESHIFT(0x00662000)  ,
    -PRESHIFT(0x0007f000)  ,
     PRESHIFT(0x0006f000)  ,
     PRESHIFT(0x00006000)   },

  { -PRESHIFT(0x00005000)  ,	 
    -PRESHIFT(0x00068000)  ,
     PRESHIFT(0x00092000)  ,
    -PRESHIFT(0x0061f000)  ,
    -PRESHIFT(0x0002d000)  ,
    -PRESHIFT(0x025ff000)  ,
    -PRESHIFT(0x026f7000)  ,
    -PRESHIFT(0x0fa13000)  ,
     PRESHIFT(0x0fa13000)  ,
     PRESHIFT(0x026f7000)  ,
     PRESHIFT(0x025ff000)  ,
     PRESHIFT(0x0002d000)  ,
     PRESHIFT(0x0061f000)  ,
    -PRESHIFT(0x00092000)  ,
     PRESHIFT(0x00068000)  ,
     PRESHIFT(0x00005000)  ,

    -PRESHIFT(0x00005000)  ,
    -PRESHIFT(0x00068000)  ,
     PRESHIFT(0x00092000)  ,
    -PRESHIFT(0x0061f000)  ,
    -PRESHIFT(0x0002d000)  ,
    -PRESHIFT(0x025ff000)  ,
    -PRESHIFT(0x026f7000)  ,
    -PRESHIFT(0x0fa13000)  ,
     PRESHIFT(0x0fa13000)  ,
     PRESHIFT(0x026f7000)  ,
     PRESHIFT(0x025ff000)  ,
     PRESHIFT(0x0002d000)  ,
     PRESHIFT(0x0061f000)  ,
    -PRESHIFT(0x00092000)  ,
     PRESHIFT(0x00068000)  ,
     PRESHIFT(0x00005000)   }

};

# if defined(ASO_SYNTH)
void synth_full(struct mad_synth *, struct mad_frame const *,
				unsigned int, unsigned int);
# else

static
void synth_full(struct mad_synth *synth, struct mad_frame const *frame,
				unsigned int nch, unsigned int ns)
{
	unsigned int phase, ch, s, sb, pe, po;
	mad_fixed_t *pcm1, *pcm2, (*filter)[2][2][16][8];
	mad_fixed_t const (*sbsample)[36][32];
	register mad_fixed_t (*fe)[8], (*fx)[8], (*fo)[8];
	register mad_fixed_t const (*Dptr)[32], *ptr;
	register mad_fixed64hi_t hi;
	register mad_fixed64lo_t lo;

	for (ch = 0; ch < nch; ++ch) {
		sbsample = &frame->sbsample[ch];
		filter   = &synth->filter[ch];
		phase    = synth->phase;
		pcm1     = synth->pcm.samples[ch];

		for (s = 0; s < ns; ++s) {
			dct32((*sbsample)[s], phase >> 1,
				(*filter)[0][phase & 1], (*filter)[1][phase & 1]);

			pe = phase & ~1;
			po = ((phase - 1) & 0xf) | 1;



			fe = &(*filter)[0][ phase & 1][0];
			fx = &(*filter)[0][~phase & 1][0];
			fo = &(*filter)[1][~phase & 1][0];

			Dptr = &D[0];

			ptr = *Dptr + po;
			ML0(hi, lo, (*fx)[0], ptr[ 0]);
			MLA(hi, lo, (*fx)[1], ptr[14]);
			MLA(hi, lo, (*fx)[2], ptr[12]);
			MLA(hi, lo, (*fx)[3], ptr[10]);
			MLA(hi, lo, (*fx)[4], ptr[ 8]);
			MLA(hi, lo, (*fx)[5], ptr[ 6]);
			MLA(hi, lo, (*fx)[6], ptr[ 4]);
			MLA(hi, lo, (*fx)[7], ptr[ 2]);
			MLN(hi, lo);

			ptr = *Dptr + pe;
			MLA(hi, lo, (*fe)[0], ptr[ 0]);
			MLA(hi, lo, (*fe)[1], ptr[14]);
			MLA(hi, lo, (*fe)[2], ptr[12]);
			MLA(hi, lo, (*fe)[3], ptr[10]);
			MLA(hi, lo, (*fe)[4], ptr[ 8]);
			MLA(hi, lo, (*fe)[5], ptr[ 6]);
			MLA(hi, lo, (*fe)[6], ptr[ 4]);
			MLA(hi, lo, (*fe)[7], ptr[ 2]);

			*pcm1++ = SHIFT(MLZ(hi, lo));

			pcm2 = pcm1 + 30;

			for (sb = 1; sb < 16; ++sb) {
				++fe;
				++Dptr;



				ptr = *Dptr + po;
				ML0(hi, lo, (*fo)[0], ptr[ 0]);
				MLA(hi, lo, (*fo)[1], ptr[14]);
				MLA(hi, lo, (*fo)[2], ptr[12]);
				MLA(hi, lo, (*fo)[3], ptr[10]);
				MLA(hi, lo, (*fo)[4], ptr[ 8]);
				MLA(hi, lo, (*fo)[5], ptr[ 6]);
				MLA(hi, lo, (*fo)[6], ptr[ 4]);
				MLA(hi, lo, (*fo)[7], ptr[ 2]);
				MLN(hi, lo);

				ptr = *Dptr + pe;
				MLA(hi, lo, (*fe)[7], ptr[ 2]);
				MLA(hi, lo, (*fe)[6], ptr[ 4]);
				MLA(hi, lo, (*fe)[5], ptr[ 6]);
				MLA(hi, lo, (*fe)[4], ptr[ 8]);
				MLA(hi, lo, (*fe)[3], ptr[10]);
				MLA(hi, lo, (*fe)[2], ptr[12]);
				MLA(hi, lo, (*fe)[1], ptr[14]);
				MLA(hi, lo, (*fe)[0], ptr[ 0]);

				*pcm1++ = SHIFT(MLZ(hi, lo));

				ptr = *Dptr - pe;
				ML0(hi, lo, (*fe)[0], ptr[31 - 16]);
				MLA(hi, lo, (*fe)[1], ptr[31 - 14]);
				MLA(hi, lo, (*fe)[2], ptr[31 - 12]);
				MLA(hi, lo, (*fe)[3], ptr[31 - 10]);
				MLA(hi, lo, (*fe)[4], ptr[31 -  8]);
				MLA(hi, lo, (*fe)[5], ptr[31 -  6]);
				MLA(hi, lo, (*fe)[6], ptr[31 -  4]);
				MLA(hi, lo, (*fe)[7], ptr[31 -  2]);

				ptr = *Dptr - po;
				MLA(hi, lo, (*fo)[7], ptr[31 -  2]);
				MLA(hi, lo, (*fo)[6], ptr[31 -  4]);
				MLA(hi, lo, (*fo)[5], ptr[31 -  6]);
				MLA(hi, lo, (*fo)[4], ptr[31 -  8]);
				MLA(hi, lo, (*fo)[3], ptr[31 - 10]);
				MLA(hi, lo, (*fo)[2], ptr[31 - 12]);
				MLA(hi, lo, (*fo)[1], ptr[31 - 14]);
				MLA(hi, lo, (*fo)[0], ptr[31 - 16]);

				*pcm2-- = SHIFT(MLZ(hi, lo));

				++fo;
			}

			++Dptr;

			ptr = *Dptr + po;
			ML0(hi, lo, (*fo)[0], ptr[ 0]);
			MLA(hi, lo, (*fo)[1], ptr[14]);
			MLA(hi, lo, (*fo)[2], ptr[12]);
			MLA(hi, lo, (*fo)[3], ptr[10]);
			MLA(hi, lo, (*fo)[4], ptr[ 8]);
			MLA(hi, lo, (*fo)[5], ptr[ 6]);
			MLA(hi, lo, (*fo)[6], ptr[ 4]);
			MLA(hi, lo, (*fo)[7], ptr[ 2]);

			*pcm1 = SHIFT(-MLZ(hi, lo));
			pcm1 += 16;

			phase = (phase + 1) % 16;
		}
	}
}
# endif


static
void synth_half(struct mad_synth *synth, struct mad_frame const *frame,
				unsigned int nch, unsigned int ns)
{
	unsigned int phase, ch, s, sb, pe, po;
	mad_fixed_t *pcm1, *pcm2, (*filter)[2][2][16][8];
	mad_fixed_t const (*sbsample)[36][32];
	register mad_fixed_t (*fe)[8], (*fx)[8], (*fo)[8];
	register mad_fixed_t const (*Dptr)[32], *ptr;
	register mad_fixed64hi_t hi;
	register mad_fixed64lo_t lo;

	for (ch = 0; ch < nch; ++ch) {
		sbsample = &frame->sbsample[ch];
		filter   = &synth->filter[ch];
		phase    = synth->phase;
		pcm1     = synth->pcm.samples[ch];

		for (s = 0; s < ns; ++s) {
			dct32((*sbsample)[s], phase >> 1,
				(*filter)[0][phase & 1], (*filter)[1][phase & 1]);

			pe = phase & ~1;
			po = ((phase - 1) & 0xf) | 1;



			fe = &(*filter)[0][ phase & 1][0];
			fx = &(*filter)[0][~phase & 1][0];
			fo = &(*filter)[1][~phase & 1][0];

			Dptr = &D[0];

			ptr = *Dptr + po;
			ML0(hi, lo, (*fx)[0], ptr[ 0]);
			MLA(hi, lo, (*fx)[1], ptr[14]);
			MLA(hi, lo, (*fx)[2], ptr[12]);
			MLA(hi, lo, (*fx)[3], ptr[10]);
			MLA(hi, lo, (*fx)[4], ptr[ 8]);
			MLA(hi, lo, (*fx)[5], ptr[ 6]);
			MLA(hi, lo, (*fx)[6], ptr[ 4]);
			MLA(hi, lo, (*fx)[7], ptr[ 2]);
			MLN(hi, lo);

			ptr = *Dptr + pe;
			MLA(hi, lo, (*fe)[0], ptr[ 0]);
			MLA(hi, lo, (*fe)[1], ptr[14]);
			MLA(hi, lo, (*fe)[2], ptr[12]);
			MLA(hi, lo, (*fe)[3], ptr[10]);
			MLA(hi, lo, (*fe)[4], ptr[ 8]);
			MLA(hi, lo, (*fe)[5], ptr[ 6]);
			MLA(hi, lo, (*fe)[6], ptr[ 4]);
			MLA(hi, lo, (*fe)[7], ptr[ 2]);

			*pcm1++ = SHIFT(MLZ(hi, lo));

			pcm2 = pcm1 + 14;

			for (sb = 1; sb < 16; ++sb) {
				++fe;
				++Dptr;



				if (!(sb & 1)) {
					ptr = *Dptr + po;
					ML0(hi, lo, (*fo)[0], ptr[ 0]);
					MLA(hi, lo, (*fo)[1], ptr[14]);
					MLA(hi, lo, (*fo)[2], ptr[12]);
					MLA(hi, lo, (*fo)[3], ptr[10]);
					MLA(hi, lo, (*fo)[4], ptr[ 8]);
					MLA(hi, lo, (*fo)[5], ptr[ 6]);
					MLA(hi, lo, (*fo)[6], ptr[ 4]);
					MLA(hi, lo, (*fo)[7], ptr[ 2]);
					MLN(hi, lo);

					ptr = *Dptr + pe;
					MLA(hi, lo, (*fe)[7], ptr[ 2]);
					MLA(hi, lo, (*fe)[6], ptr[ 4]);
					MLA(hi, lo, (*fe)[5], ptr[ 6]);
					MLA(hi, lo, (*fe)[4], ptr[ 8]);
					MLA(hi, lo, (*fe)[3], ptr[10]);
					MLA(hi, lo, (*fe)[2], ptr[12]);
					MLA(hi, lo, (*fe)[1], ptr[14]);
					MLA(hi, lo, (*fe)[0], ptr[ 0]);

					*pcm1++ = SHIFT(MLZ(hi, lo));

					ptr = *Dptr - po;
					ML0(hi, lo, (*fo)[7], ptr[31 -  2]);
					MLA(hi, lo, (*fo)[6], ptr[31 -  4]);
					MLA(hi, lo, (*fo)[5], ptr[31 -  6]);
					MLA(hi, lo, (*fo)[4], ptr[31 -  8]);
					MLA(hi, lo, (*fo)[3], ptr[31 - 10]);
					MLA(hi, lo, (*fo)[2], ptr[31 - 12]);
					MLA(hi, lo, (*fo)[1], ptr[31 - 14]);
					MLA(hi, lo, (*fo)[0], ptr[31 - 16]);

					ptr = *Dptr - pe;
					MLA(hi, lo, (*fe)[0], ptr[31 - 16]);
					MLA(hi, lo, (*fe)[1], ptr[31 - 14]);
					MLA(hi, lo, (*fe)[2], ptr[31 - 12]);
					MLA(hi, lo, (*fe)[3], ptr[31 - 10]);
					MLA(hi, lo, (*fe)[4], ptr[31 -  8]);
					MLA(hi, lo, (*fe)[5], ptr[31 -  6]);
					MLA(hi, lo, (*fe)[6], ptr[31 -  4]);
					MLA(hi, lo, (*fe)[7], ptr[31 -  2]);

					*pcm2-- = SHIFT(MLZ(hi, lo));
				}

				++fo;
			}

			++Dptr;

			ptr = *Dptr + po;
			ML0(hi, lo, (*fo)[0], ptr[ 0]);
			MLA(hi, lo, (*fo)[1], ptr[14]);
			MLA(hi, lo, (*fo)[2], ptr[12]);
			MLA(hi, lo, (*fo)[3], ptr[10]);
			MLA(hi, lo, (*fo)[4], ptr[ 8]);
			MLA(hi, lo, (*fo)[5], ptr[ 6]);
			MLA(hi, lo, (*fo)[6], ptr[ 4]);
			MLA(hi, lo, (*fo)[7], ptr[ 2]);

			*pcm1 = SHIFT(-MLZ(hi, lo));
			pcm1 += 8;

			phase = (phase + 1) % 16;
		}
	}
}


void mad_synth_frame(struct mad_synth *synth, struct mad_frame const *frame)
{
	unsigned int nch, ns;
	void (*synth_frame)(struct mad_synth *, struct mad_frame const *,
		unsigned int, unsigned int);

	nch = MAD_NCHANNELS(&frame->header);
	ns  = MAD_NSBSAMPLES(&frame->header);

	synth->pcm.samplerate = frame->header.samplerate;
	synth->pcm.channels   = nch;
	synth->pcm.length     = 32 * ns;

	synth_frame = synth_full;

	if (frame->options & MAD_OPTION_HALFSAMPLERATE) {
		synth->pcm.samplerate /= 2;
		synth->pcm.length     /= 2;

		synth_frame = synth_half;
	}

	synth_frame(synth, frame, nch, ns);

	synth->phase = (synth->phase + ns) % 16;
}


}