# Ideias

# Onde parei na leitura

subtítulo: 'The Instructions' da página: [https://austinmorlan.com/posts/chip8_emulator/](https://austinmorlan.com/posts/chip8_emulator/)


# Pendências

* configurar SDL2 e abrir uma janela -- OK
* desenhar algo na tela a partir de um frame buffer -- OK
* escalar a tela para ficar visível (com teste de cores, parece ser um escalonamento sem filtro) -- OK
* terminar de implementar instruções e fazer a rom de teste funcionar corretamente -- ok
* adicionar testes para as instruções -- ok 
* implementar um keypad viewer -- ok
* no momento o código possui defeitos que causam a terminação do processo, suponho que seja pelo uso de memória, deve haver muitos 'memory leaks'. (havia leak de duas texturas, deve haver mais algum perdido, mas por hora nada crítico) -- OK
* ajustar delay timer para decrementar em uma frequência mais próxima da original -- ok
* implementar alguma forma de renderizar texto (parcialmente feito)
* implementar um debugador simples, step a step (talvez com contador e breakpoint condicional) (parcialmente feito)
* complementar os testes das instruções com mais casos (parcialmente feito)
* implementar instruções e fluxo de input e output (parcialmente feito)
* definir como parametrizar o conceito de ciclos antes de buscar mais input
* pesquisa como saber quando deve ser feito o flush pra tela
* testar com o programa 'tetris', ainda há diversas falhas, apesar dos testes passarem
* implementar algum tipo de sistema de mapeamento de teclas do keypad
* implementar sistema de som que use o 'sound timer'
* implementar suporte a renderizar o painel monocromático com outras cores que não sejam preto e branco

# Ideias

* fazer um pequeno programar que compila para webassembly para deixar documentado como posso fazer para compilar meu interpretador para webassembly
