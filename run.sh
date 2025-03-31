LOG=/scratch/xsong/project/data/log.txt

echo $(date) >> $LOG

./gbztool info /scratch/xsong/project/data/hprc-v1.1-mc-grch38.gbz > $LOG

echo $(date) > $LOG