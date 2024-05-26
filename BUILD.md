# build
## download
Download files from github
```
git clone https://github.com/SandwichSystemToast/gearex
git submodules init
git submodules update
```
## build
### make
Release:
```
make compile_rel
make run_rel
```
Debug:
```
make compile_dbg
make run_dbg
```
### meson
```
meson setup build
meson configure build <your opts>
meson compile -C build
./build/gearx
```
## install
### make
```
make install
```
### meson
```
meson install -C build_rel
```
