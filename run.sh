# ./bin/AnchoredDS -g samples/com-lj.ungraph.txt -r samples/com-lj.ungraph-r.txt -a samples/com-lj.ungraph-a.txt -d FISTA -c -gp samples/com-lj.ungraph-p.txt -rp samples/com-lj.ungraph-rp.txt -ap samples/com-lj.ungraph-ap.txt
# ./bin/AnchoredDS -g samples/com-lj.ungraph-p.txt -r samples/com-lj.ungraph-rp.txt -a samples/com-lj.ungraph-ap.txt -d FISTA
make -j4
# ./bin/AnchoredDS -g samples/com-lj.ungraph.txt -r samples/com-lj.ungraph-r.txt -a samples/com-lj.ungraph-a.txt -c -d FISTA -seed 0 -T 16
./bin/AnchoredDS -g /mnt/data/yingli/anchored_ds/Gowalla/Gowalla.txt -r /mnt/data/yingli/anchored_ds/Gowalla/Gowalla-r-3.txt -a /mnt/data/yingli/anchored_ds/Gowalla/Gowalla-a-3.txt -c -d FISTA -T 2048 >test.out
# ./bin/AnchoredDS -g samples/sample.txt -r samples/sample-r.txt -a samples/sample-a.txt -c -d FISTA -T 1024 -e -Texact 2