#include "clipping.h"
#include <cmath>

static bool inside(const QPoint& p, const QPoint& edgeStart, const QPoint& edgeEnd)
{
    // For convex clip polygon, test if point is on the left side of edge (edgeStart->edgeEnd)
    // Using cross product (edgeEnd-edgeStart) x (p-edgeStart) >= 0
    int dx1 = edgeEnd.x() - edgeStart.x();
    int dy1 = edgeEnd.y() - edgeStart.y();
    int dx2 = p.x() - edgeStart.x();
    int dy2 = p.y() - edgeStart.y();
    int cross = dx1 * dy2 - dy1 * dx2;
    return cross >= 0; // on left or on the line
}

static QPoint intersect(const QPoint& s, const QPoint& p,
                        const QPoint& edgeStart, const QPoint& edgeEnd)
{
    // Line (s -> p): parametric s + t*(p-s)
    // Edge (edgeStart -> edgeEnd): parametric edgeStart + u*(edgeEnd-edgeStart)
    // We want intersection where the two lines meet.
    double x1 = s.x();
    double y1 = s.y();
    double x2 = p.x();
    double y2 = p.y();

    double x3 = edgeStart.x();
    double y3 = edgeStart.y();
    double x4 = edgeEnd.x();
    double y4 = edgeEnd.y();

    double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (std::abs(denom) < 1e-6) {
        // Lines are parallel; return start point as fallback.
        return s;
    }
    double pre  = (x1 * y2 - y1 * x2);
    double post = (x3 * y4 - y3 * x4);
    double x = (pre * (x3 - x4) - (x1 - x2) * post) / denom;
    double y = (pre * (y3 - y4) - (y1 - y2) * post) / denom;

    return QPoint(static_cast<int>(std::round(x)), static_cast<int>(std::round(y)));
}

std::vector<QPoint> sutherlandHodgman(const std::vector<QPoint>& subject,
                                      const std::vector<QPoint>& clip)
{
    if (subject.empty() || clip.size() < 3)
        return {};

    std::vector<QPoint> output = subject;

    for (size_t i = 0; i < clip.size(); ++i) {
        const QPoint& edgeStart = clip[i];
        const QPoint& edgeEnd   = clip[(i + 1) % clip.size()];

        std::vector<QPoint> input = output;
        output.clear();
        if (input.empty())
            break;

        QPoint s = input.back(); // start with the last point in the input polygon
        for (const QPoint& p : input) {
            if (inside(p, edgeStart, edgeEnd)) {
                if (inside(s, edgeStart, edgeEnd)) {
                    // Case 1: both inside
                    output.push_back(p);
                } else {
                    // Case 4: s outside, p inside
                    QPoint iPt = intersect(s, p, edgeStart, edgeEnd);
                    output.push_back(iPt);
                    output.push_back(p);
                }
            } else if (inside(s, edgeStart, edgeEnd)) {
                // Case 2: s inside, p outside
                QPoint iPt = intersect(s, p, edgeStart, edgeEnd);
                output.push_back(iPt);
            }
            // Case 3 (both outside) yields no points
            s = p;
        }
    }

    if (output.size() < 3)
        output.clear();

    return output;
} 