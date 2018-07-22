#pragma once
#include <cmath>
#include<vector>
#include<iostream>
#include<algorithm>

// Basic structure
struct Point {
    float x, y;

    bool operator == (const Point& rhs) const {
        return (this->x == rhs.x) && (this->y == rhs.y);
    }
    void operator = (const Point& p) {
        x = p.x;
        y = p.y;
    }
    Point(float _x, float _y) : x(_x), y(_y) {}
};

struct bbox {
    int xmin, xmax, ymin, ymax;
};

// Use different namespace to classify funcs

namespace Utils {
    std::vector<float> points2floats3d(const std::vector<Point>& pv) {
        std::vector<float> data;
        data.resize(3 * pv.size());
        for (int i = 0; i < pv.size(); ++i) {
            const int index = 3 * i;
            data[index] = pv[i].x;
            data[index + 1] = pv[i].y;
            data[index + 2] = 0.0f;
        }
        return data;
    }

    std::vector<float> scrCoor2glCoor(std::vector<float>& _data, const unsigned int scr_width,
                                        const unsigned int scr_height) {
        std::vector<float> data;
        data.resize(_data.size());
        for (int i = 0; i < _data.size(); i = i + 3) {
            data[i] = 2 * _data[i] / scr_width;
            data[i + 1] = 2 * _data[i + 1] / scr_height;
        }
        return data;
    }
}


namespace TriangleRasterization {
    int min3(const int a, const int b, const int c) {
        int temp = std::min(a, b);
        return std::min(temp, c);
    }

    int max3(const int a, const int b, const int c) {
        int temp = std::max(a, b);
        return std::max(temp, c);
    }

    void bound3(const Point& p0, const Point& p1, const Point& p2, bbox& b) {
        b.xmin = ceil(min3(p0.x, p1.x, p2.x));
        b.xmax = ceil(max3(p0.x, p1.x, p2.x));
        b.ymin = ceil(min3(p0.y, p1.y, p2.y));
        b.ymax = ceil(max3(p0.y, p1.y, p2.y));
    }

    std::vector<float> getABC(const Point& p0, const Point& p1) {
        // For a line Ax + By + C = 0;
        // v[0] is A, v[1] is B, v[2] is C
        std::vector<float> v;
        v.resize(3);

        v[0] = p0.y - p1.y;
        v[1] = p1.x - p0.x;
        v[2] = p0.x * p1.y - p1.x * p0.y;
        return v;
    }

    std::vector<float> edgeEquations(Point& p0, Point& p1, Point& p2) {
        std::vector<Point> pv;
        bbox b;
        bound3(p0, p1, p2, b);

        // Cal 3 lines paras
        std::vector<std::vector<float>> lines;
        lines.resize(3);
        lines[0] = getABC(p0, p1);
        lines[1] = getABC(p0, p2);
        lines[2] = getABC(p1, p2);

        auto f = [](const std::vector<float>& line, const Point& p) {
            return line[0] * p.x + line[1] * p.y + line[2];
        };

        // use flag to determine halfspaces
        int flag[3];
        flag[0] = (f(lines[0], p2) > 0) ? 1 : -1;
        flag[1] = (f(lines[1], p1) > 0) ? 1 : -1;
        flag[2] = (f(lines[2], p0) > 0) ? 1 : -1;

        for (int i = b.xmin; i <= b.xmax; ++i) {
            for (int j = b.ymin; j <= b.ymax; ++j) {
                bool inside = true;
                for (int k = 0; k < lines.size(); ++k) {
                    if ((lines[k][0] * i + lines[k][1] * j + lines[k][2]) * flag[k] < 0) {
                        inside = false;
                        break;
                    }
                }
                if (inside) {
                    pv.push_back(Point(i, j));
                }
            }
        }

        auto data = Utils::points2floats3d(pv);
        // convert Point Vector to 3D float Vector and return it.
        return data;
    }
}

namespace Bresenham {
    void flipX(const Point& base, Point& p) {
        float dy = base.y - p.y;
        p.y += 2 * dy;
    }

