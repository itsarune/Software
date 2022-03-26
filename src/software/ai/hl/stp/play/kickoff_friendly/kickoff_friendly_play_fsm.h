#include "software/ai/hl/stp/play/play_fsm.h"

class KickoffFriendlyPlayFSM
{
    class SetupState;

    void moveToSetupPositions(const Update& event);

    auto operator()()
    {
        using namespace boost::sml;
        
        DEFINE_SML_STATE(SetupState)
        
        DEFINE_SML_EVENT(Update)
        
        DEFINE_SML_ACTION(moveToSetupPositions)
        
        return make_transition_table(
            *SetupState_S + Update_E / moveToSetupPositions_A = SetupState_S,
            X + Update_E = X);
    }
};