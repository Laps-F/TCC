import os
import re
import pandas as pd
from collections import defaultdict
from statistics import mean, stdev
import gspread
from gspread_dataframe import set_with_dataframe
from google.oauth2.service_account import Credentials

PASTA_RESULTADOS = "resultados-base"
NOME_PLANILHA_GOOGLE = "Resultados base"
ARQUIVO_CREDENCIAIS = "shaped-buttress-383520-602982384f84.json"
# ARQUIVO_CREDENCIAIS = "arctic-ocean-459800-g7-f4353aaaa5bb.json"

REGEX_INFO = re.compile(r'Random-(\d+)-(\d+)-(\d+)-(\d+)(?:\((\d+)\))?_res\.txt')

import os
from collections import defaultdict
from statistics import mean

def carregar_dados(pasta_resultados):
    dados, dados_media, dados_min = [], [], []
    instancias_agrupadas = defaultdict(list)
    conjuntos_por_tamanho = defaultdict(list)
    conjuntos = {}

    for nome_arquivo in os.listdir(pasta_resultados):
        if not nome_arquivo.endswith(".txt"):
            continue

        caminho = os.path.join(pasta_resultados, nome_arquivo)

        with open(caminho, "r") as f:
            linhas = [linha.strip() for linha in f if linha.strip()]

        if len(linhas) < 4:
            print(f"Aviso: arquivo {nome_arquivo} parece incompleto")
            continue

        nome_instancia = linhas[0]
        tempo = float(linhas[1])
        valor_solucao = float(linhas[2])
        # trocas = int(linhas[3])
        max_pecas_padrao = int(linhas[3])
        solucao = ' '.join(linhas[4:])

        match = REGEX_INFO.match(nome_arquivo)
        if match:
            linhas_, colunas_, conjunto, instancia, execucao = match.groups()
            tamanho = f"{linhas_}x{colunas_}"
            conjunto_nome = f"conjunto {conjunto}"
            instancia = f"{instancia}"
            chave_execucao = (tamanho, conjunto_nome, instancia)
        else:
            tamanho = "Desconhecido"
            conjunto_nome = "Desconhecido"
            instancia = "Desconhecida"
            execucao = "Desconhecida"
            chave_execucao = (tamanho, conjunto_nome, instancia)

        entrada = {
            "arquivo": nome_arquivo,
            "tamanho": tamanho,
            "conjunto": conjunto_nome,
            "instancia": instancia,
            "execucao": execucao,
            "tempo (ms)": tempo,
            "valor solução": valor_solucao,
            # "trocas": trocas,
            "max_pecas_padrao": max_pecas_padrao,
            "solução": solucao,
        }

        dados.append(entrada)
        instancias_agrupadas[chave_execucao].append(entrada)

    # Calcular médias por instância
    for (tamanho, conjunto, instancia), entradas in instancias_agrupadas.items():
        entrada_media = {
            "tamanho": tamanho,
            "conjunto": conjunto,
            "instancia": instancia,
            "tempo (ms)": mean(e["tempo (ms)"] for e in entradas),
            "valor solução": mean(e["valor solução"] for e in entradas),
            "desvio valor solução": stdev([e["valor solução"] for e in entradas]) if len([e["valor solução"] for e in entradas]) > 1 else 0.0,
            # "trocas": mean(e["trocas"] for e in entradas),
            "max_pecas_padrao": mean(e["max_pecas_padrao"] for e in entradas),
        }

        dados_media.append(entrada_media)

        chave_conjunto = (conjunto, tamanho)
        conjuntos_por_tamanho[chave_conjunto].append(entrada_media)
        conjuntos.setdefault(conjunto, []).append(entrada_media)

    for (tamanho, conjunto, instancia), entradas in instancias_agrupadas.items():
        entrada_melhor = min(entradas, key=lambda e: e["valor solução"])
        
        entrada_min = {
            "tamanho": tamanho,
            "conjunto": conjunto,
            "instancia": instancia,
            "tempo (ms)": entrada_melhor["tempo (ms)"],
            "valor solução": entrada_melhor["valor solução"],
            # "trocas": entrada_melhor["trocas"],
            "max_pecas_padrao": mean(e["max_pecas_padrao"] for e in entradas),
        }

        dados_min.append(entrada_min)

    return dados, dados_media, dados_min, conjuntos_por_tamanho

