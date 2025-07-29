#include <vector>
#include <utility>
#include <algorithm>
#include <functional>

#include "ChadNet.h"

#include <unordered_set>

struct Vertex 
{
    float x = 0, y = 0;

    Vertex() = default;
    Vertex(double x, double y) : x(x), y(y) {}

    bool operator==(const Vertex& other) const {
        return (x == other.x) && (y == other.y);
    }
};
/*
namespace std{
    template <>
    struct hash<Vertex>{
        std::size_t operator()(const Vertex& v)const{
            return std::hash<double>()(v.x)^(std::hash<double>()(v.y)<< 1);
        }
    };
}*/
namespace std {
    template <>
    struct hash<Vertex> {
        std::size_t operator()(const Vertex& v) const {
            std::size_t h1 = std::hash<float>{}(v.x);
            std::size_t h2 = std::hash<float>{}(v.y);
            return h1 ^ (h2 << 1); // XOR + shift per mescolare i bit
        }
    };
}


std::unordered_set<Vertex> left_p;
std::unordered_set<Vertex> right_p;

struct Triangle {
    Vertex p1, p2, p3;

    bool operator==(const Triangle& other) const {
        return (p1 == other.p1 && p2 == other.p2 && p3 == other.p3) ||
            (p1 == other.p2 && p2 == other.p3 && p3 == other.p1) ||
            (p1 == other.p3 && p2 == other.p1 && p3 == other.p2);
    }
};

static bool Exists(std::unordered_set<Vertex>::const_iterator& iter, std::unordered_set<Vertex>& set)
{
    return (iter != set.end());
}

static bool DifferentLane(std::unordered_set<Vertex>::const_iterator& iter, std::unordered_set<Vertex>& set, 
    std::unordered_set<Vertex>::const_iterator& iter2, std::unordered_set<Vertex>& set2)
{
    return (Exists(iter, set) && Exists(iter2, set2));
}

bool inCircumcircle(Vertex a, Vertex b, Vertex c, Vertex p)
{
    double ax = a.x - p.x, ay = a.y - p.y;
    double bx = b.x - p.x, by = b.y - p.y;
    double cx = c.x - p.x, cy = c.y - p.y;

    double det = (ax * ax + ay * ay) * (bx * cy - cx * by) -
        (bx * bx + by * by) * (ax * cy - cx * ay) +
        (cx * cx + cy * cy) * (ax * by - bx * ay);

    return det > 0;
}

bool ContainsSuperTrinagleVertex(Triangle t, Vertex p1, Vertex p2, Vertex p3)
{
    // p1,p2,p3 are super triangle vertices
    return
        (t.p1.x == p1.x && t.p1.y == p1.y) ||
        (t.p2.x == p1.x && t.p2.y == p1.y) ||
        (t.p3.x == p1.x && t.p3.y == p1.y) ||
        (t.p1.x == p2.x && t.p1.y == p2.y) ||
        (t.p2.x == p2.x && t.p2.y == p2.y) ||
        (t.p3.x == p2.x && t.p3.y == p2.y) ||
        (t.p1.x == p3.x && t.p1.y == p3.y) ||
        (t.p2.x == p3.x && t.p2.y == p3.y) ||
        (t.p3.x == p3.x && t.p3.y == p3.y);
}

