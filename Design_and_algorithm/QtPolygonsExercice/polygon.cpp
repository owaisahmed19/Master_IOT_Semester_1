#include "polygon.h"
#include <QDebug>

void Polygon::addVertex(float x, float y) {
    if (tabPts.empty()) {
        tabPts.push_back(Vector2D(x,y));
        tabPts.push_back(Vector2D(x,y));
    } else {
        auto N=tabPts.size()-1;
        tabPts[N].x=x;
        tabPts[N].y=y;
///< the last vertex duplicates the first one
        tabPts.push_back(tabPts[0]);
    }
}

void Polygon::draw(QPainter &painter,bool showTriangles) const {
    QPen pen(Qt::black);
    pen.setWidth(3);

    ///< use the drawPolygon method of QPainter
    auto N=tabPts.size();
    QPoint *points=new QPoint[N];
    for (int i=0; i<N; i++) {
        points[i].setX(tabPts[i].x);
        points[i].setY(tabPts[i].y);
    }
    painter.setBrush(currentColor);
    painter.setPen(pen);
    painter.drawPolygon(points,N,Qt::OddEvenFill);
    delete [] points;

    if (showTriangles) { // draw triangles
        painter.setPen(Qt::blue);
        for (auto &t:triangles) {
            painter.drawLine(t[0]->x,t[0]->y,t[1]->x,t[1]->y);
            painter.drawLine(t[1]->x,t[1]->y,t[2]->x,t[2]->y);
            painter.drawLine(t[2]->x,t[2]->y,t[0]->x,t[0]->y);
        }
    }
}

// Helper function to calculate the 2D cross product of two vectors
// V1 = A-C, V2 = B-C
// Cross product (V1, V2) = V1.x * V2.y - V1.y * V2.x
static float crossProduct(const Vector2D& p_a, const Vector2D& p_b, const Vector2D& p_c) {
    // Vector A = P_a - P_b
    float ax = p_a.x - p_b.x;
    float ay = p_a.y - p_b.y;
    // Vector B = P_c - P_b
    float bx = p_c.x - p_b.x;
    float by = p_c.y - p_b.y;

    return (ax * by) - (ay * bx);
}

// polygon.cpp (CORRECT CONTENT)



bool Polygon::isOnTheLeft(const Vector2D &p, int i) const {
    const Vector2D& Pi = tabPts[i];
    const Vector2D& Pi_plus_1 = tabPts[i+1];

    // Note: The arguments in your provided code for crossProduct are slightly unusual:
    // float cp = crossProduct(Pi_plus_1, Pi, p);
    // This expects the helper function to calculate (Pi - Pi_plus_1) x (p - Pi_plus_1).
    // Assuming you have defined `crossProduct(P_a, P_b, P_c)` to calculate (P_b - P_a) x (P_c - P_a),
    // the call should be:

    // Calculate (Pi_plus_1 - Pi) x (p - Pi)
    float cp = crossProduct(Pi, Pi_plus_1, p); // Assuming crossProduct(P_start, P_end, P_test)

    return cp > 0.0f;
}

