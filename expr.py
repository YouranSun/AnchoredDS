import os
import argparse

dir = "/mnt/data/yingli/anchored_ds/"

parser = argparse.ArgumentParser()
parser.add_argument('filename')

args = parser.parse_args()
name = dir + args.filename
# name = "/mnt/data/yingli/anchored_ds/com-amazon/com-amazon.ungraph"
# name = "/mnt/data/yingli/anchored_ds/dblp/com-dblp.ungraph"
# name = "/mnt/data/yingli/anchored_ds/Gowalla/Gowalla"
# name = "/mnt/data/yingli/anchored_ds/cit-Patents/cit-Patents"
# name = "/mnt/data/yingli/anchored_ds/roadNet-PA/roadNet-PA"
# name = "/mnt/data/yingli/anchored_ds/wiki-topcats/wiki-topcats"
# name = "/mnt/data/yingli/anchored_ds/wikitalk/wikitalk"

def execute(prog, args, out = None):
    command = "./" + prog
    for (name, value) in args.items():
        command += " -" + name + " " + str(value)
    if (out != None):
        command += " >" + out + ".out"
    os.system(command)


os.system("make -j4")
g = name + ".txt"
st = name + "-st" + ".txt"
for t in range(10):
    seed = t
    r = name + "-r" + "-" + str(t) + ".txt"
    a = name + "-a" + "-" + str(t) + ".txt"
    execute("/bin/generator", {"g": g, "r": r, "a": a, "st": st, "seed": seed})
for t in range(10):
    seed = t
    r = name + "-r" + "-" + str(t) + ".txt"
    a = name + "-a" + "-" + str(t) + ".txt"
    
    # T_list = [0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048]
    T_list = [32]
    for i in range(0, len(T_list)):
        T = T_list[i]
        out = name + str(t) + "-" + str(i)
        # execute("/bin/generator", {"g": g, "r": r, "a": a, "st": st, "seed": seed})
        execute("/bin/AnchoredDS", {"g": g, "r": r, "a": a, "d": "FISTA", "c": "", "T": T}, out + "-FISTA")
        execute("/bin/AnchoredDS", {"g": g, "r": r, "a": a, "d": "FDP", "T": T}, out + "-FDP")

    