import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("convergencia_sem_t0.csv")

plt.figure(figsize=(10, 6))

plt.plot(df['iteracao'], df['qualidade_atual'], label='Estado Atual', alpha=0.5, color='orange')
plt.plot(df['iteracao'], df['qualidade_melhor'], label='Melhor Global', linewidth=2, color='blue')

# Calcula o maior valor do estado atual. O teto do gráfico será ele ou 0.5 (o que for maior).
limite_superior = max(df['qualidade_atual'].max(), 0.5)
plt.ylim(top=limite_superior)

# (Opcional) Se quiser travar a base no zero para não distorcer a proporção visual:
plt.ylim(bottom=0)

plt.title('Convergência do Simulated Annealing (3-SAT)')
plt.xlabel('Iterações')
plt.ylabel('Qualidade (Cláusulas Falsas / Total)')
plt.legend()
plt.grid(True)
plt.tight_layout()

plt.show()