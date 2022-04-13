#include "software/util/make_enum/make_enum.h"

MAKE_ENUM(InterplayMessageType, PASS_IN_PROGRESS);

struct InterplayMessage
{
    InterplayMessageType message_type;
};
