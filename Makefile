BUILD_DIR = build

.PHONY: init, build

init:
	mkdir $(BUILD_DIR)

build:
	$(shell cd build; cmake ..; cd ..)

clean:
	rm -f graph.*
	rm -f -r build/*
	rm -f -r out/build/*
	rm -rf out/build
	rm -f out/*_GEN.*
	rm -f out/graph.*
	rm -f logfile.txt
	rm -rf build
