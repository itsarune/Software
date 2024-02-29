struct OverallStrategy
{
    // invariant: at-most one attacker and goalie
    bool goalie;
    bool attacker;

    std::size_t defenders;
    std::size_t support_offense;
};
