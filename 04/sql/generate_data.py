import random
from pathlib import Path

USERS_COUNT = 1000
FOLDERS_COUNT = 2000
FILES_COUNT = 30000
OUTPUT_FILE = "data.sql"
BATCH_SIZE = 1000

FIRST_NAMES = [
    "Alexey", "Kirill", "Ivan", "Anna", "Olga", "Sergey", "Maria", "Dmitry",
    "Elena", "Nikita", "Pavel", "Andrey", "Roman", "Svetlana", "Artem",
    "Vera", "Denis", "Irina", "Yulia", "Maksim"
]

LAST_NAMES = [
    "Pavlo", "Famal", "Petrov", "Smirnova", "Ivanova", "Sokolov",
    "Kozlova", "Orlov", "Volkova", "Fedorov", "Lebedev", "Morozov",
    "Novikova", "Mikhailov", "Bogdanov", "Tarasova", "Borisov", "Vinogradova"
]

FOLDER_NAMES = [
    "documents", "photos", "music", "archive", "work", "personal", "shared",
    "reports", "downloads", "projects", "drafts", "scans", "books", "videos"
]

FILE_BASE_NAMES = [
    "report", "notes", "resume", "contract", "invoice", "presentation",
    "photo", "scan", "draft", "summary", "spec", "plan", "budget",
    "manual", "diagram", "meeting", "project", "statement", "guide"
]

MIME_TYPES = [
    ("application/pdf", "pdf"),
    ("text/plain", "txt"),
    ("image/jpeg", "jpg"),
    ("image/png", "png"),
    ("application/zip", "zip"),
    ("application/vnd.openxmlformats-officedocument.wordprocessingml.document", "docx"),
    ("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "xlsx"),
    ("application/vnd.openxmlformats-officedocument.presentationml.presentation", "pptx"),
]

POPULAR_WORDS = ["report", "final", "draft", "project", "alexey", "march", "test"]


def sql_escape(value: str) -> str:
    return value.replace("'", "''")


def write_insert_block(file_obj, table_name: str, columns: list[str], rows: list[tuple]):
    if not rows:
        return

    file_obj.write(f"INSERT INTO {table_name} ({', '.join(columns)}) VALUES\n")

    values_sql = []
    for row in rows:
        rendered = []
        for item in row:
            if item is None:
                rendered.append("NULL")
            elif isinstance(item, str):
                rendered.append(f"'{sql_escape(item)}'")
            else:
                rendered.append(str(item))
        values_sql.append(f"({', '.join(rendered)})")

    file_obj.write(",\n".join(values_sql))
    file_obj.write(";\n\n")


def generate_users():
    rows = []
    for user_id in range(1, USERS_COUNT + 1):
        first_name = random.choice(FIRST_NAMES)
        last_name = random.choice(LAST_NAMES)
        email = f"{first_name.lower()}.{last_name.lower()}{user_id}@example.com"
        password_hash = f"hash_{user_id:06d}"
        rows.append((email, password_hash, first_name, last_name))
    return rows


def generate_folders():
    rows = []
    for folder_id in range(1, FOLDERS_COUNT + 1):
        owner_user_id = random.randint(1, USERS_COUNT)
        folder_name = f"{random.choice(FOLDER_NAMES)}_{folder_id}"
        rows.append((folder_name, owner_user_id))
    return rows


def build_file_name(index: int) -> tuple[str, str]:
    mime_type, ext = random.choice(MIME_TYPES)

    # иногда делаем имена с повторяющимися шаблонами, чтобы поиск по маске был осмысленным
    if random.random() < 0.35:
        base = random.choice(POPULAR_WORDS)
    else:
        base = random.choice(FILE_BASE_NAMES)

    suffix = random.randint(1, 5000)
    file_name = f"{base}_{suffix}.{ext}"
    return file_name, mime_type


def approximate_size(mime_type: str) -> int:
    if mime_type.startswith("image/"):
        return random.randint(150_000, 3_000_000)
    if mime_type == "application/pdf":
        return random.randint(80_000, 1_500_000)
    if mime_type == "text/plain":
        return random.randint(300, 20_000)
    if mime_type.endswith("spreadsheetml.sheet"):
        return random.randint(50_000, 800_000)
    if mime_type.endswith("presentationml.presentation"):
        return random.randint(100_000, 3_000_000)
    if mime_type.endswith("wordprocessingml.document"):
        return random.randint(30_000, 500_000)
    if mime_type == "application/zip":
        return random.randint(200_000, 10_000_000)
    return random.randint(10_000, 1_000_000)


def generate_files():
    rows = []
    for file_id in range(1, FILES_COUNT + 1):
        folder_id = random.randint(1, FOLDERS_COUNT)
        owner_user_id = random.randint(1, USERS_COUNT)

        file_name, mime_type = build_file_name(file_id)
        ext = file_name.split(".")[-1]

        storage_key = f"users/{owner_user_id}/folders/{folder_id}/{file_id}_{file_name}"
        download_url = f"https://storage.example.com/{storage_key}"
        size_bytes = approximate_size(mime_type)

        rows.append(
            (folder_id, owner_user_id, file_name, storage_key, download_url, mime_type, size_bytes)
        )
    return rows


def write_batched(file_obj, table_name: str, columns: list[str], rows: list[tuple]):
    start = 0
    while start < len(rows):
        batch = rows[start:start + BATCH_SIZE]
        write_insert_block(file_obj, table_name, columns, batch)
        start += BATCH_SIZE


def main():
    random.seed(42)

    output_path = Path(OUTPUT_FILE)

    users = generate_users()
    folders = generate_folders()
    files = generate_files()

    with output_path.open("w", encoding="utf-8") as f:
        f.write("-- auto-generated test data\n\n")

        write_batched(
            f,
            "users",
            ["email", "password_hash", "first_name", "last_name"],
            users,
        )

        write_batched(
            f,
            "folders",
            ["name", "owner_user_id"],
            folders,
        )

        write_batched(
            f,
            "files",
            ["folder_id", "owner_user_id", "name", "storage_key", "download_url", "mime_type", "size_bytes"],
            files,
        )

    print(f"Generated {output_path.resolve()}")
    print(f"users={len(users)}, folders={len(folders)}, files={len(files)}")


if __name__ == "__main__":
    main()