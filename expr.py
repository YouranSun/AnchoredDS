import os
import argparse

dir = "/mnt/data/yingli/anchored_ds/"

parser = argparse.ArgumentParser()
parser.add_argument("-start", "--start")
parser.add_argument("-end", "--end")
args = parser.parse_args()
start = int(args.start)
end = int(args.end)

with open('datasets2.txt') as datasets:
    for filename in datasets:
        filename = filename.strip()
        if not filename: continue
        # parser = argparse.ArgumentParser()
        # parser.add_argument('filename')

        # args = parser.parse_args()
        # name = dir + args.filename

        name = dir + filename

        def execute(prog, args, out = None):
            command = "./" + prog
            for (name, value) in args.items():
                command += " -" + name + " " + str(value)
            if (out != None):
                command += " >" + out + ".out"
            os.system(command)


        # os.system("make -j4")

        g = name + ".txt"
        T = 1000000000

        for t in range(start, end):
            seed = t
            r = name + "-r" + "-" + str(t) + ".txt"
            a = name + "-a" + "-" + str(t) + ".txt"

            head, tail = os.path.split(name)
            out = os.path.join(head, "out-" + str(t))
            print(out)
            # execute("/bin/generator", {"g": g, "r": r, "a": a, "st": st, "seed": seed})
            # execute("/bin/AnchoredDS", {"g": g, "r": r, "a": a, "d": "FISTA", "T": T, "c": ""}, out + "-FISTA")
            # execute("/bin/AnchoredDS", {"g": g, "r": r, "a": a, "d": "FISTA", "T": T}, out + "-FISTA-ncore")
            execute("/bin/AnchoredDS", {"g": g, "r": r, "a": a, "d": "FW", "T": T}, out + "-FW")
            # execute("/bin/AnchoredDS", {"g": g, "r": r, "a": a, "d": "FDP", "T": T}, out + "-FDP")