run: compile pack-assets
	./build/gearex

valgrind:
	valgrind --leak-check=yes ./build/gearex

compile:
	meson setup build
	CXXFLAGS="-g" meson compile -C build

pack-assets:
	# don't include the root assets/ directory in the archive
	tar -czvf assets.tar.gz assets
