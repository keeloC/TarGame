#include <SFML/Graphics.hpp>
#include <algorithm>
#include <map>
#include <math.h>
#include <string>

class Puño {
public:
  float PuñoVelminado = 1.f;
  int PuñoNivelMinado = 1;
};

class Pala {
public:
  float PVelminado = 1.f;
  int Pnivelminado = 1;
};

class Pico {
public:
  float Velminado = 1.f;
  int Nivelminado = 1;
};

class Slot {
public:
  int tipo = 0;
  int cantidad = 0;
};

Slot hotbar[9];

bool añadirAHotbar(int tipoItem) {
  for (int i = 0; i < 9; ++i) {
    if (hotbar[i].tipo == tipoItem) {
      hotbar[i].cantidad++;
      return true;
    }
  }
  for (int i = 0; i < 9; ++i) {
    if (hotbar[i].tipo == 0) {
      hotbar[i].tipo = tipoItem;
      hotbar[i].cantidad = 1;
      return true;
    }
  }
  return false;
}

void intentarCrafteo(int producto, std::map<int, int> &inventario) {
  if (producto == 2) {
    if (inventario[3] >= 3 && inventario[4] >= 3) {
      bool itemAñadido = añadirAHotbar(2);
      if (itemAñadido) {
        inventario[3] -= 3;
        inventario[4] -= 3;
      }
    }
  } else if (producto == 3) {
    if (inventario[4] >= 3 && inventario[3] >= 3) {
      bool itemAñadido = añadirAHotbar(3);
      if (itemAñadido) {
        inventario[3] -= 3;
        inventario[4] -= 3;
      }
    }
  }
}

