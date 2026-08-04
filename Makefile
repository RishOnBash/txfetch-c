CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = txfetch
SRC = txfetch.c
INSTALL_DIR = $(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(SRC)
	@$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

install: all
	@mkdir -p $(INSTALL_DIR)
	@mv $(TARGET) $(INSTALL_DIR)
	@echo "> Installed: $(TARGET)"

clean:
	@rm -f $(INSTALL_DIR)/$(TARGET)
	@echo "> Uninstalled: $(TARGET)"
