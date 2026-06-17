import pandas as pd
from scipy.stats import wilcoxon

# 1. Carrega os dados
# Substitua pelo nome correto do seu arquivo
df = pd.read_csv("resultados_pt_vs_bks_GLOBAL.csv") 

controle = "PT_MOSP"
concorrente = "BKS"

# Define a margem de não-inferioridade (ex: 1% = 0.01)
margem_tolerancia = 0.01 

resultados = []

def analisar_nao_inferioridade(nome_grupo, grupo_dados):
    dados_validos = grupo_dados[[controle, concorrente]].dropna()
    n_validos = len(dados_validos)
    
    if n_validos == 0: return None
        
    val_pt = dados_validos[controle]
    val_bks = dados_validos[concorrente]
    
    # Calcula a tolerância ABSOLUTA para cada instância baseada no BKS
    tolerancia_absoluta = val_bks * margem_tolerancia
    
    # Diferença deslocada: (PT - BKS) - Tolerância
    diferenca_deslocada = (val_pt - val_bks) - tolerancia_absoluta
    
    try:
        # Usamos alternative='less' na diferença deslocada
        stat, p_valor = wilcoxon(diferenca_deslocada, alternative='less')
    except ValueError:
        p_valor = 1.0
        
    # Critério de sucesso
    conclusao = "Não-Inferior (Aceito)" if p_valor < 0.05 else "Inconclusivo (Rejeitado)"

    return {
        "Benchmark": nome_grupo,
        "Instâncias": n_validos,
        "p_valor (Não-Inferioridade)": p_valor,
        "Conclusão": conclusao
    }

print(f"=== Teste de Não-Inferioridade (Margem: {margem_tolerancia*100}%) ===\n")

# Analisando Global
res_global = analisar_nao_inferioridade("GLOBAL (Todos)", df)
if res_global: resultados.append(res_global)

# Analisando por Grupo
for bench, grupo in df.groupby("benchmark"):
    res_grupo = analisar_nao_inferioridade(bench, grupo)
    if res_grupo: resultados.append(res_grupo)

# Converte para DataFrame
df_resultados = pd.DataFrame(resultados)

# Exibe na tela
print("Resultados gerados com sucesso:")
print(df_resultados.to_string(index=False))

# ==========================================
# SALVANDO OS RESULTADOS
# ==========================================

# 1. Salvar em CSV (pode abrir no Excel)
nome_arquivo_csv = "teste_nao_inferioridade_BKS.csv"
df_resultados.to_csv(nome_arquivo_csv, index=False)
print(f"\n[+] Tabela salva com sucesso no arquivo: {nome_arquivo_csv}")

# 2. BÔNUS: Salvar/Exibir em formato LaTeX (com notação científica formatada)
# Descomente as linhas abaixo se quiser gerar o código da tabela para o Overleaf
# print("\n=== Código LaTeX ===")
# latex_code = df_resultados.to_latex(index=False, float_format="%.2e")
# print(latex_code)