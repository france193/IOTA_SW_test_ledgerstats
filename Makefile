target:
	clear
	gcc -g -o ledgerstats -Wall main.c functions.c graphLibrary.c queueLibrary.c

clean:
	rm main

run:
	./ledgerstats database.txt
