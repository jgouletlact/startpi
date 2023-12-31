# Gavin lyons 06-2021 
# Makefile to build example program for SSD1306_OLED_RPI library.
# URL: https://github.com/gavinlyonsrepo/SSD1306_OLED_RPI

# ************************************************
# User SRC directory Option Section
# Pick on example "SRC" dir path and ONE Only.
# Comment out the other's. 

SRC=src
# ************************************************

# ********************
# Options
# 1. make  
# 2. make clean
# 3. make run 
# ********************

CC=g++
LDFLAGS= -lbcm2835 -lrt -lX11
CFLAGS = -c -Wall 
MD=mkdir
OBJ=obj
SRCS = $(wildcard $(SRC)/*.cpp)
OBJS = $(patsubst $(SRC)/%.cpp,  $(OBJ)/%.o, $(SRCS))
BIN=./bin
TARGET=$(BIN)/startpi

all: clean pre-build $(TARGET) 

run:
	sudo $(TARGET)

pre-build:
	@echo '*******'
	@echo '[START!]'
	@echo 'Building: ' $(SRC)
	$(MD) -vp $(BIN)
	$(MD) -vp $(OBJ)


$(TARGET): $(OBJS) $(OBJ)
	@echo '[MAKE EXE FILE!]'
	$(CC)  $(OBJS) -o $@ $(LDFLAGS)
	@echo '[DONE!]'

$(OBJ)/%.o :  $(SRC)/%.cpp   $(OBJ) 
	@echo '[MAKE OBJECT FILE!]'
	$(CC) $(CFLAGS) $< -o  $@ 

clean:
	@echo '*******'
	@echo '[CLEANUP!]'
	rm -vf $(OBJS) $(TARGET)
	@echo '[DONE!]'
