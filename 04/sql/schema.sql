CREATE EXTENSION IF NOT EXISTS pg_trgm;

DROP TABLE IF EXISTS files CASCADE;
DROP TABLE IF EXISTS folders CASCADE;
DROP TABLE IF EXISTS users CASCADE;

CREATE TABLE users (
    id BIGSERIAL PRIMARY KEY,
    email VARCHAR(255) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT NOW(),

    CONSTRAINT users_email_not_empty CHECK (length(trim(email)) > 0),
    CONSTRAINT users_password_hash_not_empty CHECK (length(trim(password_hash)) > 0),
    CONSTRAINT users_first_name_not_empty CHECK (length(trim(first_name)) > 0),
    CONSTRAINT users_last_name_not_empty CHECK (length(trim(last_name)) > 0)
);

CREATE TABLE folders (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    owner_user_id BIGINT NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT NOW(),

    CONSTRAINT folders_name_not_empty CHECK (length(trim(name)) > 0),
    CONSTRAINT folders_owner_fk FOREIGN KEY (owner_user_id) REFERENCES users(id),
    CONSTRAINT folders_owner_name_unique UNIQUE (owner_user_id, name)
);

CREATE TABLE files (
    id BIGSERIAL PRIMARY KEY,
    folder_id BIGINT NOT NULL,
    owner_user_id BIGINT NOT NULL,
    name VARCHAR(255) NOT NULL,
    storage_key VARCHAR(500) NOT NULL,
    download_url VARCHAR(1000) NOT NULL,
    mime_type VARCHAR(255),
    size_bytes BIGINT NOT NULL DEFAULT 0,
    created_at TIMESTAMP NOT NULL DEFAULT NOW(),

    CONSTRAINT files_name_not_empty CHECK (length(trim(name)) > 0),
    CONSTRAINT files_storage_key_not_empty CHECK (length(trim(storage_key)) > 0),
    CONSTRAINT files_download_url_not_empty CHECK (length(trim(download_url)) > 0),
    CONSTRAINT files_size_bytes_non_negative CHECK (size_bytes >= 0),

    CONSTRAINT files_folder_fk FOREIGN KEY (folder_id) REFERENCES folders(id) ON DELETE CASCADE,
    CONSTRAINT files_owner_fk FOREIGN KEY (owner_user_id) REFERENCES users(id),

    CONSTRAINT files_folder_name_unique UNIQUE (folder_id, name),
    CONSTRAINT files_storage_key_unique UNIQUE (storage_key)
);

CREATE INDEX idx_users_last_name_first_name ON users (last_name, first_name);

CREATE INDEX idx_folders_owner_user_id ON folders (owner_user_id);

CREATE INDEX idx_files_folder_id ON files (folder_id);

CREATE INDEX idx_files_owner_user_id ON files (owner_user_id);

CREATE INDEX idx_files_mime_type ON files (mime_type);

CREATE INDEX idx_files_name_trgm ON files USING gin (name gin_trgm_ops);