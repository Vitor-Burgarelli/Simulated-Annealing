import matplotlib.pyplot as plt
import pandas as pd

# =====================================================================
# IMAGEM 1: GRÁFICO DE CONVERGÊNCIA APENAS DA ÚLTIMA RODAGEM
# =====================================================================
df_conv = pd.read_csv("convergencia.csv")

# Obtém o identificador da última rodagem presente no ficheiro
ultima_rodagem = df_conv["rodagem"].max()
df_ultima = df_conv[df_conv["rodagem"] == ultima_rodagem]

fig_conv, ax_conv = plt.subplots(figsize=(10, 6))

# Curva de exploração (estados aceites ao longo do processo)
ax_conv.plot(
    df_ultima["iteracao"],
    df_ultima["qualidade_atual"],
    label="Estado Atual",
    color="orange",
    alpha=0.45,
    linewidth=0.9,
)

# Curva da melhor solução global registada
ax_conv.plot(
    df_ultima["iteracao"],
    df_ultima["qualidade_melhor"],
    label="Melhor Global",
    color="blue",
    linewidth=1.8,
)

ax_conv.set_title(
    f"Convergência do Simulated Annealing (Última Rodagem: {ultima_rodagem})",
    fontsize=13,
    fontweight="bold",
)
ax_conv.set_xlabel("Iterações", fontsize=11)
ax_conv.set_ylabel("Qualidade (Cláusulas Falsas / Total)", fontsize=11)
ax_conv.grid(True, linestyle="--", alpha=0.6)
ax_conv.legend(loc="upper right", fontsize=10)
fig_conv.tight_layout()

# =====================================================================
# IMAGEM 2: BOX-PLOT COM A DISTRIBUIÇÃO DOS RESULTADOS FINAIS
# =====================================================================
df_finais = pd.read_csv("resultados_finais.csv")

fig_box, ax_box = plt.subplots(figsize=(6, 7))

# Box-plot das soluções finais obtidas em todas as rodagens reunidas
bp = ax_box.boxplot(
    df_finais["qualidade_final"],
    patch_artist=True,
    showmeans=True,
    meanprops={
        "marker": "^",
        "markerfacecolor": "red",
        "markeredgecolor": "red",
        "markersize": 8,
    },
)

# Adiciona a dispersão dos pontos individuais de cada rodagem
ax_box.scatter(
    [1] * len(df_finais),
    df_finais["qualidade_final"],
    color="black",
    alpha=0.6,
    zorder=3,
    label="Execuções Individuais",
)

# Estilização visual da caixa
for patch in bp["boxes"]:
  patch.set_facecolor("#5B9BD5")
  patch.set_alpha(0.65)

ax_box.set_xticks([1])
ax_box.set_xticklabels([f"Execuções (N = {len(df_finais)})"], fontsize=11)
ax_box.set_title(
    "Box-Plot dos Resultados das Execuções", fontsize=13, fontweight="bold"
)
ax_box.set_ylabel("Qualidade Final (Cláusulas Falsas / Total)", fontsize=11)
ax_box.grid(True, linestyle="--", alpha=0.5)

# Elementos auxiliares para a legenda
ax_box.plot(
    [],
    [],
    marker="^",
    color="red",
    linestyle="None",
    label="Média",
    markersize=8,
)
ax_box.plot(
    [],
    [],
    color="orange",
    linewidth=2,
    label="Mediana",
)
ax_box.legend(loc="upper right")

fig_box.tight_layout()

# Apresenta as duas janelas independentes
plt.show()