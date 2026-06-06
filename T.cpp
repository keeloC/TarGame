#include "SFML/Graphics/Texture.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <map>
#include <math.h>
#include <string>

class Pu {
public:
  float PuVelminado;
  float PuNivelminado;
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

class Arrastre {
public:
  enum OrigenArrastre { HOTBAR, INVENTARIO, ALQUIMIA };
  OrigenArrastre origen;
  bool arrastrando = false;
  int slotOrigen = -1;
  Slot itemArrastrado;
};

Arrastre estadoArrastre;

float anchoVentana = 800.f;
float altoVentana = 600.f;

float alqX = 0.f;
float alqY = 0.f;
float alqSlotSize = 35.f;
float invX = 0.f;
float invY = 0.f;
float invSlotSize = 34.f;

Slot hotbar[9];
Slot inventarioSlots[36];

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

bool añadirAInventario(int tipoItem) {
  for (int i = 0; i < 36; ++i) {
    if (inventarioSlots[i].tipo == tipoItem) {
      inventarioSlots[i].cantidad++;
      return true;
    }
  }
  for (int i = 0; i < 36; ++i) {
    if (inventarioSlots[i].tipo == 0) {
      inventarioSlots[i].tipo = tipoItem;
      inventarioSlots[i].cantidad = 1;
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

Slot slotsAlquimia[3];
Slot slotResultado;

void verificarAlquimia(Slot slotsAlquimia[3], Slot &slotResultado) {

  int a = slotsAlquimia[0].tipo;
  int b = slotsAlquimia[1].tipo;
  int c = slotsAlquimia[2].tipo;

  if (a == 3 && b == 3 && c == 3)
    slotResultado.tipo = 10;

  else if (a == 2 && b == 2 && c == 2)
    slotResultado.tipo = 11;

  else
    slotResultado.tipo = 0;
}

void iniciarArrastre(sf::Vector2f posMouseUI) {
  int slotClick = -1;
  float slotSize = 29.f;
  float startX = (anchoVentana - 9 * slotSize) / 2.f;

  float pergaminoAlto = 220.f;

  for (int i = 0; i < 9; ++i) {
    if (posMouseUI.x > startX + 3.3f + i * slotSize &&
        posMouseUI.x < startX + 3.3f + (i + 1) * slotSize &&
        posMouseUI.y > altoVentana - pergaminoAlto + 90.f &&
        posMouseUI.y < altoVentana - pergaminoAlto + 90.f + slotSize) {
      slotClick = i;
      break;
    }
  }

  if (slotClick != -1 && hotbar[slotClick].tipo != 0) {
    estadoArrastre.itemArrastrado = hotbar[slotClick];
    hotbar[slotClick].tipo = 0;
    estadoArrastre.slotOrigen = slotClick;
    estadoArrastre.arrastrando = true;
  }

  if (!estadoArrastre.arrastrando) {
    for (int i = 0; i < 36; ++i) {
      int col = i % 9;
      int fila = i / 9;
      float x = invX + col * invSlotSize;
      float y = invY + fila * invSlotSize;
      if (posMouseUI.x > x && posMouseUI.x < x + invSlotSize &&
          posMouseUI.y > y && posMouseUI.y < y + invSlotSize) {
        if (inventarioSlots[i].tipo != 0) {
          estadoArrastre.itemArrastrado = inventarioSlots[i];
          inventarioSlots[i] = Slot();
          estadoArrastre.slotOrigen = i;
          estadoArrastre.arrastrando = true;
          estadoArrastre.origen = Arrastre::INVENTARIO;
        }
        break;
      }
    }
  }

  if (!estadoArrastre.arrastrando) {
    for (int i = 0; i < 3; ++i) {
      float x = alqX + i * alqSlotSize;
      if (posMouseUI.x > x && posMouseUI.x < x + alqSlotSize &&
          posMouseUI.y > alqY && posMouseUI.y < alqY + alqSlotSize) {
        if (slotsAlquimia[i].tipo != 0) {
          estadoArrastre.itemArrastrado = slotsAlquimia[i];
          slotsAlquimia[i] = Slot();
          estadoArrastre.slotOrigen = i;
          estadoArrastre.arrastrando = true;
          estadoArrastre.origen = Arrastre::ALQUIMIA;
        }
        break;
      }
    }
  }
}
void soltarArrastre(sf::Vector2f posMouseUI) {
  int slotClick = -1;
  float slotSize = 29.f;
  float startX = (anchoVentana - 9 * slotSize) / 2.f;

  float pergaminoAlto = 220.f;

  for (int i = 0; i < 9; ++i) {
    if (posMouseUI.x > startX + 3.3f + i * slotSize &&
        posMouseUI.x < startX + 3.3f + (i + 1) * slotSize &&
        posMouseUI.y > altoVentana - pergaminoAlto + 90.f &&
        posMouseUI.y < altoVentana - pergaminoAlto + 90.f + slotSize) {
      slotClick = i;
      break;
    }
  }

  if (slotClick != -1 && estadoArrastre.arrastrando) {
    if (hotbar[slotClick].tipo == 0) {
      hotbar[slotClick] = estadoArrastre.itemArrastrado;
    } else {
      Slot temp = hotbar[slotClick];
      hotbar[estadoArrastre.slotOrigen] = temp;
      hotbar[slotClick] = estadoArrastre.itemArrastrado;
      estadoArrastre.itemArrastrado = temp;
    }
    estadoArrastre.arrastrando = false; // ← agregar esto
  }

  if (slotClick == -1 && estadoArrastre.arrastrando) {
    for (int i = 0; i < 36; ++i) {
      int col = i % 9;
      int fila = i / 9;
      float x = invX + col * invSlotSize;
      float y = invY + fila * invSlotSize;
      if (posMouseUI.x > x && posMouseUI.x < x + invSlotSize &&
          posMouseUI.y > y && posMouseUI.y < y + invSlotSize) {
        inventarioSlots[i] = estadoArrastre.itemArrastrado;
        estadoArrastre.arrastrando = false;
        break;
      }
    }
  }

  if (estadoArrastre.arrastrando) {
    for (int i = 0; i < 3; ++i) {
      float x = alqX + i * alqSlotSize;
      if (posMouseUI.x > x && posMouseUI.x < x + alqSlotSize &&
          posMouseUI.y > alqY && posMouseUI.y < alqY + alqSlotSize) {
        slotsAlquimia[i] = estadoArrastre.itemArrastrado;
        verificarAlquimia(slotsAlquimia, slotResultado);
        estadoArrastre.arrastrando = false;
        break;
      }
    }
  }

  if (estadoArrastre.arrastrando) {
    if (estadoArrastre.origen == Arrastre::HOTBAR)
      hotbar[estadoArrastre.slotOrigen] = estadoArrastre.itemArrastrado;
    else if (estadoArrastre.origen == Arrastre::INVENTARIO)
      inventarioSlots[estadoArrastre.slotOrigen] =
          estadoArrastre.itemArrastrado;
    else if (estadoArrastre.origen == Arrastre::ALQUIMIA)
      slotsAlquimia[estadoArrastre.slotOrigen] = estadoArrastre.itemArrastrado;
  }
  estadoArrastre.arrastrando = false;
  estadoArrastre.slotOrigen = -1;

  estadoArrastre.arrastrando = false;
  estadoArrastre.slotOrigen = -1;
}

int main() {

  float MitadAncho = anchoVentana / 2.f;
  float MitadAlto = altoVentana / 2.f;

  Pu objeto1;
  objeto1.PuVelminado = 1.f;
  objeto1.PuNivelminado = 5;

  Pala objeto2;
  objeto2.PVelminado = 0.5f;
  objeto2.Pnivelminado = 5;

  Pico objeto;
  objeto.Velminado = 0.5f;
  objeto.Nivelminado = 2;

  sf::RenderWindow window(sf::VideoMode(anchoVentana, altoVentana),
                          "yeamaracuyea");
  anchoVentana = window.getSize().x;
  altoVentana = window.getSize().y;
  sf::View view(sf::FloatRect(0, 0, anchoVentana, altoVentana));
  sf::View uiView(sf::FloatRect(0, 0, anchoVentana, altoVentana));

  sf::Font font;
  if (!font.loadFromFile(
          "/usr/share/fonts/liberation/LiberationSans-Regular.ttf"))
    return -1;

  // variables

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

  srand(time(NULL));
  float offsetOnda = (rand() % 1000) / 100.f;

  int mapaAlturas[COLS];

  for (int j = 0; j < COLS; ++j) {

    int alturaSuperficie =
        20 + (int)(3.0 * sin(0.08 * j + offsetOnda) + 2.0 * cos(0.04 * j));
    mapaAlturas[j] = alturaSuperficie;

    for (int i = 0; i < FILAS; ++i) {
      if (i < alturaSuperficie) {
        mundo[i][j] = 0;
      } else if (i == alturaSuperficie) {
        mundo[i][j] = 5;
      } else if (i > alturaSuperficie && i < alturaSuperficie + 8) {
        mundo[i][j] = 1;
      } else {
        mundo[i][j] = 2;
      }
    }
  }

  for (int j = 3; j < COLS - 3; ++j) {
    if (rand() % 100 < 8) {
      int alturaSuelo = mapaAlturas[j];

      mundo[alturaSuelo - 1][j] = 3;
      mundo[alturaSuelo - 2][j] = 3;

      for (int hojaY = alturaSuelo - 5; hojaY <= alturaSuelo - 3; ++hojaY) {
        for (int hojaX = j - 1; hojaX <= j + 1; ++hojaX) {
          mundo[hojaY][hojaX] = 4;
        }
      }
    }
  }

  saltoClock.restart();

  sf::Texture Picotextura;
  if (!Picotextura.loadFromFile("models/Pico.png"))
    return -1;
  sf::Sprite PicoSprite(Picotextura);

  sf::Sprite PicoUISprite(Picotextura);
  PicoUISprite.setScale(29.f / Picotextura.getSize().x,
                        29.f / Picotextura.getSize().y);

  sf::Texture Palatextura;
  if (!Palatextura.loadFromFile("models/Pala.png"))
    return -1;
  sf::Sprite PalaSprite(Palatextura);

  sf::Sprite PalaUISprite(Palatextura);
  PalaUISprite.setScale(29.f / Palatextura.getSize().x,
                        29.f / Palatextura.getSize().y);

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

  sf::Texture fondoTextura;
  if (!fondoTextura.loadFromFile("models/Fondo.png"))
    return -1;
  sf::Sprite fondoSprite(fondoTextura);
  fondoSprite.setScale(anchoVentana / fondoTextura.getSize().x,
                       altoVentana / fondoTextura.getSize().y);

  sf::Texture Tierra;
  if (!Tierra.loadFromFile("models/Tierra.png"))
    return -1;
  sf::Sprite TierraSprite(Tierra);
  TierraSprite.setScale(rectWidth / Tierra.getSize().x,
                        rectHeight / Tierra.getSize().y);

  sf::Sprite TierraUISprite(Tierra);
  TierraUISprite.setScale(29.f / Tierra.getSize().x, 29.f / Tierra.getSize().y);

  sf::Texture Tierra2;
  if (!Tierra2.loadFromFile("models/Tierra2.png"))
    return -1;
  sf::Sprite Tierra2Sprite(Tierra2);
  Tierra2Sprite.setScale(rectWidth / Tierra2.getSize().x,
                         rectHeight / Tierra2.getSize().y);

  sf::Sprite Tierra2UISprite(Tierra2);
  Tierra2UISprite.setScale(29.f / Tierra2.getSize().x,
                           29.f / Tierra2.getSize().y);

  sf::Texture Roca;
  if (!Roca.loadFromFile("models/Roca.png"))
    return -1;
  sf::Sprite RocaSprite(Roca);
  RocaSprite.setScale(rectWidth / Roca.getSize().x,
                      rectHeight / Roca.getSize().y);

  sf::Sprite RocaUISprite(Roca);
  RocaUISprite.setScale(29.f / Roca.getSize().x, 29.f / Roca.getSize().y);

  sf::Texture Madera;
  if (!Madera.loadFromFile("models/Madera.png"))
    return -1;
  sf::Sprite MaderaSprite(Madera);
  MaderaSprite.setScale(rectWidth / Madera.getSize().x,
                        rectHeight / Madera.getSize().y);

  sf::Sprite MaderaUISprite(Madera);
  MaderaUISprite.setScale(29.f / Madera.getSize().x, 29.f / Madera.getSize().y);

  sf::Texture Hoja;
  if (!Hoja.loadFromFile("models/Hoja.png"))
    return -1;
  sf::Sprite HojaSprite(Hoja);
  HojaSprite.setScale(rectWidth / Hoja.getSize().x,
                      rectHeight / Hoja.getSize().y);

  sf::Sprite HojaUISprite(Hoja);
  HojaUISprite.setScale(29.f / Hoja.getSize().x, 29.f / Hoja.getSize().y);

  sf::Texture pergaminoTextura;
  if (!pergaminoTextura.loadFromFile("models/Pergamino.png"))
    return -1;
  sf::Sprite pergaminoSprite(pergaminoTextura);

  sf::Texture alquimiaTextura;
  if (!alquimiaTextura.loadFromFile("models/Alquimia.png"))
    return -1;
  sf::Sprite alquimiaSprite(alquimiaTextura);

  sf::Texture inventarioTextura;
  if (!inventarioTextura.loadFromFile("models/Inventario.png")) {
    return -1;
  }
  sf::Sprite inventarioSprite(inventarioTextura);

  sf::Clock animClock;
  int frameActual = 0;
  bool mirandoDerecha = true;

  for (int i = 0; i < 9; ++i) {
    hotbar[i].tipo = 0;
    hotbar[i].cantidad = 0;
  }

  int slotSeleccionado = 0;

  float anchoAlquimia = 288.f;
  float altoAlquimia = 193.f;

  float inventarioAncho = 408.f;
  float inventarioAlto = 390.f;

  while (window.isOpen()) {

    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::Resized) {
        anchoVentana = event.size.width;
        altoVentana = event.size.height;
        view.setSize(anchoVentana, altoVentana);
        uiView = sf::View(sf::FloatRect(0, 0, anchoVentana, altoVentana));

        MitadAncho = anchoVentana / 2.f;
        MitadAlto = altoVentana / 2.f;
        fondoSprite.setScale(anchoVentana / fondoTextura.getSize().x,
                             altoVentana / fondoTextura.getSize().y);
      }

      sf::Vector2f posMouseUI(event.mouseButton.x, event.mouseButton.y);

      if (event.type == sf::Event::MouseButtonPressed &&
          event.mouseButton.button == sf::Mouse::Left) {

        bool recogioCrafteo = false;

        if (inventarioAbierto) {
          float resX = alqX + 3 * alqSlotSize + 69.8f;
          if (posMouseUI.x > resX && posMouseUI.x < resX + alqSlotSize &&
              posMouseUI.y > alqY && posMouseUI.y < alqY + alqSlotSize) {
            if (slotResultado.tipo != 0) {
              añadirAHotbar(slotResultado.tipo);
              slotResultado.tipo = 0;
              slotsAlquimia[0] = Slot();
              slotsAlquimia[1] = Slot();
              slotsAlquimia[2] = Slot();
              recogioCrafteo = true;
            }
          }
        }

        if (!recogioCrafteo)
          iniciarArrastre(posMouseUI);
      }

      if (event.type == sf::Event::MouseButtonReleased &&
          event.mouseButton.button == sf::Mouse::Left)
        soltarArrastre(posMouseUI);

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::E) {
          inventarioAbierto = !inventarioAbierto;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
          int tipoItem = 1;
          añadirAHotbar(tipoItem);
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

    if (hotbar[slotSeleccionado].tipo == 11) {
      nivelActual = objeto.Nivelminado;
      velActual = objeto.Velminado;
      PicoSprite.setPosition(playerPos.x + HitboxSize,
                             playerPos.y - HitboxSize * 4);

    } else if (hotbar[slotSeleccionado].tipo == 10) {
      nivelActual = objeto2.Pnivelminado;
      velActual = objeto2.PVelminado;
      PalaSprite.setPosition(playerPos.x + HitboxSize,
                             playerPos.y - HitboxSize * 4);

    } else if (hotbar[slotSeleccionado].tipo == 0) {
      nivelActual = objeto1.PuNivelminado;
      velActual = objeto1.PuVelminado;
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

          if (herramientaActual == 11) {
            if (tipoBloque == 2)
              puedeMinar = true;
          } else if (herramientaActual == 10) {
            if (tipoBloque == 1 || tipoBloque == 5)
              puedeMinar = true;
          } else {
            if (tipoBloque == 3 || tipoBloque == 4 || tipoBloque == 5 ||
                tipoBloque == 1)
              puedeMinar = true;
          }

          if (puedeMinar && nivelActual >= tipoBloque) {
            mundo[bloqueRow][bloqueCol] = 0;
            añadirAHotbar(tipoBloque);
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

    fondoSprite.setPosition(view.getCenter().x - MitadAncho,
                            view.getCenter().y - MitadAlto);
    window.draw(fondoSprite);

    window.draw(PalaSprite);
    window.draw(PicoSprite);

    int colInicio =
        std::max(0, (int)((view.getCenter().x - MitadAncho) / rectWidth));
    int colFin = std::min(
        COLS, (int)((view.getCenter().x + MitadAncho) / rectWidth) + 1);
    int filaInicio =
        std::max(0, (int)((view.getCenter().y - MitadAlto) / rectHeight));
    int filaFin = std::min(
        FILAS, (int)((view.getCenter().y + MitadAlto) / rectHeight) + 1);

    for (int i = filaInicio; i < filaFin; ++i) {
      for (int j = colInicio; j < colFin; ++j) {
        if (mundo[i][j] == 1) {
          TierraSprite.setScale(rectWidth / Tierra.getSize().x,
                                rectHeight / Tierra.getSize().y);
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
          HojaSprite.setScale(rectWidth / Hoja.getSize().x,
                              rectHeight / Hoja.getSize().y);
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

    //  Inv

    float slotSize = 29.f;
    float startX = (anchoVentana - 9 * slotSize) / 2.f;

    float pergaminoAlto = 220.f;
    float pergaminoAncho = pergaminoAlto * (1536.f / 1024.f);
    pergaminoSprite.setScale(pergaminoAncho / pergaminoTextura.getSize().x,
                             pergaminoAlto / pergaminoTextura.getSize().y);
    pergaminoSprite.setPosition(startX - (pergaminoAncho - 9 * slotSize) / 2.f,
                                altoVentana - pergaminoAlto - 5.f);
    window.draw(pergaminoSprite);

    for (int i = 0; i < 9; ++i) {
      sf::RectangleShape slot(sf::Vector2f(slotSize, slotSize));
      slot.setPosition(startX + 3.3f + i * slotSize,
                       altoVentana - pergaminoAlto + 90.f);
      slot.setFillColor(sf::Color(80, 50, 20, 20));
      slot.setOutlineThickness(0.5);
      window.draw(slot);

      if (hotbar[i].tipo == 1) {
        TierraUISprite.setPosition(startX + 3.3f + i * slotSize + 2.f,
                                   altoVentana - pergaminoAlto + 92.f);
        TierraSprite.setPosition(startX + 3.3f + i * slotSize + 2.f,
                                 altoVentana - pergaminoAlto + 92.f);
        window.draw(TierraUISprite);
      } else if (hotbar[i].tipo == 4) {
        HojaUISprite.setPosition(startX + 3.3f + i * slotSize + 2.f,
                                 altoVentana - pergaminoAlto + 92.f);
        HojaSprite.setPosition(startX + 3.3f + i * slotSize + 2.f,
                               altoVentana - pergaminoAlto + 92.f);
        window.draw(HojaUISprite);
      } else if (hotbar[i].tipo == 5) {
        Tierra2UISprite.setPosition(startX + 3.3f + i * slotSize + 2.f,
                                    altoVentana - pergaminoAlto + 92.f);
        Tierra2Sprite.setScale(rectWidth / Tierra2.getSize().x,
                               rectHeight / Tierra2.getSize().y);
        window.draw(Tierra2UISprite);
      } else if (hotbar[i].tipo == 3) {
        MaderaUISprite.setPosition(startX + 3.3f + i * slotSize + 2.f,
                                   altoVentana - pergaminoAlto + 92.f);
        window.draw(MaderaUISprite);
      }

      if (hotbar[i].cantidad > 1) {
        sf::Text cantidad;
        cantidad.setFont(font);
        cantidad.setCharacterSize(12);
        cantidad.setFillColor(sf::Color::White);
        cantidad.setString(std::to_string(hotbar[i].cantidad));
        cantidad.setPosition(startX + 3.3f + i * slotSize + slotSize - 14.f,
                             altoVentana - pergaminoAlto + 90.f + slotSize -
                                 14.f);
        window.draw(cantidad);
      }

      if (hotbar[i].tipo == 10) {
        PicoSprite.setScale(0.2f, 0.2f);
        PicoSprite.setPosition(startX + 0.01f + i * slotSize + 5.f,
                               altoVentana - pergaminoAlto + 95.f);
        window.draw(PicoSprite);
      } else if (hotbar[i].tipo == 11) {
        PalaSprite.setScale(0.2f, 0.2f);
        PalaSprite.setPosition(startX + 0.5f + i * slotSize + 5.f,
                               altoVentana - pergaminoAlto + 95.f);
        window.draw(PalaSprite);
      }
    }

    float invSpriteX = anchoVentana / 2.f - inventarioAncho / 2.f;
    float invSpriteY = altoVentana / 3.f - inventarioAlto / 2.f;
    float offsetX = 50.f;
    float offsetY = 113.f;

    invX = invSpriteX + offsetX;
    invY = invSpriteY + offsetY;

    float alqSpriteX = anchoVentana / 2.f - anchoAlquimia / 2.f;
    float alqSpriteY = altoVentana / 2.11f - altoAlquimia / 4.f;
    float alqOffsetX = 39.8f;
    float alqOffsetY = 76.f;

    alqX = alqSpriteX + alqOffsetX;
    alqY = alqSpriteY + alqOffsetY;

    if (inventarioAbierto) {

      alquimiaSprite.setScale(anchoAlquimia / alquimiaTextura.getSize().x,
                              altoAlquimia / alquimiaTextura.getSize().y);
      alquimiaSprite.setPosition(anchoVentana / 2.f - anchoAlquimia / 2.f,
                                 altoVentana / 2.11f - altoAlquimia / 4.f);
      window.draw(alquimiaSprite);

      inventarioSprite.setScale(inventarioAncho / inventarioTextura.getSize().x,
                                inventarioAlto / inventarioTextura.getSize().y);
      inventarioSprite.setPosition(anchoVentana / 2.f - inventarioAncho / 2.f,
                                   altoVentana / 2.97f - inventarioAlto / 2.f);
      window.draw(inventarioSprite);

      for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 9; ++j) {
          int index = i * 9 + j;
          sf::RectangleShape slot(sf::Vector2f(invSlotSize, invSlotSize));
          slot.setPosition(invX + j * invSlotSize, invY + i * invSlotSize);
          slot.setFillColor(sf::Color(0, 0, 0, 0));
          slot.setOutlineThickness(0.5);
          window.draw(slot);

          if (index < 36 && inventarioSlots[index].tipo != 0) {
            float sx = invX + j * invSlotSize + 2.f;
            float sy = invY + i * invSlotSize + 2.f;
            float sc = invSlotSize / 32.f;

            if (inventarioSlots[index].tipo == 1) {
              TierraUISprite.setPosition(sx, sy);
              window.draw(TierraUISprite);
            } else if (inventarioSlots[index].tipo == 4) {
              HojaUISprite.setPosition(sx, sy);
              window.draw(HojaUISprite);
            } else if (inventarioSlots[index].tipo == 5) {
              Tierra2UISprite.setPosition(sx, sy);
              window.draw(Tierra2UISprite);
            } else if (inventarioSlots[index].tipo == 2) {
              RocaUISprite.setPosition(sx, sy);
              window.draw(RocaUISprite);
            } else if (inventarioSlots[index].tipo == 3) {
              MaderaUISprite.setPosition(sx, sy);
              window.draw(MaderaUISprite);
            } else if (inventarioSlots[index].tipo == 11) {
              PicoSprite.setScale(sc, sc);
              PicoSprite.setPosition(sx, sy);
              window.draw(PicoSprite);
            } else if (inventarioSlots[index].tipo == 10) {
              PalaSprite.setScale(sc, sc);
              PalaSprite.setPosition(sx, sy);
              window.draw(PalaSprite);
            }
          }
        }
      }

      for (int i = 0; i < 3; ++i) {
        sf::RectangleShape slot(sf::Vector2f(alqSlotSize, alqSlotSize));
        slot.setPosition(alqX + i * alqSlotSize, alqY);
        slot.setFillColor(sf::Color(0, 0, 0, 0));
        slot.setOutlineThickness(0.5);
        window.draw(slot);

        if (slotsAlquimia[i].tipo != 0) {
          float sx = alqX + i * alqSlotSize + 2.f;
          float sy = alqY + 2.f;
          float sc = alqSlotSize / 32.f;

          if (slotsAlquimia[i].tipo == 1) {
            TierraUISprite.setPosition(sx, sy);
            window.draw(TierraUISprite);
          } else if (slotsAlquimia[i].tipo == 4) {
            HojaUISprite.setPosition(sx, sy);
            window.draw(HojaUISprite);
          } else if (slotsAlquimia[i].tipo == 3) {
            MaderaUISprite.setPosition(sx, sy);
            window.draw(MaderaUISprite);
          } else if (slotsAlquimia[i].tipo == 2) {
            RocaUISprite.setPosition(sx, sy);
            window.draw(RocaUISprite);
          } else if (slotsAlquimia[i].tipo == 5) {
            Tierra2UISprite.setPosition(sx, sy);
            window.draw(Tierra2UISprite);
          }
        }
      }

      sf::RectangleShape slotRes(sf::Vector2f(alqSlotSize, alqSlotSize));
      slotRes.setPosition(alqX + 3 * alqSlotSize + 69.8f, alqY);
      slotRes.setFillColor(sf::Color(0, 0, 0, 0));
      slotRes.setOutlineThickness(0.5);
      window.draw(slotRes);

      if (slotResultado.tipo == 10) {
        PalaUISprite.setPosition(alqX + 3 * alqSlotSize + 71.f, alqY + 2.f);
        window.draw(PalaUISprite);
      } else if (slotResultado.tipo == 11) {
        PicoUISprite.setPosition(alqX + 3 * alqSlotSize + 71.f, alqY + 2.f);
        window.draw(PicoUISprite);
      }
    }

    sf::Text texto;
    texto.setFont(font);
    texto.setCharacterSize(16);
    texto.setFillColor(sf::Color::White);
    window.draw(texto);

    window.display();
  }
  return 0;
}
