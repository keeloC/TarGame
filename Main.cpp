#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Clock.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>

// herramientas

class Pala {
public:
  float PVelminado = 1.f;
  int Pnivelminado = 1.f;
};

class Pico {
public:
  float Velminado = 1.f;
  int Nivelminado = 1.f;
};

// inventario

class Slot {
public:
  int tipo = 0;
  int cantidad = 0;
};

Slot hotbar[9];

int main() {

  // herramientas
  Pala objeto2;

  objeto2.PVelminado = 0.5f;
  objeto2.Pnivelminado = 1;

  Pico objeto;

  objeto.Velminado = 0.5f;
  objeto.Nivelminado = 2;

  sf::RenderWindow window(sf::VideoMode(800, 600), "My Window");
  sf::View view(sf::FloatRect(0, 0, 800, 600));

  sf::Font font;
  if (!font.loadFromFile(
          "/usr/share/fonts/liberation/LiberationSans-Regular.ttf"))
    return -1;

  sf::View uiView(sf::FloatRect(0, 0, 800, 600));

  float HitboxSize = 10.f;
  float rectWidth = 32.f;
  float rectHeight = 35.f;
  float PlayerSize = 0.5f;

  int FilaP = 14;
  int FilaPiedra = 28;

  float playerSpeed = 0.3f;

  float playerX = 800.f / 2.f - HitboxSize / 2.f;
  float playerY = FilaP * rectHeight - HitboxSize + 20;

  // Física

  float gravedad = 0.004f;
  float velocidadY = 0.f;
  float velocidadSalto = 1.f;
  float VelMaxDown = 0.5f;

  // Time of mining

  sf::Clock MinaT;

  int minaCol = -1;
  int minaRow = -1;

  int nivelActual = 0;
  float velActual = 999.f;

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

      if ((i >= 9 && i <= 12) && j % 5 == 0) {
        mundo[i][j] = 3;
      }
    }
  }

  saltoClock.restart();

  // Textures >

  // Herramientas Textura

  sf::Texture Picotextura;
  if (!Picotextura.loadFromFile("models/Pico.png"))
    return -1;
  sf::Sprite PicoSprite(Picotextura);

  sf::Texture Palatextura;
  if (!Palatextura.loadFromFile("models/Pala.png"))
    return -1;
  sf::Sprite PalaSprite(Palatextura);

  // Personaje Textura

  sf::Texture pizquierdaTextura;
  if (!pizquierdaTextura.loadFromFile("models/Pizquierda.png"))
    return -1;
  sf::Sprite pizquierdaSprite(pizquierdaTextura);
  pizquierdaSprite.setScale(PlayerSize, PlayerSize);
  pizquierdaSprite.setTextureRect(sf::IntRect(0, 0, 128, 128));

  sf::Texture pderechaTextura;
  if (!pderechaTextura.loadFromFile("models/Pderecha.png"))
    return -1;
  sf::Sprite pderechaSprite(pderechaTextura);
  pderechaSprite.setScale(PlayerSize, PlayerSize);
  pderechaSprite.setTextureRect(sf::IntRect(0, 0, 128, 128));

  // Puño

  sf::Texture puñoTextura;
  if (!puñoTextura.loadFromFile("models/Puño.png"))
    return -1;
  sf::Sprite puñoSprite(puñoTextura);

  // fondo

  sf::Texture fondoTextura;
  if (!fondoTextura.loadFromFile("models/Fondo.png"))
    return -1;
  sf::Sprite fondoSprite(fondoTextura);
  fondoSprite.setScale(800.f / fondoTextura.getSize().x,
                       600.f / fondoTextura.getSize().y);

  // Terreno >

  // Tierra

  sf::Texture Tierra;
  if (!Tierra.loadFromFile("models/Tierra.png"))
    return -1;
  sf::Sprite TierraSprite(Tierra);
  TierraSprite.setScale(rectWidth / Tierra.getSize().x,
                        rectHeight / Tierra.getSize().y);

  sf::Texture Tierra2;
  if (!Tierra2.loadFromFile("models/Tierra2.png"))
    return -1;
  sf::Sprite Tierra2Sprite(Tierra2);
  Tierra2Sprite.setScale(rectWidth / Tierra2.getSize().x,
                         rectHeight / Tierra2.getSize().y);

  // Piedra

  sf::Texture Roca;
  if (!Roca.loadFromFile("models/Roca.png"))
    return -1;
  sf::Sprite RocaSprite(Roca);
  RocaSprite.setScale(rectWidth / Roca.getSize().x,
                      rectHeight / Roca.getSize().y);

  // Madera

  sf::Texture Madera;
  if (!Madera.loadFromFile("models/Madera.png"))
    return -1;
  sf::Sprite MaderaSprite(Madera);
  MaderaSprite.setScale(rectWidth / Madera.getSize().x,
                        rectHeight / Madera.getSize().y);

  // anims

  sf::Clock animClock;
  int frameActual = 0;
  bool mirandoDerecha = true;
  std::map<int, int> inventario;

  // slot png

  hotbar[0].tipo = 1;
  hotbar[1].tipo = 2;
  hotbar[2].tipo = 3;

  int slotSeleccionado = 0;

  // While

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    if (animClock.getElapsedTime().asSeconds() >= 0.2f) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
          sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        frameActual = (frameActual + 1) % 2;
      }

      animClock.restart();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
      slotSeleccionado = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
      slotSeleccionado = 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
      slotSeleccionado = 2;

    int col = 0;
    int fila = frameActual;

    pizquierdaSprite.setTextureRect(sf::IntRect(0, fila * 128, 128, 128));
    pderechaSprite.setTextureRect(sf::IntRect(0, fila * 128, 128, 128));

    // Minería
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);

    int bloqueCol = static_cast<int>(worldPos.x / rectWidth);
    int bloqueRow = static_cast<int>(worldPos.y / rectHeight);

    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      minaCol = -1;
      minaRow = -1;
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      if (bloqueCol >= 0 && bloqueCol < COLS && bloqueRow >= 0 &&
          bloqueRow < FILAS) {

        if (bloqueCol != minaCol || bloqueRow != minaRow) {

          MinaT.restart();
          minaCol = bloqueCol;
          minaRow = bloqueRow;
        }

        if (MinaT.getElapsedTime().asSeconds() >= velActual) {
          int tipoBloque = mundo[bloqueRow][bloqueCol];
          int herramientaActual = hotbar[slotSeleccionado].tipo;
          bool puedeMinar = false;

          // Lógica de permisos:
          if (herramientaActual == 2) { // Si es el PICO
            if (tipoBloque == 2)
              puedeMinar = true;               // Solo mina PIEDRA (tipo 2)
          } else if (herramientaActual == 3) { // Si es la PALA
            if (tipoBloque == 1)
              puedeMinar = true;               // Solo mina TIERRA (tipo 1)
          } else if (herramientaActual == 1) { // Si es el PUÑO (opcional)
            if (tipoBloque == 1)
              puedeMinar = true; // Por ejemplo, solo madera
          }

          // Si tiene permiso y el nivel es suficiente
          if (puedeMinar && nivelActual >= tipoBloque) {
            mundo[bloqueRow][bloqueCol] = 0;
            inventario[tipoBloque]++;
          }

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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      newPosX -= velocidadActual;
      mirandoDerecha = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      newPosX += velocidadActual;
      mirandoDerecha = true;
    }

    // Colisión horizontal
    int filaArriba = static_cast<int>((playerPos.y + 1) / rectHeight);
    int filaAbajo =
        static_cast<int>((playerPos.y + HitboxSize - 1) / rectHeight);

    int colIzq = static_cast<int>(newPosX / rectWidth);
    int colDer = static_cast<int>((newPosX + HitboxSize) / rectWidth);

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

    int bottomRow = static_cast<int>((newPosY + HitboxSize) / rectHeight);
    int topRow = static_cast<int>(newPosY / rectHeight);

    colIzq = static_cast<int>(playerPos.x / rectWidth);
    colDer = static_cast<int>((playerPos.x + HitboxSize) / rectWidth);

    // Colisión vertical (caída)
    if (velocidadY > 0) {
      if (bottomRow >= 0 && bottomRow < FILAS) {
        if ((colIzq >= 0 && colIzq < COLS && mundo[bottomRow][colIzq] > 0) ||
            (colDer >= 0 && colDer < COLS && mundo[bottomRow][colDer] > 0)) {

          playerPos.y = bottomRow * rectHeight - HitboxSize;
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
    sf::RectangleShape player(sf::Vector2f(HitboxSize, HitboxSize));
    sf::Vector2f spritePos(playerPos.x - 30.f, playerPos.y - 54.f);

    pizquierdaSprite.setPosition(spritePos);
    pderechaSprite.setPosition(spritePos);
    player.setFillColor(sf::Color::Red);

    // Cámara
    view.setCenter(playerPos.x + HitboxSize / 2.f,
                   playerPos.y + HitboxSize / 2.f);

    window.clear(sf::Color::Black);
    window.setView(view);

    fondoSprite.setPosition(view.getCenter().x - 400, view.getCenter().y - 300);
    window.draw(fondoSprite);

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
          TierraSprite.setPosition(j * rectWidth, i * rectHeight);
          window.draw(TierraSprite);
        } else if (mundo[i][j] == 2) {
          RocaSprite.setPosition(j * rectWidth, i * rectHeight);
          window.draw(RocaSprite);
        } else if (mundo[i][j] == 3) {
          MaderaSprite.setPosition(j * rectWidth, i * rectHeight);
          window.draw(MaderaSprite);
        }
      }
    }

    // herramientas en slot

    if (hotbar[slotSeleccionado].tipo == 1) {
      nivelActual = 1;
      velActual = 10.f;
    }

    if (hotbar[slotSeleccionado].tipo == 2) {
      nivelActual = objeto.Nivelminado;
      velActual = objeto.Velminado;
      PicoSprite.setPosition(playerPos.x + HitboxSize,
                             playerPos.y - HitboxSize * 4);
      window.draw(PicoSprite);
    }
    if (hotbar[slotSeleccionado].tipo == 3) {
      nivelActual = objeto2.Pnivelminado;
      velActual = objeto2.Pnivelminado;
      PalaSprite.setPosition(playerPos.x + HitboxSize,
                             playerPos.y - HitboxSize * 4);
      window.draw(PalaSprite);
    }

    if (mirandoDerecha)
      window.draw(pderechaSprite);
    else
      window.draw(pizquierdaSprite);
    window.setView(uiView);

    // for Slots

    float slotSize = 50.f;
    float startX = (800 - 9 * slotSize) / 2.f;

    for (int i = 0; i < 9; ++i) {

      if (hotbar[i].tipo == 1) {
        puñoSprite.setScale(0.3f, 0.3f);
        puñoSprite.setPosition(startX + i * slotSize + 5,
                               600 - slotSize - 10 + 9);

      } else if (hotbar[i].tipo == 2) {
        PicoSprite.setScale(0.3f, 0.3f);
        PicoSprite.setPosition(startX + i * slotSize + 5,
                               600 - slotSize - 10 + 9);
      } else if (hotbar[i].tipo == 3) {
        PalaSprite.setScale(0.3f, 0.3f);
        PalaSprite.setPosition(startX + i * slotSize + 5,
                               600 - slotSize - 10 + 9);
      }

      sf::RectangleShape slot(sf::Vector2f(slotSize, slotSize));
      slot.setPosition(startX + i * slotSize, 600 - slotSize - 10);
      slot.setFillColor(sf::Color(100, 100, 100, 180));
      slot.setOutlineThickness(2);
      slot.setOutlineColor(sf::Color::White);
      window.draw(slot);
    }
    window.draw(puñoSprite);
    window.draw(PicoSprite);
    window.draw(PalaSprite);

    sf::Text texto;
    texto.setFont(font);
    texto.setCharacterSize(16);
    texto.setFillColor(sf::Color::White);
    texto.setString("Tierra: " + std::to_string(inventario[1]));
    texto.setPosition(10, 10);
    window.draw(texto);
    window.display();
  }
}