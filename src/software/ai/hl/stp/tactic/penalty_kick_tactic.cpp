/**
 * Implementation of the PenaltyKickTactic
 */
#include "software/ai/hl/stp/tactic/penalty_kick_tactic.h"

#include "shared/constants.h"
#include "software/ai/evaluation/calc_best_shot.h"
#include "software/ai/hl/stp/action/kick_action.h"
#include "software/ai/hl/stp/action/move_action.h"
#include "software/geom/algorithms/intersection.h"
#include "software/geom/algorithms/closest_point.h"
#include "software/logger/logger.h"


PenaltyKickTactic::PenaltyKickTactic(const Ball& ball, const Field& field,
                                     const std::optional<Robot>& enemy_goalie,
                                     bool loop_forever)
    : Tactic(loop_forever, {RobotCapability::Move}),
      ball(ball),
      field(field),
      enemy_goalie(enemy_goalie)
{
}

void PenaltyKickTactic::updateWorldParams(const World& world)
{
    this->enemy_goalie = world.enemyTeam().goalie();
    this->ball         = world.ball();
    this->field        = world.field();
}

double PenaltyKickTactic::calculateRobotCost(const Robot& robot, const World& world) const
{
    // We normalize with the total field length so that robots that are within the field
    // have a cost less than 1
    double cost = (robot.position() - world.ball().position()).length() /
                  world.field().totalXLength();
    return std::clamp<double>(cost, 0, 1);
}

