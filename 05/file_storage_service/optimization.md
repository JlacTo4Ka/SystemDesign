# Оптимизация SQL-запросов

В этом разделе приведён анализ основных запросов для системы хранения файлов. Для каждого запроса показано, как меняется план выполнения после добавления индексов или почему отдельный индекс в некоторых случаях не нужен.

Тестирование проводилось на следующих объёмах данных:

- `users` — 1000 записей
- `folders` — 2000 записей
- `files` — 30000 записей

Важно: в нескольких запросах результат содержит `rows=0`, то есть по указанному условию запись не была найдена. Для анализа это не критично, потому что всё равно видно, какой именно план выбирает PostgreSQL: последовательное сканирование таблицы или работу через индекс.

---

## 1. Поиск пользователя по email

### Запрос

```sql
EXPLAIN ANALYZE
SELECT id, email, first_name, last_name, created_at
FROM users
WHERE email = 'alexey.pavlo1@example.com';
```

### План выполнения

```text
Index Scan using users_email_key on users  (cost=0.28..8.29 rows=1 width=59) (actual time=1.417..1.449 rows=0 loops=1)
  Index Cond: ((email)::text = 'alexey.pavlo1@example.com'::text)
Planning Time: 19.008 ms
Execution Time: 3.129 ms
```

### Анализ

Для поля `email` в таблице `users` задано ограничение `UNIQUE`, поэтому PostgreSQL автоматически создал индекс `users_email_key`. В плане выполнения видно, что запрос сразу идёт через `Index Scan`, а не просматривает всю таблицу.

Это логично, потому что поиск пользователя по email — одна из базовых операций сервиса. Например, такой запрос может использоваться при авторизации, регистрации, проверке существования пользователя или получении профиля по логину.

### Вывод

Дополнительный индекс для `email` не нужен, так как он уже существует автоматически из-за `UNIQUE(email)`. Для данного запроса оптимизация уже есть на уровне схемы таблицы.

---

## 2. Поиск пользователя по имени и фамилии

### Запрос

```sql
EXPLAIN ANALYZE
SELECT id, email, first_name, last_name, created_at
FROM users
WHERE last_name = 'Pavlo'
  AND first_name = 'Alexey';
```

### До оптимизации

После удаления составного индекса PostgreSQL выполняет последовательное сканирование таблицы:

```text
Seq Scan on users  (cost=0.00..28.00 rows=2 width=59) (actual time=1.399..1.400 rows=0 loops=1)
  Filter: (((last_name)::text = 'Pavlo'::text) AND ((first_name)::text = 'Alexey'::text))
  Rows Removed by Filter: 1000
Planning Time: 1.313 ms
Execution Time: 1.633 ms
```

### После оптимизации

После создания составного индекса:

```sql
CREATE INDEX idx_users_last_name_first_name
    ON users (last_name, first_name);
```

PostgreSQL использует индексный доступ:

```text
Bitmap Heap Scan on users  (cost=4.30..9.97 rows=2 width=59) (actual time=0.983..0.984 rows=0 loops=1)
  Recheck Cond: (((last_name)::text = 'Pavlo'::text) AND ((first_name)::text = 'Alexey'::text))
  ->  Bitmap Index Scan on idx_users_last_name_first_name  (cost=0.00..4.29 rows=2 width=0) (actual time=0.948..0.949 rows=0 loops=1)
        Index Cond: (((last_name)::text = 'Pavlo'::text) AND ((first_name)::text = 'Alexey'::text))
Planning Time: 2.033 ms
Execution Time: 1.765 ms
```

### Анализ

Без индекса СУБД вынуждена читать всю таблицу `users` и проверять условие для каждой строки. После добавления индекса PostgreSQL переходит на `Bitmap Index Scan` + `Bitmap Heap Scan`.

По времени разница здесь небольшая, потому что таблица `users` пока маленькая — всего 1000 записей. Кроме того, в обоих случаях запрос не нашёл ни одной строки. Но сам план выполнения стал лучше: вместо полного прохода по таблице используется индекс.

Здесь важно не только текущее время, а то, как запрос будет вести себя при росте данных. На большем объёме таблицы составной индекс даст более заметный выигрыш.

### Вывод

Индекс `idx_users_last_name_first_name` полезен для точного поиска пользователя по фамилии и имени. На маленькой таблице ускорение почти незаметно, но с точки зрения плана выполнения и масштабируемости такой индекс оправдан.

---

## 3. Получение папок пользователя

### Запрос

```sql
EXPLAIN ANALYZE
SELECT id, owner_user_id, name, created_at
FROM folders
WHERE owner_user_id = 1;
```

### Вариант с индексом, созданным ограничением `UNIQUE(owner_user_id, name)`

