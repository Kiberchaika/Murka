#pragma once

#include <array>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

class Delaunator {
    typedef float fp; // You can change the floating-point precision here
    #define NIL (std::numeric_limits<std::size_t>::max)()
    #define EPSILON (std::numeric_limits<fp>::epsilon)()

private:
    std::vector<std::size_t> _triangles;
    std::vector<std::size_t> _halfedges;

    std::size_t _hashSize;
    std::vector<std::size_t> _hullPrev;
    std::vector<std::size_t> _hullNext;
    std::vector<std::size_t> _hullTri;
    std::vector<std::size_t> _hullHash;

    std::vector<std::size_t> _ids;
    std::vector<fp> _dists;

    fp _cx, _cy;
    std::size_t _hullStart;

    std::vector<std::size_t> EDGE_STACK;

public:

    std::size_t trianglesLen;
    std::vector<std::size_t> triangles;
    std::vector<std::size_t> halfedges;
    std::vector<std::size_t> hull;

    std::size_t n = 0; // number of points
    std::vector<fp> coords;

    Delaunator(const std::vector<fp>& _coords) : coords(_coords) {
        n = coords.size() / 2;
        
        // arrays that will store the triangulation graph
        const std::size_t maxTriangles = (std::max)(2 * n - 5, (std::size_t)0ul);
        _triangles.resize(maxTriangles * 3);
        _halfedges.resize(maxTriangles * 3);

        // temporary arrays for tracking the edges of the advancing convex hull
        _hashSize = (std::size_t) std::ceil(std::sqrt(n));
        _hullPrev.resize(n);
        _hullNext.resize(n);
        _hullTri.resize(n);
        
        _hullHash.resize(_hashSize); 

		size_t a = (std::numeric_limits<size_t>::max)();
		std::fill(_hullHash.begin(), _hullHash.end(), a);

        // temporary arrays for sorting points
        _ids.resize(n);
        _dists.resize(n);

        EDGE_STACK.resize(512);

        update();
    }

private:
    inline void update() {

        // populate an array of point indices; calculate input data bbox
        fp minX = (std::numeric_limits<fp>::max)();
        fp minY = (std::numeric_limits<fp>::max)();
        fp maxX = (std::numeric_limits<fp>::lowest)();
        fp maxY = (std::numeric_limits<fp>::lowest)();

        for (std::size_t i = 0; i < n; i++) {
            const fp x = coords[2 * i];
            const fp y = coords[2 * i + 1];
            if (x < minX) minX = x;
            if (y < minY) minY = y;
            if (x > maxX) maxX = x;
            if (y > maxY) maxY = y;
            _ids[i] = i;
        }
        const fp cx = (minX + maxX) / 2;
        const fp cy = (minY + maxY) / 2;

        fp minDist = (std::numeric_limits<fp>::max)();
        std::size_t i0, i1, i2;

        // pick a seed point close to the center
        for (std::size_t i = 0; i < n; i++) {
            const fp d = sqr_dist(cx, cy, coords[2 * i + 0], coords[2 * i + 1]);
            if (d < minDist) {
                i0 = i;
                minDist = d;
            }
        }
        fp i0x = coords[2 * i0];
        fp i0y = coords[2 * i0 + 1];

        minDist = (std::numeric_limits<fp>::max)();
        
        // find the point closest to the seed
        for (std::size_t i = 0; i < n; i++) {
            if (i == i0) continue;
            const fp d = sqr_dist(i0x, i0y, coords[2 * i], coords[2 * i + 1]);
            if (d < minDist) {
                i1 = i;
                minDist = d;
            }
        }
        fp i1x = coords[2 * i1];
        fp i1y = coords[2 * i1 + 1];

        fp minRadius = (std::numeric_limits<fp>::max)();

        // find the third point which forms the smallest circumcircle with the first two
        for (std::size_t i = 0; i < n; i++) {
            if (i == i0 || i == i1) continue;
            const fp r = circumradius(i0x, i0y, i1x, i1y, coords[2 * i], coords[2 * i + 1]);
            if (r < minRadius) {
                i2 = i;
                minRadius = r;
            }
        }
        fp i2x = coords[2 * i2];
        fp i2y = coords[2 * i2 + 1];

        if (minRadius == (std::numeric_limits<fp>::max)()) {
            // order collinear points by dx (or dy if all x are identical)
            // and return the list as a hull
            for (std::size_t i = 0; i < n; i++) {
                _dists[i] = (std::max)((coords[2 * i] - coords[0]), (coords[2 * i + 1] - coords[1]));
            }
            std::sort(_ids.begin(), _ids.end(), [this](std::size_t i, std::size_t j) { return _dists[i] < _dists[j]; });
            hull = std::vector<std::size_t>(n);
            std::size_t j = 0;
            for (std::size_t i = 0, d0 = (std::numeric_limits<fp>::min)(); i < n; i++) {
                std::size_t id = _ids[i];
                if (_dists[id] > d0) {
                    hull[j++] = id;
                    d0 = _dists[id];
                }
            }
            hull = std::vector<std::size_t>(hull.begin(), hull.begin() + j);
            triangles = std::vector<std::size_t>(0);
            halfedges = std::vector<std::size_t>(0);
            return;
        }

        // swap the order of the seed points for counter-clockwise orientation
        if (orient2d(i0x, i0y, i1x, i1y, i2x, i2y) < 0) {
            const std::size_t i = i1;
            const fp x = i1x;
            const fp y = i1y;
            i1 = i2;
            i1x = i2x;
            i1y = i2y;
            i2 = i;
            i2x = x;
            i2y = y;
        }

        const auto& center = circumcenter(i0x, i0y, i1x, i1y, i2x, i2y);
        _cx = center[0];
        _cy = center[1];

        for (std::size_t i = 0; i < n; i++) {
            _dists[i] = sqr_dist(coords[2 * i], coords[2 * i + 1], _cx, _cy);
        }

        // sort the points by distance from the seed triangle circumcenter
        std::sort(_ids.begin(), _ids.end(), [this](std::size_t i, std::size_t j) { return _dists[i] < _dists[j]; });

        // set up the seed triangle as the starting hull
        _hullStart = i0;
        std::size_t hullSize = 3;

        _hullNext[i0] = _hullPrev[i2] = i1;
        _hullNext[i1] = _hullPrev[i0] = i2;
        _hullNext[i2] = _hullPrev[i1] = i0;

        _hullTri[i0] = 0;
        _hullTri[i1] = 1;
        _hullTri[i2] = 2;

        std::fill(_hullHash.begin(), _hullHash.end(), NIL);
        _hullHash[_hashKey(i0x, i0y)] = i0;
        _hullHash[_hashKey(i1x, i1y)] = i1;
        _hullHash[_hashKey(i2x, i2y)] = i2;

        trianglesLen = 0;
        _addTriangle(i0, i1, i2, NIL, NIL, NIL);

        fp xp = (std::numeric_limits<fp>::quiet_NaN)();
        fp yp = (std::numeric_limits<fp>::quiet_NaN)();
        
        for (std::size_t k = 0; k < _ids.size(); k++) {
            const std::size_t i = _ids[k];
            const fp x = coords[2 * i];
            const fp y = coords[2 * i + 1];

            // skip near-duplicate points
            if (std::abs(x - xp) <= EPSILON && std::abs(y - yp) <= EPSILON) continue; // <======= remove k<0 check
            xp = x;
            yp = y;

            // skip seed triangle points
            if (i == i0 || i == i1 || i == i2) continue;

            // find a visible edge on the convex hull using edge hash
            std::size_t start = 0;
            for (std::size_t j = 0, key = _hashKey(x, y); j < _hashSize; j++) {
                start = _hullHash[(key + j) % _hashSize];
                if (start != NIL && start != _hullNext[start]) break;
            }

            start = _hullPrev[start];
            std::size_t e = start, q;
            while (q = _hullNext[e], orient2d(x, y, coords[2 * e], coords[2 * e + 1], coords[2 * q], coords[2 * q + 1]) >= 0) {
                e = q;
                if (e == start) {
                    e = NIL;
                    break;
                }
            }
            if (e == NIL) continue; // likely a near-duplicate point; skip it

            // add the first triangle from the point
            std::size_t t = _addTriangle(e, i, _hullNext[e], NIL, NIL, _hullTri[e]);

            // recursively flip triangles from the point until they satisfy the Delaunay condition
            _hullTri[i] = _legalize(t + 2);
            _hullTri[e] = t; // keep track of boundary triangles on the hull
            hullSize++;

            // walk forward through the hull, adding more triangles and flipping recursively
            std::size_t ni = _hullNext[e];
            while (q = _hullNext[ni], orient2d(x, y, coords[2 * ni], coords[2 * ni + 1], coords[2 * q], coords[2 * q + 1]) < 0) {
                t = _addTriangle(ni, i, q, _hullTri[i], NIL, _hullTri[ni]);
                _hullTri[i] = _legalize(t + 2);
                _hullNext[ni] = ni; // mark as removed
                hullSize--;
                ni = q;
            }

            // walk backward from the other side, adding more triangles and flipping
            if (e == start) {
                while (q = _hullPrev[e], orient2d(x, y, coords[2 * q], coords[2 * q + 1], coords[2 * e], coords[2 * e + 1]) < 0) {
                    t = _addTriangle(q, i, e, NIL, _hullTri[e], _hullTri[q]);
                    _legalize(t + 2);
                    _hullTri[q] = t;
                    _hullNext[e] = e; // mark as removed
                    hullSize--;
                    e = q;
                }
            }

            // update the hull indices
            _hullStart = _hullPrev[i] = e;
            _hullNext[e] = _hullPrev[ni] = i;
            _hullNext[i] = ni;

            // save the two new edges in the hash table
            _hullHash[_hashKey(x, y)] = i;
            _hullHash[_hashKey(coords[2 * e], coords[2 * e + 1])] = e;
        }


        hull = std::vector<std::size_t>(hullSize);
        for (std::size_t i = 0, e = _hullStart; i < hullSize; i++) {
            hull[i] = e;
            e = _hullNext[e];
        }

        // trim typed triangle mesh arrays
        triangles = std::vector<std::size_t>(_triangles.begin(), _triangles.begin() + trianglesLen);
        halfedges = std::vector<std::size_t>(_halfedges.begin(), _halfedges.begin() + trianglesLen);
    }

