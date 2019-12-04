#!/bin/bash

#+======================================================================
#+======================================================================

mvn clean compile

echo "***** Starting dynamic tests *****"

mkdir ./target/valgrind-reports
##/usr/bin/valgrind --tool=memcheck --xml=yes --xml-file=./target/valgrind-reports/memcheck-result-report.xml --leak-check=yes ./target/bin/unit-tests
##/usr/bin/valgrind --tool=helgrind --xml=yes --xml-file=./target/valgrind-reports/helgrind-result-report.xml ./target/bin/unit-tests
/usr/bin/valgrind --log-file=./target/valgrind-reports/memcheck-result-report.out --tool=memcheck --leak-check=yes --track-origins=yes ./target/bin/unit-tests
#/usr/bin/valgrind --log-file=./target/valgrind-reports/helgrind-result-report.out --tool=helgrind ./target/bin/unit-tests

echo "***** End of dynamic tests *****"