std::vector<Triangle> delaunay(std::vector<Vertex>& points, Vertex p1 = { -1000, -1000 }, Vertex p2 = { 1000, -1000 }, Vertex p3 = { 0, 1000 })
{
    std::vector<Triangle> triangles;

    triangles.push_back({ p1 , p2, p3});

    for (const auto& p : points)
    {
        std::vector<Triangle> badTriangles;
        std::vector<std::pair<Vertex, Vertex>> edges;

        for (const auto& t : triangles)
        {
            if (inCircumcircle(t.p1, t.p2, t.p3, p))
            {
                badTriangles.push_back(t);
                edges.push_back({ t.p1, t.p2 });
                edges.push_back({ t.p2, t.p3 });
                edges.push_back({ t.p3, t.p1 });
            }
        }

        for (const auto& t : badTriangles)
        {
            triangles.erase(std::remove(triangles.begin(), triangles.end(), t), triangles.end());
        }

       for (size_t i = 0; i < edges.size(); i++)
        {
            for (size_t j = i + 1; j < edges.size(); j++)
            {
                if ((edges[i].first.x == edges[j].second.x && edges[i].first.y == edges[j].second.y &&
                    edges[i].second.x == edges[j].first.x && edges[i].second.y == edges[j].first.y) ||
                    (edges[i].first.x == edges[j].first.x && edges[i].first.y == edges[j].first.y &&
                        edges[i].second.x == edges[j].second.x && edges[i].second.y == edges[j].second.y))
                {
                    edges.erase(edges.begin() + j);
                    edges.erase(edges.begin() + i);
                    i--;
                    break;
                }
            }
        }

        for (const auto& edge : edges)
        {
            triangles.push_back({ edge.first, edge.second, p });
        }

    }

    triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
        [&](const Triangle& t) {
            return (t.p1.x == p1.x && t.p1.y == p1.y) ||
                (t.p2.x == p1.x && t.p2.y == p1.y) ||
                (t.p3.x == p1.x && t.p3.y == p1.y) ||
                (t.p1.x == p2.x && t.p1.y == p2.y) ||
                (t.p2.x == p2.x && t.p2.y == p2.y) ||
                (t.p3.x == p2.x && t.p3.y == p2.y) ||
                (t.p1.x == p3.x && t.p1.y == p3.y) ||
                (t.p2.x == p3.x && t.p2.y == p3.y) ||
                (t.p3.x == p3.x && t.p3.y == p3.y);
        }), triangles.end());

    return triangles;
}

void GenParabola(std::vector<Vertex>& points, double x_min = 0, double x_max = 10., double a = 0.05, double h = 5.0, double k = 5.0, int num_points = 100)
{

    // y = a * (x * h)^2 + k

    for (int i = 0; i < num_points; i++)
    {
        double x = x_min + (x_max - x_min) * i / (num_points - 1);
        double y = a * (x - h) * (x - h) + k;
        points.push_back(Vertex(x, y));
        left_p.insert(Vertex(x, y));
    }

    int size = points.size();
    for (int i = 0; i < num_points; i++)
    {
        points.push_back(Vertex(points[i].x, points[i].y + .5f));
        right_p.insert(Vertex(points[i].x, points[i].y + .5f));
    }
}

void GenRetta(std::vector<Vertex>& points, double x_min = 0, double x_max = 10., double y_min = 0, double y_max = 5., int num_points = 100)
{

    for (int i = 0; i < num_points; i++)
    {
        double x = x_min + (x_max - x_min) * i / (num_points - 1);
        double y = y_min + (y_max - y_min) * (i % 2);
        if (i % 2 == 0)
            left_p.insert(Vertex(x, y));
        else
            right_p.insert(Vertex(x, y));
        points.push_back(Vertex(x, y));
    }
}