    std::size_t _hashKey(fp x, fp y) {
        return ((std::size_t)std::floor(pseudoAngle(x - _cx, y - _cy) * (fp)_hashSize)) % _hashSize;
    }

    void _link(std::size_t a, std::size_t b) {
        _halfedges[a] = b;
        if (b != NIL) _halfedges[b] = a;
    }
    
    // add a new triangle given vertex indices and adjacent half-edge ids
    std::size_t _addTriangle(std::size_t i0, std::size_t i1, std::size_t i2, std::size_t a, std::size_t b, std::size_t c) {
        const std::size_t  t = trianglesLen;

        _triangles[t] = i0; // i1 // clockwise
        _triangles[t + 1] = i1; // i0 // clockwise
        _triangles[t + 2] = i2;

        _link(t, a);
        _link(t + 1, b);
        _link(t + 2, c);

        trianglesLen += 3;

        return t;
    }

    std::size_t _legalize(std::size_t a) {

        std::size_t i = 0;
        std::size_t ar = 0;

        // recursion eliminated with a fixed-size stack
        while (true) {
            const std::size_t b = _halfedges[a];

            /* if the pair of triangles doesn't satisfy the Delaunay condition
             * (p1 is inside the circumcircle of [p0, pl, pr]), flip them,
             * then do the same check/flip recursively for the new pair of triangles
             *
             *           pl                    pl
             *          /||\                  /  \
             *       al/ || \bl            al/    \a
             *        /  ||  \              /      \
             *       /  a||b  \    flip    /___ar___\
             *     p0\   ||   /p1   =>   p0\---bl---/p1
             *        \  ||  /              \      /
             *       ar\ || /br             b\    /br
             *          \||/                  \  /
             *           pr                    pr
             */
            const std::size_t a0 = 3 * (a / 3);
            ar = a0 + (a + 2) % 3;

            if (b == NIL) { // convex hull edge
                if (i == 0) break;
                a = EDGE_STACK[--i];
                continue;
            }

            const std::size_t b0 = 3 * (b / 3);
            const std::size_t al = a0 + (a + 1) % 3;
            const std::size_t bl = b0 + (b + 2) % 3;

            const std::size_t p0 = _triangles[ar];
            const std::size_t pr = _triangles[a];
            const std::size_t pl = _triangles[al];
            const std::size_t p1 = _triangles[bl];


            const bool illegal = inCircle(
                coords[2 * p0], coords[2 * p0 + 1],
                coords[2 * pr], coords[2 * pr + 1],
                coords[2 * pl], coords[2 * pl + 1],
                coords[2 * p1], coords[2 * p1 + 1]);

            if (illegal) {
                _triangles[a] = p1;
                _triangles[b] = p0;

                const std::size_t hbl = _halfedges[bl];

                // edge swapped on the other side of the hull (rare); fix the halfedge reference
                if (hbl == NIL) {
                    std::size_t e = _hullStart;
                    do {
                        if (_hullTri[e] == bl) {
                            _hullTri[e] = a;
                            break;
                        }
                        e = _hullPrev[e];
                    } while (e != _hullStart);
                }
                _link(a, hbl);
                _link(b, _halfedges[ar]);
                _link(ar, bl);

                const std::size_t br = b0 + (b + 1) % 3;

                // don't worry about hitting the cap: it can only happen on extremely degenerate input
                if (i < EDGE_STACK.size()) {
                    EDGE_STACK[i++] = br;
                }
            } else {
                if (i == 0) break;
                a = EDGE_STACK[--i];
            }
        }

        return ar;
    }

// Utilities