```text
Bitmap Heap Scan on folders  (cost=4.29..10.26 rows=2 width=35) (actual time=1.420..1.421 rows=0 loops=1)
  Recheck Cond: (owner_user_id = 1)
  ->  Bitmap Index Scan on folders_owner_name_unique  (cost=0.00..4.29 rows=2 width=0) (actual time=1.405..1.406 rows=0 loops=1)
        Index Cond: (owner_user_id = 1)
Planning Time: 5.168 ms
Execution Time: 1.594 ms
```

### Вариант с отдельным индексом

```sql
CREATE INDEX idx_folders_owner_user_id
    ON folders (owner_user_id);
```

```text
Bitmap Heap Scan on folders  (cost=4.29..10.26 rows=2 width=35) (actual time=1.318..1.320 rows=0 loops=1)
  Recheck Cond: (owner_user_id = 1)
  ->  Bitmap Index Scan on idx_folders_owner_user_id  (cost=0.00..4.29 rows=2 width=0) (actual time=1.315..1.316 rows=0 loops=1)
        Index Cond: (owner_user_id = 1)
Planning Time: 2.691 ms
Execution Time: 1.395 ms
```

### Анализ

В таблице `folders` уже есть уникальное ограничение на пару `(owner_user_id, name)`. PostgreSQL создаёт под него индекс автоматически, и этот индекс уже подходит для условия `WHERE owner_user_id = ?`, потому что `owner_user_id` — первый столбец в составном индексе.

После добавления отдельного индекса по `owner_user_id` план почти не меняется. Время тоже отличается несущественно.

То есть запрос и так уже был оптимизирован за счёт существующего составного индекса. Отдельный индекс можно создать для читаемости или явного разделения ответственности, но с практической точки зрения он здесь почти дублирует уже имеющийся.

### Вывод

Для получения папок пользователя отдельный индекс по `owner_user_id` не является обязательным, потому что PostgreSQL уже может использовать индекс, созданный ограничением `UNIQUE(owner_user_id, name)`.

---

## 4. Получение файлов папки

### Запрос

```sql
EXPLAIN ANALYZE
SELECT id, folder_id, owner_user_id, name, storage_key, mime_type, size_bytes, created_at
FROM files
WHERE folder_id = 1;
```

### Вариант с индексом, созданным ограничением `UNIQUE(folder_id, name)`

```text
Bitmap Heap Scan on files  (cost=4.49..95.47 rows=26 width=190) (actual time=1.244..13.478 rows=26 loops=1)
  Recheck Cond: (folder_id = 1)
  Heap Blocks: exact=25
  ->  Bitmap Index Scan on files_folder_name_unique  (cost=0.00..4.48 rows=26 width=0) (actual time=1.039..1.040 rows=26 loops=1)
        Index Cond: (folder_id = 1)
Planning Time: 10.290 ms
Execution Time: 13.600 ms
```

### Вариант с отдельным индексом

```sql
CREATE INDEX idx_files_folder_id
    ON files (folder_id);
```

```text
Bitmap Heap Scan on files  (cost=4.49..95.47 rows=26 width=190) (actual time=0.321..0.403 rows=26 loops=1)
  Recheck Cond: (folder_id = 1)
  Heap Blocks: exact=25
  ->  Bitmap Index Scan on idx_files_folder_id  (cost=0.00..4.48 rows=26 width=0) (actual time=0.305..0.306 rows=26 loops=1)
        Index Cond: (folder_id = 1)
Planning Time: 3.980 ms
Execution Time: 0.532 ms
```

### Анализ

Как и в случае с папками, в таблице `files` уже есть составной уникальный индекс `(folder_id, name)`, который PostgreSQL умеет использовать для фильтрации по `folder_id`.

Но в этом запросе отдельный индекс `idx_files_folder_id` показал себя лучше: время выполнения снизилось довольно заметно — примерно с 13.6 мс до 0.5 мс. На практике это означает, что для частой операции открытия папки отдельный индекс по `folder_id` действительно может быть полезен, даже несмотря на наличие составного индекса.

Возможно, такой результат связан с тем, что простой индекс по одному полю компактнее и удобнее именно для запроса, где используется только `folder_id` без `name`.

### Вывод

Хотя составной индекс `(folder_id, name)` уже частично покрывает этот запрос, отдельный индекс `idx_files_folder_id` оказался полезным и дал лучшее время выполнения. Для операции получения содержимого папки его имеет смысл оставить.

---

## 5. Получение файлов пользователя

### Запрос

```sql
EXPLAIN ANALYZE
SELECT id, folder_id, owner_user_id, name, storage_key, mime_type, size_bytes, created_at
FROM files
WHERE owner_user_id = 1;
```

### До оптимизации

