#include <iostream>
#include "placement.h"
#include <ctime>
#include <cmath>

int main(int argc, char *argv[]){
    Placement placement;
    read_info(argv[1], placement);
    read_net(argv[2], placement);
    
    
    placement.initial_placement();
    
    double alpha = placement.HPWL() / 4.0;
    double cost = placement.HPWL() / alpha;
    double best_cost = cost;
    double new_cost, delta_cost;
    double avg_uphill_cost = AverageUpCost(placement);
    double T0 = -avg_uphill_cost / log(0.9);
    double c_T = T0;
    double cost_change;
    int base_grid_id, tar_grid_id, upstep, step;
    int k = 30, c = 5;

    srand(time(NULL));

    int s, a, b, type;
    time_t t1, t2;
    
    t1 = time(NULL);
    step = 0;
    while(true){
        cost_change = 0;
        upstep = 0;
        for(int j = 0; j < 20; j++){
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
            else
            {
                continue;
            }

            new_cost = placement.HPWL() / alpha;
            delta_cost = new_cost - cost;
            cost_change += abs(delta_cost);
            upstep++;
            
            if(new_cost > cost && ((double) rand() / (RAND_MAX + 1.0)) > exp(- delta_cost / c_T)){
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
            else{
                cost = new_cost;
            }
        }
        step++;
        if(step < k){
            c_T = T0 * (cost_change / upstep) / (step * c);
        }
        else
        {
            c_T = T0 * (cost_change / upstep) / step;
        }

        t2 = time(NULL);
        if(t2 - t1 > 1500){
            break;
        }
    }

    cout << "completed " << argv[3] << ", WL = " << placement.HPWL() <<  endl;
    output(argv[3], placement);
}