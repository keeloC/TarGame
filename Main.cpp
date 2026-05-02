#include "SFML/System/Clock.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "My Window");
  sf::View view(sf::FloatRect(0, 0, 800, 600));

  float playerSize = 10.f;
  float rectWidth = 32.f;
  float rectHeight = 35.f;

  int FilaP = 14;
  int FilaPiedra = 28;

  float playerSpeed = 0.3f;

  float playerX = 800.f / 2.f - playerSize / 2.f;
  float playerY = FilaP * rectHeight - playerSize;

  // Física
  float gravedad = 0.004f;
  float velocidadY = 0.f;
  float velocidadSalto = 1.f;
  float VelMaxDown = 0.5f;

  // Time of mining

  sf::Clock MinaT;
  float tiempodeminado = 1.f;
  int minaCol = -1;
  int minaRow = -1;

  bool enElPiso = true;
  sf::Clock saltoClock;

  sf::Vector2f playerPos(playerX, playerY);

  const int FILAS = 50;
  const int COLS = 200;
  int mundo[FILAS][COLS];

  // Generación mundo
  for (int i = 0; i < FILAS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      if (i >= FilaPiedra)
        mundo[i][j] = 2;
      else if (i >= FilaP)
        mundo[i][j] = 1;
      else
        mundo[i][j] = 0;
    }
  }

  saltoClock.restart();

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // Minería
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

    int bloqueCol = static_cast<int>(worldPos.x / rectWidth);
    int bloqueRow = static_cast<int>(worldPos.y / rectHeight);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      if (bloqueCol >= 0 && bloqueCol < COLS && bloqueRow >= 0 &&
          bloqueRow < FILAS) {

        if (bloqueCol != minaCol || bloqueRow != minaRow) {

          MinaT.restart();
          minaCol = bloqueCol;
          minaRow = bloqueRow;
        }

        if (MinaT.getElapsedTime().asSeconds() >= tiempodeminado) {
          mundo[bloqueRow][bloqueCol] = 0;
          minaCol = -1;
          minaRow = -1;
        }
      }
    }

    // Movimiento horizontal
    float velocidadActual = playerSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
      velocidadActual *= 1.8f;

    float newPosX = playerPos.x;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
      newPosX -= velocidadActual;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
      newPosX += velocidadActual;

    // Colisión horizontal
    int filaArriba = static_cast<int>((playerPos.y + 1) / rectHeight);
    int filaAbajo =
        static_cast<int>((playerPos.y + playerSize - 1) / rectHeight);

    int colIzq = static_cast<int>(newPosX / rectWidth);
    int colDer = static_cast<int>((newPosX + playerSize) / rectWidth);

    bool colisionX = false;

    for (int i = filaArriba; i <= filaAbajo; ++i) {
      if (i >= 0 && i < FILAS) {
        if ((colIzq >= 0 && colIzq < COLS && mundo[i][colIzq] > 0) ||
            (colDer >= 0 && colDer < COLS && mundo[i][colDer] > 0)) {
          colisionX = true;
          break;
        }
      }
    }

    if (!colisionX)
      playerPos.x = newPosX;

    // Física vertical
    enElPiso = false;

    velocidadY += gravedad;
    if (velocidadY > VelMaxDown)
      velocidadY = VelMaxDown;

    float newPosY = playerPos.y + velocidadY;

    int bottomRow = static_cast<int>((newPosY + playerSize) / rectHeight);
    int topRow = static_cast<int>(newPosY / rectHeight);

    colIzq = static_cast<int>(playerPos.x / rectWidth);
    colDer = static_cast<int>((playerPos.x + playerSize) / rectWidth);

    // Colisión vertical (caída)
    if (velocidadY > 0) {
      if (bottomRow >= 0 && bottomRow < FILAS) {
        if ((colIzq >= 0 && colIzq < COLS && mundo[bottomRow][colIzq] > 0) ||
            (colDer >= 0 && colDer < COLS && mundo[bottomRow][colDer] > 0)) {

          playerPos.y = bottomRow * rectHeight - playerSize;
          velocidadY = 0;
          enElPiso = true;
        } else {
          playerPos.y = newPosY;
        }
      }
    }
    // Colisión vertical (subida)
    else if (velocidadY < 0) {
      if (topRow >= 0 && topRow < FILAS) {
        if ((colIzq >= 0 && colIzq < COLS && mundo[topRow][colIzq] > 0) ||
            (colDer >= 0 && colDer < COLS && mundo[topRow][colDer] > 0)) {

          playerPos.y = (topRow + 1) * rectHeight;
          velocidadY = 0;
        } else {
          playerPos.y = newPosY;
        }
      }
    }

    // Salto
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && enElPiso &&
        saltoClock.getElapsedTime().asSeconds() >= 0.2f) {

      velocidadY = -velocidadSalto;
      saltoClock.restart();
    }

    // Jugador
    sf::RectangleShape player(sf::Vector2f(playerSize, playerSize));
    player.setPosition(playerPos);
    player.setFillColor(sf::Color::Red);

    // Cámara
    view.setCenter(playerPos.x + playerSize / 2.f,
                   playerPos.y + playerSize / 2.f);

    window.clear(sf::Color::Black);
    window.setView(view);

    int colInicio = std::max(0, (int)((view.getCenter().x - 400) / rectWidth));
    int colFin =
        std::min(COLS, (int)((view.getCenter().x + 400) / rectWidth) + 1);

    int filaInicio =
        std::max(0, (int)((view.getCenter().y - 300) / rectHeight));
    int filaFin =
        std::min(FILAS, (int)((view.getCenter().y + 300) / rectHeight) + 1);

    for (int i = filaInicio; i < filaFin; ++i) {
      for (int j = colInicio; j < colFin; ++j) {
        sf::RectangleShape rect(sf::Vector2f(rectWidth, rectHeight));

        if (mundo[i][j] == 1) {
          rect.setFillColor(sf::Color::Blue);
        } else if (mundo[i][j] == 2) {
          rect.setFillColor(sf::Color::Magenta);
        } else
          continue;

        rect.setPosition(j * rectWidth, i * rectHeight);
        window.draw(rect);
      }
    }

    window.draw(player);
    window.display();
  }
}