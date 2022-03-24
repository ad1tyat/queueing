#include "routing.h"


double get_rand(){
    return (double)rand()/(double)RAND_MAX;
}

bool custom_sort(pair<int, packet> a, pair<int, packet> b){
    return a.second.st_time < b.second.st_time;
}

double standard_deviation(vector<int> &v){
    long int sum = 0;
    for(auto &x : v) sum+=x;
    long double mean = (long double)sum/(long double)v.size();
    double variance = 0;
    for(auto &x : v){
        variance += pow((long double)x - (long double)mean, 2);
    }
    variance = (double)variance/(double)(v.size());
    return (double)sqrt(variance);
}
double mean(vector<int> &v){
    long int sum = 0;
    for(auto &x : v) sum+=x;
    return (long double)sum/(long double)v.size();
}


void init_struct(packet& pac, int idx, float st_time, float en_time, int ip_port, int op_port){
    pac.idx = idx;
    pac.st_time = st_time;
    pac.en_time = en_time;
    pac.ip_port = ip_port;
    pac.op_port = op_port;
}

int phase_1(string queue_type){

    for(int i = 0;i<number_ports;i++){
        double val = get_rand();
        if(val < packetgenprob){
            packet tmp;

            int op_port = rand()%number_ports;
            
            float st_time = current_time;
            float additional = ((double)rand()/(double)RAND_MAX)/(double)100; // to get [t+0.001, t + 0.01]
            st_time += additional;

            init_struct(tmp, global_index, st_time, -1, i, op_port);
            global_index++;
            
            // push tmp to buffer


            // if ISLIP, send it to Virtual Output Queue
            if(queue_type == "ISLIP"){
                int csize = ip_port_voq[i][op_port].size();
                if(csize >= buffer_size){
                    // input buffer full, drop the packet;
                }else{
                    cout<<"Packet from "<<i<<" to "<<op_port<<" generated."<<endl;
                    ip_port_voq[i][op_port].push(tmp);
                }
            }else{
                int csize = ip_port[i].size();
                if(csize >= buffer_size){
                    // input buffer full, drop the packet;
                }else{
                    ip_port[i].push(tmp);
                }
            }
        }
    }

    return 1;
}


int phase_2_inq(){
    vector<vector<pair<int,packet>>> op_temp;
    op_temp.resize(number_ports);
    for(int i = 0;i<number_ports;i++){
        if(ip_port[i].size() > 0){
            auto curr = ip_port[i].front();
            op_temp[curr.op_port].push_back({curr.ip_port, curr});
        }
    }
    for(int i = 0;i<number_ports;i++){
        if(op_temp[i].size() == 1){
            
            auto curr_id = op_temp[i][0].first;
            auto curr = op_temp[i][0].second;
            ip_port[curr_id].pop();
            if(op_port[i].size() < buffer_size)
                op_port[i].push(curr);

        }else if(op_temp[i].size() > 1){
            int sz = op_temp[i].size();
            int rnd_ind = rand()%sz;
            
            auto curr_id = op_temp[i][rnd_ind].first;
            auto curr = op_temp[i][rnd_ind].second;
            ip_port[curr_id].pop();
            if(op_port[i].size() < buffer_size)
                op_port[i].push(curr);
        }
    }
    return 0;
}

int phase_2_kouq(){
    vector<vector<pair<int,packet>>> op_temp;
    op_temp.resize(number_ports);
    for(int i = 0;i<number_ports;i++){
        if(ip_port[i].size() > 0){
            auto curr = ip_port[i].front();
            op_temp[curr.op_port].push_back({curr.ip_port, curr});
        }
    }

    for(int i = 0;i<number_ports;i++){
        if(op_temp[i].size() == 1){
            
            auto curr_id = op_temp[i][0].first;
            auto curr = op_temp[i][0].second;
            ip_port[curr_id].pop();
            if(op_port[i].size() < buffer_size)
                op_port[i].push(curr);

        }else if(op_temp[i].size() > 1){
            // take k randomly, sort them, put them in buffer
            
            // take k randomly :
            auto rng = std::default_random_engine {};
            shuffle(op_temp[i].begin(), op_temp[i].end(), rng);
            
            // sort the k packets
            int lim = min(knockout_time, (int)op_temp[i].size());
            sort(op_temp[i].begin(), op_temp[i].begin() + lim, custom_sort);
            
            if(op_temp[i].size() > knockout_time){
                kouq_drop_cnt++;
            }

            // push k to buffer
            for(int j = 0;j<lim;j++){
                auto curr_id = op_temp[i][j].first;
                auto curr = op_temp[i][j].second;
                ip_port[curr_id].pop();
                
                if(op_port[i].size() < buffer_size)
                    op_port[i].push(curr);
            }

            // drop remaining packets
            for(int j = lim;j<(int)op_temp[i].size();j++){
                auto curr_id = op_temp[i][j].first;
                auto curr = op_temp[i][j].second;
                ip_port[curr_id].pop();
            }
        }
    }

    return 0;
}

