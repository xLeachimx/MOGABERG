COMPILEDFILES = Genetics.o Object.o
TEMPFOLDER = TEMP
PROGNAME = STGA

all: main

main: copy $(COMPILEDFILES)
	cd $(TEMPFOLDER) && \
	g++ $(COMPILEDFILES) main.cpp -o $(PROGNAME)
	cp $(TEMPFOLDER)/$(PROGNAME) $(PROGNAME)

copy:
	mkdir $(TEMPFOLDER)
	cp Genetics/* $(TEMPFOLDER)/
	cp Object/* $(TEMPFOLDER)/
	cp main.cpp $(TEMPFOLDER)/

Genetics.o:
	cd $(TEMPFOLDER) && \
	g++ Genetics.cpp -c

Object.o:
	cd $(TEMPFOLDER) && \
	g++ Object.cpp -c

clean:
	rm -rf $(TEMPFOLDER)