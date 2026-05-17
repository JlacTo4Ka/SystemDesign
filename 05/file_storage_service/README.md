# Задание №4 по предмету System Design
Выполнил: Павловский Алексей М8О-107СВ-25  
Вариант: Файловое хранилище (`file_storage_service`)

## 1. Проектирование документной модели

### Выбор сущностей для переноса в MongoDB

В предыдущей лабораторной работе сервис `file_storage_service` был реализован с использованием реляционной базы данных PostgreSQL. Основные сущности системы:

- Пользователь (`users`)
- Папки (`folders`)
- Файлы (`files`)

В рамках данной лабораторной работы в MongoDB были перенесены:

- Папки (`folders`)
- Файлы (`files`)

При этом сущность пользователей (`users`), а также связанные с ней данные (аутентификация, поиск пользователя, логин) остаются в PostgreSQL.

### Обоснование выбора

- Пользователи являются отдельной сущностью системы и уже реализованы в PostgreSQL, поэтому не переносятся в MongoDB
- Папка и файлы образуют иерархическую структуру, которая хорошо подходит для документной модели
- При работе с файловым сервисом чаще всего требуется получить папку вместе со списком файлов
- MongoDB удобно подходит для хранения вложенных структур и массивов объектов

### Выбор коллекций

В MongoDB была спроектирована следующая коллекция:

- `folders`

Дополнительно отдельная коллекция для файлов **не создается**, так как файлы логически принадлежат папке и в рамках данной модели не используются отдельно от нее.

### Структура документа

Документ коллекции `folders` имеет следующую структуру:

```json
{
  "_id": 1001,
  "owner_user_id": 87,
  "name": "projects_1001",
  "category": "projects",
  "shared": false,
  "created_at": ISODate("2026-04-20T10:00:00Z"),
  "last_accessed_at": ISODate("2026-04-20T10:30:00Z"),
  "tags": ["project", "work", "important"],
  "settings": {
    "color": "blue",
    "pinned": true,
    "access_level": "private"
  },
  "files": [
    {
      "id": 1,
      "name": "invoice_4413.txt",
      "storage_key": "users/662/folders/360/1_invoice_4413.txt",
      "download_url": "https://storage.example.com/users/662/folders/360/1_invoice_4413.txt",
      "mime_type": "text/plain",
      "size_bytes": 6736,
      "created_at": ISODate("2026-04-20T10:05:00Z"),
      "tags": ["invoice", "text"],
      "metadata": {
        "extension": "txt",
        "source": "postgres-seed"
      },
      "is_deleted": false
    }
  ]
}
```

### Embedded documents и References

В разработанной модели используются оба подхода:

#### Embedded documents (вложенные документы)
- Файлы (`files`) хранятся внутри документа папки
- Каждая папка содержит массив файлов

#### References (ссылки)
- Связь с пользователем реализована через поле `owner_user_id`
- Пользователь хранится в PostgreSQL и не дублируется в MongoDB

### Обоснование использования embedded

Выбор вложенной модели для файлов обусловлен следующими причинами:

1. **Логическая зависимость**  
   Файл в рамках данной модели принадлежит папке

2. **Совместное использование данных**  
   При получении папки обычно требуется список всех файлов

3. **Снижение количества запросов**  
   Один запрос вместо нескольких, так как все данные папки находятся в одном документе

4. **Производительность**  
   Содержимое папки читается одним обращением к коллекции

### Обоснование использования reference

Поле `owner_user_id` хранится как ссылка, так как:

- Пользователи находятся в PostgreSQL
- Пользовательские данные не требуется дублировать в MongoDB
- В рамках проекта уже существует отдельная логика работы с пользователями через PostgreSQL

---

## 2. Создание базы данных и коллекций

### Создание MongoDB

Для работы с базой данных MongoDB был использован Docker.  
MongoDB была добавлена в существующий проект как второй backend, при этом PostgreSQL и ранее реализованный API не изменялись.

Для запуска базы данных в `docker-compose.yml` был добавлен сервис `mongo`:

