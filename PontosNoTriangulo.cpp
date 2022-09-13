// **********************************************************************
// PUCRS/Escola Politecnica
// COMPUTAÇÃO GRÁFICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho - pinho@pucrs.br
//
// Alterado para o trabalho da disciplina por:
// Pedro da Cunha Gaspary - 21101429 e Lucas Marchesan Cunha - 21101060
// **********************************************************************
#include <chrono>
#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;

#ifdef WIN32
#include <glut.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "ListaDeCoresRGB.h"
#include "Poligono.h"
#include "Ponto.h"
#include "Temporizador.h"
Temporizador T;
double AccumDeltaT = 0;

// Variaveis que controlam o triangulo do campo de visao
Poligono PontosDoCenario, CampoDeVisao, TrianguloBase, Envelope;
size_t pontosInternos, pontosFalsos, maxPontosNodo = 20;
float AnguloDoCampoDeVisao = 0.0;
float DimensaoDoCampoDeVisao = 0.25;

// Limites logicos da area de desenho
Ponto Minimo, Maximo, Tamanho, Meio;
Ponto PosicaoDoCampoDeVisao, vetoresTriangulo[3], PontoClicado;
unsigned long int QTD_PONTOS;
size_t qtd_colisoes, qtd_forcaBruta;
bool desenhaEixos = true;
bool FoiClicado = false;

// Variaveis que controlam as propriedades dos algoritmos
bool bool_forcaBruta = true;
bool bool_Envelope = false;
bool bool_QuadTree = false;
bool desenhaQuadTree = false;
bool pinta_QuadTree = false;

typedef struct nodo_quadtree {
    Ponto Min, Max;                  // Limites do nodo
    bool folha;                      // Se o nodo estiver cheio, ele deve ser dividido
    struct nodo_quadtree *filho[4];  // 4 nodos-filhos
    Poligono pontos;                 // pontos dentro do nodo
} QUADTREE;

