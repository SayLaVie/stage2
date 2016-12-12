#MakeFile for stage2 - Michael McCarver and Eric Sunday
CFLAGS = -g -Wall

CC = g++

TARGET = stage2

SRCS =  stage2actionroutines.cpp stage2emissions.cpp stage2frommain.cpp stage2lexicalscanner.cpp stage2main.cpp stage2newproductions.cpp stage2productions.cpp stage2tempmanip.cpp stage2translationgrammaractions.cpp stage2finalproductions.cpp stage2finalemissions.cpp

OBJS = $(SRCS:.c=.o)

MAIN = stage2

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

clean:
	rm -f *.o core *~ $(MAIN)
