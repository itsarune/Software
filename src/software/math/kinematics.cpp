#include "software/math/kinematics.h"

Duration timeToDestination(const Point& start, const Point& destination, const Vector& velocity)
{
    if (velocity.length() == 0)
    {
        return Duration::fromSeconds(std::numeric_limits<double>::infinity());
    }
    Vector travel_vector = destination - start;

    // project the velocity onto the travel vector
    double speed_to_destination = std::abs(velocity.dot(travel_vector) / travel_vector.length());

    if (speed_to_destination == 0)
    {
        return Duration::fromSeconds(std::numeric_limits<double>::infinity());
    }

    return Duration::fromSeconds(
        travel_vector.length() / speed_to_destination);
}

Duration timeToDestination(const Point& start, const Point& destination, const Vector& initial_velocity,
        double acceleration)
{
    if (acceleration == 0)
    {
        return timeToDestination(start, destination, initial_velocity);
    }
    Vector travel_vector = destination - start;

    // project the initial velocity onto the travel vector
    double initial_speed = std::abs(initial_velocity.dot(travel_vector) / travel_vector.length());
    double travel_distance_m = travel_vector.length();
    
    return Duration::fromSeconds((-initial_speed + std::sqrt(initial_speed * initial_speed + 2 * acceleration * travel_distance_m))
        / (2 * acceleration));
}
