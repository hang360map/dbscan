#include "dbscan.h"
#include<random>
#include<boost/algorithm/string.hpp>
#include<fstream>
#include<boost/lexical_cast.hpp>
#include<sstream>

using namespace boost;

Info::Info():
    id_(-1),
    group_(-1),
    visited_(false),
    x_(0.0),
    y_(0.0)
{

}

Info::Info(int id, string value, double x, double y){
    id_ = id;
    group_ = -1;
    value_ = value;
    visited_ = false;
    x_ = x;
    y_ = y;
}

void Info::setData(int id, int group, string value, bool visited, double x, double y){
    id_ = id;
    group_ = group;
    value_ = value;
    visited_ = visited;
    x_ = x;
    y_ = y;
    }

void Info::print(){
    std::cout << "id_: " << id_ << ", group_: " << group_ << ", value_: " << value_ << ", visited: " << visited_ << ", x_: " << x_ << ", y_: " << y_ << std::endl;
}


DBscan::DBscan( vector<Info> info){
    info_ = info;
}

vector<Info> DBscan::getInfo(){
    return info_;
}

void DBscan::grouping(map<int, vector<Info> > &vec){
    map<int, vector<Info> >::iterator it;
    for(int i = 0; i < info_.size(); i++){
        it = vec.find(info_[i].group_);
        if(it != vec.end()){
            it->second.push_back(info_[i]);
        }else{
            vector<Info> tmp;
            tmp.push_back(info_[i]);
            vec.insert(pair<int, vector<Info> >(info_[i].group_, tmp));
        }
    }
}

void DBscan::setInfo(vector<Info> info){
    info_ = info;
}
void DBscan::judge_minpts(double radius, vector<Info> pip, Info r, vector<Info> &N){
    int count_tmp = 0;
    for(int i = 0; i < pip.size(); i++){
double dis = earth_distance(r.x_, r.y_, pip[i].x_, pip[i].y_);
        if(dis < radius && dis != 0.0){
            count_tmp++;
            N.push_back(pip[i]);
        }
    }
}
void DBscan::do_dbscan(int minpts, double radius){
    vector<Info> pip = info_;
    info_.clear();
    int group = 1;
    while(pip.size() > minpts){
        vector<Info> N;
        int r = rand() % pip.size();
        pip[r].group_ = group;
        pip[r].visited_ = true;
        N.push_back(pip[r]);
        judge_minpts(radius, pip, pip[r], N);
        std::cout << "########## pip: " << pip.size() << std::endl;
        std::cout << "########## N: " << N.size() << std::endl;
        std::cout << "######### group: " << group << std::endl;
        if(N.size() > minpts){
            std::cout << "%%%%%%%%%%%%%%%%%%%%::" << N.size() << std::endl;
            for(int i = 0; i < N.size(); i++){
                if(N[i].visited_)
                    continue;
                N[i].visited_ = true;
                vector<Info> N1;
                judge_minpts(radius, pip, N[i], N1);
                std::cout << "###########judge:" << N1.size() << " N:" << N.size() << std::endl;
                if(N1.size() > minpts){
                    for(int j = 0; j < N1.size(); j++){
                        bool flag_has = false;
                        for(int k = 0; k < N.size(); k++){
                            if(N[k].id_ == N1[j].id_){
flag_has = true;
                                break;
                            }
                        }
                        if(flag_has){
                            continue;
                        }
                        N.push_back(N1[j]);
                    }
                }
                if(N[i].group_ == -1){
                    N[i].group_ = group;
                }
            }
        }else{
            pip[r].group_ = -2;
            for(vector<Info>::iterator it = pip.begin(); it != pip.end();){
                if(it->id_ == pip[r].id_){
                    pip.erase(it);
                    break;
                }else{
                    it++;
                }
            }
            continue;
        }

        group++;
        for(int i = 0; i < N.size(); i++){
            info_.push_back(N[i]);

            for(vector<Info>::iterator it = pip.begin(); it != pip.end();){
                if(it->id_ == N[i].id_ && it != pip.end()){
                    pip.erase(it);
                    break;
}else if(it != pip.end()){
                    it++;
                }
            }
        }
        std::cout << "!!!!!!!!!!!!!!info: " << info_.size() << std::endl;
        std::cout << "!!!!!!!!!!!!!!pip: " << pip.size() << std::endl;
   }
}

double DBscan::earth_distance(double lat1, double lng1, double lat2, double lng2){
    double radLat1 = deg2rad(lat1);
    double radLat2 = deg2rad(lat2);
    double a = radLat1 - radLat2;
    double b = deg2rad(lng1) - deg2rad(lng2);

    double dst = 2.0 * asin( (sqrt( pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2))));

    dst = dst * 6378.137;
    return dst;
}

double DBscan::deg2rad(double d){
    return d * 3.1415926535898 / 180.0;
}

DBscan getData(){
    string input_file = "poi_beijing";
    ifstream input(input_file.c_str());
    string line;
    DBscan data;
    vector<Info> info;
    int id = 0;
    while(getline(input, line)){
vector<string> set;
        boost::split(set, line, boost::is_any_of("\t"));
        if(set.size() >= 6){
            Info t;
            string value = set[0] + "\t" + set[3] + "\t" + set[4] + "\t" + set[5];
            vector<string> set1;
            boost::split(set1, set[5], boost::is_any_of("|"));
            if(set1.size() != 2)
                continue;
            boost::trim(set1[0]);
            boost::trim(set1[1]);
            //double x = lexical_cast<double>(set1[1]);
            //double y = lexical_cast<double>(set1[0]);
            stringstream ss;
            double x = 0, y = 0;
            ss << set1[1];
            ss >> x;
            ss.clear();
            ss << set1[0];
            ss >> y;
            t.setData(id, -1, value, false, x, y);
            id++;
            std::cout << "id: " << id  << ", x: " << t.x_ << ", y: " << t.y_ << std::endl;
            info.push_back(t);
        }
    }
    data.setInfo(info);
    input.close();
    return data;
}

void cluster_to_file(map<int, vector<Info> > vec, string tar)
{
    ofstream output(tar.c_str());
    for(map<int, vector<Info> >::iterator it = vec.begin(); it != vec.end(); it++){
output << "group\t" << it->first << "\n";
        for(int i = 0; i < it->second.size(); i++){
            output << "id\t" << it->second[i].id_ << "\tvalue\t" << it->second[i].value_ << "\n";
        }
        output << "\n";
    }
    output.close();
}


int main()
{
    DBscan db = getData();
    db.do_dbscan(1000, 0.5);
    map<int, vector<Info> > vec;
    db.grouping(vec);
    cluster_to_file(vec, "test");
    return 0;
}
