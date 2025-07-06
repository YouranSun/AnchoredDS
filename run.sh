# ./bin/AnchoredDS -g samples/com-lj.ungraph.txt -r samples/com-lj.ungraph-r.txt -a samples/com-lj.ungraph-a.txt -d FISTA -c -gp samples/com-lj.ungraph-p.txt -rp samples/com-lj.ungraph-rp.txt -ap samples/com-lj.ungraph-ap.txt
# ./bin/AnchoredDS -g samples/com-lj.ungraph-p.txt -r samples/com-lj.ungraph-rp.txt -a samples/com-lj.ungraph-ap.txt -d FISTA
make -j4
# ./bin/AnchoredDS -g samples/com-lj.ungraph.txt -r samples/com-lj.ungraph-r.txt -a samples/com-lj.ungraph-a.txt -c -d FISTA -seed 0 -T 16
# ./bin/AnchoredDS -g /mnt/data/yingli/anchored_ds/Gowalla/Gowalla.txt -r /mnt/data/yingli/anchored_ds/Gowalla/Gowalla-r-3.txt -a /mnt/data/yingli/anchored_ds/Gowalla/Gowalla-a-3.txt -c -d FISTA -T 2048 >test.out
# ./bin/AnchoredDS -g samples/sample.txt -r samples/sample-r.txt -a samples/sample-a.txt -c -d FISTA -T 1024
# ./bin/AnchoredDS -g samples/sample3.txt -r samples/sample3-r.txt -a samples/sample3-a.txt -d FISTA -T 1024
# ./bin/AnchoredDS -g samples/sample3.txt -r samples/sample3-r.txt -a samples/sample3-a.txt -d FW -T 1024

# ./bin/AnchoredDS -g /mnt/data/yingli/anchored_ds/dblp/com-dblp.ungraph.txt -r /mnt/data/yingli/anchored_ds/dblp/com-dblp.ungraph-r-0.txt -a /mnt/data/yingli/anchored_ds/dblp/com-dblp.ungraph-a-0.txt -c -d FISTA -T 1024
./bin/AnchoredDS -g /mnt/data/yingli/anchored_ds/OF/OF.txt -r /mnt/data/yingli/anchored_ds/OF/OF-r-0.txt -a /mnt/data/yingli/anchored_ds/OF/OF-a-0.txt -c -d FISTA -T 1024
# ./bin/AnchoredDS -g /mnt/data/yingli/anchored_ds/email/email.txt -r /mnt/data/yingli/anchored_ds/email/email-r-10.txt -a /mnt/data/yingli/anchored_ds/email/email-a-10.txt -c -d FISTA -T 1048576
# ./bin/AnchoredDS -g /mnt/data/yingli/anchored_ds/ego-facebook/facebook_combined.txt -r /mnt/data/yingli/anchored_ds/ego-facebook/facebook_combined-r-0.txt -a /mnt/data/yingli/anchored_ds/ego-facebook/facebook_combined-a-0.txt -c -d FISTA -T 1024
# ./bin/AnchoredDS -g /mnt/data/yingli/anchored_ds/youtube/youtube.txt -r /mnt/data/yingli/anchored_ds/youtube/youtube-r-0.txt -a /mnt/data/yingli/anchored_ds/youtube/youtube-a-0.txt -c -d FISTA -T 1024
# ./bin/AnchoredDS -g /mnt/data/yingli/anchored_ds/LiveJournal/soc-LiveJournal1.txt -r /mnt/data/yingli/anchored_ds/LiveJournal/soc-LiveJournal1-r-10.txt -a /mnt/data/yingli/anchored_ds/LiveJournal/soc-LiveJournal1-a-10.txt -c -d FISTA -T 1048576