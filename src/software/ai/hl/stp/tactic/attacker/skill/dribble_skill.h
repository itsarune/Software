#include "software/ai/hl/stp/tactic/attacker/skill/skill.h"
#include "software/ai/hl/stp/tactic/dribble/dribble_fsm.h"
#include "software/ai/hl/stp/tactic/tactic_fsm.h"

class DribbleSkill : public Skill
{
   public:
    DribbleSkill(const TbotsProto::AiConfig& ai_config,
                 std::shared_ptr<Strategy> strategy, double initial_score);

    bool done() const override;

    void updatePrimitive(const Robot& robot, const World& world,
                         const TacticUpdate& tactic_update) override;

   private:
    FSM<DribbleFSM> fsm;
    DribbleFSM::ControlParams control_params;
};
