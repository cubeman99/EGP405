
mkdir Release
mkdir Release/Executable
mkdir Release/Source

copy build/GameNetworking.sln Release/Source
copy build/GameNetworking.vcxproj Release/Source
copy build/GameNetworking.vcxproj.filters Release/Source
copy build/*.h Release/Source
copy build/*.cpp Release/Source

copy build/Debug/GameNetworking.exe Release/Executable/GameNetworking.exe

