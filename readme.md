# Lynx API — Banco de Dados SQLite com Docker

Este projeto utiliza **SQLite** rodando dentro de um **container Docker** para padronizar o ambiente de desenvolvimento, facilitar testes e versionar o schema do banco de dados.

> ⚠️ SQLite não é um servidor. O Docker é usado apenas para gerenciar o arquivo `.db` e executar scripts de inicialização.

---

## 📦 Pré-requisitos

- Docker Desktop instalado e em execução
- Docker Compose habilitado
- PowerShell ou terminal compatível

---

## 🚀 Subindo o banco de dados

### 1️⃣ Construir e iniciar o container

```bash
docker compose up -d --build
```

O que esse comando faz:

* Cria a imagem Docker do SQLite
* Executa o script init.sql
* Cria o arquivo lynx.db
* Mantém o container rodando em background (-d)


```bash
docker ps
```

O que esse comando faz:

* Lista os containers em execução
* Confirma se o container lynx_sqlite está ativo


```bash
docker exec -it lynx_sqlite sqlite3 /db/lynx.db
```

O que esse comando faz:

* Abre um terminal interativo dentro do container
* Conecta diretamente ao banco lynx.db