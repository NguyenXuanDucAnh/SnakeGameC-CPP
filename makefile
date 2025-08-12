all:
	echo "compiling..."
	g++ src/main.cpp -o build/run
	echo "compile done"
run:
	./build/run
