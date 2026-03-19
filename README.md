# 🎮 Tic Tac Toe (Machine Learning)

Este é um jogo da velha clássico desenvolvido em C que utiliza **Aprendizado por Reforço (Q-Learning)** para treinar uma inteligência artificial. O programa oferece uma interface colorida via terminal e três níveis de dificuldade baseados no volume de treinamento da IA.

---

## ✨ Features

- **Modo Player vs Player:** Jogue localmente contra um amigo.
- **Modo Player vs IA:** Enfrente uma IA que aprende jogando contra si mesma através de milhares de simulações.
- **Níveis de Dificuldade:**
  - **Easy:** Treinamento rápido (500 iterações).
  - **Normal:** Equilíbrio entre tempo e habilidade (3.000 iterações).
  - **Hard:** IA altamente competitiva (200.000 iterações).
- **Interface Imersiva:** Efeito *typewriter* (digitação) e cores ANSI para melhor feedback visual.
- **Cross-Platform:** Sistema inteligente de limpeza de tela compatível com Windows, Linux e macOS.

---

## ⬇️ Setup (Clonando o Repositório)

Se este projeto estiver hospedado no GitHub, você pode cloná-lo para sua máquina local:

```bash
git clone https://github.com/CodeMarcsX/TicTacToeAI.git
cd Tic-Tac-Toe-AI
```

---

### 🛠️ Como Compilar e Rodar
Pré-requisitos

Você precisará de um compilador C instalado, como o GCC ou Clang.
Compilação

```bash
gcc main.c -o tictactoe
```

---

### Execução

No Linux/macOS:
Bash

```bash
./tictactoe
```
No Windows:

```bash
.\tictactoe.exe
```

---

⚠️ Important: If the banner or special characters appear "bugged" in the terminal, first set the code page to UTF-8:
```bash
chcp 65001
```

---

## 🚀 Como Jogar

1. Escolha o Modo: Selecione Player vs Player ou Player vs IA no menu principal.

2. Treinamento: Se escolher a IA, aguarde alguns instantes enquanto ela "aprende" as melhores jogadas.

3. Entrada de Posições: O tabuleiro utiliza o mapeamento numérico de 1 a 9:
```bash
      1 | 2 | 3
     ---+---+---
      4 | 5 | 6
     ---+---+---
      7 | 8 | 9
```

Reiniciar: Ao final, basta digitar 'y' para uma revanche ou 'n' pra voltar ao menu iniciar.

---

## 🧠 Lógica da IA (Q-Learning)

### A IA utiliza uma Q-Table para gerenciar os 19.683 estados possíveis do jogo.

Ela aprende através de um sistema de recompensas:

- Vitória: +1.0 ponto.

- Empate: +0.3 pontos.

- Derrota: -1.0 ponto.

As jogadas são escolhidas com base nos valores aprendidos, com um fator de "ruído" (aleatoriedade) para garantir que ela explore novas estratégias em vez de repetir sempre os mesmos movimentos.

---
### Desenvolvido como um estudo de Inteligência Artificial e lógica de programação em C.
