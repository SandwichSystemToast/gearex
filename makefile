run: compile_dbg pack-assets
	./build/gearex

valgrind:
	valgrind --leak-check=yes ./build/gearex

setup:
	CXX=clang++ meson setup build_dbg
	meson configure build_dbg -Ddebug=true

	CXX=clang++ meson setup build_rel
	meson configure build_rel -Ddebug=false -Db_lto=true -Doptimization=3

compile_dbg: setup
	meson compile -C build_dbg

compile_rel: setup
	meson compile -C build_rel

compile: compile_dbg

pack-assets:
	# don't include the root assets/ directory in the archive
	tar -czvf assets.tar.gz assets
