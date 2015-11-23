#! /bin/sh
#
# usage: runtest [<testfile>...]
# without args, runs all *.test files in the current directory
#
MAKE=make

#export NEDPATH="../../../src;"

TESTFILES=$*
if [ "x$TESTFILES" = "x" ]; then TESTFILES='*.test'; fi
if [ ! -d work ];  then mkdir work; fi
#rm -rf work/lib
#cp -pPR lib work/       # OSX dos not support cp -a
#EXTRA_INCLUDES="-I../../src/"
#EXTRA_INCLUDES="$EXTRA_INCLUDES -I./lib "
opp_test gen $OPT -v $TESTFILES || exit 1
echo
(cd work; opp_makemake -f --deep -L../../src -lstorage-simulation -P . --no-deep-includes -I../../src; $MAKE) || exit 1
echo
#env
opp_test run -v $OPT $TESTFILES || exit 1
echo
echo Results can be found in ./work