bool PenaltyKickTactic::evaluatePenaltyShot()
{
    // If there is no goalie, the net is wide open
    if (!enemy_goalie.has_value())
    {
        return true;
    }

    // The value of a penalty shot is proportional to how far away the enemy goalie is
    // from the current shot of the robot

    // We will make a penalty shot if the enemy goalie cannot accelerate in time to block
    // it
    Segment goal_line = Segment(field.enemyGoalpostPos(), field.enemyGoalpostNeg());

    Ray shot_ray = Ray(ball.position(),
                       Vector(robot_->orientation().cos(), robot_->orientation().sin()));

    std::vector<Point> intersections = intersection(shot_ray, goal_line);
	
    if (!intersections.empty())
    {
        // If we have an intersection, calculate if we have a viable shot

		const Segment ball_to_goal =
			Segment(intersections[0], ball.position());
		const Point block_position = closestPoint(enemy_goalie.value().position(),
												  ball_to_goal);
		const Vector goalie_to_block_position =
			(block_position - enemy_goalie.value().position());
		const Segment ball_to_block = Segment(ball.position(), block_position);
		const double time_to_pass_keeper =
			fabs(ball_to_block.length() / PENALTY_KICK_SHOT_SPEED) +
			SSL_VISION_DELAY;
												  
        // const double shooter_to_goal_distance =
        //     (robot.value().position() - intersections[0]).length();
        // const Vector goalie_to_block_position =
        //     (intersections[0] - enemy_goalie.value().position());

        // Based on constant acceleration -> // dX = init_vel*t + 0.5*a*t^2
        //          dX - init_vel - (0.5*a*t)t
        const double max_enemy_movement_x =
            enemy_goalie.value().velocity().x() * time_to_pass_keeper +
            0.5 * std::copysign(1, goalie_to_block_position.x())
			* PENALTY_KICK_GOALIE_MAX_ACC * pow(time_to_pass_keeper, 2);
        const double max_enemy_movement_y =
            enemy_goalie.value().velocity().y() * time_to_pass_keeper +
            0.5 * std::copysign(1, goalie_to_block_position.y())
			* PENALTY_KICK_GOALIE_MAX_ACC * pow(time_to_pass_keeper, 2);

        // If the maximum distance that the goalie can move is less than actual
        // distance it must move to reach the ball, return true for a viable
        // shot
        // Not simplifying this if statement makes the code logic slightly
        // easier to understand
        if ((fabs(goalie_to_block_position.x()) >
             (fabs(max_enemy_movement_x) + ROBOT_MAX_RADIUS_METERS)) ||
            (fabs(goalie_to_block_position.y()) >
             (fabs(max_enemy_movement_y) + ROBOT_MAX_RADIUS_METERS)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // If a shot in our current direction will not end in a goal, don't
        // shoot
        return false;
    }
}

Point PenaltyKickTactic::evaluateNextShotPosition()
{
    // Evaluate if the goalie is closer to the negative or positive goalpost
    if (enemy_goalie.has_value())
    {
        double goalie_dist_to_neg_goalpost =
            (field.enemyGoalpostNeg() - enemy_goalie.value().position()).lengthSquared();
        double goalie_dist_to_pos_goalpost =
            (field.enemyGoalpostPos() - enemy_goalie.value().position()).lengthSquared();

        return goalie_dist_to_neg_goalpost > goalie_dist_to_pos_goalpost
                   ? field.enemyGoalpostNeg() + Vector(0, PENALTY_KICK_POST_OFFSET)
                   : field.enemyGoalpostPos() + Vector(0, -PENALTY_KICK_POST_OFFSET);
    }
    else
    {
        // Return the center of the enemy goal
        return Point(field.enemyGoalpostPos().x(), 0);
    }
}

void PenaltyKickTactic::calculateNextAction(ActionCoroutine::push_type& yield)
{
    // We will need to keep track of time so we don't break the rules by taking
    // too long
    const Timestamp penalty_kick_start = robot_->timestamp();
    const Timestamp complete_approach  = penalty_kick_start + Duration::fromSeconds(4);

    auto approach_ball_move_act = std::make_shared<MoveAction>(false);
    auto approach_goalie_act    = std::make_shared<MoveAction>(
        false, MoveAction::ROBOT_CLOSE_TO_DEST_THRESHOLD, Angle());
    auto kick_act               = std::make_shared<KickAction>();
    Vector behind_ball_vector = (ball.position() - field.enemyGoalpostPos());
    // A point behind the ball for the kicker to approach
    Point behind_ball =
        ball.position() + behind_ball_vector.normalize(BALL_MAX_RADIUS_METERS +
                                                       DIST_TO_FRONT_OF_ROBOT_METERS);

    // approach the ball in preparation to shoot
    while (!approach_ball_move_act->done() &&
           ((robot_.value().position() - behind_ball).length() >
            MoveAction::ROBOT_CLOSE_TO_DEST_THRESHOLD) &&
           (robot_->timestamp() <= complete_approach))
    {
        approach_ball_move_act->updateControlParams(
            *robot_, behind_ball, (-behind_ball_vector).orientation(), 0,
            DribblerMode::MAX_FORCE, BallCollisionType::ALLOW);
        yield(approach_ball_move_act);
		std::cout << "(robot->timestamp() <= complete_approach) "
				  << (robot_->timestamp() - penalty_kick_start) << '\n';
    }

    const Timestamp penalty_start_approaching_goalie = robot_->timestamp();

    // prepare to shoot:
    // keep moving forward toward the goalkeeper until we have a viable shot or
    // our timeout runs out
    Angle shot_angle;
    do
    {
        const Point next_shot_position = evaluateNextShotPosition();
        shot_angle = (next_shot_position - ball.position()).orientation();
        const Point next_robot_position = robot_.value().position() + Vector(0.48, 0);
		std::cout << "robot->timestamp() - penalty_start_approaching_goalie " <<
			(robot_->timestamp() - penalty_start_approaching_goalie) << '\n';
        approach_goalie_act->updateControlParams(*robot_, next_robot_position, shot_angle,
                                                 0, DribblerMode::MAX_FORCE,
                                                 BallCollisionType::ALLOW);
        yield(approach_goalie_act);
    } while (((robot_->timestamp() - penalty_start_approaching_goalie) <=
              PENALTY_FORCE_SHOOT_TIMEOUT) &&
             !evaluatePenaltyShot());

    // shoot
    do
    {
        kick_act->updateControlParams(*robot_, ball.position(), shot_angle,
                                         PENALTY_KICK_SHOT_SPEED);
        yield(kick_act);
    } while (!kick_act->done() &&
             ((robot_->timestamp() - penalty_kick_start) < PENALTY_SHOT_TIMEOUT));
}

void PenaltyKickTactic::accept(TacticVisitor& visitor) const
{
    visitor.visit(*this);
}

Ball PenaltyKickTactic::getBall() const
{
    return this->ball;
}

Field PenaltyKickTactic::getField() const
{
    return this->field;
}
