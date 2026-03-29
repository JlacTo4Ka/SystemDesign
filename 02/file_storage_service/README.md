# Домашнее задание 02: Разработка REST API сервиса

**Выполнил:** Павловский Алексей М8О-107СВ-25  
**Вариант 11:** Хранение файлов


---

## 1. Проектирование REST API

### Таблица API Endpoints

| Метод | URL | Описание |
|------|-----|----------|
| POST | /v1/users | Создание нового пользователя |
| GET | /v1/users/by-login/{login} | Поиск пользователя по логину |
| GET | /v1/users/search | Поиск пользователей |
| POST | /v1/auth/login | Аутентификация |
| POST | /v1/folders | Создание папки |
| GET | /v1/folders | Получение списка папок |
| POST | /v1/folders/{folder_id}/files | Создание файла |
| GET | /v1/files/by-name/{name} | Получение файла |
| DELETE | /v1/files/{file_id} | Удаление файла |
| DELETE | /v1/folders/{folder_id} | Удаление папки |
| GET | /ping | Проверка сервиса |

---

## Примеры запросов и статус-коды

### Создание пользователя

```bash
curl -X POST http://localhost:8080/v1/users \
-H "Content-Type: application/json" \
-d '{"login":"user1","password":"123","first_name":"Ivan","last_name":"Ivanov"}'
```

Ответ (201 Created):

```json
{"id":1,"login":"user1","first_name":"Ivan","last_name":"Ivanov"}
```

Возможные статусы:

- 201 - пользователь создан  
- 400 - некорректные данные  
- 409 - пользователь уже существует  
- 500 - ошибка сервера  

---

### Аутентификация

```bash
curl -X POST http://localhost:8080/v1/auth/login \
-H "Content-Type: application/json" \
-d '{"login":"user1","password":"123"}'
```

Ответ (200 OK):

```json
{"token":"JWT_TOKEN"}
```

Возможные статусы:

- 200 - успешно  
- 400 - некорректные данные  
- 401 - неверный логин или пароль  
- 500 - ошибка сервера  

---

### Создание папки

```bash
curl -X POST http://localhost:8080/v1/folders \
-H "Authorization: Bearer <TOKEN>" \
-H "Content-Type: application/json" \
-d '{"name":"my-folder","owner_user_id":1}'
```

Ответ (201 Created):

```json
{"id":1,"name":"my-folder","owner_user_id":1}
```

Возможные статусы:

- 201 - папка создана  
- 400 - некорректные данные  
- 401 - нет токена  
- 500 - ошибка сервера  

---

### Получение списка папок

```bash
curl http://localhost:8080/v1/folders
```

Ответ (200 OK)

Возможные статусы:

- 200 - успешно  
- 500 - ошибка сервера  

---

### Создание файла

```bash
curl -X POST http://localhost:8080/v1/folders/1/files \
-H "Authorization: Bearer <TOKEN>" \
-H "Content-Type: application/json" \
-d '{"name":"file.txt","content":"hello","owner_user_id":1}'
```

Ответ (201 Created)

Возможные статусы:

- 201 - файл создан  
- 400 - некорректные данные  
- 401 - нет токена  
- 404 - папка не найдена  
- 500 - ошибка сервера  

---

### Получение файла по имени

```bash
curl http://localhost:8080/v1/files/by-name/file.txt
```

Ответ (200 OK)

Возможные статусы:

- 200 - успешно  
- 404 - файл не найден  
- 500 - ошибка сервера  

---

### Удаление файла

```bash
curl -X DELETE http://localhost:8080/v1/files/1 \
-H "Authorization: Bearer <TOKEN>"
```

Ответ (200 OK)

Возможные статусы:

- 200 - удалено  
- 401 - нет токена  
- 404 - файл не найден  
- 500 - ошибка сервера  

---

### Удаление папки

```bash
curl -X DELETE http://localhost:8080/v1/folders/1 \
-H "Authorization: Bearer <TOKEN>"
```

Ответ (200 OK)

Возможные статусы:

- 200 - удалено  
- 401 - нет токена  
- 404 - папка не найдена  
- 500 - ошибка сервера  

---

## 2. Реализация

REST API реализован на C++ с использованием Yandex Userver.

### Стек

- C++
- userver
- JSON
- JWT
- Docker

---

### Хранилище

Используется in-memory хранилище, реализованное через:
- `std::unordered_map` - хранение сущностей
- `std::mutex`

Особенности:
- данные хранятся в памяти процесса
- при перезапуске сервиса данные теряются

---

### DTO

- user_dto
- folder_dto
- file_dto
- auth_dto

---

## 3. Аутентификация

JWT:

- login - token
- token - доступ к защищённым endpoint

---

## 4. Документация

Конфигурация API:

```
configs/static_config.yaml
```

---

## 5. Тестирование

```bash
make test-debug
```

### Покрытие

- создание пользователя
- логин
- папки
- файлы
- ошибки

---

## Запуск

```bash
docker compose up --build
```

```
http://localhost:8080
```
