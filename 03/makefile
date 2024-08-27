TARGET = ./main.out
HDRS_DIR = \
		03
SRCS = \
		main.c \
		task03.c
.PHONY: all, build, clean, run

$(TARGET):
	gcc -I $(HDRS_DIR) $(SRCS) -o $(TARGET)

all: 
	clean build

build: 
	$(TARGET)

clean:
	rm -rf $(TARGET)
run:
	$(TARGET)