    void flipXY(Point& p) {
        float temp = p.x;
        p.x = p.y;
        p.y = temp;
    }

    void swap(Point& p1, Point& p2) {
        float temp;
        temp = p1.x;
        p1.x = p2.x;
        p2.x = temp;

        temp = p1.y;
        p1.y = p2.y;
        p2.y = temp;
    }

    std::vector<float> genLineData(Point v0, Point v1) {
        // Preparation: Use transformation to  convert all situation to normal situation:
        // 1. v0.x < v1.x, v0.y < v1.y
        // 2. delta_x > delta_y
        if (v0.x > v1.x) {
            swap(v0, v1);
        }

        bool isflipXY = false;
        if (std::abs(v0.x - v1.x) < std::abs(v0.y - v1.y)) {
            flipXY(v0);
            flipXY(v1);
            isflipXY = true;
        }

        if (v0.x > v1.x) {
            swap(v0, v1);
        }
        bool isflipX = false;
        if (v0.y > v1.y) {
            flipX(v0, v1);
            isflipX = true;
        }

        // Implement the most normal situation
        int delta_x = v1.x - v0.x;
        int delta_y = v1.y - v0.y;

        int p = 2 * delta_y - delta_x;

        std::vector<Point> pv;
        pv.clear();
        pv.push_back(v0);

        int count = delta_x;
        while (count--) {
            Point lastP = pv.back();
            if (p <= 0) {
                pv.push_back(Point(lastP.x + 1, lastP.y));
                p += 2 * delta_y;
            }
            else {
                pv.push_back(Point(lastP.x + 1, lastP.y + 1));
                p = p + 2 * delta_y - 2 * delta_x;
            }
        }

        // Reverse the transformation in the preparation
        if (isflipX) {
            Point base = pv.front();
            for (auto& p : pv) {
                flipX(base, p);
            }
        }

        if (isflipXY) {
            std::for_each(pv.begin(), pv.end(), flipXY);
        }
        
        auto data = Utils::points2floats3d(pv);
        // convert Point Vector to 3D float Vector and return it.
        return data;
    }

    std::vector<float> genTriangleData(Point& p0, Point& p1, Point& p2, bool isfill = false) {
        auto data = genLineData(p0, p1);
        auto line2data = genLineData(p0, p2);
        auto line3data = genLineData(p1, p2);

        data.insert(data.end(), line2data.begin(), line2data.end());
        data.insert(data.end(), line3data.begin(), line3data.end());

        if (isfill) {
            // bonus
            auto insideData = TriangleRasterization::edgeEquations(p0, p1, p2);
            data.insert(data.end(), insideData.begin(), insideData.end());
        }
        return data;
    }

    void addCirclePlot(std::vector<Point>& pv, const Point& origin, const float x, const float y) {
        std::vector<Point> eightPoints = {
            Point(x + origin.x, y + origin.y), Point(y + origin.x, x + origin.y),
            Point(y + origin.x, -x + origin.y), Point(x + origin.x, -y + origin.y),
            Point(-x + origin.x, -y + origin.y), Point(-y + origin.x, -x + origin.y),
            Point(-y + origin.x, x + origin.y), Point(-x + origin.x, y + origin.y)
        };
        pv.insert(pv.end(), eightPoints.begin(), eightPoints.end());
    }

    std::vector<float> genCircleData(Point& origin, const int R) {
        if (R < 2) {
            return std::vector<float>();
        }

        std::vector<Point> pv;
        pv.clear();

        int x, y, d;

        x = 0;
        y = R;
        d = 3 - 2 * R;
        addCirclePlot(pv, origin, x, y);
        while (x < y) {
            if (d < 0) {
                d = d + 4 * x + 6;
            }
            else {
                d = d + 4 * (x - y) + 10;
                --y;
            }
            ++x;
            addCirclePlot(pv, origin, x, y);
        }
        auto data = Utils::points2floats3d(pv);
        return data;
    }
}