QUADTREE *tree;
// **********************************************************************
// GeraPontos(unsigned long int qtd, Ponto Min, Ponto Max)
//      Metodo que gera pontos aleatorios no intervalo [Min..Max]
// **********************************************************************
void GeraPontos(unsigned long int qtd, Ponto Min, Ponto Max) {
    time_t t;
    Ponto Escala;
    Escala = (Max - Min) * (1.0 / 1000.0);
    srand((unsigned)time(&t));
    for (int i = 0; i < qtd; i++) {
        float x = rand() % 1000;
        float y = rand() % 1000;
        x = x * Escala.x + Min.x;
        y = y * Escala.y + Min.y;
        PontosDoCenario.insereVertice(Ponto(x, y));
    }
}
// **********************************************************************
//  bool colide(Ponto min1, Ponto max1, Ponto min2, Ponto max2)
//   verifica se o envelope 1 esta dentro do envelope 2
//     pode ser usado para pontos se tratar o ponto como um envelope
//     que tem o mesmo min e max
// **********************************************************************
bool colide(Ponto min1, Ponto max1, Ponto min2, Ponto max2) {
    if (min1.x <= max2.x && max1.x >= min2.x &&
        min1.y <= max2.y && max1.y >= min2.y) {
        return true;
    }
    return false;
}
// **********************************************************************
// void CriaTrianguloDoCampoDeVisao()
//  Cria um triangulo a partir do vetor (1,0,0), girando este vetor
//  em 45 e -45 graus.
//  Este vetor fica armazenado nas variaveis "TrianguloBase" e
//  "CampoDeVisao"
// **********************************************************************
void CriaTrianguloDoCampoDeVisao() {
    Ponto vetor = Ponto(1, 0, 0);

    TrianguloBase.insereVertice(Ponto(0, 0, 0));
    CampoDeVisao.insereVertice(Ponto(0, 0, 0));

    vetor.rotacionaZ(45);
    TrianguloBase.insereVertice(vetor);
    CampoDeVisao.insereVertice(vetor);

    vetor.rotacionaZ(-90);
    TrianguloBase.insereVertice(vetor);
    CampoDeVisao.insereVertice(vetor);
}
// **********************************************************************
// void CriaEnvelope()
//  Cria um envelope a partir do vetor (1,0,0), girando este vetor
//  em 90 graus
// **********************************************************************
void CriaEnvelope() {
    Ponto vetor = Ponto(1, 0, 0);

    Envelope.insereVertice(Ponto(0, 0, 0));

    vetor.rotacionaZ(90);
    Envelope.insereVertice(vetor);

    vetor.rotacionaZ(90);
    Envelope.insereVertice(vetor);

    vetor.rotacionaZ(90);
    Envelope.insereVertice(vetor);
}
// **********************************************************************
// void PosicionaTrianguloDoCampoDeVisao(float dimensao)
//  Posiciona o campo de visao na posicao PosicaoDoCampoDeVisao,
//  com a orientacao "AnguloDoCampoDeVisao".
//  O tamanho padrao do campo de visao eh de 25% da largura da janela.
// **********************************************************************
void PosicionaTrianguloDoCampoDeVisao(float dimensao) {
    float tamanho = Tamanho.x * dimensao;

    Ponto temp;
    for (int i = 0; i < TrianguloBase.getNVertices(); i++) {
        temp = TrianguloBase.getVertice(i);
        temp.rotacionaZ(AnguloDoCampoDeVisao);
        CampoDeVisao.alteraVertice(i, PosicaoDoCampoDeVisao + temp * tamanho);
    }
    for (int j = 0; j < 3; j++) {
        vetoresTriangulo[j] = CampoDeVisao.getVertice(j) - CampoDeVisao.getVertice((j + 1) % 3);
    }
}
// **********************************************************************
// void AvancaCampoDeVisao(float distancia)
//  Move o campo de visao "distancia" unidades pra frente ou pra tras.
// **********************************************************************
void AvancaCampoDeVisao(float distancia) {
    Ponto vetor = Ponto(1, 0, 0);
    vetor.rotacionaZ(AnguloDoCampoDeVisao);
    PosicaoDoCampoDeVisao = PosicaoDoCampoDeVisao + vetor * distancia;
}
// **********************************************************************
//  void PosicionaEnvelope(Poligono *envelope)
//      Posiciona o envelope a partir dos vertices
//      do triangulo do campo de visao
// **********************************************************************
void PosicionaEnvelope(Poligono *envelope) {
    float esquerda, direita, cima, baixo;
    for (int i = 0; i < CampoDeVisao.getNVertices(); i++) {
        if (i == 0) {
            esquerda = CampoDeVisao.getVertice(i).x;
            direita = CampoDeVisao.getVertice(i).x;
            cima = CampoDeVisao.getVertice(i).y;
            baixo = CampoDeVisao.getVertice(i).y;
        } else {
            if (CampoDeVisao.getVertice(i).x < esquerda) {
                esquerda = CampoDeVisao.getVertice(i).x;
            }
            if (CampoDeVisao.getVertice(i).x > direita) {
                direita = CampoDeVisao.getVertice(i).x;
            }
            if (CampoDeVisao.getVertice(i).y < baixo) {
                baixo = CampoDeVisao.getVertice(i).y;
            }
            if (CampoDeVisao.getVertice(i).y > cima) {
                cima = CampoDeVisao.getVertice(i).y;
            }
        }
    }
    envelope->alteraVertice(0, Ponto(esquerda, baixo, 0));
    envelope->alteraVertice(1, Ponto(direita, baixo, 0));
    envelope->alteraVertice(2, Ponto(direita, cima, 0));
    envelope->alteraVertice(3, Ponto(esquerda, cima, 0));
}
// **********************************************************************
//  void subdivide(QUADTREE *nodo, Ponto min, Ponto max)
//      subdivide o nodo em 4 nodos-filhos
// **********************************************************************
void subdivide(QUADTREE *nodo, Ponto min, Ponto max) {
    // define o centro do nodo
    float meioX = (nodo->Min.x + nodo->Max.x) / 2;
    float meioY = (nodo->Min.y + nodo->Max.y) / 2;

    // filho direita em cima
    nodo->filho[0] = new nodo_quadtree;
    nodo->filho[0]->Min.x = meioX;
    nodo->filho[0]->Min.y = meioY;
    nodo->filho[0]->Max = nodo->Max;

    // filho esquerda em cima
    nodo->filho[1] = new nodo_quadtree;
    nodo->filho[1]->Min.x = nodo->Min.x;
    nodo->filho[1]->Min.y = meioY;
    nodo->filho[1]->Max.x = meioX;
    nodo->filho[1]->Max.y = nodo->Max.y;

    // filho esquerda em baixo
    nodo->filho[2] = new nodo_quadtree;
    nodo->filho[2]->Min = nodo->Min;
    nodo->filho[2]->Max.x = meioX;
    nodo->filho[2]->Max.y = meioY;

    // filho direita em baixo
    nodo->filho[3] = new nodo_quadtree;
    nodo->filho[3]->Min.x = meioX;
    nodo->filho[3]->Min.y = nodo->Min.y;
    nodo->filho[3]->Max.x = nodo->Max.x;
    nodo->filho[3]->Max.y = meioY;
}
// **********************************************************************
// void calculaPontosNoNodo(Poligono *pontos, Ponto min, Ponto max)
//      salva os pontos dentro de cada nodo
// **********************************************************************
void calculaPontosNoNodo(Poligono *pontos, Ponto min, Ponto max) {
    for (int i = 0; i < PontosDoCenario.getNVertices(); i++) {
        Ponto ponto = PontosDoCenario.getVertice(i);

        if (colide(ponto, ponto, min, max)) {
            pontos->insereVertice(ponto);
        }
    }
}
// **********************************************************************
// void criaQuadTree(nodo_quadtree *nodo, Ponto min, Ponto max)
//      Cria a quadtree e seus nodos filhos
// **********************************************************************
void criaQuadTree(nodo_quadtree *nodo, Ponto min, Ponto max) {
    nodo->Min = min;
    nodo->Max = max;
    calculaPontosNoNodo(&(nodo->pontos), nodo->Min, nodo->Max);
    nodo->folha = nodo->pontos.getNVertices() > maxPontosNodo ? false : true;

    if (!nodo->folha) {
        subdivide(nodo, min, max);
        for (int i = 0; i < 4; i++) {
            criaQuadTree(nodo->filho[i], nodo->filho[i]->Min, nodo->filho[i]->Max);
        }
    }
}
void inicializaQuadTree() {
    tree = new QUADTREE;
    criaQuadTree(tree, Minimo, Maximo);
}
// **********************************************************************
// void init()
//  Faz as inicializacoes das variaveis de estado da aplicacao
// **********************************************************************
void init() {
    // Gera ou Carrega os pontos do cenario.
    // Note que o "aspect ratio" dos pontos deve ser o mesmo da janela.
    int controle = 0;
    while (controle != 1 && controle != 2) {
        cout << "\nVoce deseja: \n1 - Gerar pontos aleatorios \n2 - Carregar um arquivo" << endl;
        cin >> controle;
    }
    if (controle == 1) {
        cout << "\nQuantos pontos voce deseja gerar: " << endl;
        cin >> QTD_PONTOS;
        GeraPontos(QTD_PONTOS, Ponto(0, 0), Ponto(500, 500));
    } else if (controle == 2) {
        cout << "\nQual arquivo voce gostaria de carregar: " << endl;
        cout << "1 - CenarioDeTeste \n2 - CenarioGrande \n3 - EstadoRS \n4 - PontosDenteDeSerra \n5 - PoligonoDeTeste" << endl;
        cin >> controle;
        const char *dir = "tests/";
        const char *caso = "";
        switch (controle) {
            case 1:
                caso = "CenarioDeTeste.txt";
                break;
            case 2:
                caso = "CenarioGrande.txt";
                break;
            case 3:
                caso = "EstadoRS.txt";
                break;
            case 4:
                caso = "PontosDenteDeSerra.txt";
                break;
            case 5:
                caso = "PoligonoDeTeste.txt";
                break;
        }
        char *s = new char[strlen(dir) + strlen(caso) + 1];
        strcpy(s, dir);
        strcat(s, caso);
        PontosDoCenario.LePoligono(s);
    }

    PontosDoCenario.obtemLimites(Minimo, Maximo);
    Minimo.x--;
    Minimo.y--;
    Maximo.x++;
    Maximo.y++;

    Meio = (Maximo + Minimo) * 0.5;  // Ponto central da janela
    Tamanho = (Maximo - Minimo);     // Tamanho da janela em X,Y

    // Ajusta variaveis do triangulo que representa o campo de visao
    PosicaoDoCampoDeVisao = Meio;
    AnguloDoCampoDeVisao = 0;

    // Cria o triangulo que representa o campo de visao
    CriaTrianguloDoCampoDeVisao();
    CriaEnvelope();
    PosicionaTrianguloDoCampoDeVisao(DimensaoDoCampoDeVisao);
    PosicionaEnvelope(&Envelope);
    inicializaQuadTree();
}
double nFrames = 0;
double TempoTotal = 0;
// **********************************************************************
//
// **********************************************************************
void animate() {
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0 / 30)  // fixa a atualizacao da tela em 30
    {
        AccumDeltaT = 0;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0) {
        cout << "\nTempo Acumulado: " << TempoTotal << " segundos. " << endl;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames / TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape(int w, int h)
//  trata o redimensionamento da janela OpenGL
// **********************************************************************
void reshape(int w, int h) {
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Minimo.x, Maximo.x, Minimo.y, Maximo.y, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
//  void DesenhaEixos(Ponto meio, Ponto min, Ponto max)
//      Desenha os eixos a partir dos pontos indicados na tela
// **********************************************************************
void DesenhaEixos(Ponto meio, Ponto min, Ponto max) {
    glBegin(GL_LINES);
    //  eixo horizontal
    glVertex2f(min.x, meio.y);
    glVertex2f(max.x, meio.y);
    //  eixo vertical
    glVertex2f(meio.x, min.y);
    glVertex2f(meio.x, max.y);
    glEnd();
}
// **********************************************************************
//  void PintaQuadTree(nodo_quadtree *nodo, int controle)
//      Pinta os nodos da quadtree com cores diferentes para cada nivel
// **********************************************************************
void PintaQuadTree(nodo_quadtree *nodo, int controle) {
    switch (controle) {
        case 0:
            glColor3f(0, 0, 0);  // preto
            break;              // Nao muda de cor, usado para quadtree monocromatica
        case 1:
            glLineWidth(2);
            glColor3f(0, 0, 1);  // Azul
            controle = 2;        // Muda a cor para o proximo nivel da quadtree
            break;
        case 2:
            glLineWidth(2);
            glColor3f(1, 0, 1);  // Rosa
            controle = 3;        // Muda a cor para o proximo nivel da quadtree
            break;
        case 3:
            glLineWidth(2);
            glColor3f(0.99, 0.87, 0.43);  // Laranja
            controle = 4;                 // Muda a cor para o proximo nivel da quadtree
            break;
        case 4:
            glLineWidth(2);
            glColor3f(0.53, 0.82, 0.97);  // Ciano
            controle = 1;                 // Muda a cor para o proximo nivel da quadtree
            break;
    }

    // define o centro do nodo
    float meioX = (nodo->Min.x + nodo->Max.x) / 2;
    float meioY = (nodo->Min.y + nodo->Max.y) / 2;

    DesenhaEixos(Ponto(meioX, meioY), nodo->Min, nodo->Max);

    for (int i = 0; i < 4; i++) {
        if (!nodo->filho[i]->folha)
            PintaQuadTree(nodo->filho[i], controle);
    }
}
// **********************************************************************
//  void pintaPonto(Poligono pontos, int cor)
//      pinta um ponto com uma cor indicada
// **********************************************************************
void pintaPonto(Ponto ponto, int cor) {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);

    defineCor(cor);
    glVertex3f(ponto.x, ponto.y, ponto.z);
    glEnd();
}
// **********************************************************************
//  bool forcaBruta(Ponto ponto)
//      Executa o algoritmo de forca bruta para um ponto
// **********************************************************************
bool forcaBruta(Ponto ponto) {
    Ponto auxiliar = {0, 0, 0};
    qtd_forcaBruta++;
    for (int j = 0; j < 3; j++) {
        Ponto vetorTriangulo = vetoresTriangulo[j];
        Ponto vetorPonto = ponto - CampoDeVisao.getVertice(j);

        ProdVetorial(vetorTriangulo, vetorPonto, auxiliar);
        if (auxiliar.z < 0)
            return false;
    }
    pontosInternos++;
    pintaPonto(ponto, Green);
    return true;
}
// **********************************************************************
//  void calculaEnvelope(Poligono pontos, Ponto min, Ponto max)
//      verifica se os pontos estão dentro de um envelope
// **********************************************************************
void calculaEnvelope(Poligono pontos, Ponto min, Ponto max) {
    for (int i = 0; i < pontos.getNVertices(); i++) {
        Ponto ponto = pontos.getVertice(i);

        if (colide(ponto, ponto, min, max)) {
            if (!forcaBruta(ponto)) {
                pontosFalsos++;
                pintaPonto(ponto, DarkYellow);
            }
        }
    }
}
// **********************************************************************
//  void calculaQuadTree(nodo_quadtree *nodo)
//      verifica os pontos que estão dentro de nodos
//       que tem colisao com o envelope do triangulo
// **********************************************************************
void calculaQuadTree(nodo_quadtree *nodo, Ponto minEnv, Ponto maxEnv) {
    if (colide(nodo->Min, nodo->Max, minEnv, maxEnv)) {
        if (nodo->folha) {
            qtd_colisoes++;
            for (size_t i = 0; i < nodo->pontos.getNVertices(); i++) {
                Ponto ponto = nodo->pontos.getVertice(i);
                if (!forcaBruta(ponto)) {
                    pontosFalsos++;
                    pintaPonto(ponto, DarkYellow);
                }
            }
        } else {
            for (int i = 0; i < 4; i++) {
                calculaQuadTree(nodo->filho[i], minEnv, maxEnv);
            }
        }
    }
}
// **********************************************************************
//  void display (void)
//      Funcao responsavel por desenhar os objetos na tela
// **********************************************************************
void display(void) {
    // Limpa a tela coma cor de fundo
    glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites lógicos da área OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (desenhaEixos) {
        glLineWidth(1);
        glColor3f(0, 0, 0);  // R, G, B  [0..1]
        DesenhaEixos(Meio, Minimo, Maximo);
    }

    glPointSize(2);
    glColor3f(1, 0, 0);  // R, G, B  [0..1]
    PontosDoCenario.desenhaVertices();

    if (bool_forcaBruta) {
        for (int i = 0; i < PontosDoCenario.getNVertices(); i++) {
            forcaBruta(PontosDoCenario.getVertice(i));
        }
    }

    if (bool_Envelope) {
        glLineWidth(3);
        glColor3f(0, 0, 0);
        Envelope.desenhaPoligono();
        calculaEnvelope(PontosDoCenario, Envelope.getVertice(0), Envelope.getVertice(2));
    }

    if (bool_QuadTree) {
        glLineWidth(3);
        glColor3f(0, 0, 0);
        Envelope.desenhaPoligono();
        calculaQuadTree(tree, Envelope.getVertice(0), Envelope.getVertice(2));
    }

    if (desenhaQuadTree) {
        glLineWidth(2);
        if (pinta_QuadTree) {
            PintaQuadTree(tree, 1);
        } else {
            PintaQuadTree(tree, 0);
        }
    }

    glLineWidth(3);
    glColor3f(0, 0, 0);  // R, G, B  [0..1]
    CampoDeVisao.desenhaPoligono();

    if (FoiClicado) {
        PontoClicado.imprime("- Ponto no universo: ", "\n");
        FoiClicado = false;
    }

    glutSwapBuffers();
}
// **********************************************************************
//  void printResults()
//      Imprime a quantidade de pontos dentro do triangulo,
//        quantos passaram pelo algoritmo de envelope ou quadtree
//        e quantos estao fora e nao passaram pelos algoritmos
// **********************************************************************
void printResults() {
    // Zera os contadores de pontos
    pontosInternos = 0;
    pontosFalsos = 0;
    qtd_colisoes = 0;
    qtd_forcaBruta = 0;

    auto start = chrono::high_resolution_clock::now();

    if (bool_forcaBruta) {
        cout << "\n\nAlgoritmo de Forca Bruta: " << endl;
        for (int i = 0; i < PontosDoCenario.getNVertices(); i++) {
            forcaBruta(PontosDoCenario.getVertice(i));
        }
    } else if (bool_Envelope) {
        cout << "\n\nAlgoritmo de Envelope: " << endl;
        calculaEnvelope(PontosDoCenario, Envelope.getVertice(0), Envelope.getVertice(2));
    } else if (bool_QuadTree) {
        cout << "\n\nAlgoritmo de Quad Tree: " << endl;
        calculaQuadTree(tree, Envelope.getVertice(0), Envelope.getVertice(2));
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    cout << "\nNumero de pontos dentro do triangulo: " << pontosInternos << endl;
    if (bool_Envelope) {
        cout << "Numero de pontos dentro do envelope: " << pontosFalsos << endl;
    } else if (bool_QuadTree) {
        cout << "Numero de pontos dentro de nodos em colisao com o envelope: " << pontosFalsos << endl;
        cout << "Numero de nodos em colisao com o envelope: " << qtd_colisoes << endl;
    }
    cout << "Numero de pontos fora do triangulo e dos filtros: "
         << PontosDoCenario.getNVertices() - (pontosInternos + pontosFalsos)
         << endl;
    cout << "Numero de vezes que o algoritmo de forca bruta foi executado: " << qtd_forcaBruta << endl;
    cout << "Tempo de execucao do algoritmo: " << duration.count() << " micros\n"
         << endl;
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo numero de segundos e informa quanto frames
//        se passaram neste periodo.
// **********************************************************************
void ContaTempo(double tempo) {
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while (true) {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0) {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//      Funcao responsavel pelos eventos do teclado
// **********************************************************************
void keyboard(unsigned char key, int x, int y) {
    size_t aux;
    switch (key) {
        case 'a':  // Altera o numero maximo de pontos
                   // nos nodos da quadtree
            cout << "\nDigite o numero maximo de pontos"
                 << " para cada nodo da quadtree: " << endl;
            cin >> maxPontosNodo;
            inicializaQuadTree();
            break;
        case 'd':  // Desenha a QuadTree na tela
            desenhaQuadTree = !desenhaQuadTree;
            PosicionaEnvelope(&Envelope);
            break;
        case 'e':  // Ativa o algoritmo de envelope
            if (bool_forcaBruta || bool_QuadTree) {
                bool_forcaBruta = false;
                bool_Envelope = true;
                bool_QuadTree = false;
                PosicionaEnvelope(&Envelope);
            }
            break;
        case 'f':  // Ativa o algoritmo de forca bruta
            if (bool_Envelope || bool_QuadTree) {
                bool_Envelope = false;
                bool_forcaBruta = true;
                bool_QuadTree = false;
            }
            break;
        case 'q':  // Ativa o algoritmo de quadtree
            if (bool_Envelope || bool_forcaBruta) {
                bool_Envelope = false;
                bool_forcaBruta = false;
                bool_QuadTree = true;
                PosicionaEnvelope(&Envelope);
            }
            break;
        case 'm':  // Aumenta o tamanho do campo de visao
            if (DimensaoDoCampoDeVisao < 0.75) {
                PosicionaTrianguloDoCampoDeVisao(DimensaoDoCampoDeVisao += 0.05);
                if (bool_Envelope || bool_QuadTree || desenhaQuadTree)
                    PosicionaEnvelope(&Envelope);
            }
            break;
        case 'n':  // Diminui o tamanho do campo de visao
            if (DimensaoDoCampoDeVisao > 0.1) {
                PosicionaTrianguloDoCampoDeVisao(DimensaoDoCampoDeVisao -= 0.05);
                if (bool_Envelope || bool_QuadTree || desenhaQuadTree)
                    PosicionaEnvelope(&Envelope);
            }
            break;
        case 'p':  // Altera a opcao de pintura da quadtree
            pinta_QuadTree = !pinta_QuadTree;
            break;
        case 's':  // Imprime os resultados no terminal
            printResults();
            break;
        case 't':  // Conta o numero de frames em um determinado tempo
            ContaTempo(3);
            break;
        case ' ':  // Altera a opcao de desenhar os eixos
            desenhaEixos = !desenhaEixos;
            break;
        case 27:      // Termina o programa qdo
            exit(0);  // a tecla ESC for pressionada
            break;
        default:
            break;
    }
    glutPostRedisplay();
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
// **********************************************************************
void arrow_keys(int a_keys, int x, int y) {
    switch (a_keys) {
        case GLUT_KEY_LEFT:  // Se pressionar LEFT
            AnguloDoCampoDeVisao += 2;
            break;
        case GLUT_KEY_RIGHT:  // Se pressionar RIGHT
            AnguloDoCampoDeVisao -= 2;
            break;
        case GLUT_KEY_UP:
            AvancaCampoDeVisao(2);
            break;
        case GLUT_KEY_DOWN:
            AvancaCampoDeVisao(-2);
            break;
        default:
            break;
    }
    PosicionaTrianguloDoCampoDeVisao(DimensaoDoCampoDeVisao);  // Atualiza posicao do campo de visao
    PosicionaEnvelope(&Envelope);                              // Atualiza posicao do envelope
    glutPostRedisplay();
}
// **********************************************************************
// Esta funcao captura o clique do botao direito do mouse sobre a area de
// desenho e converte a coordenada para o sistema de referencia definido
// na glOrtho (ver funcao reshape)
// Este codigo e baseado em http://hamala.se/forums/viewtopic.php?t=20
// **********************************************************************
void Mouse(int button, int state, int x, int y) {
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLfloat wx = x, wy, wz;
    GLdouble ox = 0.0, oy = 0.0, oz = 0.0;

    if (state != GLUT_DOWN) return;
    if (button != GLUT_RIGHT_BUTTON) return;
    cout << "Botao da direita! ";

    glGetIntegerv(GL_VIEWPORT, viewport);
    y = viewport[3] - y;
    wy = y;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(wx, wy, wz, modelview, projection, viewport, &ox, &oy, &oz);
    PontoClicado = Ponto(ox, oy, oz);
    FoiClicado = true;
}
// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int main(int argc, char **argv) {
    cout << "Programa OpenGL" << endl;

    // executa algumas inicializacoes
    init();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize(500, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de titulo da janela.
    glutCreateWindow("Poligonos em OpenGL");

    // Define a cor do fundo da tela (Branco)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // sera chamada automaticamente quando
    // for necessario redesenhar a janela
    glutDisplayFunc(display);

    // Define o tratador de evento para
    // a invalidacao da tela. A funcao "display"
    // será chamada automaticamente sempre que a
    // maquina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // sera chamada automaticamente quando
    // o usuario alterar o tamanho da janela
    glutReshapeFunc(reshape);

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // será chamada automaticamente sempre
    // o usuario pressionar uma tecla comum
    glutKeyboardFunc(keyboard);

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" sera chamada
    // automaticamente sempre o usuario
    // pressionar uma tecla especial
    glutSpecialFunc(arrow_keys);

    glutMouseFunc(Mouse);

    // inicia o tratamento dos eventos
    glutMainLoop();

    return 0;
}