int phase_2_islip(){

    for(int i = 0;i<number_ports;i++){
        op_port_reqs[i].clear();
        ip_port_grants[i].clear();
    }

    // implemented multi iteration 
    // for each iteration :
    // 1. request
    // 2. grant
    // 3. accept

    // request
    // for each input port
    for(int i = 0;i<number_ports;i++){
        // for each virtual output queue
        for(int j = 0;j<number_ports;j++){
            // if there is a packet
            if(ip_port_voq[i][j].size() > 0){
                // send request to op port
                auto curr = ip_port_voq[i][j].front();
                op_port_reqs[j].push_back({i, curr});
            }
        }
    }
    // grant :
    // for each output port
    for(int i = 0;i<number_ports;i++){
        int highest_priority = op_port_priority[i];
        set<int> ip_port_reqs;
        for(auto &x : op_port_reqs[i]){
            ip_port_reqs.insert(x.first);
        }
        bool granted = 0;
        for(int j = 0;j<number_ports;j++){
            int curr = (highest_priority + j)%number_ports;
            if(ip_port_reqs.find(curr) != ip_port_reqs.end()){
                op_port_grant[i] = curr;
                cout<<curr<<" "<<i<<endl; 
                assert(ip_port_voq[curr][i].size() > 0);
                assert(ip_port_voq[curr][i].front().op_port == i);
                assert(ip_port_voq[curr][i].front().ip_port == curr);
                ip_port_grants[curr].push_back(i);
                granted = 1;
                break;
            }
        }
    }
    // accept :
    // for each input port
    for(int i = 0;i<number_ports;i++){
        if(ip_port_grants[i].size() > 0){

            
            int highest_priority = ip_port_priority[i];
            set<int> all_grants;

            for(auto &x : ip_port_grants[i]){
                all_grants.insert(x);
            }

            bool accepted = 0;
            for(int j = 0;j<number_ports;j++){
                int curr = (highest_priority + j)%number_ports;
                if(all_grants.count(curr)){
                    // WE ARE ACCEPTING THIS PACKET
                    // IP = i, OP = curr
                    assert(ip_port_voq[i][curr].size() > 0);
                    auto pack = ip_port_voq[i][curr].front();
                    // popping the transmitted packet
                    ip_port_voq[i][curr].pop();

                    // pushing it to output buffer
                    if(op_port[curr].size() < buffer_size){
                        op_port[curr].push(pack);
                    }

                    // Setting IP Port Priority
                    ip_port_priority[i] = (curr + 1)%number_ports;
                    
                    // Setting OP Port Priority
                    op_port_priority[curr] = (i + 1)%number_ports;
                    break;
                }
            }
        }
    }    
    return 0;
}

int phase_2(){
    if(queue_type == "INQ"){
        phase_2_inq();
    }else if(queue_type == "KOUQ"){
        phase_2_kouq();
    }else if(queue_type == "ISLIP"){
        phase_2_islip();
    }else{
        cout<<"ERROR : Queue type is undefined. Should be one of : INQ, KOUQ, ISLIP."<<endl;
        return -1;
    }
    return 1;
}

int phase_3(){
    for(int i = 0;i<number_ports;i++){
        if(op_port[i].size() > 0){
            // packet at head of queue will be transmitted
            auto curr = op_port[i].front();
            op_port[i].pop();
            int en_time = current_time + 1;
            int st_time = curr.st_time;
            sum_of_delays += en_time - st_time; 
            all_delays.push_back(en_time - st_time);
            link_util++;
        }
    }
    sum_of_kouq += kouq_drop_cnt;
    kouq_drop_cnt = 0;
    return 0;
}


