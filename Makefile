

tests: tests\tests.cpp c:\msys64\ucrt64\lib\libgtest.a
	g++ tests\tests.cpp c:\msys64\ucrt64\lib\libgtest.a -o build\tests

orderbook.o: C:\Users\ericv\Documents\MATCHING_ENGINE\src\orderbook.cpp
	g++ -c  C:\Users\ericv\Documents\MATCHING_ENGINE\src\orderbook.cpp
testorderbook: tests\testorderbook.cpp c:\msys64\ucrt64\lib\libgtest.a orderbook.o
	g++ tests\testorderbook.cpp c:\msys64\ucrt64\lib\libgtest.a  -o build\testorderbook

