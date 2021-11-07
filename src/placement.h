#include <vector>
#include <string>
using namespace std;

class Point{
private:
    double _x, _y;
public:
    Point(double a, double b);
    void SetPoint(double x, double y);
    double x() {return _x;}
    double y() {return _y;}
    void print();
};

class Module : public Point{
public:
    Module(double a, double b): Point(a, b){}
};

class Net{
public:
    vector<Module*> pin_list;
};

class Grid{
    int l_num;
    int f_num;
    int full;
    int _x, _y;
public:
    Grid(int a, int b):l_num(0), f_num(0), full(2), _x(a), _y(b){}
    int l() {return l_num;}
    int f() {return f_num;}
    void insert(int type); // 0 = LUT, 1 = FF
    void remove(int type);
    bool f_full();
    bool l_full();
    int x() {return _x;}
    int y() {return _y;}
    void print();
};

class Placement{
public:
    int C, R, P, Q;
    int lut_num, ff_num;
    vector<Module*> PI;
    vector<Module*> PO;
    vector<Module*> lut_list;
    vector<Module*> ff_list;
    vector<Net*> net_list;
    vector<Grid*> grid_map;

    void initial_placement();
    double HPWL();
    Grid* grid(int x, int y) {return grid_map[(x - 1) * R + (y - 1)];}
    int grid_id(int x, int y) {return (x - 1) * R + (y - 1);}
    void exchange(int mod_id1, int mod_id2, int type);
    void MoveToEmpty(int mod_id1, int empty_grid_id, int type);
};

double AverageUpCost(Placement &placement);
void read_info(string filename, Placement &placement);
void read_net(string filename, Placement &placement);
void output(string filename, Placement &placement);


