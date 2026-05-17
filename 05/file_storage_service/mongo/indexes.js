db = db.getSiblingDB('file_storage_mongo');

db.folders.createIndex({ owner_user_id: 1 });
db.folders.createIndex({ name: 1 });
db.folders.createIndex({ category: 1 });
db.folders.createIndex({ shared: 1 });
db.folders.createIndex({ "files.name": 1 });
db.folders.createIndex({ "files.size_bytes": 1 });
