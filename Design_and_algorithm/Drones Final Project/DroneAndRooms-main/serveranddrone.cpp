#include "serveranddrone.h"
#include <QDebug>

Link::Link(Server *n1,Server *n2,const QPair<Vector2D,Vector2D> &edge):
    node1(n1),node2(n2) {
    // computation of the length of the link
    Vector2D center=0.5*(edge.first+edge.second);
    distance = (center-Vector2D(n1->position.x(),n1->position.y())).length();
    distance += (center-Vector2D(n2->position.x(),n2->position.y())).length();
    edgeCenter=QPointF(center.x,center.y);
}

void Link::draw(QPainter &painter) {
    painter.drawLine(node1->position,edgeCenter);
    painter.drawLine(node2->position,edgeCenter);
}

// Motions of the drone to reach the "destination" position
// Updates the drone's position, velocity, and orientation every time step.
void Drone::move(qreal dt) {
    // 1. Safety check: ensure drone has a target and is within a recognized area
    if (!target || !connectedTo) return;

    // Calculate vector and distance to the current intermediate destination
    Vector2D dir = destination - position;
    double d = dir.length();

    // --- State Transition Logic (Exercise 3) ---
    // Triggered when the drone is close enough to its current waypoint
    if (d < minDistance) {

        // Case A: Drone reached a Server position
        if (destination == Vector2D(connectedTo->position.x(), connectedTo->position.y())) {
            // If this isn't the final destination, find the next door to cross
            if (connectedTo != target) {
                // bestDistance[target->id].first stores the next Link to take
                Link *nextLink = connectedTo->bestDistance[target->id].first;
                if (nextLink) {
                    // Head toward the middle of the shared edge (the door)
                    destination = nextLink->getEdgeCenter();
                }
            }
        }
        // Case B: Drone reached a Door (Edge Center)
        else {
            Link *currentLink = connectedTo->bestDistance[target->id].first;
            if (currentLink) {
                // Determine the server on the opposite side of the door
                // If node1 is where we just came from, move to node2, and vice versa.
                connectedTo = (currentLink->getNode1() == connectedTo) ?
                                  currentLink->getNode2() : currentLink->getNode1();

                // Set the new destination to the center of the next room's server
                destination = Vector2D(connectedTo->position.x(), connectedTo->position.y());
            }
        }

        // Recalculate direction and distance for the new waypoint
        dir = destination - position;
        d = dir.length();
    }

    // --- Physics and Movement Calculations ---
    // Implements acceleration, maximum speed capping, and arrival smoothing


    if (d < slowDownDistance) {
        // Linear deceleration to avoid overshooting the waypoint
        speed = (d * speedLocal / slowDownDistance) * dir;
    } else {
        // Standard acceleration logic
        speed += (accelation * dt / d) * dir;

        // Cap speed at speedMax
        if (speed.length() > speedMax) {
            speed.normalize();
            speed *= speedMax;
        }
    }

    // Update the position based on calculated velocity
    position += (dt * speed);

    // --- Orientation (Azimut) Update ---
    // Rotates the drone icon to face the direction of travel
    if (speed.length() > 0.01) {
        // Normalize speed to get the direction vector
        Vector2D Vn = (1.0 / speed.length()) * speed;

        // Calculate angle in degrees using trigonometry
        // Handles vertical movement and quadrant adjustments for the UI.
        azimut = (Vn.y == 0) ? (Vn.x > 0 ? -90 : 90) :
                     (Vn.y > 0 ? 180.0 - 180.0 * atan(Vn.x / Vn.y) / M_PI :
                          -180.0 * atan(Vn.x / Vn.y) / M_PI);
    }
}

Server* Drone::overflownArea(QList<Server>& list) {
    auto it=list.begin();
    while (it!=list.end() && !it->area.contains(position)) {
        it++;
    }
    connectedTo= it!=list.end()?&(*it):nullptr;
    return connectedTo;
}