def calcular_estatisticas(conjuntos_por_tamanho, dados_min):
    estat_conjuntos = []

    for (conjunto_nome, tamanho), lista in conjuntos_por_tamanho.items():
        tempos = [d["tempo (ms)"] for d in lista]
        valores = [d["valor solução"] for d in lista]

        desvios_instancia = [d.get("desvio valor solução", 0.0) for d in lista]
        media_desvios_instancia = mean(desvios_instancia) if desvios_instancia else 0.0

        # Filtrar dados_min correspondentes ao conjunto e tamanho
        candidatos = [
            d for d in dados_min
            if d["conjunto"] == conjunto_nome and d["tamanho"] == tamanho
        ]

        if candidatos:
            media_valor_min = mean(d["valor solução"] for d in candidatos)
            media_tempo_min = mean(d["tempo (ms)"] for d in candidatos)
        else:
            media_valor_min = None
            media_tempo_min = None

        estat_conjuntos.append({
            "conjunto": conjunto_nome,
            "tamanho": tamanho,
            "n_instâncias": len(lista),
            "média tempo (ms)": mean(tempos),
            "média valor solução": mean(valores),
            "média tempo melhor solução (ms)": media_tempo_min,
            "média melhor valor solução": media_valor_min,
             "desvio valor solução": media_desvios_instancia,
        })

    return estat_conjuntos

def exportar_para_google_sheets(df_dict, nome_planilha, cred_path):
    credenciais = Credentials.from_service_account_file(
        cred_path,
        scopes=[
            "https://www.googleapis.com/auth/spreadsheets",
            "https://www.googleapis.com/auth/drive"
        ]
    )
    cliente = gspread.authorize(credenciais)

    try:
        planilha = cliente.open(nome_planilha)
    except gspread.SpreadsheetNotFound:
        planilha = cliente.create(nome_planilha)
        planilha.share(credenciais.service_account_email, perm_type='user', role='writer')

    for nome_aba, dataframe in df_dict.items():
        try:
            aba = planilha.worksheet(nome_aba)
            planilha.del_worksheet(aba)
        except gspread.exceptions.WorksheetNotFound:
            pass
        aba = planilha.add_worksheet(title=nome_aba, rows="1000", cols="20")
        set_with_dataframe(aba, dataframe)