```text
Seq Scan on files  (cost=0.00..1263.00 rows=29 width=82) (actual time=0.046..54.595 rows=26 loops=1)
  Filter: (owner_user_id = 1)
  Rows Removed by Filter: 29974
Planning Time: 2.711 ms
Execution Time: 54.835 ms
```

### После оптимизации

После создания индекса:

```sql
CREATE INDEX idx_files_owner_user_id
    ON files (owner_user_id);
```

PostgreSQL начинает использовать индекс:

```text
Bitmap Heap Scan on files  (cost=4.51..105.15 rows=29 width=82) (actual time=0.399..0.870 rows=26 loops=1)
  Recheck Cond: (owner_user_id = 1)
  Heap Blocks: exact=26
  ->  Bitmap Index Scan on idx_files_owner_user_id  (cost=0.00..4.50 rows=29 width=0) (actual time=0.370..0.370 rows=26 loops=1)
        Index Cond: (owner_user_id = 1)
Planning Time: 2.546 ms
Execution Time: 1.056 ms
```

### Анализ

Здесь разница уже заметная. Без индекса PostgreSQL просматривает все 30000 строк таблицы `files` и отбрасывает почти все записи. После создания индекса СУБД находит нужные строки через `Bitmap Index Scan`, и время выполнения уменьшается примерно с 54.8 мс до 1.0 мс.

Это один из самых показательных примеров в работе, потому что здесь улучшение видно и по плану, и по фактическому времени.

### Вывод

Индекс `idx_files_owner_user_id` обязателен для запросов, где нужно получать все файлы конкретного пользователя. Для такой операции он даёт существенное ускорение.

---

## 6. Поиск файлов по маске имени

### Запрос

```sql
EXPLAIN ANALYZE
SELECT id, folder_id, owner_user_id, name, storage_key, mime_type, size_bytes, created_at
FROM files
WHERE name ILIKE '%report%';
```

### До оптимизации

```text
Seq Scan on files  (cost=0.00..1263.00 rows=2714 width=190) (actual time=0.028..34.261 rows=2520 loops=1)
  Filter: ((name)::text ~~* '%report%'::text)
  Rows Removed by Filter: 27480
Planning Time: 8.700 ms
Execution Time: 34.553 ms
```

### После оптимизации

После создания trigram-индекса:

```sql
CREATE EXTENSION IF NOT EXISTS pg_trgm;

CREATE INDEX idx_files_name_trgm
    ON files USING gin (name gin_trgm_ops);
```

План выполнения после этого:

```text
Bitmap Heap Scan on files  (cost=53.10..975.03 rows=2714 width=190) (actual time=2.536..8.104 rows=2520 loops=1)
  Recheck Cond: ((name)::text ~~* '%report%'::text)
  Heap Blocks: exact=834
  ->  Bitmap Index Scan on idx_files_name_trgm  (cost=0.00..52.43 rows=2714 width=0) (actual time=2.394..2.395 rows=2520 loops=1)
        Index Cond: ((name)::text ~~* '%report%'::text)
Planning Time: 1.713 ms
Execution Time: 8.340 ms
```

### Анализ

Обычный B-Tree индекс плохо подходит для поиска по шаблону вида `ILIKE '%...%'`, потому что строка ищется не по началу значения, а по подстроке. Поэтому без специального индекса PostgreSQL вынужден делать `Seq Scan` по всей таблице `files`.

После добавления trigram-индекса запрос начинает использовать `GIN`-индекс, и время выполнения уменьшается примерно с 34.6 мс до 8.3 мс. Для текстового поиска по имени файла это хороший результат.

### Вывод

Для поиска файлов по маске имени trigram-индекс действительно нужен. Это как раз тот случай, где специальный индекс даёт реальный выигрыш и хорошо подходит под логику файлового сервиса.

---

## Общий вывод

В ходе анализа были рассмотрены основные запросы системы хранения файлов: поиск пользователя, получение папок, получение файлов и поиск файлов по имени.

По результатам `EXPLAIN ANALYZE` можно сделать такие выводы:

1. Индексы, созданные автоматически через `UNIQUE`, уже помогают части запросов и в некоторых случаях делают отдельные индексы избыточными.
2. Для внешних ключей, по которым часто выполняется фильтрация (`folder_id`, `owner_user_id`), обычные B-Tree индексы дают заметное ускорение.
3. Для поиска по подстроке через `ILIKE '%...%'` обычные индексы не подходят, поэтому нужен trigram-индекс `pg_trgm`.
4. На маленьких таблицах ускорение может быть почти незаметным, но при росте объёма данных индексная оптимизация становится важнее.

Самые заметные улучшения были получены для запросов:

- получение файлов пользователя;
- получение файлов папки;
- поиск файлов по маске имени.

Таким образом, добавленные индексы в целом оправданы, а планы выполнения после оптимизации стали лучше для наиболее частых операций сервиса.