    inline fp dist(fp x0, fp y0, fp x1, fp y1) {
        return sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
    }

    inline fp sqr_dist(fp x0, fp y0, fp x1, fp y1) {
        return (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0);
    }

    inline fp circumradius(fp ax, fp ay, fp bx, fp by, fp cx, fp cy) {
        const fp dx = bx - ax;
        const fp dy = by - ay;
        const fp ex = cx - ax;
        const fp ey = cy - ay;

        const fp bl = dx * dx + dy * dy;
        const fp cl = ex * ex + ey * ey;
        const fp d = 0.5 / (dx * ey - dy * ex);

        const fp x = (ey * bl - dy * cl) * d;
        const fp y = (dx * cl - ex * bl) * d;

        return x * x + y * y;
    }

    inline std::array<fp, 2> circumcenter(fp ax, fp ay, fp bx, fp by, fp cx, fp cy) {
        const fp dx = bx - ax;
        const fp dy = by - ay;
        const fp ex = cx - ax;
        const fp ey = cy - ay;

        const fp bl = dx * dx + dy * dy;
        const fp cl = ex * ex + ey * ey;
        const fp d = 0.5 / (dx * ey - dy * ex);

        const fp x = ax + (ey * bl - dy * cl) * d;
        const fp y = ay + (dx * cl - ex * bl) * d;

        return {x, y};
    }

    inline fp orient2d(fp ax, fp ay, fp bx, fp by, fp cx, fp cy) { //orient2dfast from robust-predicates
        return (ay - cy) * (bx - cx) - (ax - cx) * (by - cy);
    }

    // monotonically increases with real angle, but doesn't need expensive trigonometry
    inline fp pseudoAngle(fp dx, fp dy) {
        const fp p = dx / (std::abs(dx) + std::abs(dy));
        return (dy > 0.0 ? 3.0 - p : 1.0 + p) / 4.0; // [0..1]
    }

    inline bool inCircle(fp ax, fp ay, fp bx, fp by, fp cx, fp cy, fp px, fp py) {
        const fp dx = ax - px;
        const fp dy = ay - py;
        const fp ex = bx - px;
        const fp ey = by - py;
        const fp fx = cx - px;
        const fp fy = cy - py;

        const fp ap = dx * dx + dy * dy;
        const fp bp = ex * ex + ey * ey;
        const fp cp = fx * fx + fy * fy;

        return (dx * (ey * cp - bp * fy) -
                dy * (ex * cp - bp * fx) +
                ap * (ex * fy - ey * fx)) < 0;
    }
};

