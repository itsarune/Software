#pragma once

#include <gtest/gtest.h>

#include "software/simulated_tests/validation/validation_function.h"
#include "software/world/world.h"
#include "software/geom/algorithms/contains.h"

/**
 * Checks that the ball always moves forward until it is scored.
 * 
 * @param world_ptr the world pointer given by the simulator. Gets updated every tick
 * @param yield yields control to the next routine (coroutines)
 */
void ballAlwaysMovesForward(std::shared_ptr<World> world_ptr,
                    ValidationCoroutine::push_type& yield);