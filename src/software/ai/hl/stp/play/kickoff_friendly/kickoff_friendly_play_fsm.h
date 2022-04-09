#include "software/ai/hl/stp/play/play_fsm.h"
#include "software/ai/hl/stp/tactic/move/move_tactic.h"

struct KickoffFriendlyPlayFSM
{
    class SetupState;

    struct ControlParams
    {
    };
    
    DEFINE_PLAY_UPDATE_STRUCT_WITH_CONTROL_AND_COMMON_PARAMS
        
    void moveToSetupPositions(const Update& event);

    auto operator()()
    {
        using namespace boost::sml;
        
        DEFINE_SML_STATE(SetupState)
        
        DEFINE_SML_EVENT(Update)
        
        DEFINE_SML_ACTION(moveToSetupPositions)
//        DEFINE_SML_ACTION(startPass)
//        
//        DEFINE_SML_GUARD(normalStartIssue)
        
        return make_transition_table(
            *SetupState_S + Update_E / moveToSetupPositions_A = SetupState_S,
            X + Update_E = X);
    }
};