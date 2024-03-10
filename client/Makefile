all:
	cmake -S. -B build
	cmake --build build
	cp build/client .
	cp -r client client1/
	cp -r client client2/
clean:
	rm -r build client1/me.info client2/me.info
	rm -r client1/client client2/client
