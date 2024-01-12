all: qoiconv_orig qoiconv_fu

clean:
	$(RM) -f qoiconv_orig qoiconv_fu QOI-stdio.o QOI.o

qoiconv_orig: .dependencies/qoi/qoiconv.c
	$(CC) -std=c99 -Wall -Wextra -pedantic-errors -O3 -I .dependencies/stb -o $@ $<

QOI-stdio.o: .dependencies/qoi-fu/QOI-stdio.c
	$(CC) -c -std=c99 -Wall -Wextra -pedantic-errors -O3 -I .dependencies/qoi-fu -I .dependencies/qoi-fu/transpiled -o $@ $<

QOI.o: .dependencies/qoi-fu/transpiled/QOI.c
	$(CC) -c -std=c99 -Wall -Wextra -pedantic-errors -O3 -I .dependencies/qoi-fu -I .dependencies/qoi-fu/transpiled -o $@ $<

qoiconv_fu: qoiconv.cpp QOI-stdio.o QOI.o
	$(CXX) -std=c++2a -Wall -Wextra -pedantic-errors -O3 -I .dependencies/stb -I .dependencies/qoi-fu -I .dependencies/qoi-fu/transpiled -o $@ $^

.PHONY: all clean
