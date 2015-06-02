all:
	gcc ./src/*.c -o ./bin/server -std=c99 -lpthread -I./include -I./src -llua -lhiredis
	cp ./script/* ./bin -r

clean:
	rm -rf ./bin/*