```yaml
mongo:
  image: mongo:7
  container_name: file_storage_mongo
  restart: unless-stopped
  ports:
    - "27017:27017"
  environment:
    MONGO_INITDB_ROOT_USERNAME: mongo
    MONGO_INITDB_ROOT_PASSWORD: mongo
    MONGO_INITDB_DATABASE: file_storage_mongo
  volumes:
    - mongo_data:/data/db
    - ./mongo/data.js:/docker-entrypoint-initdb.d/data.js:ro
  healthcheck:
    test: ["CMD", "mongosh", "--quiet", "--username", "mongo", "--password", "mongo", "--authenticationDatabase", "admin", "--eval", "db.adminCommand({ ping: 1 })"]
    interval: 10s
    timeout: 5s
    retries: 10
```

### Создание коллекции

В рамках лабораторной работы используется MongoDB база данных:

- `file_storage_mongo`

Для хранения данных была создана коллекция:

- `folders`

Данная коллекция содержит документы папок, внутри которых хранятся вложенные документы файлов.

### Структура коллекции

Каждый документ коллекции `folders` содержит:

- идентификатор папки
- идентификатор владельца
- имя папки
- категорию
- признак совместного доступа
- дату создания
- дату последнего доступа
- массив тегов
- объект `settings`
- массив вложенных документов `files`

Каждый вложенный документ `file` содержит:

- идентификатор файла
- имя файла
- `storage_key`
- `download_url`
- `mime_type`
- `size_bytes`
- дату создания
- массив тегов
- объект `metadata`
- признак `is_deleted`

### Добавление тестовых данных

Для заполнения базы данных был подготовлен файл `data.js`.

В данном файле выполняется:

- выбор базы данных `file_storage_mongo`;
- очистка коллекции `folders`;
- вставка тестовых документов в коллекцию `folders`.

В коллекцию было добавлено 10 документов папок, каждый документ содержит вложенный массив файлов.

### Используемые типы данных MongoDB

При заполнении коллекции были использованы следующие типы данных MongoDB:

- **Text** — имя папки, категория, имя файла, `storage_key`, `download_url`, `mime_type`
- **Number** — идентификаторы папок, пользователей, файлов, размер файла
- **ISODate("...")** — дата создания папки, дата последнего доступа, дата создания файла
- **Array** — массивы `tags` и `files`
- **Embedded Document** — объект `settings`, объект `metadata`, вложенные документы файлов
- **Bool** — поля `shared`, `pinned`, `is_deleted`

### Проверка корректности запуска MongoDB

После запуска контейнеров были выполнены команды:

```bash
docker exec -it file_storage_mongo mongosh -u mongo -p mongo --authenticationDatabase admin
```

Внутри `mongosh`:

```javascript
show dbs
use file_storage_mongo
show collections
db.folders.countDocuments()
db.folders.findOne()
```

Результат показал, что:

- база `file_storage_mongo` успешно создана
- коллекция `folders` существует
- в коллекции содержится 10 документов

---

## 3. Реализация CRUD операций

### Операции Create

На этапе создания данных были реализованы следующие операции:

- добавление новой папки через `insertOne`
- добавление нескольких папок через `insertMany`
- добавление нового файла в массив `files` через `$push`

### Операции Read

Для чтения данных были реализованы следующие запросы:

- получение списка всех папок
- получение папки по `_id`
- поиск папок по `owner_user_id`
- поиск по имени папки
- поиск по нескольким условиям
- поиск папки, содержащей файл с определенным условием

При выполнении запросов используются операторы:

- `$eq`
- `$ne`
- `$gt`
- `$lt`
- `$in`
- `$and`
- `$or`

Также используется:

- `find`
- `findOne`
- `$elemMatch`

### Операции Update

Для изменения данных были реализованы:

- обновление имени папки
- обновление поля `settings.access_level` у нескольких документов
- изменение имени вложенного файла
- добавление элемента в массив через `$addToSet`

### Операции Delete

Для удаления данных были реализованы:

- удаление папки через `deleteOne`
- удаление нескольких папок через `deleteMany`
- удаление файла из массива `files` через `$pull`

### Особенности реализации

