#pragma once

#include "software/ai/hl/stp/tactic/offense_support_tactics/receiver/receiver_tactic.h"

class CherryPickerTactic : public ReceiverTactic
{
    public:
        inline std::vector<EighteenZoneId> getTargetPassZones() const
        {
            return {EighteenZoneId::ZONE_13, EighteenZoneId::ZONE_16, EighteenZoneId::ZONE_14, EighteenZoneId::ZONE_15, EighteenZoneId::ZONE_18};
        }
};
