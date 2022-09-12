
# Trabalho sobre Métodos de Colisão

Trabalho 1 da disciplina de Computação Gráfica desenvolvido por Marcio Pinho e alterado por Pedro Gaspary e Lucas Cunha.

## Como compilar

Para compilar o programa, basta executar, na pasta raíz do projeto, o comando:

	$ make 
O programa será compilado automaticamente.

## Como executar

Para executar o programa, basta executar , na pasta raíz do projeto, o comando: 

	$ ./Triangulo 
O programa será executado automaticamente.

## Como limpar

Para limpar o projeto, basta executar, na pasta raíz do projeto, o comando:

	$ make clean 
O programa será limpo automaticamente.

## Comandos do Programa

### Movimento do Campo de Visão

O campo de visão pode ser movimentado, rotacionado e ter sua dimensão alterada usando os seguintes comandos:

`up` - Avança o campo de visão para "frente"
`down` - Avança o campo de visão para "trás"
`left` - Rotaciona o campo de visão no sentido anti-horário
`right` - Rotaciona o campo de visão no sentido horário
`m` - Aumenta a dimensão do campo de visão em 5% da largura da janela
`n` - Reduz a dimensão do campo de visão em 5% da largura da janela
>A dimensão não pode ser inferior a 5% nem superior a 75% da largura da janela

### Algoritmos

Os algoritmos que encontram os pontos dentro do campo de visão podem ser alterados com os seguintes comandos:

`e` - Ativa o algoritmo de envelope 
`f` - Ativa o algoritmo de força bruta
`q` - Ativa o algoritmo de quadtree
>O algoritmo de envelope e quadtree, quando ativados, desenham um envelope em volta do campo de visão

`a` - Altera o numero maximo de pontos por nodo da quadtree 
>Deve receber um numero inteiro maior que zero e inferior ao numero maximo de pontos na tela como parametro. Valores inválidos, ou muito pequenos, como (1 e 2) podem causar segmentation faults

Importante notar que não é possível desligar um algoritmo, apenas alterar o algoritmo que está em execução.  

### Desenhos na Tela

`space` - Liga/desliga o desenho dos eixos x e y na tela
`d` - Liga/desliga o desenho da quadtree na tela
`p` - Liga/desliga a pintura dos níveis da quadtree
>Os níveis são pintados nos seus eixos, com cores que se repetem a cada quatro níveis
 
 ### Resultados
 
`s` - Imprime os resultados para o algoritmo sendo executado
>Os algoritmos são executados continuamente durante a execução do programa. Porém, quando essa tecla é pressionada, é feita uma execução única do algoritmo atual para mostrar o tempo decorrido. Além disso, é mostrado:
> - A quantidade de pontos dentro do campo de visão
> - A quantidade de pontos que passaram no filtro de envelope ou quadtree, mas que estão fora do campo de visão
> - A quantidade de pontos que não passaram em nenhum filtro e estão fora do triângulo

`t` - Conta o número de frames nos próximos 3 segundos
`esc` - Termina o programa
