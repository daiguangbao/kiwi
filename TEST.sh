#!/bin/sh

cd ./build/test

./core/FactoryTest  &&
./core/NodeTest &&
./generic/ArrayContainerTest && 
./generic/MultiArrayContainerTest && 
./generic/PointTest &&
./generic/ValueContainerTest &&
./image/CairoImageContainerTest &&
./text/PlainTextContainerTest

echo '\n\n--end of the test suite--\n\n'
