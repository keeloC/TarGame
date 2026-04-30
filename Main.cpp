#include "SFML/Graphics/Color.hpp"
#include <SFML/Graphics.hpp>

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "My Window");
  sf::View view(sf::FloatRect(0, 0, 800, 600));

  float playerSize = 40.f;
  float rectWidth = 32.f;
  float rectHeight = 35.f;

  int FilaP = 14;

  float playerSpeed = 0.3f;
  float playerX = 800.f / 2.f - playerSize / 2.f;
  float playerY = FilaP * rectHeight - playerSize;

  // Gravedad

  float gravedad = 0.004f;
  float velocidadY = 0.f;

  // salto

  float velocidadSalto = 1.f;
  float VelMaxDown = 0.5f;

  // bool

  bool enElPiso = true;
  sf::Clock saltoClock;

  sf::Vector2f playerPos(playerX, playerY);

  const int FILAS = 50;
  const int COLS = 200;
  int mundo[FILAS][COLS];

  for (int i = 0; i < FILAS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      if (i >= FilaP) {
        mundo[i][j] = 1;
      } else {
        mundo[i][j] = 0;
      }
    }
  }

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    float velocidadActual = playerSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
      velocidadActual = playerSpeed * 1.8f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
      playerPos.x -= velocidadActual;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
      playerPos.x += velocidadActual;

    enElPiso = false;

    velocidadY += gravedad;
    playerPos.y += velocidadY;

    if (velocidadY > VelMaxDown) {
      velocidadY = VelMaxDown;
    }

    if (playerPos.y + playerSize >= FilaP * rectHeight) {
      playerPos.y = FilaP * rectHeight - playerSize;
      velocidadY = 0;
      enElPiso = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && enElPiso &&
        saltoClock.getElapsedTime().asSeconds() >= 0.5f) {
      velocidadY = -velocidadSalto;
      saltoClock.restart();
    }

    sf::RectangleShape player(sf::Vector2f(playerSize, playerSize));
    player.setPosition(playerPos);
    player.setFillColor(sf::Color::Red);

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
        std::min(FILAS - 1, (int)((view.getCenter().y + 300) / rectHeight) + 1);

    for (int i = filaInicio; i < filaFin; ++i) {
      for (int j = colInicio; j < colFin; ++j) {
        sf::RectangleShape rectangle(sf::Vector2f(rectWidth, rectHeight));

        if (mundo[i][j] == 1) {
          rectangle.setFillColor(sf::Color::Blue);
          rectangle.setPosition(j * rectWidth, i * rectHeight);
          window.draw(rectangle);
        }
      }
    }

    window.draw(player);
    window.display();
  }
}
