#include "placement.h"
#include <fstream>
#include <iostream>
using namespace std;

Point::Point(double a, double b){
    _x = a;
    _y = b;
}

void Point::SetPoint(double a, double b){
    _x = a;
    _y = b;
}

void Point::print(){
    cout << "(" << _x << "," << _y << ")" << endl; 
}

void Grid::print(){
    cout << "(" << _x << ", " << _y << ")" << endl;
}

void Grid::insert(int type){
    if(type == 0){
        if(this->l_num >= 2){
            cout << "LUT is full" << endl;
            exit(-1);
        }
        l_num ++;
    }
    else if(type == 1){
        if(f_num >= 2){
            cout << "FF is full" << endl;
            exit(-1);
        }
        f_num ++;
    }
}

void Grid::remove(int type){
    if(type == 0){
        if(l_num <= 0){
            cout << "LUT is empty" << endl;
            exit(-1);
        }
        l_num --;
    }
    else if(type == 1){
        if(f_num <= 0){
            cout << "FF is empty" << endl;
            exit(-1);
        }
        f_num --;
    }
}

bool Grid::l_full(){
    if(l_num == full){
        return true;
    }
    else
        return false;
}

bool Grid::f_full(){
    if(f_num == full){
        return true;
    }
    else
        return false;
}

double Placement::HPWL(){
    double max_x, max_y;
    double min_x, min_y;
    double hpwl = 0.0;
    for(auto n:net_list){
        max_x = 0.0; max_y = 0.0;
        min_x = 1e6; min_y = 1e6;
        for(auto p:n->pin_list){
            max_x = max(max_x, p->x());
            max_y = max(max_y, p->y());
            min_x = min(min_x, p->x());
            min_y = min(min_y, p->y());
        }
        hpwl += (max_x - min_x) + (max_y - min_y); 
    }
    return hpwl;
}

void Placement::initial_placement(){
    unsigned li = 0, fi = 0;
    for(auto g:grid_map){
        if(li < lut_list.size()){
            lut_list[li++]->SetPoint(g->x(), g->y());
            g->insert(0);
        }
        if(li < lut_list.size()){
            lut_list[li++]->SetPoint(g->x(), g->y());
            g->insert(0);
        }

        if(fi < ff_list.size()){
            ff_list[fi++]->SetPoint(g->x(), g->y());
            g->insert(1);
        }
        if(fi < ff_list.size()){
            ff_list[fi++]->SetPoint(g->x(), g->y());
            g->insert(1);
        }
    }
}

void Placement::exchange(int mod_id1, int mod_id2, int type){
    int temp_x, temp_y;
    if(type == 0){
        temp_x = lut_list[mod_id1]->x();
        temp_y = lut_list[mod_id1]->y();
        lut_list[mod_id1]->SetPoint(lut_list[mod_id2]->x(), lut_list[mod_id2]->y());
        lut_list[mod_id2]->SetPoint(temp_x, temp_y);
    }
    else{
        temp_x = ff_list[mod_id1]->x();
        temp_y = ff_list[mod_id1]->y();
        ff_list[mod_id1]->SetPoint(ff_list[mod_id2]->x(), ff_list[mod_id2]->y());
        ff_list[mod_id2]->SetPoint(temp_x, temp_y);
    }
}

void Placement::MoveToEmpty(int mod_id1, int empty_grid_id, int type){
    if(type == 0){
        grid(lut_list[mod_id1]->x(), lut_list[mod_id1]->y())->remove(type);
        lut_list[mod_id1]->SetPoint(grid_map[empty_grid_id]->x(), grid_map[empty_grid_id]->y());
        grid_map[empty_grid_id]->insert(type);
    }
    else{
        grid(ff_list[mod_id1]->x(), ff_list[mod_id1]->y())->remove(type);
        ff_list[mod_id1]->SetPoint(grid_map[empty_grid_id]->x(), grid_map[empty_grid_id]->y());
        grid_map[empty_grid_id]->insert(type);
    }
}

