# TipTranslate 🖥️📖

Desktop app em **C++ / Qt** para **capturar uma área da tela, rodar OCR e traduzir o texto automaticamente** (ex: com DeepL API).  
Focado em produtividade para quem lê conteúdo em outros idiomas (documentações, artigos, jogos, etc).

> **Stack:** Qt 6, C++17, Tesseract OCR, DeepL API (ou similar)

---

## ✨ Funcionalidades

- 🧊 **Ícone na bandeja do sistema** (System Tray)
- ⌨️ **Atalho global** `Ctrl + Shift + T` para iniciar captura
- 🖼️ **Seleção de área da tela** com overlay (arrastar para marcar o retângulo)
- 🔍 **OCR da área selecionada** usando Tesseract
- 🌎 **Tradução automática** do texto reconhecido (ex.: via API do DeepL)
- 🪟 **Janela flutuante** para exibir o resultado (texto original + tradução)
- ⚙️ Configurações de:
  - idioma de origem/destino
  - chave de API (DeepL)
  - atalho global (futuro)

---

## 🎯 Objetivo do projeto

Este projeto foi criado com dois objetivos principais:

1. **Resolver um problema real**: ler conteúdo em inglês/idiomas estrangeiros de forma rápida, sem precisar copiar/colar manualmente em tradutores online.
2. **Servir como projeto de portfólio** em C++/Qt, mostrando:
   - uso de Qt Widgets
   - integração com APIs HTTP
   - uso de OCR (Tesseract)
   - hotkeys globais no Windows
   - empacotamento com `windeployqt`

---

## 🧱 Arquitetura (resumida)

- `main.cpp`  
  Inicializa o `QApplication`, cria o ícone de tray e configura o menu (Capturar / Sair).  
  Também conecta o atalho global (Ctrl+Shift+T) com o fluxo de captura.

- `HotkeyListener`  
  Implementa `QAbstractNativeEventFilter` no Windows e registra um **global hotkey**:
  - `Ctrl + Shift + T` → dispara sinal `hotkeyPressed()`.

- `OverlaySelect`  
  Tela semi-transparente em full screen que permite o usuário selecionar retângulos com o mouse.  
  Emite um sinal `areaSelected(QRect, QPixmap)` com:
  - a área selecionada
  - o print da tela recortado

- `FloatingPane`  
  Pequena janela flutuante exibindo:
  - texto OCR
  - texto traduzido
  (e futuramente botões de copiar, re-traduzir, etc.)

- `Translator`  
  Classe responsável pela chamada HTTP à API de tradução (ex.: DeepL).  
  - Usa `QNetworkAccessManager`
  - Método `translate(const QString &text)`
  - Emite sinal `translated(const QString &text)`

---

## 🧩 Tecnologias usadas

- **Linguagem:** C++17
- **Framework GUI:** Qt 6 (Qt Widgets)
- **OCR:** Tesseract (via vcpkg)
- **Tradução:** DeepL API (endpoint `https://api-free.deepl.com`)
- **Build system:** CMake
- **Plataforma alvo:** Windows (testado em Windows 10/11)

---

## 🚀 Como rodar localmente (Windows)

### 1. Pré-requisitos

- [API DeepL] Chave de Api previamente configurada.
- [Qt 6.x](https://www.qt.io/download)
- [CMake](https://cmake.org/)
- [vcpkg](https://github.com/microsoft/vcpkg) instalado e configurado
- Tesseract instalado via vcpkg:
```bash
vcpkg install tesseract:x64-windows leptonica:x64-windows
```

### 2. Clonar o repositório
```bash
git clone https://github.com/SEU_USUARIO/tiptranslate.git
cd tiptranslate
```

### 3. Adicionar idiomas

Por padrão o Tesseract vem, em geral, com suporte a inglês.
Para usar outros idiomas (ex.: português, espanhol, etc.):

Vá até o repositório oficial de dados de idioma do Tesseract:

https://github.com/tesseract-ocr/tessdata

Baixe os arquivos .traineddata dos idiomas desejados, por exemplo:
por.traineddata (português)
eng.traineddata (inglês)
spa.traineddata (espanhol)

Localize a pasta tessdata usada pela sua instalação do Tesseract. Exemplos comuns no Windows:
Tesseract via vcpkg: algo como
C:\vcpkg\installed\x64-windows\share\tessdata\
ou
C:\vcpkg\installed\x64-windows\share\tesseract\tessdata\
Copie os arquivos .traineddata baixados para dentro da pasta tessdata.
Observação: o app assume que o Tesseract consegue localizar a pasta tessdata via PATH/variáveis de ambiente padrão da instalação. Caso necessário, você pode ajustar o caminho no código.


### 4. Gerar o build (CMake + Qt)

Com CMake (ajuste caminhos conforme seu ambiente):

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Ou abra o projeto diretamente no Qt Creator (CMakeLists.txt) e configure um kit com MSVC + Qt 6.


### 5. Criar o config.ini na pasta de build

O aplicativo lê suas configurações a partir de um arquivo config.ini localizado na mesma pasta do executável, ou seja, dentro da pasta de build (onde está o tiptranslate.exe).

Após compilar, vá até a pasta de build e crie um arquivo chamado config.ini neste diretório (mesmo nível do tiptranslate.exe).

Exemplo de conteúdo mínimo do config.ini:

```ini

[ocr]
# Idioma padrão para OCR (deve existir em tessdata)
lang=eng+por

[deepl]
api_key=API-KEY-AQUI
source=auto
target=pt

```


### 6. Ao executar
O aplicativo deve aparecer na bandeja do sistema.
Pressione Ctrl + Shift + T para iniciar uma captura da tela, rodar o OCR e exibir a tradução.
Pressione ESC para fechar a tela da tradução
