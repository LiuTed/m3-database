#!/usr/bin/python3

dic = {}
with open("output.log", "r") as fin:
    for line in fin:
        vec = line.rstrip('\n').split()
        if len(vec) < 2:
            continue
        cid = int(vec[1])
        dic.setdefault(cid, [])
        if len(vec) > 5:
            err = float(vec[5])
            dic[cid].append(err)

for ent in dic.keys():
    dic[ent].sort()
    for i in dic[ent][0:2]:
        print("{:.4f}".format(i))
