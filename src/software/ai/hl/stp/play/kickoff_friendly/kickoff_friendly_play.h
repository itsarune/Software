#include "software/ai/hl/stp/play/play.h"
#include "software/ai/hl/stp/play/crease_defense/crease_defense_play.h"
#include "software/ai/hl/stp/play/shoot_or_pass/shoot_or_pass_play.h"
#include "software/ai/hl/stp/tactic/all_tactics.h"

class KickoffFriendlyPlay : public Play
{
    public:
        KickoffFriendlyPlay(std::shared_ptr<const AiConfig> config);
        
        void getNextTactics(TacticCoroutine::push_type &yield, const World &world) override;
        void updateTactics(const PlayUpdate &play_update) override;
        
    private:
        PriorityTacticVector choosePassOrChip(const PlayUpdate &play_update);
    
        std::shared_ptr<KickoffFriendlyPlay>    kickoff_friendly_play;
        std::shared_ptr<CreaseDefensePlay>     crease_defense_play;
        std::shared_ptr<ShootOrPassPlay>        shoot_or_pass_play;
};