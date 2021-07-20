#ifndef __SOPG_MSG__
#define __SOPG_MSG__

/* ****************************************************************************************************************** */
/*                                                    Public Types                                                    */
/* ****************************************************************************************************************** */

typedef enum {
    TEXT,
    SIGNAL
} SopgMessageType_t;

typedef struct {
    SopgMessageType_t type;
    union {
        char ch;
        uint8_t signal;
    } data;
} SopgMessage_t;



#endif