void GenElisse(std::vector<Vertex>& points)
{
    std::vector<Vertex> points_left = { {48.5, 0.0},
 {48.15198391417631, 3.3834707733248752},
 {47.10647070785283, 6.7375231805073375},
 {45.36345866984716, 10.023606992823483},
 {42.93288342012054, 13.192462798554182},
 {39.839971663923386, 16.18894901442733},
 {36.125781596602764, 18.957901292657336},
 {31.845143238538757, 21.448087127253217},
 {27.064078477504502, 23.614193720485467},
 {21.857525314385118, 25.417671513351763},
 {16.3074518568205, 26.827076805454812},
 {10.501231033933696, 27.8182340804505},
 {4.5301492905911, 28.37433829668532},
 {-1.5120289605106652, 28.48602781940615},
 {-7.5304789301185595, 28.151428046710436},
 {-13.430729217445325, 27.376160489876185},
 {-19.120076602584327, 26.17331590338238},
 {-24.509009746969028, 24.563395339748556},
 {-29.512665762446094, 22.574223578407505},
 {-34.05237560726598, 20.240824319178273},
 {-38.057401444682675, 17.605192028216745},
 {-41.46700655998859, 14.715757058344867},
 {-44.23292398384878, 11.626071819037271},
 {-46.321853009957785, 8.391927849329097},
 {-47.71654984712274, 5.066307457524475},
 {-48.41302951135611, 1.693440758376883},
 {-48.41302951135611, -1.6934407583768885},
 {-47.71654984712274, -5.066307457524468},
 {-46.32185300995779, -8.39192784932909},
 {-44.23292398384878, -11.626071819037268},
 {-41.4670065599886, -14.71575705834486},
 {-38.05740144468268, -17.605192028216738},
 {-34.05237560726599, -20.24082431917827},
 {-29.512665762446105, -22.574223578407498},
 {-24.509009746969, -24.56339533974856},
 {-19.120076602584327, -26.17331590338238},
 {-13.430729217445348, -27.37616048987618},
 {-7.53047893011855, -28.15142804671044},
 {-1.5120289605106663, -28.48602781940615},
 {4.530149290591078, -28.374338296685323},
 {10.501231033933696, -27.8182340804505},
 {16.30745185682049, -26.827076805454816},
 {21.85752531438513, -25.41767151335176},
 {27.064078477504495, -23.614193720485467},
 {31.845143238538743, -21.448087127253228},
 {36.12578159660277, -18.957901292657333},
 {39.83997166392338, -16.188949014427337},
 {42.93288342012055, -13.192462798554175},
 {45.36345866984716, -10.023606992823487},
 {47.10647070785282, -6.737523180507349},
 {48.15198391417631, -3.383470773324873},
 {48.5, 0.0} };

    std::vector<Vertex> points_right = { {51.5, 0.0},
    {51.090067053017265, 3.9898266665579714},
    {49.87322289564812, 7.897300025995292},
    {47.883764270588415, 11.650892978405691},
    {45.168336008457906, 15.193150611606423},
    {41.779719571698784, 18.477480870067737},
    {37.775110125463144, 21.463837326136098},
    {33.21668678188545, 24.116207872029207},
    {28.172418818546078, 26.401949392781702},
    {22.716310263268713, 28.292125967951975},
    {16.928028091145464, 29.76218926282183},
    {10.89207728671606, 30.79266498512554},
    {4.696686655739101, 31.36971228101675},
    {-1.5674768951063776, 31.485515363395802},
    {-7.808686557749971, 31.138500278165125},
    {-13.93556978976294, 30.333378100492965},
    {-19.858510726683598, 29.08101517331484},
    {-25.490990253030994, 27.398128887317757},
    {-30.750797875479538, 25.306810058406874},
    {-35.56101898902667, 22.833891064688245},
    {-39.850656007884375, 20.01023632542964},
    {-43.55470701297283, 16.870172714296476},
    {-46.61360319810359, 13.451548874169745},
    {-48.97234703275786, 9.797232597453522},
    {-50.58076012126744, 5.958663611469747},
    {-51.3973033623483, 2.000213609659684},
    {-51.3973033623483, -2.0002136096596907},
    {-50.58076012126744, -5.958663611469738},
    {-48.97234703275787, -9.797232597453512},
    {-46.61360319810359, -13.451548874169738},
    {-43.554707012972834, -16.87017271429647},
    {-39.85065600788438, -20.010236325429634},
    {-35.561018989026685, -22.833891064688242},
    {-30.75079787547955, -25.306810058406867},
    {-25.490990253030965, -27.398128887317768},
    {-19.858510726683598, -29.08101517331484},
    {-13.935569789762964, -30.33337810049296},
    {-7.808686557749962, -31.13850027816513},
    {-1.5674768951063787, -31.485515363395802},
    {4.696686655739077, -31.369712281016753},
    {10.89207728671606, -30.79266498512554},
    {16.928028091145453, -29.762189262821835},
    {22.716310263268724, -28.29212596795197},
    {28.17241881854607, -26.401949392781702},
    {33.216686781885436, -24.116207872029218},
    {37.77511012546315, -21.463837326136094},
    {41.77971957169878, -18.477480870067744},
    {45.16833600845791, -15.193150611606416},
    {47.883764270588415, -11.650892978405695},
    {49.87322289564811, -7.897300025995305},
    {51.090067053017265, -3.9898266665579682},
    {51.5, 0.0} };

    for (auto& p : points_left)
    {
        points.push_back(p);
        left_p.insert(p);
    }

    for (auto& p : points_right)
    {
        points.push_back(p);
        right_p.insert(p);
    }
}

