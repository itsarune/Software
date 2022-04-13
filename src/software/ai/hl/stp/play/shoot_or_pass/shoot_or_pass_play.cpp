#include "software/ai/hl/stp/play/shoot_or_pass/shoot_or_pass_play.h"

#include "shared/constants.h"
#include "shared/parameter/cpp_dynamic_parameters.h"
#include "software/logger/logger.h"
#include "software/util/generic_factory/generic_factory.h"

ShootOrPassPlay::ShootOrPassPlay(std::shared_ptr<const AiConfig> config, std::vector<InterplayMessage> incoming_play_messages)
    : Play(config, true), fsm{ShootOrPassPlayFSM{config}}, control_params{}
{
	control_params = {};
	for (InterplayMessage message : incoming_play_messages)
	{
		if (message.message_type == InterplayMessageType::PASS_IN_PROGRESS)
		{
			PassInProgressMessage pass_in_progress_message = static_cast<PassInProgressMessage>(message);
			fsm = {ShootOrPassPlayFSM{config, std::make_optional<Pass>(pass_in_progress.pass)}};
			return;		
		}	
	}
	fsm = {ShootOrPassPlayFSM{config}};
}

void ShootOrPassPlay::getNextTactics(TacticCoroutine::push_type &yield,
                                     const World &world)
{
    // This function doesn't get called so it does nothing
    while (true)
    {
        yield({{}});
    }
}

void ShootOrPassPlay::updateTactics(const PlayUpdate &play_update)
{
    fsm.process_event(ShootOrPassPlayFSM::Update(control_params, play_update));
}

// Register this play in the genericFactory
static TGenericFactory<std::string, Play, ShootOrPassPlay, AiConfig> factory;
