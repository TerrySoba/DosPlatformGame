docker build -t w64build .
docker run -v C:\Users\Torsten\program_sources\DosPlatformGame\:/src -it w64build /cross_build.sh
