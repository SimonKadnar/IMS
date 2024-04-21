radiator: radiator.cpp
	g++ -o radiator radiator.cpp -lsimlib -lm

clean: radiator.o radiator
	rm -rf $^
run:radiator
	./radiator