def montar_estatisticas_pageRank():
    estatistica = [
                {"conjunto": "conjunto 2", "tamanho": "400x400", "media temppo (ms)": 0.03, "media valor solução": 82.60},
                {"conjunto": "conjunto 4", "tamanho": "400x400", "media temppo (ms)": 0.04, "media valor solução": 177.60},
                {"conjunto": "conjunto 6", "tamanho": "400x400", "media temppo (ms)": 0.05, "media valor solução": 254.80},
                {"conjunto": "conjunto 8", "tamanho": "400x400", "media temppo (ms)": 0.06, "media valor solução": 302.80},
                {"conjunto": "conjunto 10", "tamanho": "400x400", "media temppo (ms)": 0.07, "media valor solução": 331.40},
                {"conjunto": "conjunto 14", "tamanho": "400x400", "media temppo (ms)": 0.09, "media valor solução": 361.70},
                {"conjunto": "conjunto 18", "tamanho": "400x400", "media temppo (ms)": 0.10, "media valor solução": 375.60},
                {"conjunto": "conjunto 20", "tamanho": "400x400", "media temppo (ms)": 0.10, "media valor solução": 380.40},
                {"conjunto": "conjunto 24", "tamanho": "400x400", "media temppo (ms)": 0.11, "media valor solução": 386.70},
                {"conjunto": "conjunto 28", "tamanho": "400x400", "media temppo (ms)": 0.13, "media valor solução": 390.10},
                {"conjunto": "conjunto 30", "tamanho": "400x400", "media temppo (ms)": 0.14, "media valor solução": 391.70},
                {"conjunto": "conjunto 34", "tamanho": "400x400", "media temppo (ms)": 0.16, "media valor solução": 393.90},
                {"conjunto": "conjunto 2", "tamanho": "600x600", "media temppo (ms)": 0.05, "media valor solução": 125.40},
                {"conjunto": "conjunto 4", "tamanho": "600x600", "media temppo (ms)": 0.09, "media valor solução": 266.40},
                {"conjunto": "conjunto 6", "tamanho": "600x600", "media temppo (ms)": 0.12, "media valor solução": 379.20},
                {"conjunto": "conjunto 8", "tamanho": "600x600", "media temppo (ms)": 0.14, "media valor solução": 452.60},
                {"conjunto": "conjunto 10", "tamanho": "600x600", "media temppo (ms)": 0.15, "media valor solução": 494.50},
                {"conjunto": "conjunto 14", "tamanho": "600x600", "media temppo (ms)": 0.17, "media valor solução": 540.10},
                {"conjunto": "conjunto 18", "tamanho": "600x600", "media temppo (ms)": 0.19, "media valor solução": 562.00},
                {"conjunto": "conjunto 20", "tamanho": "600x600", "media temppo (ms)": 0.20, "media valor solução": 569.90},
                {"conjunto": "conjunto 24", "tamanho": "600x600", "media temppo (ms)": 0.23, "media valor solução": 577.30},
                {"conjunto": "conjunto 28", "tamanho": "600x600", "media temppo (ms)": 0.26, "media valor solução": 584.20},
                {"conjunto": "conjunto 30", "tamanho": "600x600", "media temppo (ms)": 0.27, "media valor solução": 586.60},
                {"conjunto": "conjunto 34", "tamanho": "600x600", "media temppo (ms)": 0.30, "media valor solução": 590.00},
                {"conjunto": "conjunto 38", "tamanho": "600x600", "media temppo (ms)": 0.34, "media valor solução": 591.60},
                {"conjunto": "conjunto 40", "tamanho": "600x600", "media temppo (ms)": 0.36, "media valor solução": 592.90},
                {"conjunto": "conjunto 2", "tamanho": "800x800", "media temppo (ms)": 0.09, "media valor solução": 161.50},
                {"conjunto": "conjunto 4", "tamanho": "800x800", "media temppo (ms)": 0.15, "media valor solução": 354.70},
                {"conjunto": "conjunto 6", "tamanho": "800x800", "media temppo (ms)": 0.20, "media valor solução": 504.20},
                {"conjunto": "conjunto 8", "tamanho": "800x800", "media temppo (ms)": 0.22, "media valor solução": 596.60},
                {"conjunto": "conjunto 10", "tamanho": "800x800", "media temppo (ms)": 0.25, "media valor solução": 658.50},
                {"conjunto": "conjunto 14", "tamanho": "800x800", "media temppo (ms)": 0.28, "media valor solução": 720.50},
                {"conjunto": "conjunto 18", "tamanho": "800x800", "media temppo (ms)": 0.32, "media valor solução": 749.80},
                {"conjunto": "conjunto 20", "tamanho": "800x800", "media temppo (ms)": 0.34, "media valor solução": 758.40},
                {"conjunto": "conjunto 24", "tamanho": "800x800", "media temppo (ms)": 0.37, "media valor solução": 770.60},
                {"conjunto": "conjunto 28", "tamanho": "800x800", "media temppo (ms)": 0.42, "media valor solução": 778.00},
                {"conjunto": "conjunto 30", "tamanho": "800x800", "media temppo (ms)": 0.44, "media valor solução": 781.20},
                {"conjunto": "conjunto 34", "tamanho": "800x800", "media temppo (ms)": 0.49, "media valor solução": 785.30},
                {"conjunto": "conjunto 38", "tamanho": "800x800", "media temppo (ms)": 0.53, "media valor solução": 788.90},
                {"conjunto": "conjunto 40", "tamanho": "800x800", "media temppo (ms)": 0.57, "media valor solução": 789.80},
                {"conjunto": "conjunto 44", "tamanho": "800x800", "media temppo (ms)": 0.62, "media valor solução": 791.90},
                {"conjunto": "conjunto 48", "tamanho": "800x800", "media temppo (ms)": 0.69, "media valor solução": 793.20},
                {"conjunto": "conjunto 50", "tamanho": "800x800", "media temppo (ms)": 0.74, "media valor solução": 793.80},
                {"conjunto": "conjunto 2", "tamanho": "1000x1000", "media temppo (ms)": 0.13, "media valor solução": 200.80},
                {"conjunto": "conjunto 4", "tamanho": "1000x1000", "media temppo (ms)": 0.22, "media valor solução": 439.30},
                {"conjunto": "conjunto 6", "tamanho": "1000x1000", "media temppo (ms)": 0.29, "media valor solução": 630.90},
                {"conjunto": "conjunto 8", "tamanho": "1000x1000", "media temppo (ms)": 0.34, "media valor solução": 748.60},
                {"conjunto": "conjunto 10", "tamanho": "1000x1000", "media temppo (ms)": 0.37, "media valor solução": 821.20},
                {"conjunto": "conjunto 14", "tamanho": "1000x1000", "media temppo (ms)": 0.42, "media valor solução": 899.10},
                {"conjunto": "conjunto 18", "tamanho": "1000x1000", "media temppo (ms)": 0.48, "media valor solução": 935.10},
                {"conjunto": "conjunto 20", "tamanho": "1000x1000", "media temppo (ms)": 0.50, "media valor solução": 945.90},
                {"conjunto": "conjunto 24", "tamanho": "1000x1000", "media temppo (ms)": 0.55, "media valor solução": 962.90},
                {"conjunto": "conjunto 28", "tamanho": "1000x1000", "media temppo (ms)": 0.62, "media valor solução": 972.10},
                {"conjunto": "conjunto 30", "tamanho": "1000x1000", "media temppo (ms)": 0.65, "media valor solução": 975.30},
                {"conjunto": "conjunto 34", "tamanho": "1000x1000", "media temppo (ms)": 0.71, "media valor solução": 981.60},
                {"conjunto": "conjunto 38", "tamanho": "1000x1000", "media temppo (ms)": 0.77, "media valor solução": 985.00},
                {"conjunto": "conjunto 40", "tamanho": "1000x1000", "media temppo (ms)": 0.80, "media valor solução": 986.70},
                {"conjunto": "conjunto 44", "tamanho": "1000x1000", "media temppo (ms)": 0.88, "media valor solução": 989.20},
                {"conjunto": "conjunto 48", "tamanho": "1000x1000", "media temppo (ms)": 0.98, "media valor solução": 991.40},
                {"conjunto": "conjunto 50", "tamanho": "1000x1000", "media temppo (ms)": 1.04, "media valor solução": 991.60},
                {"conjunto": "conjunto 54", "tamanho": "1000x1000", "media temppo (ms)": 1.14, "media valor solução": 992.90}
            ]
    
    return pd.DataFrame(estatistica)

