-- Создание нового пользователя
INSERT INTO users (email, password_hash, first_name, last_name)
VALUES ('new.user@example.com', 'hash_new', 'New', 'User');

-- Поиск пользователя по логину (email)
SELECT id, email, first_name, last_name, created_at
FROM users
WHERE email = 'alexey.pavlo1@example.com';

-- Поиск пользователя по маске имени и фамилии
SELECT id, email, first_name, last_name, created_at
FROM users
WHERE first_name ILIKE '%alex%'
   OR last_name ILIKE '%pav%';

-- Создание новой папки
INSERT INTO folders (name, owner_user_id)
VALUES ('new_folder', 1);

-- Получение списка всех папок
SELECT f.id,
       f.name,
       f.owner_user_id,
       f.created_at,
       u.email AS owner_email
FROM folders f
JOIN users u ON u.id = f.owner_user_id
ORDER BY f.id;

-- Создание файла в папке
INSERT INTO files (
    folder_id,
    owner_user_id,
    name,
    storage_key,
    download_url,
    mime_type,
    size_bytes
) VALUES (1, 1, 'contract.pdf', 'users/1/folders/1/contract.pdf', 
    'https://storage.example.com/users/1/folders/1/contract.pdf',
    'application/pdf',
    350000
);

-- Получение файла по имени
SELECT f.id, f.name, f.storage_key, f.download_url,
       f.mime_type, f.size_bytes, f.created_at,
       fld.name AS folder_name,
       u.email AS owner_email
FROM files f
JOIN folders fld ON fld.id = f.folder_id
JOIN users u ON u.id = f.owner_user_id
WHERE f.name = 'resume.pdf';

-- Поиск файлов по маске имени
SELECT f.id, f.name, f.storage_key, f.download_url,
       f.mime_type, f.size_bytes, f.created_at,
       fld.name AS folder_name,
       u.email AS owner_email
FROM files f
JOIN folders fld ON fld.id = f.folder_id
JOIN users u ON u.id = f.owner_user_id
WHERE f.name ILIKE '%report%'
ORDER BY f.id;

-- Удаление файла
DELETE FROM files
WHERE id = 1;

-- Удаление папки
DELETE FROM folders
WHERE id = 1;

-- Получение всех файлов конкретной папки
SELECT id, name, storage_key, download_url, mime_type, size_bytes, created_at
FROM files
WHERE folder_id = 1
ORDER BY id;

-- Получение всех файлов конкретного пользователя
SELECT id, folder_id, name, mime_type, size_bytes, created_at
FROM files
WHERE owner_user_id = 1
ORDER BY id;

-- Получение папок конкретного пользователя
SELECT id, name, created_at
FROM folders
WHERE owner_user_id = 1
ORDER BY id;