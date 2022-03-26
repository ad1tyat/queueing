#include <bits/stdc++.h>
using namespace std;

int number_ports = 8; // number of ports N, d efault 8
int buffer_size = 4; // buffer size B, default = 4
float packetgenprob = 0.5; // packet generation probability, default = 0.5
int max_time_slots = 10000; // simulation time, default = 10000
float knockout_factor = 0.6; // knockout time
int knockout_time = -1; // knockout time

string queue_type = "INQ"; // specifies the queue type
string output_file = "output.txt";
string debug_file = "debug.txt";

struct packet{
    int idx;
    float st_time;
    float en_time;
    int ip_port;
    int op_port;
};

int current_time = 0;
int global_index = 1;

int sum_of_delays = 0;
int tot_transmitted = 0;
int link_util = 0;
int kouq_drop_cnt = 0;
int sum_of_kouq = 0;

vector<int> all_delays;

vector<queue<packet>> ip_port;
vector<queue<packet>> op_port;

vector<vector<queue<packet>>> ip_port_voq;
vector<int> ip_port_voq_tot;
vector<vector<pair<int,packet>>> op_port_reqs;
vector<vector<int>> ip_port_grants;

vector<int> op_port_grant;

vector<int> ip_port_priority;
vector<int> op_port_priority;