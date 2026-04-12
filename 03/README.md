# Задание №3 по предмету System Design

## Выполнил: Павловский Алексей Валерьевич
Группа: М8О-107СВ-25 

## Вариант: Сервис хранения файлов

## 1. Проектирование схемы базы данных

### Сущности и таблицы

Для реализации варианта используются следующие таблицы:

- `users` - пользователи системы;
- `folders` - папки пользователей;
- `files` - файлы, принадлежащие пользователям и размещённые в папках.

Дополнительно в базе может присутствовать служебная таблица `u_clients`, которая создаётся userver для работы с PostgreSQL и не относится к предметной области.

### Таблица `users`

Хранит данные пользователей системы.

Поля:

- `id` BIGSERIAL - первичный ключ;
- `email` VARCHAR(255) - логин пользователя, уникален;
- `password_hash` VARCHAR(255) - пароль пользователя;
- `first_name` VARCHAR(100) - имя;
- `last_name` VARCHAR(100) - фамилия;
- `created_at` TIMESTAMP - дата и время создания записи.

### Таблица `folders`

Хранит папки, принадлежащие пользователям.

Поля:

- `id` BIGSERIAL - первичный ключ;
- `owner_user_id` BIGINT - владелец папки;
- `name` VARCHAR(255) - название папки;
- `created_at` TIMESTAMP - дата и время создания папки.

### Таблица `files`

Хранит файлы, загруженные в систему.

Поля:

- `id` BIGSERIAL - первичный ключ;
- `folder_id` BIGINT - папка, в которой лежит файл;
- `owner_user_id` BIGINT - владелец файла;
- `name` VARCHAR(255) - имя файла;
- `storage_key` TEXT - внутренний ключ хранения;
- `download_url` TEXT - ссылка на скачивание файла;
- `mime_type` VARCHAR(255) - MIME-тип файла;
- `size_bytes` BIGINT - размер файла в байтах;
- `created_at` TIMESTAMP - дата и время создания записи.

### Первичные ключи

Во всех таблицах используются первичные ключи типа `BIGSERIAL`:

- `users.id`
- `folders.id`
- `files.id`

### Внешние ключи

В схеме используются следующие внешние ключи:

- `folders.owner_user_id` -> `users.id`
- `files.folder_id` -> `folders.id`
- `files.owner_user_id` -> `users.id`

### Выбор типов данных

В проекте используются следующие типы данных:

- `BIGSERIAL` - для первичных ключей;
- `BIGINT` - для внешних ключей и поля размера файла;
- `VARCHAR(100)` / `VARCHAR(255)` - для имён, фамилий, email и названий;
- `TEXT` - для `storage_key` и `download_url`;
- `TIMESTAMP` - для дат создания.

### Ограничения целостности

В схеме используются следующие ограничения.

#### `users`

- `email` - `NOT NULL`, `UNIQUE`
- `password_hash` - `NOT NULL`
- `first_name` - `NOT NULL`
- `last_name` - `NOT NULL`

#### `folders`

- `owner_user_id` - `NOT NULL`
- `name` - `NOT NULL`
- одна и та же папка не должна дублироваться у одного пользователя: `UNIQUE (owner_user_id, name)`

#### `files`

- `folder_id` - `NOT NULL`
- `owner_user_id` - `NOT NULL`
- `name` - `NOT NULL`
- `storage_key` - `NOT NULL`, `UNIQUE`
- уникальность имени файла в пределах папки: `UNIQUE (folder_id, name)`
- `size_bytes` - `NOT NULL CHECK (size_bytes >= 0)`

### Связи между таблицами

- `users` -> `folders` - связь 1:N, один пользователь может иметь несколько папок;
- `folders` -> `files` - связь 1:N, одна папка может содержать несколько файлов;
- `users` -> `files` - связь 1:N, один пользователь может владеть несколькими файлами.

## 2. Создание базы данных

### Создание PostgreSQL

