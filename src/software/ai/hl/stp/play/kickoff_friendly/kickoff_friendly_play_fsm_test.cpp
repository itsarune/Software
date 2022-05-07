#include "software/ai/hl/stp/play/kickoff_friendly/kickoff_friendly_play_fsm.h"

#include "software/test_util/test_util.h"
#include "shared/parameter/cpp_dynamic_parameters.h"
#include <gtest/gtest.h>

TEST(KickoffFriendlyPlaySetupFSMTest, test_transitions)
{
	World world = ::TestUtil::createBlankTestingWorldDivB();

	FSM<KickoffFriendlyPlayFSM> fsm(
					KickoffFriendlyPlayFSM{});

	EXPECT_TRUE(fsm.is(boost::sml::state<KickoffFriendlyPlayFSM::StartState));
}
