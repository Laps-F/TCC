import os
import re
import pandas as pd
from statistics import mean

PASTA_RESULTADOS = "resultados"
ARQUIVO_EXCEL = "tabela_resultados.xlsx"

dados = []
conjuntos = {}
agrupamento_por_tamanho = {}

# Regex para extrair info do nome do arquivo
regex_info = re.compile(r'Random-(\d+)-(\d+)-(\d+)-(\d+)_res\.txt')

for nome_arquivo in os.listdir(PASTA_RESULTADOS):
    if not nome_arquivo.endswith(".txt"):
        continue

    caminho = os.path.join(PASTA_RESULTADOS, nome_arquivo)

    with open(caminho, "r") as f:
        linhas = [linha.strip() for linha in f.readlines() if linha.strip()]

    if len(linhas) < 3:
        print(f"Aviso: arquivo {nome_arquivo} parece incompleto")
        continue

    nome_instancia = linhas[0]
    tempo = float(linhas[1])
    valor_solucao = float(linhas[2])
    solucao = ' '.join(linhas[3:])

    match = regex_info.match(nome_arquivo)
    if match:
        linhas_, colunas_, conjunto, indice = match.groups()
        tamanho = f"{linhas_}x{colunas_}"
        conjunto_nome = f"conjunto {conjunto}"
    else:
        tamanho = "Desconhecido"
        conjunto_nome = "Desconhecido"

    entrada = {
        "arquivo": nome_arquivo,
        "tamanho": tamanho,
        "conjunto": conjunto_nome,
        "tempo (ms)": tempo,
        "valor solução": valor_solucao,
        "solução": solucao
    }

    dados.append(entrada)

    if conjunto_nome not in conjuntos:
        conjuntos[conjunto_nome] = []
    conjuntos[conjunto_nome].append(entrada)

    if tamanho not in agrupamento_por_tamanho:
        agrupamento_por_tamanho[tamanho] = []
    agrupamento_por_tamanho[tamanho].append(entrada)

# DataFrame geral
df = pd.DataFrame(dados)

# Estatísticas por conjunto
estat_conjuntos = []

for nome_conjunto, lista in conjuntos.items():
    tamanho = lista[0]["tamanho"]
    tempos = [d["tempo (ms)"] for d in lista]
    valores = [d["valor solução"] for d in lista]
    estat_conjuntos.append({
        "conjunto": nome_conjunto,
        "tamanho": tamanho,
        "n_instâncias": len(lista),
        "média tempo (ms)": mean(tempos),
        "média valor solução": mean(valores)
    })

# Estatísticas gerais por tamanho
estat_geral_por_tamanho = []

for tamanho, lista in agrupamento_por_tamanho.items():
    tempos = [d["tempo (ms)"] for d in lista]
    valores = [d["valor solução"] for d in lista]
    estat_geral_por_tamanho.append({
        "tamanho": tamanho,
        "n_instâncias": len(lista),
        "média tempo (ms)": mean(tempos),
        "média valor solução": mean(valores)
    })

# Resumo geral de todas as instâncias
todos_tempos = [d["tempo (ms)"] for d in dados]
todos_valores = [d["valor solução"] for d in dados]
estat_geral = {
    "conjunto": "GERAL",
    "tamanho": "Todos os tamanhos",
    "n_instâncias": len(dados),
    "média tempo (ms)": mean(todos_tempos),
    "média valor solução": mean(todos_valores)
}

# Junta tudo
df_resumo_conjuntos = pd.DataFrame(estat_conjuntos).sort_values(["tamanho", "conjunto"])
df_resumo_tamanhos = pd.DataFrame(estat_geral_por_tamanho).sort_values("tamanho")

# Adiciona o resumo geral de todos os tamanhos ao final da aba 'Resumo Geral por Tamanho'
df_resumo_tamanhos.loc[len(df_resumo_tamanhos)] = pd.Series({
    "tamanho": "GERAL",
    "n_instâncias": len(dados),
    "média tempo (ms)": mean(todos_tempos),
    "média valor solução": mean(todos_valores)
})

# Exporta para Excel
with pd.ExcelWriter(ARQUIVO_EXCEL, engine='openpyxl') as writer:
    df.to_excel(writer, index=False, sheet_name="Resultados")
    df_resumo_conjuntos.to_excel(writer, index=False, sheet_name="Resumo por Conjunto")
    df_resumo_tamanhos.to_excel(writer, index=False, sheet_name="Resumo Geral por Tamanho")
    # Adiciona o resumo geral no final da aba 'Resumo Geral por Tamanho'
    df_resumo_tamanhos.loc[len(df_resumo_tamanhos)] = pd.Series(estat_geral)

print(f"✅ Excel '{ARQUIVO_EXCEL}' gerado com abas 'Resultados', 'Resumo por Conjunto' e 'Resumo Geral por Tamanho'.")
