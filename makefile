CC		= gcc
CFLAGS	= -Wextra -Wall -O2 -std=c11
LDLIBS	= -lcsfml-graphics -lcsfml-window -lcsfml-system -lcsfml-audio -lm
FILES	= $(wildcard Sources/*.c)

game: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o game $(LDLIBS)

clean: game
	rm game