Так как файлы хранятся внутри документа папки, операции с файлами выполняются через обновление массива `files`, а не через отдельную коллекцию.

Это соответствует выбранной документной модели и позволяет выполнять основные действия с папками и файлами без использования JOIN-операций.

MongoDB запросы для всех операций находятся в файле `queries.js`.

---

## 4. Валидация схем

### Выбор коллекции для валидации

В рамках лабораторной работы валидация была реализована для коллекции:

- `folders`

Данная коллекция является основной в разработанной документной модели и содержит как данные папки, так и вложенные документы файлов.

### Проверяемые поля

Для документа папки были определены обязательные поля:

- `_id`
- `owner_user_id`
- `name`
- `category`
- `shared`
- `created_at`
- `last_accessed_at`
- `tags`
- `settings`
- `files`

Для вложенного документа файла обязательными являются:

- `id`
- `name`
- `storage_key`
- `download_url`
- `mime_type`
- `size_bytes`
- `created_at`
- `tags`
- `metadata`
- `is_deleted`

### Ограничения схемы

В схеме были заданы следующие ограничения:

- `name` папки должен быть строкой длиной не менее 3 символов
- `category` должна входить в допустимый список значений (`enum`)
- `shared` должен иметь тип `bool`
- `created_at` и `last_accessed_at` должны иметь тип `date`
- `tags` должны быть массивом строк
- `settings` должен быть объектом с обязательными полями `color`, `pinned`, `access_level`
- `files` должен быть массивом объектов
- `name` файла должен быть строкой длиной не менее 3 символов
- `storage_key` должен соответствовать шаблону `^users/.+`
- `download_url` должен соответствовать шаблону `^https://storage.example.com/.+`
- `size_bytes` должен быть целым числом больше 0
- `is_deleted` должен иметь тип `bool`

### Реализация

Валидация была оформлена в файле `validation.js`.

Для задания схемы используется оператор `collMod` и валидатор `$jsonSchema`.

### Проверка работы валидации

Для проверки корректности работы схемы была выполнена вставка валидного и невалидного документов.

Пример валидной вставки:

```javascript
db.folders.insertOne({
  _id: 2001,
  owner_user_id: 1001,
  name: "mongo_valid_folder",
  category: "documents",
  shared: false,
  created_at: new Date("2026-04-21T14:00:00Z"),
  last_accessed_at: new Date("2026-04-21T14:10:00Z"),
  tags: ["mongo", "valid"],
  settings: {
    color: "blue",
    pinned: true,
    access_level: "private"
  },
  files: [
    {
      id: 1,
      name: "valid_file.pdf",
      storage_key: "users/1001/folders/2001/1_valid_file.pdf",
      download_url: "https://storage.example.com/users/1001/folders/2001/1_valid_file.pdf",
      mime_type: "application/pdf",
      size_bytes: 1024,
      created_at: new Date("2026-04-21T14:05:00Z"),
      tags: ["pdf"],
      metadata: {
        extension: "pdf",
        source: "validation-test"
      },
      is_deleted: false
    }
  ]
})
```

Пример невалидной вставки:

```javascript
db.folders.insertOne({
  _id: 2002,
  owner_user_id: 1002,
  name: "bad_folder",
  category: "wrong_category",
  shared: "false",
  created_at: "not_a_date",
  last_accessed_at: new Date("2026-04-21T15:05:00Z"),
  tags: ["bad"],
  settings: {
    color: "red",
    pinned: false,
    access_level: "private"
  },
  files: []
})
```

Ожидаемый результат:

- MongoDB отклоняет вставку документа
- в ответе возвращается сообщение об ошибке валидации

---

## 5. Индексы

Для ускорения выполнения запросов были созданы индексы:

- по полю `owner_user_id`
- по полю `name`
- по полю `category`
- по полю `shared`
- по вложенному полю `files.name`
- по вложенному полю `files.size_bytes`

Создание индексов происходит в файле `indexes.js`.

---

## 6. Подключение базы данных MongoDB к API

На данном этапе в API были переведены сущности:

- `folders`

Для этого в приложение был добавлен компонент `mongo-folder-db`, использующий MongoDB через `userver::components::Mongo`.

