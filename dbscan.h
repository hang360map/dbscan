#include<iostream>
#include<vector>
#include<map>
#include<cmath>
#include<random>

using namespace std;

class Info{
public:
    int id_;
    int group_;
    string value_;
    bool visited_;
    double x_;
    double y_;

    Info();
    Info(int id, string value, double x, double y);
    void setData(int id, int group, string value, bool visited, double x, double y);
    void print();
};

class DBscan{
public:
    DBscan(){};
    ~DBscan(){};
    DBscan(vector<Info> info);


    void grouping(map<int, vector<Info> > &vec);
    void setInfo(vector<Info> info);
    vector<Info> getInfo();

    void do_dbscan(int minpts, double radius);
     void judge_minpts(double radius, vector<Info> pip, Info r, vector<Info> &N);

    static double deg2rad(double d);
    static double earth_distance(double lat1, double lng1, double lat2, double lng2);
private:
    vector<Info> info_;
};