int main(int argc,char* argv[]){
    // setting a seed for rand()
    srand(time(0));

    // processing command line inputs
    for(int i = 1;i<argc;i+=2){
        string arg_str = (string)argv[i];
        if(arg_str == "-N"){
            number_ports = stoi((string)argv[i + 1]);
        }else if(arg_str == "-B"){
            buffer_size = stoi((string)argv[i + 1]);
        }else if(arg_str == "-p"){
            packetgenprob = stod((string)argv[i + 1]);
        }else if(arg_str == "-queue"){
            queue_type = argv[i + 1];
        }else if(arg_str == "-K"){
            knockout_factor = stod((string)argv[i + 1]);
        }else if(arg_str == "-out"){
            output_file = argv[i + 1];
        }else if(arg_str == "-T"){
            max_time_slots = stod((string)argv[i + 1]);
        }else{
            cout<<"ERROR : Command line argument is undefined."<<endl;
            cout<<"Argument should be of type: -N, -B, -p, -queue, -K, -out, -T"<<endl;
            return 0;
        }
    }
    
    // resizing port buffers according to number of ports
    ip_port.resize(number_ports);
    op_port.resize(number_ports);

    // setting knockout time
    if(queue_type == "KOUQ"){
        knockout_time = ((double)knockout_factor*(double)number_ports);
    }

    if(queue_type == "ISLIP"){

        ip_port_priority.resize(number_ports);
        op_port_priority.resize(number_ports);
        ip_port_voq.reserve(number_ports);

        op_port_reqs.resize(number_ports);
        op_port_grant.resize(number_ports);
        ip_port_grants.resize(number_ports);

        for(int i = 0; i<number_ports;i++){
            ip_port_voq[i].resize(number_ports);
            ip_port_priority[i] = 0;
            op_port_priority[i] = 0;
        }
    }

    fstream fout;
    fout.open(output_file,  std::fstream::out | std::fstream::app);

    fout<<setprecision(5);

    fstream dout;
    dout.open(debug_file,  std::fstream::out);

    // simulating the timeslots
    for(int tm = 0;tm<max_time_slots;tm++){

        // initiaing phase 1
        int phase1_over = phase_1(queue_type);

        if(phase1_over >= 0){
            
            dout<<"Time slot "<<tm<<" :Phase 1 finished successfully."<<endl;
            int phase2_over = phase_2();

            if(phase2_over >= 0){

                dout<<"Time slot "<<tm<<" :Phase 2 finished successfully."<<endl;
                int phase_3_over = phase_3();

                if(phase_3_over >= 0){

                    dout<<"Time slot "<<tm<<" :Phase 3 finished successfully."<<endl;
                    dout<<"Time slot "<<tm<<" :All three phases finished successfully."<<endl;
                }else{
                    cout<<"ERROR"<<endl;
                    return 1;
                }
            }else{
                cout<<"ERROR"<<endl;
                return 1;
            }
        }else{
            cout<<"ERROR"<<endl;
            return 1;
        }
        current_time++;
    }
    // avg Packet delay = mean(all_delays)
    // standard deviation Packet delay = standard_deviation(all_delays)
    fout<<number_ports<<'\t'<<packetgenprob<<'\t'<<queue_type<<'\t';
    if(all_delays.size() > 0){

        double avg_link_util = ((double)link_util / (double)number_ports)/(double)max_time_slots;
        double avg_kouq_drop_probab = ((double)sum_of_kouq / (double)number_ports)/ (double)max_time_slots;
        // fout<<"Avg PD : "<<mean(all_delays)<<'\t';
        // fout<<"Std Dev PD : "<<standard_deviation(all_delays)<<'\t';
        // fout<<"Avg link util : "<<avg_link_util<<'\t';
        fout<<mean(all_delays)<<'\t';
        fout<<standard_deviation(all_delays)<<'\t';
        fout<<avg_link_util<<'\t';
        if(queue_type == "KOUQ")
            fout<<avg_kouq_drop_probab<<'\t';
    }
    fout<<endl;
    fout.close();
    dout.close();
    return 0;
}