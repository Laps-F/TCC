import pandas as pd
from scipy.stats import shapiro, ttest_rel, wilcoxon
from statsmodels.stats.multitest import multipletests

caminho_arquivo = "../Results-tables/Final_Results.xlsx" 

# 1. Pula as duas primeiras linhas do Excel (Linha 1 vazia e Linha 2 do "PieceRank")
df = pd.read_excel(caminho_arquivo, sheet_name=1, skiprows=2)

# Limpa nomes de colunas removendo espaços ocultos (ex: "size " vira "size")
df.columns = df.columns.str.strip()

# Print de segurança: verifica se as colunas foram lidas certinho
print("Colunas lidas pelo Pandas:", df.columns.tolist())

# 2. Definição do controle e concorrente
algoritmo_controle = "S*"     # Melhor solução do PT-MOSP
concorrente = "Sol."          # Solução do PieceRank
resultados = []

def analisar_par(nome_grupo, grupo_dados, controle, concorrente):
    grupo_dados = grupo_dados.copy()
    
    # Blindagem contra a vírgula brasileira no Excel
    # Se a coluna for do tipo texto/object, troca vírgula por ponto antes de converter
    if grupo_dados[controle].dtype == 'object':
        grupo_dados[controle] = grupo_dados[controle].astype(str).str.replace(',', '.')
    if grupo_dados[concorrente].dtype == 'object':
        grupo_dados[concorrente] = grupo_dados[concorrente].astype(str).str.replace(',', '.')
        
    # Converte para número
    grupo_dados[controle] = pd.to_numeric(grupo_dados[controle], errors='coerce')
    grupo_dados[concorrente] = pd.to_numeric(grupo_dados[concorrente], errors='coerce')
    
    dados_validos = grupo_dados[[controle, concorrente]].dropna()
    n_validos = len(dados_validos)
    
    if n_validos == 0:
        return None
        
    val_controle = dados_validos[controle]
    val_concorrente = dados_validos[concorrente]
    
    wins = (val_controle < val_concorrente).sum()
    ties = (val_controle == val_concorrente).sum()
    losses = (val_controle > val_concorrente).sum()
    
    diferencas = val_controle - val_concorrente
    
    if ties == n_validos:
        return {
            "Concorrente": "PieceRank", "Grupo": nome_grupo, 
            "Instâncias": n_validos, "Wins": wins, "Ties": ties, "Losses": losses,
            "Teste": "N/A (Empate total)", "p_valor_bruto": 1.0
        }

    if len(set(diferencas)) > 2:
        stat_shapiro, p_shapiro = shapiro(diferencas)
    else:
        p_shapiro = 0.0 
    
    if p_shapiro > 0.05:
        stat, p_valor = ttest_rel(val_controle, val_concorrente, alternative='less')
        teste = "t-Student (Unilateral)"
    else:
        try:
            stat, p_valor = wilcoxon(val_controle, val_concorrente, zero_method='wilcox', alternative='less')
        except ValueError:
            p_valor = 1.0 
        teste = "Wilcoxon (Unilateral)"

    return {
        "Concorrente": "PieceRank", 
        "Grupo": nome_grupo, 
        "Instâncias": n_validos, 
        "Wins": wins, "Ties": ties, "Losses": losses,
        "Teste": teste, 
        "p_valor_bruto": p_valor
    }

print("\nExecutando testes estatísticos: PT-MOSP (S*) vs PieceRank (Sol.)...\n")

res_global = analisar_par("Global", df, algoritmo_controle, concorrente)
if res_global: resultados.append(res_global)

for tamanho, grupo in df.groupby("size"):
    res_grupo = analisar_par(tamanho, grupo, algoritmo_controle, concorrente)
    if res_grupo: resultados.append(res_grupo)

df_resultados = pd.DataFrame(resultados)

p_valores_brutos = df_resultados["p_valor_bruto"].values
reject, pvals_corrected, _, _ = multipletests(p_valores_brutos, alpha=0.05, method='holm')
df_resultados["p_valor_ajustado"] = pvals_corrected

colunas_exibicao = ["Concorrente", "Grupo", "Instâncias", "Wins", "Ties", "Losses", "p_valor_ajustado", "Teste"]
tabela_final = df_resultados[colunas_exibicao]

print("=== Tabela de Resultados Estatísticos ===")
print(tabela_final.to_string(index=False))

tabela_final.to_csv("tabela_estatistica_PT_vs_PieceRank.csv", index=False)