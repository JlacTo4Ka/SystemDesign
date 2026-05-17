db = db.getSiblingDB('file_storage_mongo');

const collections = db.getCollectionNames();

if (!collections.includes("folders")) {
  db.createCollection("folders");
}

db.runCommand({
  collMod: "folders",
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: [
        "_id",
        "owner_user_id",
        "name",
        "category",
        "shared",
        "created_at",
        "last_accessed_at",
        "tags",
        "settings",
        "files"
      ],
      properties: {
        _id: {
          bsonType: ["int", "long"],
          description: "Идентификатор папки должен быть целым числом"
        },
        owner_user_id: {
          bsonType: ["int", "long"],
          description: "Идентификатор владельца должен быть целым числом"
        },
        name: {
          bsonType: "string",
          minLength: 3,
          description: "Название папки должно быть строкой длиной не менее 3 символов"
        },
        category: {
          bsonType: "string",
          enum: [
            "projects",
            "downloads",
            "drafts",
            "shared",
            "documents",
            "music",
            "work",
            "videos",
            "books",
            "scans",
            "personal",
            "photos",
            "archive",
            "reports"
          ],
          description: "Категория папки должна входить в допустимый список"
        },
        shared: {
          bsonType: "bool",
          description: "Поле shared должно иметь тип bool"
        },
        created_at: {
          bsonType: "date",
          description: "Дата создания папки должна иметь тип date"
        },
        last_accessed_at: {
          bsonType: "date",
          description: "Дата последнего доступа должна иметь тип date"
        },
        tags: {
          bsonType: "array",
          description: "Поле tags должно быть массивом строк",
          items: {
            bsonType: "string"
          }
        },
        settings: {
          bsonType: "object",
          required: ["color", "pinned", "access_level"],
          properties: {
            color: {
              bsonType: "string",
              minLength: 3,
              description: "Цвет должен быть строкой"
            },
            pinned: {
              bsonType: "bool",
              description: "Поле pinned должно иметь тип bool"
            },
            access_level: {
              bsonType: "string",
              enum: ["private", "shared", "team", "shared-updated"],
              description: "Уровень доступа должен входить в допустимый список"
            }
          }
        },
        files: {
          bsonType: "array",
          description: "Поле files должно быть массивом файлов",
          items: {
            bsonType: "object",
            required: [
              "id",
              "name",
              "storage_key",
              "download_url",
              "mime_type",
              "size_bytes",
              "created_at",
              "tags",
              "metadata",
              "is_deleted"
            ],
            properties: {
              id: {
                bsonType: ["int", "long"],
                description: "Идентификатор файла должен быть целым числом"
              },
              name: {
                bsonType: "string",
                minLength: 3,
                description: "Название файла должно быть строкой длиной не менее 3 символов"
              },
              storage_key: {
                bsonType: "string",
                pattern: "^users/.+",
                description: "storage_key должен начинаться с users/"
              },
              download_url: {
                bsonType: "string",
                pattern: "^https://storage\\.example\\.com/.+",
                description: "download_url должен начинаться с https://storage.example.com/"
              },
              mime_type: {
                bsonType: "string",
                minLength: 3,
                description: "mime_type должен быть строкой"
              },
              size_bytes: {
                bsonType: ["int", "long"],
                minimum: 1,
                description: "Размер файла должен быть целым числом больше 0"
              },
              created_at: {
                bsonType: "date",
                description: "Дата создания файла должна иметь тип date"
              },
              tags: {
                bsonType: "array",
                description: "Теги файла должны храниться в массиве",
                items: {
                  bsonType: "string"
                }
              },
              metadata: {
                bsonType: "object",
                required: ["extension", "source"],
                properties: {
                  extension: {
                    bsonType: "string",
                    minLength: 2,
                    description: "Расширение файла должно быть строкой"
                  },
                  source: {
                    bsonType: "string",
                    minLength: 3,
                    description: "Источник данных должен быть строкой"
                  }
                }
              },
              is_deleted: {
                bsonType: "bool",
                description: "Поле is_deleted должно иметь тип bool"
              }
            }
          }
        }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});
