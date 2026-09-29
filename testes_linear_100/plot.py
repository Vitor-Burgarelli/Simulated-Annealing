import pandas as pd
import matplotlib.pyplot as plt

# Le o CSV agregado gerado pelo annealing_batch.c
df = pd.read_csv("resultados.csv")

# --- Tabela resumo por valor de t ---
resumo = df.groupby("t").agg(
    taxa_sucesso=("sucesso", "mean"),
    media_iteracao_convergencia=("iteracao_convergencia", lambda x: x[x != -1].mean()),
    mediana_iteracao_convergencia=("iteracao_convergencia", lambda x: x[x != -1].median()),
    qualidade_final_media=("qualidade_final", "mean"),
    n_runs=("run", "count"),
).reset_index()

resumo["taxa_sucesso"] = resumo["taxa_sucesso"] * 100  # em %

print(resumo.to_string(index=False))
resumo.to_csv("resumo_por_t.csv", index=False)

# --- Grafico: taxa de sucesso por t ---
plt.figure(figsize=(8, 5))
plt.bar(resumo["t"].astype(str), resumo["taxa_sucesso"], color="steelblue")
plt.title("Taxa de sucesso por valor de t")
plt.xlabel("t (fator de resfriamento)")
plt.ylabel("Taxa de sucesso (%)")
plt.ylim(0, 105)
plt.grid(axis="y")
plt.tight_layout()
plt.savefig("taxa_sucesso_por_t.png")

# --- Grafico: media de iteracoes ate convergir, por t ---
plt.figure(figsize=(8, 5))
plt.bar(resumo["t"].astype(str), resumo["media_iteracao_convergencia"], color="darkorange")
plt.title("Média de iterações até convergir, por valor de t")
plt.xlabel("t (fator de resfriamento)")
plt.ylabel("Iterações até qualidade = 0 (média)")
plt.grid(axis="y")
plt.tight_layout()
plt.savefig("iteracoes_medias_por_t.png")

# --- Grafico: sobrepondo as curvas de convergencia (1 run ilustrativa por t) ---
plt.figure(figsize=(10, 6))
for t in [1, 2, 3, 4, 5]:
    try:
        curva = pd.read_csv(f"convergencia_t{t}.csv")
        plt.plot(curva["iteracao"], curva["qualidade_melhor"], label=f"t={t}")
    except FileNotFoundError:
        pass

plt.title("Comparação da curva 'Melhor Global' por valor de t")
plt.xlabel("Iterações")
plt.ylabel("Qualidade (Cláusulas Falsas / Total)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("comparacao_melhor_global.png")

print("\nGráficos salvos: taxa_sucesso_por_t.png, iteracoes_medias_por_t.png, comparacao_melhor_global.png")
print("Resumo salvo em: resumo_por_t.csv")