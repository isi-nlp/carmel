theirbaseline() {
    perl -ne 'if (/\[(.*-baseline)\]/) { print $1; exit }' "$@"
}
baselinetune() {
    lntune "$@"
    baseline=`theirbaseline $theirs`
    echo $baseline $theirs $mine
}
nocommands() {
    perl -e 'while(<>) { $sec = $1 if /^\[(.*)\]/; print ($sec eq "commands" ? "# $_" : $_) }' "$@"
}
mycommands() {
    [[ $baseline ]] || baselinetune "$@"
    if ! [[ $tunestart ]] ; then
        if [[ $lp = eng-swe ]] ; then tunestart=/c01_data/mdreyer/xmt-test/eng-swe/20141031-baseline-retune-pro1-i10/tune_slot/tune_work/iter_9/weights.yaml
        else
            tunestart=/c01_data/mdreyer/xmt-test/pol-eng/data/weights-init.yml
        fi
    fi
    cp $tunestart $mine.weights-init.yml
    tunestart=`realpath $tunestart`
    ln -sf $tunestart $mine.weights-init.yml.origin
    tunestart=$mine.weights-init.yml
    tunestart=`realpath $tunestart`
    xmt=${xmt:-/c01_data/mdreyer/software/coretraining/main/LW/CME/xmt-rpath}
    showvars_required baseline theirs mine tunestart xmt
    require_file $xmt
    xmt=`abspath $xmt`
    require_file $theirs $tunestart
    l0s="0 1e-5 4e-5 4e-4 1e-4 4e-3 1e-3"
    for l0 in $l0s; do
        echo $l0 > STDERR
        mert=$(echo ~graehl/pub/mert-l0=$l0)
        require_file $mert
        cat<<EOF
[l0=$l0]
task=xtrain
inherit=$baseline
mert-bin=/home/graehl/pub/mert-l0=$l0
prep_slot=[$baseline]/prep_slot
wa_slot=[$baseline]/wa_slot
cap_slot=[$baseline]/cap_slot
rules_slot=[$baseline]/rules_slot
db_slot=[$baseline]/db_slot
lm_slot=[$baseline]/lm_slot
tuning-max-pro-iter=0
tuning-max-mert-iter=30
tuning-orig-weights-file=$tunestart
xmt-bin=$xmt
decoding-memory-request-gb=28
num-decoder-threads=2
tuning-num-pieces=10

EOF
    done
    echo
    echo [commands]
    for l0 in $l0s; do
        echo l0=$l0
    done
}
mytune() {
    [[ $baseline ]] || baselinetune "$@"
    (nocommands $theirs; mycommands) > $mine
    tail -100 $mine
    echo $mine
}
lntune() {
    src=$1
    trg=${2:-lntune ita eng [basedir]}
    lp=$1-$2
    base=${3:-/c01_data/mdreyer/xmt-test}
    origin=$base/$lp
    fromapex=$origin/kraken.$lp.apex
    require_dir $base/$lp
    require_file $fromapex
    [[ -d $lp ]] || cp -nRs $base/$lp .
    require_dir $lp
    theirs=$lp/their.apex
    cp $fromapex $theirs
    mine=$lp/my.apex
}
