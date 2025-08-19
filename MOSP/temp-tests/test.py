import os
import csv

# Caminho da pasta com os arquivos txt
pasta = 'test_results_2'

# Lista para armazenar os dados extraídos
dados = []

# Itera sobre os arquivos da pasta
for nome_arquivo in os.listdir(pasta):
    if nome_arquivo.endswith('.txt'):
        caminho_arquivo = os.path.join(pasta, nome_arquivo)
        with open(caminho_arquivo, 'r') as f:
            info = {'Arquivo': nome_arquivo}
            for linha in f:
                chave, valor = linha.strip().split(':')
                info[chave.strip()] = int(valor.strip())  # converte para int para ordenar corretamente
            dados.append(info)

# Ordena os dados por MCL, PTL, TempUpdate, nesta ordem
dados.sort(key=lambda x: (x['MCL'], x['PTL'], x['TempUpdate']))

# Define as colunas do CSV
colunas = ['Arquivo', 'MCL', 'PTL', 'TempUpdate', 'Solução', 'Tempo']

# Caminho do arquivo CSV de saída
saida_csv = os.path.join('test_results(high_temp).csv')

# Escreve os dados no CSV
with open(saida_csv, 'w', newline='') as f:
    writer = csv.DictWriter(f, fieldnames=colunas)
    writer.writeheader()
    writer.writerows(dados)

print(f'CSV gerado com sucesso em: {saida_csv}')