def sort_dataframe(dataframe):
    dataframe[["n_linhas", "n_colunas"]] = dataframe["tamanho"].str.extract(r"(\d+)x(\d+)").astype(int)
    dataframe["conjunto_num"] = dataframe["conjunto"].str.extract(r"(\d+)").astype(int)

    if "instancia" in dataframe.columns:
        dataframe["instancia_num"] = dataframe["instancia"].astype(int)
        if "execucao" in dataframe.columns:
            dataframe["execucao_num"] = dataframe["execucao"].astype(int)
            sort_cols = ["n_linhas", "n_colunas", "conjunto_num", "instancia_num", "execucao_num"]
        else:
            sort_cols = ["n_linhas", "n_colunas", "conjunto_num", "instancia_num"]
    else:
        sort_cols = ["n_linhas", "n_colunas", "conjunto_num"]

    dataframe = dataframe.sort_values(sort_cols)

    dataframe = dataframe.drop(columns=sort_cols)

    return dataframe

def main():
    dados, dados_media, dados_min, conjuntos = carregar_dados(PASTA_RESULTADOS)
    estat_conjuntos = calcular_estatisticas(conjuntos, dados_min)
    
    df = pd.DataFrame(dados)
    df = sort_dataframe(df)

    df_media = pd.DataFrame(dados_media)
    df_media = sort_dataframe(df_media)

    df_melhores = pd.DataFrame(dados_min)
    df_melhores = sort_dataframe(df_melhores)

    df_conjuntos = pd.DataFrame(estat_conjuntos)
    df_conjuntos = sort_dataframe(df_conjuntos)

    df_pageRank_conjuntos = montar_estatisticas_pageRank()

    abas = {
        "Resultados Completos": df,
        "Resumo Por Istância (Médias)": df_media,
        "Resumo Por Instância (Melhores)": df_melhores,
        "Resumo por Conjunto ": df_conjuntos,
        "Resumo PageRank por Conjunto": df_pageRank_conjuntos,
    }

    exportar_para_google_sheets(abas, NOME_PLANILHA_GOOGLE, ARQUIVO_CREDENCIAIS)
    print(f"✅ Dados exportados com sucesso para a planilha do Google Sheets: {NOME_PLANILHA_GOOGLE}")

main()
