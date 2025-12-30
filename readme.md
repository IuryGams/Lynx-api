# Lynx API — Banco de Dados SQLite com Docker

Este projeto utiliza **SQLite** rodando dentro de um **container Docker** para padronizar o ambiente de desenvolvimento, facilitar testes e versionar o schema do banco de dados. A API é desenvolvida em **C++** usando o **Crow**.

> ⚠️ SQLite não é um servidor. O Docker é usado apenas para gerenciar o arquivo `.db` e executar scripts de inicialização.

---

## 📦 Pré-requisitos

Antes de rodar o projeto, você precisa ter instalado e configurado o seguinte:

1. **PowerShell 5+** ou outro terminal compatível.
2. **CMake 3.25+** (para configurar e buildar o projeto).
3. **Visual Studio 2019 ou superior** (com suporte a C++ e build tools).
4. **vcpkg** para gerenciar dependências C++:
   - Clone o repositório do vcpkg: `git clone https://github.com/microsoft/vcpkg.git`
   - Vá para a pasta do vcpkg: `cd vcpkg`
   - Compile o vcpkg: `.ootstrap-vcpkg.bat`
   - Adicione o vcpkg ao PATH do usuário ou use `CMAKE_TOOLCHAIN_FILE` no preset.
5. **Docker Desktop** (com Docker Compose habilitado) para rodar o SQLite.

> Certifique-se de que o Docker e o vcpkg estejam acessíveis pelo PATH.

---

## 🚀 Rodando o projeto

### 1️⃣ Subir o banco de dados no Docker

1. Abra o terminal na pasta do projeto.
2. Suba o container com o banco de dados e execute o script `init.sql`:

```powershell
docker compose up -d --build
```

3. Verifique se o container está rodando:

```powershell
docker ps
```

4. Para acessar o banco SQLite dentro do container:

```powershell
docker exec -it lynx_sqlite sqlite3 /db/lynx.db
sqlite> .tables
sqlite> SELECT * FROM customers;
```

> O Docker cria o arquivo `lynx.db` e mantém o container rodando em background.

### 2️⃣ Rodar o projeto com build.ps1

Depois de instalar os pré-requisitos e subir o Docker, você pode rodar o projeto com o script `build.ps1`:

```powershell
# Abra o PowerShell na pasta do projeto
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process  # permite execução de scripts temporariamente
.\build.ps1 -Config Debug  # ou Release
```

O script vai automaticamente:
1. Configurar o projeto com CMake usando o preset `crow_api`.
2. Buildar o projeto no modo Debug ou Release.
3. Executar o executável da API.

> A API estará disponível em **http://localhost:8000** e o SQLite estará rodando no container Docker.
