# Лабораторная работа №5 по предмету System Design

**Выполнил:** Павловский Алексей Валерьевич М8О-107СВ-25
**Вариант 11:** Сервис хранения файлов (`file_storage_service`)

## Описание работы

В данной лабораторной работе в уже существующий проект `file_storage_service` были добавлены механизмы повышения производительности и устойчивости системы:

* кеширование через Redis;
* rate limiting для критичного endpoint-а авторизации;
* использование Redis одновременно для кеша и для хранения счётчиков ограничений.

Проект продолжает архитектуру предыдущих лабораторных работ:

* PostgreSQL используется для пользователей, авторизации, папок и файлов;
* MongoDB используется для документного хранения папок;
* Redis используется как in-memory key-value хранилище для кеша и rate limiting.

## 1. Анализ производительности

В рамках лабораторной работы рассматривается файловый сервис, предоставляющий REST API для авторизации, регистрации пользователей, работы с папками и файлами.

### Основные хранилища данных

| Хранилище  | Назначение                              |
| ---------- | --------------------------------------- |
| PostgreSQL | пользователи, авторизация, папки, файлы |
| MongoDB    | документное хранение папок              |
| Redis      | кеширование и счётчики rate limiting    |

### Часто выполняемые операции

| Endpoint                       | Описание                             | Причина высокой нагрузки                        |
| ------------------------------ | ------------------------------------ | ----------------------------------------------- |
| `POST /v1/auth/login`          | авторизация пользователя             | чувствителен к brute-force и повторным запросам |
| `GET /v1/folders`              | получение списка папок из PostgreSQL | часто используется при отображении интерфейса   |
| `GET /v1/files/by-name/{name}` | получение файла по имени             | частое чтение данных                            |
| `GET /v2/mongo/folders`        | получение списка папок из MongoDB    | часто вызывается при чтении документных данных  |

### Потенциально медленные операции

Потенциально медленными являются операции, связанные с обращением к базам данных:

* проверка логина и пароля в PostgreSQL;
* получение списка папок из PostgreSQL;
* получение списка папок из MongoDB;
* поиск файла по имени.

### Требования к производительности

| Метрика       | Описание                         | Целевое значение                                        |
| ------------- | -------------------------------- | ------------------------------------------------------- |
| Response Time | время от запроса до ответа       | до 100–150 мс для чтения, до 150–200 мс для авторизации |
| Throughput    | количество запросов в секунду    | должно сохраняться стабильным при повторных чтениях     |
| Stability     | устойчивость к всплескам трафика | один клиент не должен перегружать `/v1/auth/login`      |

### Вывод

Наиболее удобными кандидатами для кеширования в сервисе являются endpoint-ы получения списков папок:

* `GET /v1/folders`
* `GET /v2/mongo/folders`

Для rate limiting выбран endpoint:

* `POST /v1/auth/login`

## 2. Стратегия кеширования

Для повышения производительности используется Redis.

### Выбранные endpoint-ы

Кеширование реализовано для следующих endpoint-ов:

| Endpoint                | Источник данных | Ключ в Redis              | TTL       |
| ----------------------- | --------------- | ------------------------- | --------- |
| `GET /v1/folders`       | PostgreSQL      | `cache:folders:pg:all`    | 60 секунд |
| `GET /v2/mongo/folders` | MongoDB         | `cache:folders:mongo:all` | 60 секунд |

### Используемая стратегия

Используется стратегия **Cache-Aside**:

1. Сервис сначала проверяет наличие данных в Redis.
2. Если данные найдены — возвращает их из кеша.
3. Если данных нет — обращается в базу.
4. Полученный результат сериализуется в JSON и сохраняется в Redis с TTL.
5. Ответ возвращается клиенту.

### Инвалидация кеша

При изменении списка папок кеш удаляется.

#### PostgreSQL

* `POST /v1/folders`
* `DELETE /v1/folders/{folder_id}`

#### MongoDB

* `POST /v2/mongo/folders`
* `DELETE /v2/mongo/folders/{folder_id}`

### Проверка кеша

После вызова:

```bash
curl http://localhost:8080/v1/folders
curl http://localhost:8080/v2/mongo/folders
```

в Redis появляются ключи:

* `cache:folders:pg:all`
* `cache:folders:mongo:all`

Проверка:

```bash
docker exec -it file_storage_redis redis-cli
KEYS *
TTL cache:folders:pg:all
TTL cache:folders:mongo:all
GET cache:folders:mongo:all
```

## 3. Проектирование rate limiting

Rate limiting реализован для защиты endpoint-а авторизации.

### Выбранный endpoint

| Endpoint              | Причина ограничения                | Уровень риска |
| --------------------- | ---------------------------------- | ------------- |
| `POST /v1/auth/login` | возможен brute-force подбор пароля | высокий       |

### Выбранный алгоритм

Используется алгоритм **Fixed Window Counter**.

Принцип работы:

1. Для клиента формируется Redis-ключ.
2. В Redis хранится число запросов за окно 60 секунд.
3. При каждом запросе счётчик увеличивается.
4. Если лимит превышен — возвращается `429 Too Many Requests`.
5. По TTL ключ автоматически удаляется после окончания окна.

