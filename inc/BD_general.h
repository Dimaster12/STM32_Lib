#ifndef __general_H_dim
#define __general_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#define NO_MATTER 0

#define BIT_15_MASK	(0x8000)
#define BIT_14_MASK	(0x4000)
#define BIT_13_MASK	(0x2000)
#define BIT_12_MASK	(0x1000)
#define BIT_11_MASK	(0x800)
#define BIT_10_MASK	(0x400)
#define BIT_9_MASK	(0x200)
#define BIT_8_MASK	(0x100)
	 
#define BIT_7_MASK	(0x80)
#define BIT_6_MASK	(0x40)
#define BIT_5_MASK	(0x20)
#define BIT_4_MASK	(0x10)
#define BIT_3_MASK	(0x08)
#define BIT_2_MASK	(0x04)
#define BIT_1_MASK	(0x02)
#define BIT_0_MASK	(0x01)

#if defined (F1XX)
	#include "stm32f1xx_hal.h"
	#define ARM_MATH_CM3
#elif defined (F2XX) 
	#include "stm32f2xx_hal.h"
	#define ARM_MATH_CM3
#elif defined (F3XX)
	#include "stm32f3xx_hal.h"
	#define ARM_MATH_CM4
#elif defined (F4XX)
	#include "stm32f4xx_hal.h"
	#define ARM_MATH_CM4
#elif defined (F7XX)
	#include "stm32f7xx_hal.h"
	#define ARM_MATH_CM7
#endif

#include <math.h>
//#include <arm_math.h>
#include <string.h>
#include <stdbool.h>

//#define FIXMATH_NO_64BIT 
#define FIXMATH_NO_ROUNDING
#define FIXMATH_NO_OVERFLOW
//#define FIXMATH_OPTIMIZE_8BIT

#include "fix16.h"

#define Q16_ADD(a, b)		fix16_add(a, b)
#define Q16_SUB(a, b)		fix16_sub(a, b)
#define Q16_MUL(a, b)		fix16_mul(a, b)
#define Q16_DIV(a, b)		fix16_div(a, b)

#define Q16toFL(a)			fix16_to_float(a)
#define FLtoQ16(a)			fix16_from_float(a)
#define Q16toINT(a)			fix16_to_int(a)
#define INTtoQ16(a)			fix16_from_int(a)

#ifdef __cplusplus
}
#endif
#endif
