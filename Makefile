build:
	clang main.c -o storybox

run: build
	./storybox

clean:
	rm -rf storybox