# Nome do executável
TARGET = mainMOSP

# Pasta de código-fonte
SRC = mainMOSP.cpp

# Flags de compilação
CXX = g++
CXXFLAGS = ../PTAPI-main/include/*.h ./MOSP.cpp -std=c++20 -Wshadow -Wall -O3 -Wno-unused-result -lpthread -march=native

# Diretórios
INST_DIR = ../Frinhani/Instances
RESULT_DIR = resultados

# Parâmetros fixos
PARAMS = --TEMP_INIT 0.1 --TEMP_FIM 1 --N_REPLICAS 2 --MCL 50 --PTL 60 --TEMP_DIST 3 --TYPE_UPDATE 1 --TEMP_UPDATE 20 --THREAD_USED 2

# Lista de instâncias (todos os .txt na pasta)
INSTANCES = $(wildcard $(INST_DIR)/*.txt)

# Regra principal
all: $(TARGET)

# Compilação
$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET) 

# Executa para todas as instâncias
run-all: $(TARGET)
	@mkdir -p $(RESULT_DIR)
	find $(INST_DIR) -name '*.txt' | while read file; do \
		echo "==> Executando $$file..."; \
		./$(TARGET) "$$file" $(PARAMS) || echo "Erro ao processar $$file"; \
	done


# Limpa binários e resultados
clean:
	rm -f $(TARGET)
	rm -f $(RESULT_DIR)/*.txt

.PHONY: all run-all clean