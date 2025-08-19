import pandas as pd
from scipy.stats import shapiro, ttest_rel, wilcoxon

# Carrega o CSV
df = pd.read_csv("results_for_statistics.csv")

resultados_normais = []
resultados_nao_normais = []
comparacoes = []

# Para cada tamanho distinto
for tamanho, grupo in df.groupby("tamanho"):

    # Calcula as diferenças entre Sol. e S*
    diferencas = grupo["Sol."] - grupo["S*"]

    # Teste Shapiro na diferença para normalidade
    stat_shapiro, p_shapiro = shapiro(diferencas)

    # Armazena grupo normal ou não normal
    if p_shapiro > 0.05:
        resultados_normais.append(tamanho)
    else:
        resultados_nao_normais.append(tamanho)

    # Faz o teste estatístico pareado adequado
    if p_shapiro > 0.05:
        stat, p_valor = ttest_rel(grupo["Sol."], grupo["S*"])
        teste = "t de Student pareado"
    else:
        stat, p_valor = wilcoxon(grupo["Sol."], grupo["S*"])
        teste = "Wilcoxon"

    comparacoes.append({
        "tamanho": tamanho,
        "teste_normalidade_p": p_shapiro,
        "teste": teste,
        "estatistica": stat,
        "p_valor": p_valor
    })

print("Grupos com distribuição normal:", resultados_normais)
print("Grupos com distribuição NÃO normal:", resultados_nao_normais)
print("Comparações estatísticas:")
for c in comparacoes:
    print(c)
