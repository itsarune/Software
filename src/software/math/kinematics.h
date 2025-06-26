#pragma once

#include "software/geom/point.h"
#include "software/time/duration.h"

Duration timeToDestination(const Point& start, const Point& destination, const Vector& velocity);

Duration timeToDestination(const Point& start, const Point& destination, const Vector& initial_velocity,
        double acceleration);
