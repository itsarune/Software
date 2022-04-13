#include "software/ai/hl/stp/play/interplay_messages/interplay_messages.h"

struct PassInProgressMessage : InterplayMessage
{
    message_type = PASS_IN_PROGRESS;
    Pass pass;
}
