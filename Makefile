# Configurações de compilação
## usando g++ não precisa do parâmetro -lstdc++
CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++2a
LFLAGS = -lSDL2
DEF = -DDEV_CODE_ENABLED

# Configurações gerais
BUILD_FOLDER_NAME=target
SOURCE_FOLDER_NAME=src

build-folder-setup:
	@ mkdir -p $(BUILD_FOLDER_NAME)

main: build-folder-setup  $(SOURCE_FOLDER_NAME)/main.cc  
	$(CC) $(SOURCE_FOLDER_NAME)/main.cc -o $(BUILD_FOLDER_NAME)/main $(CFLAGS) $(LFLAGS) $(DEF)
run: main
	@echo "cd ./$(BUILD_FOLDER_NAME)"
	@echo ./main
	@echo "================"
	@cd ./$(BUILD_FOLDER_NAME) && ./main
	@echo "\n================\n"
