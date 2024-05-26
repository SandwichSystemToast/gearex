run_dbg: compile_dbg pack-assets
	./build_dbg/gearex

run_rel: compile_rel pack-assets
	./build_rel/gearex

run: compile_dbg

valgrind:
	valgrind --leak-check=yes ./build/gearex

setup:
	CXX=clang++ meson setup build_dbg
	meson configure build_dbg -Ddebug=true -Dprefix=/usr/bin/

	CXX=clang++ meson setup build_rel
	meson configure build_rel -Ddebug=false -Db_lto=true -Doptimization=3 -Dprefix=/usr/bin/

compile_dbg: setup
	meson compile -C build_dbg

compile_rel: setup
	meson compile -C build_rel

compile: compile_dbg

install: compile_rel
	meson install -C build_rel
