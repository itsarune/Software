#include "software/ai/evaluation/scoring/support_tactics/feasibility_scorer.h"

FeasibilityScorer::FeasibilityScorer(std::shared_ptr<Strategy> strategy)
    : strategy_(strategy)
{
}

double FeasibilityScorer::score(
    const TypedSupportTacticCandidate<CherryPickerTactic> &candidate)
{
    return 1;
}

double FeasibilityScorer::score(
    const TypedSupportTacticCandidate<ReceiverTactic> &candidate)
{
    return 1;
}