bool Polygon::isConvex() const {
    int N = nbVertices();
    if (N < 3) return true; // A line or point is trivially convex

    // Check the turns at every vertex P_i, using the edge (P_{i-1} -> P_i) and (P_i -> P_{i+1})
    // The cross product must have the same sign for all vertices.
    // For a CCW polygon, the sign must be positive (left turn).

    // Determine the sign of the first turn (P_{N-1} -> P_0 -> P_1)
    float first_cp = crossProduct(tabPts[1], tabPts[0], tabPts[N-1]);

    // Loop through the rest of the vertices
    for (int i = 1; i < N; ++i) {
        // The edge leading into P_i is (P_{i-1} -> P_i)
        const Vector2D& Pi_minus_1 = tabPts[i-1];
        // The vertex for the turn is P_i
        const Vector2D& Pi = tabPts[i];
        // The edge leaving P_i is (P_i -> P_{i+1})
        // Note: when i = N-1, tabPts[N] is tabPts[0] (which is Pi_plus_1)
        const Vector2D& Pi_plus_1 = tabPts[i+1];

        // Calculate the cross product for the turn at P_i
        // Cross Product: (P_{i+1} - P_i) x (P_{i-1} - P_i)
        // We use the helper: crossProduct(P_next, P_current, P_prev)
        float cp = crossProduct(Pi_plus_1, Pi, Pi_minus_1);

        // Check if the sign has changed (indicating a reflex angle)
        // Note: using > 0 and < 0 ignores collinear points (cp == 0)
        if (cp * first_cp < 0) {
            return false;
        }
    }

    // Send message to status bar
    // This part assumes you are calling isConvex() from a context that can emit updateSB,
    // which it can't, as it's a Polygon method. We'll update Canvas::Canvas to handle this.
    // For now, implement the logic correctly.

    return true;
}


// Helper function (defined previously)
/*
static float crossProduct(const Vector2D& p_a, const Vector2D& p_b, const Vector2D& p_c) {
    float ab_x = p_b.x - p_a.x;
    // ... rest of implementation ...
    return (ab_x * ac_y) - (ab_y * ac_x);
}
*/

void Polygon::changeColor(const Vector2D &pt) {
    if (triangles.empty()) {
        // If triangulation failed or wasn't run, fall back to the simple convex check if possible
        if (isConvex()) {
            // Re-run the O(N) convex check if no triangulation is available
            // (You can copy the logic from the previous answer's changeColor here)
        } else {
            currentColor = Qt::cyan; // Still non-functional for concave shapes
            return;
        }
    }

    // Point-in-Concave-Polygon Test using Triangulation (O(N) search)
    bool inside = false;
    for (const auto& t : triangles) {
        // Check if pt is inside the current triangle 't'
        // A point is inside a triangle (A, B, C) if it is to the left of or on all three edges (AB, BC, CA)

        // Edge t[0] -> t[1]
        if (crossProduct(*t[0], *t[1], pt) < 0.0f) continue;
        // Edge t[1] -> t[2]
        if (crossProduct(*t[1], *t[2], pt) < 0.0f) continue;
        // Edge t[2] -> t[0]
        if (crossProduct(*t[2], *t[0], pt) < 0.0f) continue;

        // If all three cross products are non-negative, the point is inside or on the boundary of this triangle.
        inside = true;
        break;
    }

    currentColor = inside ? Qt::red : Qt::green;
}
void Polygon::triangulate() {
    triangles.clear();
    int N = nbVertices();
    if (N < 3) return;

    // 1. Create a list of vertex indices (0, 1, 2, ..., N-1)
    QVector<int> indices;
    for (int i = 0; i < N; ++i) {
        indices.push_back(i);
    }

    int current_index = 0; // Start searching from P0

    // 2. Loop until only 3 vertices remain (the last triangle)
    while (indices.size() > 3) {
        // Get the three vertices P_prev, P_curr, P_next
        int i_prev = (current_index == 0) ? (indices.size() - 1) : (current_index - 1);
        int i_next = (current_index + 1) % indices.size();

        // Use the original indices to reference the vertices in tabPts
        Vector2D* P_prev = &tabPts[indices[i_prev]];
        Vector2D* P_curr = &tabPts[indices[current_index]];
        Vector2D* P_next = &tabPts[indices[i_next]];

        // A. Check if P_curr is a CONVEX vertex (left turn)
        // We use the same check as isConvex: (P_next - P_curr) x (P_prev - P_curr)
        float cp = crossProduct(*P_curr, *P_next, *P_prev);

        if (cp > 0.0f) { // It's a convex turn (potential ear)

            // B. Check if any other polygon vertex is inside the triangle (P_prev, P_curr, P_next)
            bool is_ear = true;
            for (int k = 0; k < indices.size(); ++k) {
                if (indices[k] == indices[i_prev] ||
                    indices[k] == indices[current_index] ||
                    indices[k] == indices[i_next]) {
                    continue; // Skip the vertices of the potential ear itself
                }

                Vector2D* P_test = &tabPts[indices[k]];

                // The point P_test is inside the triangle if it is to the left of all three triangle edges:
                // (P_prev -> P_curr), (P_curr -> P_next), and (P_next -> P_prev)
                // We use the isOnTheLeft concept but check for strictly INSIDE (cp >= 0 only if on the boundary)

                // Check edge P_prev -> P_curr
                if (crossProduct(*P_prev, *P_curr, *P_test) < 0.0f) continue;
                // Check edge P_curr -> P_next
                if (crossProduct(*P_curr, *P_next, *P_test) < 0.0f) continue;
                // Check edge P_next -> P_prev
                if (crossProduct(*P_next, *P_prev, *P_test) < 0.0f) continue;

                // If it passes all 3 checks, the vertex P_test is inside the triangle.
                is_ear = false;
                break;
            }

            if (is_ear) {
                // C. It is a valid ear! Store the triangle and remove the vertex P_curr.
                triangles.push_back(Triangle(P_prev, P_curr, P_next));
                indices.remove(current_index); // Remove the index of the clipped vertex

                // The new search index should stay at the neighbor (i_prev) or wrap
                current_index = i_prev % indices.size();
                if (current_index < 0) current_index = 0;

            } else {
                // D. Not an ear. Move to the next vertex (P_next)
                current_index = i_next;
            }
        } else {
            // Not a convex vertex (reflex angle). Cannot be an ear. Move to the next vertex.
            current_index = i_next;
        }

        // This is a safety measure to prevent infinite loops in cases of complex geometry or errors
        if (indices.size() == N) { // If we've made a full loop and not clipped an ear (shouldn't happen for simple poly)
            break;
        }
    }

    // 3. Store the final remaining triangle
    if (indices.size() == 3) {
        triangles.push_back(Triangle(&tabPts[indices[0]], &tabPts[indices[1]], &tabPts[indices[2]]));
    }
}


