# Nome do executável
TARGET = mainMOSP

# Pasta de código-fonte
SRC = mainMOSP.cpp

# Flags de compilação
CXX = g++
CXXFLAGS = ../PTAPI-main/include/*.h ./MOSP.cpp -std=c++20 -Wshadow -Wall -O3 -Wno-unused-result -lpthread -march=native

# Diretórios de instâncias
INST_DIR_A = ../Frinhani/Instances
INST_DIR_B = ../Instances/Challenge

RESULT_DIR_A = resultados
RESULT_DIR_B = resultados-challenge

# Parâmetros fixos
PARAMS = --TEMP_INIT 0.1 --TEMP_FIM 20 --N_REPLICAS 3 --MCL 100 --PTL 600 --TEMP_DIST 3 --TYPE_UPDATE 1 --TEMP_UPDATE 20 --THREAD_USED 3
# PARAMS = --TEMP_INIT 0.1 --TEMP_FIM 1 --N_REPLICAS 5 --MCL 500 --PTL 600 --TEMP_DIST 3 --TYPE_UPDATE 1 --TEMP_UPDATE 20 --THREAD_USED 4

# Regra principal
all: $(TARGET)

# Compilação
$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET) 

# Executa para todas as instâncias no diretório A
run-all-A: $(TARGET)
	@mkdir -p $(RESULT_DIR_A)
	find $(INST_DIR_A) -name '*.txt' | while read file; do \
		echo "==> Executando $$file..."; \
		./$(TARGET) "$$file" $(PARAMS) --READ 1; \
	done

# Executa para todas as instâncias no diretório B
run-all-B: $(TARGET)
	@mkdir -p $(RESULT_DIR_B)
	find $(INST_DIR_B) -name '*.txt' | while read file; do \
		echo "==> Executando $$file..."; \
		./$(TARGET) "$$file" $(PARAMS) --READ 0; \
	done

run-one: $(TARGET)
	@mkdir -p $(RESULT_DIR_A)
	@if [ -z "$(FILE)" ]; then \
		echo "Uso: make run-one FILE=nome_da_instancia.txt"; \
		exit 1; \
	fi
	@echo "==> Executando $(FILE)..."
	./$(TARGET) "$(INST_DIR_A)/$(FILE)" $(PARAMS)

# Limpa binários e resultados
clean:
	rm -f $(TARGET)
	rm -f $(RESULT_DIR_A)/*.txt
	rm -f $(RESULT_DIR_B)/*.txt
	

.PHONY: all run-all-A run-all-B run-one clean