В слое доступа к данным был реализован отдельный класс `MongoFolderStorage`, через который выполняются следующие операции:

- создание папки
- получение списка папок
- удаление папки

При этом остальные сущности системы продолжают использовать PostgreSQL:

- пользователи
- аутентификация
- файлы
- старая реализация папок из прошлой лабораторной работы

Таким образом, в сервисе используется комбинированная схема хранения:

- **PostgreSQL** — для пользовательских и уже реализованных сущностей
- **MongoDB** — для документного хранения папок и вложенных файлов

Для MongoDB были добавлены отдельные endpoint-ы:

- `POST /v2/mongo/folders`
- `GET /v2/mongo/folders`
- `DELETE /v2/mongo/folders/{id}`

Старые PostgreSQL endpoint-ы при этом не изменялись.

### Проверка работы MongoDB API

В ходе тестирования были выполнены следующие действия:

1. Через `POST /v1/users` был создан пользователь
2. Через `POST /v1/auth/login` был получен JWT
3. Через `POST /v2/mongo/folders` была создана папка `mongo_docs`
4. Через `GET /v2/mongo/folders` был получен список MongoDB-папок
5. Через `mongosh` было подтверждено наличие созданного документа в коллекции `folders`
6. Через `DELETE /v2/mongo/folders/{id}` созданная папка была успешно удалена

Пример успешного ответа при создании папки:

```json
{"id":1776710245,"name":"mongo_docs","owner_user_id":1001}
```

Пример документа в MongoDB:

```javascript
{
  _id: 1776710245,
  owner_user_id: 1001,
  name: 'mongo_docs',
  category: 'documents',
  shared: false,
  created_at: ISODate('2026-04-20T18:37:25.812Z'),
  last_accessed_at: ISODate('2026-04-20T18:37:25.812Z'),
  tags: [],
  settings: { color: 'blue', pinned: false, access_level: 'private' },
  files: []
}
```

---

## 7. Запуск проекта

### 1. Перейти в каталог сервиса

```bash
cd file_storage_service
```

### 2. Поднять проект

```bash
docker compose up -d
```

### 3. Проверить PostgreSQL

```bash
docker exec -it jwt_auth_postgres psql -U postgres -d file_storage_db
```

Внутри PostgreSQL:

```sql
SELECT COUNT(*) FROM users;
SELECT COUNT(*) FROM folders;
SELECT COUNT(*) FROM files;
```

### 4. Проверить MongoDB

```bash
docker exec -it file_storage_mongo mongosh -u mongo -p mongo --authenticationDatabase admin
```

Внутри MongoDB:

```javascript
show dbs
use file_storage_mongo
show collections
db.folders.countDocuments()
db.folders.findOne()
db.folders.getIndexes()
```

Ожидается, что коллекция `folders` существует и содержит 10 тестовых документов, а после ручного тестирования может содержать дополнительные документы.

### 5. Проверить API

```bash
curl.exe -i http://localhost:8080/ping
```

Ожидается:

```http
HTTP/1.1 200 OK
```

### 6. Пример проверки MongoDB endpoint-ов

Создание пользователя:

```bash
curl.exe -i -X POST "http://localhost:8080/v1/users" -H "Content-Type: application/json" --data-binary "@user.json"
```

Логин:

```bash
curl.exe -i -X POST "http://localhost:8080/v1/auth/login" -H "Content-Type: application/json" --data-binary "@login.json"
```

Создание MongoDB-папки:

```bash
curl.exe -i -X POST "http://localhost:8080/v2/mongo/folders" -H "Content-Type: application/json" -H "Authorization: Bearer TOKEN" --data-binary "@mongo-folder.json"
```

Получение списка MongoDB-папок:

```bash
curl.exe -i "http://localhost:8080/v2/mongo/folders"
```

Удаление MongoDB-папки:

```bash
curl.exe -i -X DELETE "http://localhost:8080/v2/mongo/folders/FOLDER_ID" -H "Authorization: Bearer TOKEN"
```

### 7. Запуск тестов

```bash
make test-debug
```
