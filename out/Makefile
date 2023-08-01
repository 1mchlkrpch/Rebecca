BUILD_DIR = build
OUT_DIR   = out

.PHONY: init build clean

init:
	mkdir $(BUILD_DIR)

build:
	$(shell cd build; cmake ..; cd ..)

clean:
	rm -f *_GEN.c
	rm -f Tokenizer_GEN.h
	rm -f graph.*
	rm -f -r build/*
	rmdir build