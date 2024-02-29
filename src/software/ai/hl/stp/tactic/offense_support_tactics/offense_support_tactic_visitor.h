#pragma once

class OffenseSupportTactic;
class ReceiverTactic;

class OffenseSupportTacticVisitor
{
    public:
        virtual void visit(const OffenseSupportTactic &tactic) = delete;

        virtual void visit(const ReceiverTactic &tactic) = 0;
};
