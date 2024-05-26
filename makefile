run: compile_debug pack-assets
	./build/gearex

valgrind:
	valgrind --leak-check=yes ./build/gearex

setup:
	CXX=clang++ CXX_LD=lld meson setup build_debug
	meson configure build -Ddebug=true

	CXX=clang++ CXX_LD=lld meson setup build_release
	meson configure build -Ddebug=false -Db_lto=true -Doptimization=3

compile_db: setup
	meson compile -C build_debug

compile_rl: setup
	meson compile -C build_release

pack-assets:
	# don't include the root assets/ directory in the archive
	tar -czvf assets.tar.gz assets
