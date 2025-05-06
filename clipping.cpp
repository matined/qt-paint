#include "clipping.h"
#include <cmath>

static bool inside(const QPoint& p, const QPoint& edgeStart, const QPoint& edgeEnd, int orientationSign)
{
    // For convex clip polygon, test if point lies on the inner side of the edge.
    // orientationSign is +1 for CCW clip polygon, -1 for CW clip polygon.
    int dx1 = edgeEnd.x() - edgeStart.x();
    int dy1 = edgeEnd.y() - edgeStart.y();
    int dx2 = p.x() - edgeStart.x();
    int dy2 = p.y() - edgeStart.y();
    int cross = dx1 * dy2 - dy1 * dx2;
    return orientationSign * cross >= 0; // inside if on the correct side or on the edge
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

    // Determine orientation sign of clip polygon (CCW positive, CW negative)
    long long area2 = 0;
    for (size_t i = 0; i < clip.size(); ++i) {
        const QPoint& a = clip[i];
        const QPoint& b = clip[(i + 1) % clip.size()];
        area2 += static_cast<long long>(a.x()) * b.y() - static_cast<long long>(b.x()) * a.y();
    }
    int orientationSign = (area2 >= 0) ? 1 : -1;

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
            if (inside(p, edgeStart, edgeEnd, orientationSign)) {
                if (inside(s, edgeStart, edgeEnd, orientationSign)) {
                    // Case 1: both inside
                    output.push_back(p);
                } else {
                    // Case 4: s outside, p inside
                    QPoint iPt = intersect(s, p, edgeStart, edgeEnd);
                    output.push_back(iPt);
                    output.push_back(p);
                }
            } else if (inside(s, edgeStart, edgeEnd, orientationSign)) {
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