int main()
{

    if(!ChadConnect("127.0.0.1", CHAD_DEFAULT_PORT)) 
    {
        return -1;
    }
    
    std::vector<Vertex> points;
    GenElisse(points);

    std::vector<Triangle> triangles = delaunay(points);
    std::vector<Vertex> pt;

    for (auto& t : triangles)
    {
        bool existsP1 = false;
        bool existsP2 = false;
        bool existsP3 = false;

        std::unordered_set<Vertex>::const_iterator gotP1Left = left_p.find(t.p1);
        std::unordered_set<Vertex>::const_iterator gotP1Right = right_p.find(t.p1);
        if (!(Exists(gotP1Left, left_p) && Exists(gotP1Right, right_p)))
        {
            existsP1 = true;
        }

        std::unordered_set<Vertex>::const_iterator gotP2Left = left_p.find(t.p2);
        std::unordered_set<Vertex>::const_iterator gotP2Right = right_p.find(t.p2);
        if (!(Exists(gotP2Left, left_p) && Exists(gotP2Right, right_p)))
        {
            existsP2 = true;
        }

        std::unordered_set<Vertex>::const_iterator gotP3Left = left_p.find(t.p3);
        std::unordered_set<Vertex>::const_iterator gotP3Right = right_p.find(t.p3);
        if (!(Exists(gotP3Left, left_p) && Exists(gotP3Right, right_p)))
        {
            existsP3 = true;
        }

        if (existsP1 && existsP2)
        {
            if (DifferentLane(gotP1Right, right_p, gotP2Left, left_p) || DifferentLane(gotP1Left, left_p, gotP2Right, right_p))
            {
                double midX = (t.p1.x + t.p2.x) / 2.0;
                double midY = (t.p1.y + t.p2.y) / 2.0;
                pt.push_back({midX, midY});
            }
        }
        else if (existsP2 && existsP3)
        {
            if (DifferentLane(gotP2Right, right_p, gotP3Left, left_p)|| DifferentLane(gotP2Left, left_p, gotP3Right, right_p))
            {
                double midX = (t.p2.x + t.p3.x) / 2.0;
                double midY = (t.p2.y + t.p3.y) / 2.0;
                pt.push_back({ midX, midY });
            }
        }
        else if (existsP3 && existsP1)
        {
            if (DifferentLane(gotP3Right, right_p, gotP1Left, left_p)|| DifferentLane(gotP3Left, left_p, gotP1Right, right_p))
            {
                double midX = (t.p3.x + t.p1.x) / 2.0;
                double midY = (t.p3.y + t.p1.y) / 2.0;
                pt.push_back({ midX, midY });
            }
        }

    }

    for (auto& t : triangles)
    {
        ChadSendLine(t.p1.x, t.p1.y, t.p2.x, t.p2.y);
        ChadSendLine(t.p2.x, t.p2.y, t.p3.x, t.p3.y);
        ChadSendLine(t.p3.x, t.p3.y, t.p1.x, t.p1.y);
    }

    for (auto& p : pt)
    {
        ChadSendPoint(p.x, p.y);
    }

    ChadDisconnect();

	return 0;
}
