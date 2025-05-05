#ifndef CLIPPING_H
#define CLIPPING_H

#include <QPoint>
#include <vector>

// Sutherland–Hodgman polygon clipping algorithm
// Clips the subject polygon by the clip polygon (assumed to be convex).
// Returns the resulting clipped polygon vertices in order. If the result has
// fewer than 3 vertices, the returned vector will be empty.
std::vector<QPoint> sutherlandHodgman(const std::vector<QPoint>& subject,
                                      const std::vector<QPoint>& clip);

#endif // CLIPPING_H 