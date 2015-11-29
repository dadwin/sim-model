#! /bin/sh
#
# usage: runtest [<testfile>...]
# without args, runs all *.test files in the current directory
#
MAKE=make

# start with valgrind if the first argument is --valgrind
VALGRIND=""
VALGRIND_OPTIONS="-v --tool=memcheck --trace-children=yes --leak-check=yes --show-reachable=no --leak-resolution=high --num-callers=40 --freelist-vol=4000000"

if [ "$1" = "--valgrind" ] ; then
   echo "##### Running with Valgrind! ######"
   VALGRIND="valgrind $VALGRIND_OPTIONS"
   shift
fi

#export NEDPATH="../../../src;"

TESTFILES=$*
if [ "x$TESTFILES" = "x" ]; then TESTFILES='*.test'; fi
if [ ! -d work ];  then mkdir work; fi

opp_test gen $OPT -v $TESTFILES || exit 1

echo
(cd work; opp_makemake -f --deep -L../../src -lstorage-simulation -P . --no-deep-includes -I../../src; $MAKE) || exit 1
echo
#env
$VALGRIND opp_test run -v $OPT $TESTFILES || exit 1
echo
echo Results can be found in ./work