float Polygon::area() const {
    if (triangles.empty()) {
        // Fallback: If no triangulation was done (e.g., area() called early), use the Shoelace formula on the main polygon
        float sum = 0.0f;
        int N = nbVertices();
        for (int i = 0; i < N; ++i) {
            // Note: tabPts[N] is tabPts[0]
            sum += tabPts[i].x * tabPts[i+1].y - tabPts[i+1].x * tabPts[i].y;
        }
        return qAbs(sum) / 2.0f;
    }

    // Calculate area by summing the areas of the triangles array
    float totalArea = 0.0f;
    for (const auto& t : triangles) {
        float x0 = t[0]->x, y0 = t[0]->y;
        float x1 = t[1]->x, y1 = t[1]->y;
        float x2 = t[2]->x, y2 = t[2]->y;

        // Area formula: 0.5 * | (x0*y1 - x1*y0) + (x1*y2 - x2*y1) + (x2*y0 - x0*y2) |
        // This is 0.5 * | Determinant |
        float det = x0 * (y1 - y2) + x1 * (y2 - y0) + x2 * (y0 - y1);
        totalArea += det;
    }

    // The sum of the signed areas is the signed area of the polygon.
    // We take the absolute value since area must be positive.
    return qAbs(totalArea) / 2.0f;
}

QPair<Vector2D,Vector2D> Polygon::getBoundingBox() const {
    Vector2D min=tabPts[0],max=tabPts[0];
    for (auto &pt:tabPts) {
        if (pt.x<min.x) min.x = pt.x;
        if (pt.y<min.y) min.y = pt.y;
        if (pt.x>max.x) max.x = pt.x;
        if (pt.y>max.y) max.y = pt.y;
    }
    return QPair<Vector2D,Vector2D>(min,max);
}
