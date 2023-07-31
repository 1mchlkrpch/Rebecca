BUILD_DIR = build

.PHONY: init, build

init:
	mkdir $(BUILD_DIR)

build:
	$(shell cd build; cmake ..; cd ..)
