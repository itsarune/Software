#include "software/ai/hl/stp/play/kickoff_friendly/kickoff_friendly_play_fsm.h"

#include <gtest/gtest.h>

TEST(KickoffFriendlyPlaySetupFSMTest, test_transitions)
{
	World world = ::TestUtil::createSSLDivisionBField();

	FSM<KickoffFriendlyPlayFSM> fsm(
					KickoffFriendlyPlayFSM{std::make_shared<const ThunderbotsConfig>()->getAiConfig()});
	EXPECT_TRUE(fsm.is(boost::sml::state<KickoffFriendlyPlayFSM::StartState));
}
