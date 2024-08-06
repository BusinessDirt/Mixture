echo "$PWD"
./vendor/premake/bin/premake5 --cc=clang gmake2  --file=Build.lua
make
bin/Debug-macosx-ARM64/App/App