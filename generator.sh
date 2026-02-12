KNOWN_CLASS="data/KCg-m-pairs.tsv"
NUM_ITERS=100

BANC_MIRRORED="/scratch/preserve/wayne/FlyWire/Skeletons/banc_mirrored"
FAFB="/scratch/preserve/wayne/FlyWire/Skeletons/fafb_banc_space/240721"

make debug -j8 && ./nblast++ -g "$KNOWN_CLASS","$NUM_ITERS" -i "$BANC_MIRRORED","$FAFB"