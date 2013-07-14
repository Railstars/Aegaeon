#ifndef __DCC_DECODER_H__
#define __DCC_DECODER_H__

#include <stdint.h>


#define DCC_FLAGS_READY (1<<0)
#define DCC_FLAGS_OPS_MODE_MASK (1<<1)
#define DCC_OPERATIONS_MODE (DCC_flags & (1<<1))
#define DCC_SERVICE_MODE (!DCC_OPERATIONS_MODE)

#define DCC_FLAGS_DC_MODE_MASK (1<<2)
#define DCC_DC_MODE (DCC_flags & (1<<2))
#define DCC_DCC_MODE (!DCC_DC_MODE)

void DCC_Decoder_Process_Bit(uint8_t bit_val);

void DCC_Decoder_Update(void);

#endif //__DCC_DECODER_H__
