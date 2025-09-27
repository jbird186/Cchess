all:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build --config Debug

clean:
	rm -rf build