Для работы с базой данных PostgreSQL используется Docker, что позволяет запускать проект в изолированной среде и не ставить БД отдельно в систему.

Для запуска базы данных используется сервис `postgres` в файле `docker-compose.yml`:

```yaml
services:
  postgres:
    image: postgres:16
    container_name: jwt_auth_postgres
    environment:
      POSTGRES_DB: file_storage_db
      POSTGRES_USER: postgres
      POSTGRES_PASSWORD: postgres
    ports:
      - "5432:5432"
    volumes:
      - postgres_data:/var/lib/postgresql/data
      - ./sql/schema.sql:/docker-entrypoint-initdb.d/01-schema.sql:ro
      - ./sql/data.sql:/docker-entrypoint-initdb.d/02-data.sql:ro
```

### Создание таблиц

Для создания схемы базы данных подготовлен файл `schema.sql`. В нём описаны:

- все таблицы системы;
- первичные ключи;
- внешние ключи;
- ограничения целостности;
- индексы для ускорения поиска и фильтрации.

Порядок создания таблиц выбран с учётом зависимостей: сначала создаётся таблица `users`, затем `folders`, после этого `files`.

### Добавление тестовых данных

Для заполнения базы данных подготовлен файл `data.sql`.

Он содержит тестовые записи для таблиц:

- `users`
- `folders`
- `files`

Перед тестированием в БД находилось:

- `users` - 1000 записей;
- `folders` - 2000 записей;
- `files` - 30000 записей.

## 3. Создание индексов

### Анализ типовых запросов

В системе выполняются следующие основные операции:

- поиск пользователя по логину;
- поиск пользователя по имени и фамилии;
- получение папок пользователя;
- получение файлов папки;
- получение файлов пользователя;
- поиск файлов по маске имени.

### Созданные индексы

#### Индексы на первичные ключи

Создаются автоматически PostgreSQL.

#### Дополнительные индексы

```sql
CREATE INDEX idx_users_last_name_first_name
    ON users(last_name, first_name);

CREATE INDEX idx_folders_owner_user_id
    ON folders(owner_user_id);

CREATE INDEX idx_files_folder_id
    ON files(folder_id);

CREATE INDEX idx_files_owner_user_id
    ON files(owner_user_id);

CREATE EXTENSION IF NOT EXISTS pg_trgm;

CREATE INDEX idx_files_name_trgm
    ON files USING gin (name gin_trgm_ops);
```

Назначение:

- `idx_users_last_name_first_name` - ускоряет поиск пользователей по фамилии и имени;
- `idx_folders_owner_user_id` - ускоряет получение папок конкретного пользователя;
- `idx_files_folder_id` - ускоряет получение файлов папки;
- `idx_files_owner_user_id` - ускоряет получение файлов пользователя;
- `idx_files_name_trgm` - ускоряет поиск файлов по маске через `ILIKE`.

Часть индексов создаётся автоматически ограничениями `PRIMARY KEY` и `UNIQUE`. Например:

- `users.email`
- `folders(owner_user_id, name)`
- `files(folder_id, name)`
- `files.storage_key`

Эти ограничения формируют соответствующие индексы в PostgreSQL.

## 4. Оптимизация запросов

Для задания были подготовлены SQL-запросы для всех основных операций варианта. Они размещены в файле `queries.sql`.

Анализ плана выполнения, сравнение запросов до и после индексации, а также выводы по ускорению вынесены в файл `optimization.md`.

Основные результаты:

- поиск пользователя по `email` использует индекс, созданный ограничением `UNIQUE`;
- поиск пользователя по имени и фамилии перестаёт выполнять последовательное сканирование после добавления составного индекса;
- получение файлов пользователя и папки заметно ускоряется после индексации внешних ключей;
- поиск файлов по маске `%report%` ускоряется за счёт trigram-индекса.

## 5. Партицирование (опционально)

Партиционирование таблиц в проекте не применялось.

Это связано с тем, что:

