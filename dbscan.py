import os
import sys
import math
import random
import re

default_encoding = "utf-8"

PI = 3.1415926535898
EARTH_RADIUS = 6378.137
if sys.getdefaultencoding() != default_encoding:
    reload(sys)
    sys.setdefaultencoding(default_encoding)

def deg2rad(d):
    return d * PI / 180.0

def earth_distance(lat1, lng1, lat2, lng2):
    radLat1 = deg2rad(lat1)
    radLat2 = deg2rad(lat2)
    a = radLat1 - radLat2
    b = deg2rad(lng1) - deg2rad(lng2)

    dst = 2.0 * math.asin((math.sqrt(math.pow(math.sin(a / 2), 2) + math.cos(radLat1) * math.cos(radLat2) * math.pow(math.sin(b / 2), 2))))
    dst = dst * EARTH_RADIUS
    return dst

class Info:
    id_ = 0
    group_ = 0
    value_ = ""
    visited_ = 0
    x_ = 0.0
    y_ = 0.0
    def __init__(self):
        self.id_ = 0
    def setData(self, id_, group_, value_, visited_, x_, y_):
        self.id_ = id_
        self.group_ = group_
        self.value_ = value_
        self.visited_ = visited_
        self.x_ = x_
        self.y_ = y_
    def print_info(self):
        print "id_: " + str(self.id_) + ", group_: " + str(self.group_) + ", value_: " + self.value_ + ", visited: " + str(self.visited_) + ", x_: " + str(self.x_) + ", y_: "+ str(self.y_)

class DBscan:
    info_ = []
    def __init__(self):
        self.info_ = []
    def grouping(self, vec):
        for i in xrange(0, len(self.info_)):
            if vec.has_key(self.info_[i].group_):
                vec[self.info_[i].group_].append(self.info_[i])
            else:
                tmp = []
                tmp.append(self.info_[i])
                vec[self.info_[i].group_] = tmp
        return vec
    def setInfo(self, info_):
        self.info_ = info_
    def judge_minpts(self, radius, pip, r, N):
        for i in xrange(0, len(pip)):
            dis = earth_distance(r.x_, r.y_, pip[i].x_, pip[i].y_)
            if dis < radius and dis != 0.0:
                N.append(pip[i])
        return N
    def do_dbscan(self, minpts, radius):
        pip = self.info_
        newinfo = []
        group = 1
        while len(pip) >= minpts:
            N = []
            r = random.randint(0, len(pip) - 1)
            pip[r].group_ = group
            pip[r].visited_ = 1
            N.append(pip[r])
            N = self.judge_minpts(radius, pip, pip[r], N)
            if len(N) >= minpts:
                i = 0
                while i < len(N):
 if N[i].visited_ == 1:
                        i = i + 1
                        continue
                    N[i].visited_ = 1
                    N1 = []
                    N1 = self.judge_minpts(radius, pip, N[i], N1)
                    if len(N1) >= minpts:
                        j = 0
                        while j < len(N1):
                            flag_has = 0
                            k = 0
                            while k < len(N):
                                if N[k].id_ == N1[j].id_:
                                    flag_has = 1
                                    break
                                k = k + 1
                            if flag_has == 1:
                                j = j + 1
                                continue
                            N.append(N1[j])
                            j = j + 1
                    if N[i].group_ == 0:
                        N[i].group_ = group
                    i = i + 1
            else:
                del pip[r]
                continue
            group = group + 1
            for i in xrange(0, len(N)):
                newinfo.append(N[i])
                for j in xrange(0, len(pip)):
                    if pip[j].id_ == N[i].id_:
                        del pip[j]
                        break
        self.info_ = newinfo
def getData(vec):
    data = DBscan()
    info = []
    ID = 0
    for i in xrange(0, len(vec)):
        t = Info()
        tmp_set = vec[i].split("\t")
        xys = tmp_set[4].split(",")
        if len(xys) != 2:
            continue
        x = float(xys[0])
        y = float(xys[1])
        t.setData(ID, 0, vec[i], 0, x, y)
        ID = ID + 1
        info.append(t)
    data.setInfo(info)
    return data

def judge_branch_school(line):
    line = line.strip().strip("\t").strip("\n")
    tmp_set = line.split("\t")
    if len(tmp_set) < 3 or line.find("培训机构") != -1 or line.find("驾校") != -1 or line.find("媒体") != -1:
        return 0
    sub_vec = []
    sub_vec.append("校区")
    sub_vec.append("分校")
    sub_vec.append("大学")
    for i in xrange(0, len(sub_vec)):
        m1 = re.search('.*' + sub_vec[i] + '$', tmp_set[1])
        m2 = re.search('.*' + sub_vec[i] + '\)$', tmp_set[1])
        m3 = re.search('.*' + sub_vec[i] + '）$', tmp_set[1])
        if m1 or m2 or m3:
            return 1
    return 0

if __name__ == "__main__":
    input_file = open("college_by_city", "r")
    vec = []
    for line in input_file:
        line = line.strip().strip("\t").strip("\n")
        tmp_set = line.split("\t")
        if len(tmp_set) >= 3:
            vec.append(line)
        elif len(tmp_set) < 3:
            if len(vec) <= 1:
                del vec[:]
                continue
            if len(vec) > 1:
                db = getData(vec)
                db.do_dbscan(2, 1)
                vec_dict = {}
                vec_dict = db.grouping(vec_dict)
                #print len(vec_dict)
                print_flag = 0
                if len(vec_dict) <= 1:
                    del vec[:]
                    continue
                #print len(vec_dict)
                for (k, v) in vec_dict.items():
                    #if len(v) <= 1:
                     #   continue
                    branch_school = []
                    for i in xrange(0, len(v)):
                        if judge_branch_school(v[i].value_) == 1: #以分校，校区，学校结尾的直接加入结果集
                            branch_school.append(v[i].value_)
                    if len(branch_school) == 0:
                        shortest = ""
                        shortest_flag = -1
                        sub_flag = 0
                        for j in xrange(0, len(v)):
shortest_tmp = v[j].value_.split("\t")
                            if v[j].value_.find("校区") != -1 or v[j].value_.find("分校") != -1:
                                shortest = shortest_tmp[1]
                                shortest_flag = j
                                sub_flag = 1
                                break
                        if sub_flag == 0:
                            for j in xrange(0, len(v)):
                                shortest_tmp = v[j].value_.split("\t")
                                if j == 0 and shortest == "":
                                    shortest = shortest_tmp[1]
                                    shortest_flag = j
                                else:
                                    if len(shortest_tmp[1]) < len(shortest):
                                        shortest = shortest_tmp[1]
                                        shortest_flag = j
                        else:
                            for j in xrange(0, len(v)):
                                shortest_tmp = v[j].value_.split("\t")
                                if (v[j].value_.find("校区") != -1 or v[j].value_.find("分校") != -1) and len(shortest_tmp[1]) < len(shortest):
                                    shortest = shortest_tmp[1]
                                    shortest_flag = j

                        if shortest_flag != -1:
                            branch_school.append(v[shortest_flag].value_)
                    if len(branch_school) != 0:
                        print_flag = 1
                        for j in xrange(0, len(branch_school)):
                            print branch_school[j]
                if print_flag == 1:
                    print "\n"

            del vec[:]
    input_file.close()