int main() {
  Puño objeto1;
  objeto1.PuñoVelminado = 1.f;
  objeto1.PuñoNivelMinado = 5;

  Pala objeto2;
  objeto2.PVelminado = 0.5f;
  objeto2.Pnivelminado = 5;

  Pico objeto;
  objeto.Velminado = 0.5f;
  objeto.Nivelminado = 2;

  sf::RenderWindow window(sf::VideoMode(800, 600), "My Window");
  sf::View view(sf::FloatRect(0, 0, 800, 600));
  sf::View uiView(sf::FloatRect(0, 0, 800, 600));

  sf::Font font;
  if (!font.loadFromFile(
          "/usr/share/fonts/liberation/LiberationSans-Regular.ttf"))
    return -1;

  float HitboxSize = 10.f;
  float rectWidth = 32.f;
  float rectHeight = 35.f;
  float PlayerSize = 0.5f;
  int FilaP = 14;
  int FilaPiedra = 28;
  float playerSpeed = 0.1f;

  const int FILAS = 50;
  const int COLS = 400;
  int mundo[FILAS][COLS];

  float playerX = (COLS / 2) * rectWidth;
  float playerY = FilaP * rectHeight - HitboxSize + 20;

  float gravedad = 0.0038f;
  float velocidadY = 0.f;
  float velocidadSalto = 0.68f;
  float VelMaxDown = 0.5f;

  sf::Clock MinaT;
  int minaCol = -1;
  int minaRow = -1;
  int nivelActual = 0;
  float velActual = 999.f;

  bool enElPiso = true;
  sf::Clock saltoClock;
  bool inventarioAbierto = false;
  sf::Vector2f playerPos(playerX, playerY);

  for (int i = 0; i < FILAS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      if (i >= FilaPiedra)
        mundo[i][j] = 2;
      else if (i == FilaP) {
        mundo[i][j] = 5;
      } else if (i >= FilaP)
        mundo[i][j] = 1;
      else
        mundo[i][j] = 0;

      if ((i >= 8 && i <= 13) && j % 7 == 0) {
        mundo[i][j] = 3;
      }
      if ((i >= 5 && i <= 9) && (j % 7 == 0 || j % 7 == 1 || j % 7 == 6 ||
                                 j % 7 == 5 || j % 7 == 2)) {
        mundo[i][j] = 4;
      }
    }
  }

  saltoClock.restart();

  sf::Texture Picotextura;
  if (!Picotextura.loadFromFile("models/Pico.png"))
    return -1;
  sf::Sprite PicoSprite(Picotextura);

  sf::Texture Palatextura;
  if (!Palatextura.loadFromFile("models/Pala.png"))
    return -1;
  sf::Sprite PalaSprite(Palatextura);

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

  sf::Texture puñoTextura;
  if (!puñoTextura.loadFromFile("models/Puño.png"))
    return -1;
  sf::Sprite puñoSprite(puñoTextura);

  sf::Texture fondoTextura;
  if (!fondoTextura.loadFromFile("models/Fondo.png"))
    return -1;
  sf::Sprite fondoSprite(fondoTextura);
  fondoSprite.setScale(800.f / fondoTextura.getSize().x,
                       600.f / fondoTextura.getSize().y);

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

  sf::Texture Roca;
  if (!Roca.loadFromFile("models/Roca.png"))
    return -1;
  sf::Sprite RocaSprite(Roca);
  RocaSprite.setScale(rectWidth / Roca.getSize().x,
                      rectHeight / Roca.getSize().y);

  sf::Texture Madera;
  if (!Madera.loadFromFile("models/Madera.png"))
    return -1;
  sf::Sprite MaderaSprite(Madera);
  MaderaSprite.setScale(rectWidth / Madera.getSize().x,
                        rectHeight / Madera.getSize().y);

  sf::Texture Hoja;
  if (!Hoja.loadFromFile("models/Hoja.png"))
    return -1;
  sf::Sprite HojaSprite(Hoja);
  HojaSprite.setScale(rectWidth / Hoja.getSize().x,
                      rectHeight / Hoja.getSize().y);

  sf::Clock animClock;
  int frameActual = 0;
  bool mirandoDerecha = true;
  std::map<int, int> inventario;

  for (int i = 0; i < 9; ++i) {
    hotbar[i].tipo = 0;
    hotbar[i].cantidad = 0;
  }

  int slotSeleccionado = 0;

  while (window.isOpen()) {

    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::E) {
          inventarioAbierto = !inventarioAbierto;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
          int tipoItem = 1;
          añadirAHotbar(tipoItem);
        }

        if (event.key.code == sf::Keyboard::C) {
          intentarCrafteo(2, inventario);
        }
        if (event.key.code == sf::Keyboard::V) {
          intentarCrafteo(3, inventario);
        }
      }
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
      slotSeleccionado = 3;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
      slotSeleccionado = 4;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
      slotSeleccionado = 5;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7))
      slotSeleccionado = 6;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8))
      slotSeleccionado = 7;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9))
      slotSeleccionado = 8;

    if (hotbar[slotSeleccionado].tipo == 1) {
      nivelActual = objeto1.PuñoNivelMinado;
      velActual = objeto1.PuñoVelminado;
    } else if (hotbar[slotSeleccionado].tipo == 2) {
      nivelActual = objeto.Nivelminado;
      velActual = objeto.Velminado;
      PicoSprite.setPosition(playerPos.x + HitboxSize,
                             playerPos.y - HitboxSize * 4);

    } else if (hotbar[slotSeleccionado].tipo == 3) {
      nivelActual = objeto2.Pnivelminado;
      velActual = objeto2.PVelminado;
      PalaSprite.setPosition(playerPos.x + HitboxSize,
                             playerPos.y - HitboxSize * 4);

    } else if (hotbar[slotSeleccionado].tipo == 0) {
      nivelActual = objeto1.PuñoNivelMinado;
      velActual = objeto1.PuñoVelminado;
    }

    int fila = frameActual;
    pizquierdaSprite.setTextureRect(sf::IntRect(0, fila * 128, 128, 128));
    pderechaSprite.setTextureRect(sf::IntRect(0, fila * 128, 128, 128));

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

          if (herramientaActual == 2) {
            if (tipoBloque == 2)
              puedeMinar = true;
          } else if (herramientaActual == 3) {
            if (tipoBloque == 1 || tipoBloque == 5)
              puedeMinar = true;
          } else if (herramientaActual == 1) {
            if (tipoBloque == 3 || tipoBloque == 4)
              puedeMinar = true;
          } else if (herramientaActual == 0) {
            if (tipoBloque == 3 || tipoBloque == 4 || tipoBloque == 5 ||
                tipoBloque == 1)
              puedeMinar = true;
          }

          if (puedeMinar && nivelActual >= tipoBloque) {
            mundo[bloqueRow][bloqueCol] = 0;
            inventario[tipoBloque]++;
          }

          minaCol = -1;
          minaRow = -1;
        }
      }
    }

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

    int filaArriba = static_cast<int>((playerPos.y + 1) / rectHeight);
    int filaAbajo =
        static_cast<int>((playerPos.y + HitboxSize - 1) / rectHeight);
    int colIzq = static_cast<int>(newPosX / rectWidth);
    int colDer = static_cast<int>((newPosX + HitboxSize) / rectWidth);
    bool colisionX = false;

    for (int i = filaArriba; i <= filaAbajo; ++i) {
      if (i >= 0 && i < FILAS) {
        if ((colIzq >= 0 && colIzq < COLS &&
             (mundo[i][colIzq] == 1 || mundo[i][colIzq] == 2 ||
              mundo[i][colIzq] == 5)) ||
            (colDer >= 0 && colDer < COLS &&
             (mundo[i][colDer] == 1 || mundo[i][colDer] == 2 ||
              mundo[i][colDer] == 5))) {
          colisionX = true;
          break;
        }
      }
    }

    if (!colisionX)
      playerPos.x = newPosX;

    enElPiso = false;
    velocidadY += gravedad;
    if (velocidadY > VelMaxDown)
      velocidadY = VelMaxDown;

    float newPosY = playerPos.y + velocidadY;
    int bottomRow = static_cast<int>((newPosY + HitboxSize) / rectHeight);
    int topRow = static_cast<int>(newPosY / rectHeight);
    colIzq = static_cast<int>(playerPos.x / rectWidth);
    colDer = static_cast<int>((playerPos.x + HitboxSize) / rectWidth);

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
    } else if (velocidadY < 0) {
      if (topRow >= 0 && topRow < FILAS) {
        if ((colIzq >= 0 && colIzq < COLS &&
             (mundo[topRow][colIzq] == 1 || mundo[topRow][colIzq] == 2 ||
              mundo[bottomRow][colIzq] == 5)) ||
            (colDer >= 0 && colDer < COLS &&
             (mundo[topRow][colDer] == 1 || mundo[topRow][colDer] == 2 ||
              mundo[bottomRow][colDer] == 5))) {
          playerPos.y = (topRow + 1) * rectHeight;
          velocidadY = 0;
        } else {
          playerPos.y = newPosY;
        }
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && enElPiso &&
        saltoClock.getElapsedTime().asSeconds() >= 0.2f) {
      velocidadY = -velocidadSalto;
      saltoClock.restart();
    }

    sf::RectangleShape player(sf::Vector2f(HitboxSize, HitboxSize));
    sf::Vector2f spritePos(playerPos.x - 30.f, playerPos.y - 54.f);
    pizquierdaSprite.setPosition(spritePos);
    pderechaSprite.setPosition(spritePos);
    player.setFillColor(sf::Color::Red);

    view.setCenter(playerPos.x + HitboxSize / 2.f,
                   playerPos.y + HitboxSize / 2.f);

    window.clear(sf::Color::Black);
    window.setView(view);

    fondoSprite.setPosition(view.getCenter().x - 400, view.getCenter().y - 300);
    window.draw(fondoSprite);

    window.draw(PalaSprite);
    window.draw(PicoSprite);

    int colInicio = std::max(0, (int)((view.getCenter().x - 400) / rectWidth));
    int colFin =
        std::min(COLS, (int)((view.getCenter().x + 400) / rectWidth) + 1);
    int filaInicio =
        std::max(0, (int)((view.getCenter().y - 300) / rectHeight));
    int filaFin =
        std::min(FILAS, (int)((view.getCenter().y + 300) / rectHeight) + 1);

    for (int i = filaInicio; i < filaFin; ++i) {
      for (int j = colInicio; j < colFin; ++j) {
        if (mundo[i][j] == 1) {
          TierraSprite.setPosition(j * rectWidth, i * rectHeight);
          window.draw(TierraSprite);
        } else if (mundo[i][j] == 5) {
          Tierra2Sprite.setPosition(j * rectWidth, i * rectHeight);
          window.draw(Tierra2Sprite);
        } else if (mundo[i][j] == 2) {
          RocaSprite.setPosition(j * rectWidth, i * rectHeight);
          window.draw(RocaSprite);
        } else if (mundo[i][j] == 3) {
          MaderaSprite.setPosition(j * rectWidth, i * rectHeight);
          window.draw(MaderaSprite);
        } else if (mundo[i][j] == 4) {
          HojaSprite.setPosition(j * rectWidth, i * rectHeight);
          window.draw(HojaSprite);
        }
      }
    }

    if (mirandoDerecha)
      window.draw(pderechaSprite);
    else
      window.draw(pizquierdaSprite);

    window.setView(uiView);

    float slotSize = 50.f;
    float startX = (800 - 9 * slotSize) / 2.f;

    for (int i = 0; i < 9; ++i) {
      sf::RectangleShape slot(sf::Vector2f(slotSize, slotSize));
      slot.setPosition(startX + i * slotSize, 600 - slotSize - 10);
      slot.setFillColor(sf::Color(100, 100, 100, 180));
      slot.setOutlineThickness(2);
      slot.setOutlineColor(sf::Color::White);
      window.draw(slot);

      if (hotbar[i].tipo == 1) {
        puñoSprite.setScale(0.3f, 0.3f);
        puñoSprite.setPosition(startX + i * slotSize + 5,
                               600 - slotSize - 10 + 9);
        window.draw(puñoSprite);
      } else if (hotbar[i].tipo == 2) {
        PicoSprite.setScale(0.3f, 0.3f);
        PicoSprite.setPosition(startX + i * slotSize + 5,
                               600 - slotSize - 10 + 9);
        window.draw(PicoSprite);
      } else if (hotbar[i].tipo == 3) {
        PalaSprite.setScale(0.3f, 0.3f);
        PalaSprite.setPosition(startX + i * slotSize + 5,
                               600 - slotSize - 10 + 9);
        window.draw(PalaSprite);
      }
    }

    if (inventarioAbierto) {
      int startY = 150;
      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
          sf::RectangleShape square(sf::Vector2f(slotSize, slotSize));
          square.setPosition(startX + j * slotSize, startY + i * slotSize);
          square.setFillColor(sf::Color(150, 150, 150, 180));
          window.draw(square);

          int index = i * 3 + j;
          if (index < inventario.size()) {
            Slot item = hotbar[index];
            if (item.tipo == 1) {
              puñoSprite.setScale(0.2f, 0.2f);
              puñoSprite.setPosition(startX + j * slotSize + 5,
                                     startY + i * slotSize + 5);
              window.draw(puñoSprite);
            } else if (item.tipo == 2) {
              PicoSprite.setScale(0.2f, 0.2f);
              PicoSprite.setPosition(startX + j * slotSize + 5,
                                     startY + i * slotSize + 5);
              window.draw(PicoSprite);
            } else if (item.tipo == 3) {
              PalaSprite.setScale(0.2f, 0.2f);
              PalaSprite.setPosition(startX + j * slotSize + 5,
                                     startY + i * slotSize + 5);
              window.draw(PalaSprite);
            }
          }
        }
      }
    }

    sf::Text texto;
    texto.setFont(font);
    texto.setCharacterSize(16);
    texto.setFillColor(sf::Color::White);
    texto.setString("Tierra: " + std::to_string(inventario[1]) +
                    " | Tierra2: " + std::to_string(inventario[5]) +
                    " | Roca: " + std::to_string(inventario[2]) +
                    " | Madera: " + std::to_string(inventario[3]) +
                    " | Hoja: " + std::to_string(inventario[4]));
    texto.setPosition(10, 10);
    window.draw(texto);

    window.display();
  }
  return 0;
}
