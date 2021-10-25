#include <iostream>
#include <deque>
#include <chrono>
#include <cmath>
#include "g.h"

using namespace std::chrono;
using namespace std;


int NUM_IT = 100; // TODO: 100 may be changed.
double MAX = 0;

// double g(double t) {
//     return sin(2 * (t-100)) / 4 + 10;
// }

void ini_deque(deque<pair<pair<double, double>, pair<double, double>> >& pairs, double a, double b, int worker_num) {
    double interval = (b - a) / worker_num;
    double prev = a;
    double cur = a + interval;
    for (int i = 0; i < worker_num; i++) {
        pairs.push_back({{prev, g(prev)}, {cur, g(cur)}});
        prev = cur;
        cur += interval;
    }
}

int main() {

    int worker_num = 0;
    double a = 1;
    double b = 100;
    double s = 12;
    double epsilon = 1e-7;
    double min_interval = epsilon / s;
    // cout << "min_interval "<< min_interval << endl;
    deque<pair<pair<double, double>, pair<double, double>> >  pairs;

    #pragma omp parallel
    {// Code inside this region runs in parallel.
        #pragma omp critical(dataupdate)
        {
            worker_num++;
            // cout << worker_num << endl;
        }
    }

    std::cout << "Number of workers is: " << worker_num << std::endl;

    auto start = high_resolution_clock::now();
    ini_deque(pairs, a, b, worker_num);
    MAX = max(g(a), g(b));
    // cout << MAX << endl;
    // for (auto p: pairs) cout << p.first << " " << p.second << endl;
    while(!pairs.empty()) {
        int num_responsibility = ceil((double)pairs.size() / (double)worker_num);
        // cout << "num_responsibility " << num_responsibility << endl;
        // break;
        #pragma omp parallel 
        {// Code inside this region runs in parallel.
            deque<pair<pair<double, double>, pair<double, double>> >  pairs_tmp;
            for (int i = 0; i < num_responsibility; i++) {
                #pragma omp critical(dataupdate) 
                { // TODO: may need another way to pick up pairs (take all pairs together)
                    if (!pairs.empty()) {
                        pairs_tmp.push_back(pairs.front());
                        pairs.pop_front();
                    }
                }
            }
            // #pragma omp critical(dataupdate) 
            // {
            //     cout << "This worker: " << endl;
            //     for (auto p: pairs_tmp) cout << p.first << " " << p.second << endl;
            // }

            // begin calculation!
            for (int i = 0; i < NUM_IT; i++) {
                if (pairs_tmp.empty()) break;
                else {
                    pair<pair<double, double>, pair<double, double>> cur_pair = pairs_tmp.back();
                    pairs_tmp.pop_back();
                    // update @MAX
                    double tmp_max = max(cur_pair.first.second, cur_pair.second.second);
                    #pragma omp critical(dataupdate) 
                    {
                        MAX = MAX > tmp_max ? MAX : tmp_max;
                    }
                    // insert into deque @pairs_tmp
                    if ((cur_pair.second.first - cur_pair.first.first) / 2 < min_interval) { continue;}
                    double mid_point = (cur_pair.first.first + cur_pair.second.first) / 2;
                    double g_mid_point = g(mid_point);
                    double possible_max_left = (cur_pair.first.second + g_mid_point + s * (mid_point - cur_pair.first.first)) / 2;
                    
                    if (possible_max_left >= MAX + epsilon) pairs_tmp.push_back({cur_pair.first, {mid_point, g_mid_point}});
                    double possible_max_right = (g_mid_point + cur_pair.second.second + s * (cur_pair.second.first - mid_point)) / 2;
                    if (possible_max_right >= MAX + epsilon) pairs_tmp.push_back({{mid_point, g_mid_point}, cur_pair.second});
                    // cout << "?" << pairs_tmp.size() << endl;
                }
            } // for
            // cout << pairs_tmp.size() << endl;
            #pragma omp critical(dataupdate) 
            {
                pairs.insert(pairs.end(), pairs_tmp.begin(), pairs_tmp.end());
            }
        } //omp parallel
    }


    cout << "MAX is " << MAX << endl;

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);
    cout << "Time: " << duration.count() << endl;
}