### Ограничение

| Endpoint              | Алгоритм             | Лимит                   | Ключ Redis                     | TTL       |
| --------------------- | -------------------- | ----------------------- | ------------------------------ | --------- |
| `POST /v1/auth/login` | Fixed Window Counter | 10 запросов / 60 секунд | `rate_limit:login:{client_ip}` | 60 секунд |

В локальном тестировании использовался ключ:

* `rate_limit:login:local-client`

### Заголовки rate limiting

В ответах используются заголовки:

* `X-RateLimit-Limit`
* `X-RateLimit-Remaining`
* `X-RateLimit-Reset`

### Проверка работы

Один успешный запрос:

```bash
curl -i -X POST "http://localhost:8080/v1/auth/login" \
  -H "Content-Type: application/json" \
  --data-binary "@login.json"
```

Пример результата:

* `HTTP 200 OK`
* `X-RateLimit-Limit: 10`
* `X-RateLimit-Remaining: 9`
* `X-RateLimit-Reset: 60`

Проверка превышения лимита:

```bash
for i in {1..12}; do
  curl -i -X POST "http://localhost:8080/v1/auth/login" \
    -H "Content-Type: application/json" \
    --data-binary "@login.json"
done
```

Результат:

* первые 10 запросов — `200 OK`;
* следующие запросы — `429 Too Many Requests`.

### Проверка через Redis MONITOR

```bash
docker exec -it file_storage_redis redis-cli MONITOR
```

В процессе тестирования были зафиксированы команды:

```text
"get" "rate_limit:login:local-client"
"set" "rate_limit:login:local-client" "1" "PX" "60000"
"set" "rate_limit:login:local-client" "2" "PX" "60000"
"set" "rate_limit:login:local-client" "3" "PX" "60000"
```

Это подтверждает хранение счётчика в Redis.

## 4. Анализ влияния механизмов

### Влияние кеширования

Кеширование позволяет:

* снизить число повторных обращений к PostgreSQL и MongoDB;
* уменьшить время ответа для повторных запросов;
* повысить пропускную способность при чтении списков папок.

### Влияние rate limiting

Rate limiting позволяет:

* ограничить число запросов от одного клиента;
* защитить `/v1/auth/login` от злоупотреблений;
* уменьшить риск brute-force атак;
* повысить устойчивость API к всплескам запросов.

### Метрики для мониторинга

| Метрика            | Назначение                                |
| ------------------ | ----------------------------------------- |
| Response Time      | время обработки одного запроса            |
| Throughput         | количество запросов в секунду             |
| Cache Hit Rate     | эффективность кеширования                 |
| Rate Limit Rejects | количество отклонённых запросов по лимиту |

## 5. Запуск проекта

### 5.1. Перейти в каталог проекта

```bash
cd file_storage_service
```

### 5.2. Поднять все сервисы

```bash
docker compose up --build -d
```

После запуска должны работать:

| Сервис     | Порт    |
| ---------- | ------- |
| API        | `8080`  |
| PostgreSQL | `5432`  |
| MongoDB    | `27017` |
| Redis      | `6379`  |

### 5.3. Проверка доступности API

```bash
curl http://localhost:8080/ping
```

Ожидается ответ `HTTP 200 OK`.

### 5.4. Проверка Redis

```bash
docker exec -it file_storage_redis redis-cli ping
```

Ожидаемый результат:

```text
PONG
```

### 5.5. Проверка кеширования

```bash
curl http://localhost:8080/v1/folders
curl http://localhost:8080/v2/mongo/folders
```

После этого:

```bash
docker exec -it file_storage_redis redis-cli
KEYS *
TTL cache:folders:pg:all
TTL cache:folders:mongo:all
GET cache:folders:mongo:all
```

### 5.6. Проверка rate limiting

Подготовить файл `login.json`:

```json
{
  "login": "alex@example.com",
  "password": "123456"
}
```

Проверка одного запроса:

```bash
curl -i -X POST "http://localhost:8080/v1/auth/login" \
  -H "Content-Type: application/json" \
  --data-binary "@login.json"
```

Проверка серии запросов:

```bash
for i in {1..12}; do
  curl -i -X POST "http://localhost:8080/v1/auth/login" \
    -H "Content-Type: application/json" \
    --data-binary "@login.json"
done
```

Ожидаемый результат:

* сначала `200 OK`;
* после превышения лимита — `429 Too Many Requests`.

### 5.7. Проверка работы Redis для rate limiting

```bash
docker exec -it file_storage_redis redis-cli MONITOR
```

После выполнения запросов на логин Redis должен показать операции `GET` и `SET` по ключу:

* `rate_limit:login:local-client`

## 6. Запуск тестов

Если используется dev container или Linux-окружение с `make`:

```bash
make test-debug
```

## 7. Итог

В ходе лабораторной работы в сервис `file_storage_service` были добавлены:

* кеширование списков папок из PostgreSQL и MongoDB;
* инвалидация кеша при изменении данных;
* rate limiting для endpoint-а авторизации;
* Redis как общее хранилище для кеша и счётчиков ограничений.

Это позволило повысить производительность и устойчивость сервиса без переработки основной архитектуры проекта.
