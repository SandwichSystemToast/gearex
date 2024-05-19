run: compile
	./build/gearex

compile:
	meson setup build
	CXXFLAGS="-g" meson compile -C build