- объём данных учебный и сравнительно небольшой;
- необходимые запросы уже ускоряются индексами;
- в рамках работы не требовалось делить данные по диапазонам дат, категориям или пользователям.

## 6. Подключение API к базе данных

В рамках этого этапа сервис был переведён с in-memory хранилища на использование PostgreSQL.

### Выполненные изменения

- в проект добавлен компонент `userver::components::Postgres`;
- в `CMakeLists.txt` подключён компонент `postgresql` и линковка `userver::postgresql`;
- реализован класс `PostgresStorage`, работающий с БД через `ClusterPtr`;
- `StorageComponent` был переведён на получение `Cluster` из `postgres-db`;
- хендлеры создания пользователя, авторизации, работы с папками и файлами были переведены на вызовы PostgreSQL вместо in-memory контейнеров.

### Особенности реализации

- используются параметризованные SQL-запросы (`$1`, `$2`, ...);
- логин пользователя в API соответствует полю `email` в БД;
- при создании файла формируются `storage_key` и `download_url`;
- размер файла сохраняется в поле `size_bytes`;
- взаимодействие с БД выполняется через `userver::storages::postgres::ClusterPtr`.

### Проверка работоспособности

Корректность работы проверялась двумя способами:

- HTTP-запросами (`curl.exe`) к API;
- прямыми SQL-запросами к PostgreSQL через `psql`.

Проверки показали, что после HTTP-запросов изменения сразу отражаются в таблицах базы данных. Это подтверждает, что PostgreSQL используется как основное хранилище данных.

### Пример успешной проверки

#### 1. Проверка запуска сервиса

```bash
curl.exe -i http://localhost:8080/ping
```

Результат:

```http
HTTP/1.1 200 OK
Content-Length: 0
```

Это подтверждает успешный запуск сервиса.

#### 2. Создание пользователя через API

Содержимое файла `user.json`:

```json
{
  "login": "alex@example.com",
  "password": "123456",
  "first_name": "Alex",
  "last_name": "Pavlov"
}
```

Запрос:

```bash
curl.exe -i -X POST "http://localhost:8080/v1/users" -H "Content-Type: application/json" --data-binary "@user.json"
```

Ответ сервиса:

```http
HTTP/1.1 201 Created
{"id":1001,"login":"alex@example.com","first_name":"Alex","last_name":"Pavlov"}
```

После этого в PostgreSQL была выполнена проверка:

```sql
SELECT id, email, password_hash, first_name, last_name
FROM users
WHERE email = 'alex@example.com';
```

Новая запись появилась в таблице `users`, что подтверждает сохранение данных именно в БД.

#### 3. Авторизация пользователя

Содержимое файла `login.json`:

```json
{
  "login": "alex@example.com",
  "password": "123456"
}
```

Запрос:

```bash
curl.exe -i -X POST "http://localhost:8080/v1/auth/login" -H "Content-Type: application/json" --data-binary "@login.json"
```

Ответ сервиса:

```http
HTTP/1.1 200 OK
{"token":"<jwt_token>","user_id":1001,"login":"alex@example.com"}
```

Это подтверждает, что чтение пользователя для авторизации также выполняется из PostgreSQL.

## Запуск проекта

### 1. Перейти в каталог сервиса

```bash
cd file_storage_service
```

### 2. Поднять PostgreSQL и сервис

```bash
docker compose up --build
```

Или в фоне:

```bash
docker compose up -d --build
```

### 3. Проверить health endpoint

```bash
curl.exe -i http://localhost:8080/ping
```

### 4. Подключиться к PostgreSQL внутри контейнера

```bash
docker exec -it jwt_auth_postgres psql -U postgres -d file_storage_db
```

### 5. Проверить состояние таблиц

```sql
SELECT COUNT(*) FROM users;
SELECT COUNT(*) FROM folders;
SELECT COUNT(*) FROM files;
```

### 6. Запуск тестов 

```
make test-debug
```