double AverageUpCost(Placement &placement){
    double alpha = placement.HPWL() / 4.0;
    double cost = placement.HPWL()/alpha;
    double new_cost;
    double uphill = 0;
    int upstep = 0;
    int base_grid_id, tar_grid_id;
    int k = 7, c = 10;

    srand(time(NULL));

    int s, a, b, type;

    upstep = 0;
    for(int j = 0; j < 200; j++){
        s = rand() % 2;
        type = rand() % 2;
        if(s == 0 && type == 0 && placement.lut_num != 0){
            a = rand() % placement.lut_num;
            b = rand() % placement.lut_num;
            placement.exchange(a, b, 0);
        }
        else if(s == 0 && type == 1 && placement.ff_num != 0){
            a = rand() % placement.ff_num;
            b = rand() % placement.ff_num;
            placement.exchange(a, b, 1);
        }
        else if(s == 1 && type == 0 && placement.lut_num != 0){
            vector<int> empty;
            for(unsigned j = 0; j < placement.grid_map.size(); j++){
                if(!placement.grid_map[j]->l_full())
                    empty.push_back(j);
            }
            a = rand() % placement.lut_num;
            b = rand() % empty.size();
            base_grid_id = placement.grid_id(placement.lut_list[a]->x(), placement.lut_list[a]->y());
            placement.MoveToEmpty(a, empty[b], 0);
        }
        else if(s == 1 && type == 1 && placement.ff_num != 0){
            vector<int> empty;
            for(unsigned j = 0; j < placement.grid_map.size(); j++){
                if(!placement.grid_map[j]->f_full())
                    empty.push_back(j);
            }
            a = rand() % placement.ff_num;
            b = rand() % empty.size();
            base_grid_id = placement.grid_id(placement.ff_list[a]->x(), placement.ff_list[a]->y());
            placement.MoveToEmpty(a, empty[b], 1);
        }
        else{
            continue;
        }

        new_cost = placement.HPWL()/alpha;
        
        if(new_cost > cost){
            uphill += new_cost - cost;
            upstep++;
        }

        if(s == 0 && type == 0){
            placement.exchange(a, b, 0);
        }
        else if(s == 0 && type == 1){
            placement.exchange(a, b, 1);
        }
        else if(s == 1 && type == 0){
            placement.MoveToEmpty(a, base_grid_id, 0);
        }
        else if(s == 1 && type == 1){
            placement.MoveToEmpty(a, base_grid_id, 1);
        }
    }

    return uphill / upstep;
}


void read_info(string filename, Placement &placement){
    ifstream is(filename);
    string s;
    int a, b;
    double x, y;

    is >> s;
    is >> placement.C; is >> placement.R;
    is >> s;
    is >> placement.P; is >> placement.Q;

    for(int i = 1; i <= placement.C; i++){
        for(int j = 1; j <= placement.R; j++){
            placement.grid_map.push_back(new Grid(i, j));
        }
    }
    
    is >> s;
    is >> a;
    for(int i = 0; i < a; i++){
        is >> s;
        is >> x;
        is >> y;
        placement.PI.push_back(new Module(x, y));
    }

    is >> s;
    is >> a;
    for(int i = 0; i < a; i++){
        is >> s;
        is >> x;
        is >> y;
        placement.PO.push_back(new Module(x, y));
    }

    is >> s;
    is >> a; is >> b;
    placement.lut_num = a;
    placement.ff_num = b;
    for(int i = 0; i < a; i++){
        placement.lut_list.push_back(new Module(1, 1));
    }
    for(int i = 0; i < b; i++){
        placement.ff_list.push_back(new Module(1, 1));
    }
}

void read_net(string filename, Placement &placement){
    ifstream is(filename);
    int net_num;
    string s;
    char c;
    int a;
    is >> net_num;
    for(int i = 0; i < net_num; i++){
        is >> s;
        Net* temp = new Net();
        is.get(c);
        while(c != '\n'){
            if(c == 'I'){
                is >> a;
                temp->pin_list.push_back(placement.PI[a-1]);
            }
            else if(c == 'O'){
                is >> a;
                temp->pin_list.push_back(placement.PO[a-1]);
            }
            else if(c == 'F'){
                is >> a;
                temp->pin_list.push_back(placement.ff_list[a-1]);
            }
            else if(c == 'L'){
                is >> a;
                temp->pin_list.push_back(placement.lut_list[a-1]);
            }
            is.get(c);
        }
        placement.net_list.push_back(temp);
    }
}

void output(string filename, Placement &placement){
    ofstream fout(filename);
    int i = 1;
    for(auto l:placement.lut_list){
        fout << "L" << i++ << " ";
        fout << l->x() << " " << l->y() << endl;
    }
    i = 1;
    for(auto f:placement.ff_list){
        fout << "F" << i++ << " ";
        fout << f->x() << " " << f->y() << endl;
    }

    fout.close();
}
