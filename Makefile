

tests: tests\tests.cpp C:\msys64\ucrt64\lib\libgtest.a
	g++ tests\tests.cpp C:\msys64\ucrt64\lib\libgtest.a -o build\tests

