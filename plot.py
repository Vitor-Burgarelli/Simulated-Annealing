import pandas as pd
import matplotlib.pyplot as plt

# Lê os dados gerados pelo código em C
# LEMBRAR DE MUDAR PARA O ARQUIVO QUE ESTIVER LENDO NA HORA
df = pd.read_csv("convergencia_com_t0.csv")

plt.figure(figsize=(10, 6))

# Plota a qualidade do estado atual (útil para ver o "ruído" do algoritmo de Metropolis)
plt.plot(df['iteracao'], df['qualidade_atual'], label='Estado Atual', alpha=0.5, color='orange')

# Plota a evolução da melhor solução global
plt.plot(df['iteracao'], df['qualidade_melhor'], label='Melhor Global', linewidth=2, color='blue')

plt.title('Convergência do Simulated Annealing (3-SAT)')
plt.xlabel('Iterações')
plt.ylabel('Qualidade (Cláusulas Falsas / Total)')
plt.legend()
plt.grid(True)
plt.tight_layout()

# Mostra o gráfico no ecrã
plt.show()