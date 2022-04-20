#include "software/ai/hl/stp/play/kickoff_friendly/kickoff_friendly_play.h"

KickoffFriendlyPlay::KickoffFriendlyPlay(std::shared_ptr<const AiConfig> config)
    : Play(config, true),
      kickoff_friendly_play(std::make_shared<KickoffFriendlyPlay>(ai_config)),
      crease_defense_play(std::make_shared<CreaseDefensePlay>(ai_config)),
      shoot_or_pass_play(std::make_shared<ShootOrPassPlay>(ai_config))
{
}

void KickoffFriendlyPlay::getNextTactics(TacticCoroutine::push_type &yield, const World &world)
{
    // This function doesn't get called so it does nothing
    while (true)
    {
        yield({{}});
    }
}

void KickoffFriendlyPlay::updateTactics(const PlayUpdate &play_update)
{
    PriorityTacticVector tactics_to_return;
    
    if (play_update.world.gameState().isSetupState())
    {   
        int num_crease_defenders    = 2;
        int num_setup_tactics       = play_update.num_tactics - num_crease_defenders;
        if (num_setup_tactics < 1) 
        {
            num_setup_tactics       = 1;
            num_crease_defenders    -= 1;
        }
        
        crease_defense_play->updateControlParams(play_update.world.ball().position(),
                                                TbotsProto::MaxAllowedSpeedMode::PHYSICAL_LIMIT);
        crease_defense_play->updateTactics(
            PlayUpdate(play_update.world, num_crease_defenders,
                       [&tactics_to_return](PriorityTacticVector new_tactics)
                       {
                          for (const auto &tactic_vector : new_tactics)
                          {
                              tactics_to_return.push_back(tactic_vector);
                          }
                        }));
        
        kickoff_friendly_play->updateTactics(PlayUpdate(play_update.world, num_setup_tactics,
                                                        [&tactics_to_return](PriorityTacticVector new_tactics)
                                                        {
                                                            for (const auto &tactic_vector : new_tactics)
                                                            {
                                                                tactics_to_return.push_back(tactic_vector);
                                                            }
                                                        }));
    }
    else
    {
        PriorityTacticVector new_tactics = choosePassOrChip(play_update);
        for (const auto &tactic_vector : new_tactics)
        {
            tactics_to_return.push_back(tactic_vector);
        }
    }
    
    play_update.set_tactics(tactics_to_return);
}

PriorityTacticVector KickoffFriendlyPlay::choosePassOrChip(const PlayUpdate &play_update)
{
    PriorityTacticVector tactics;
    
    // if we only have 1 robot available, chip away
    if (play_update.num_tactics == 1)
    {
        std::shared_ptr<KickoffChipTactic>kickoff_chip_tactic = std::make_shared<KickoffChipTactic>();
        kickoff_chip_tactic->updateControlParams(play_update.world.ball().position(), 
                                                 play_update.world.field().centerPoint() + Vector(play_update.world.field().xLength() / 6, 0));
        
        tactics[0].push_back(kickoff_chip_tactic);
    }
    else if (play_update.num_tactics > 1)
    {
        int num_attackers = std::max(static_cast<int>(play_update.num_tactics)-2, 2);
        int num_defenders = static_cast<int>(play_update.num_tactics) - num_attackers;
        
        crease_defense_play->updateControlParams(play_update.world.ball().position(),
                                                TbotsProto::MaxAllowedSpeedMode::PHYSICAL_LIMIT);
        crease_defense_play->updateTactics(PlayUpdate(play_update.world, num_defenders,
                                                     [&tactics](PriorityTacticVector new_tactics)
                                                      {
                                                          for (const auto &tactic_vector : new_tactics)
                                                          {
                                                              tactics.push_back(tactic_vector);
                                                          }
                                                      }));
        
        shoot_or_pass_play->updateTactics(PlayUpdate(play_update.world, num_attackers,
                                                    [&tactics](PriorityTacticVector new_tactics)
                                                     {
                                                         for (const auto &tactic_vector : new_tactics)
                                                         {
                                                             tactics.push_back(tactic_vector);
                                                         }
                                                     }));
    }
    else
    {
        LOG(WARNING) << "KickoffFriendPlay was passed in 0 tactics to work with.";
    }
    
    return tactics;
}
