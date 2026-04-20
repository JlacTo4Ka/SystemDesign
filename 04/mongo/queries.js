db = db.getSiblingDB('file_storage_mongo');

print("READ: все папки");
db.folders.find().pretty();

print("READ: папка по _id через $eq");
db.folders.find({ _id: { $eq: 1001 } }).pretty();

print("READ: папки, где owner_user_id != 87");
db.folders.find({ owner_user_id: { $ne: 87 } }).pretty();

print("READ: папки, где _id > 1005");
db.folders.find({ _id: { $gt: 1005 } }).pretty();

print("READ: папки, где _id < 1004");
db.folders.find({ _id: { $lt: 1004 } }).pretty();

print("READ: папки, где owner_user_id входит в список");
db.folders.find({
  owner_user_id: { $in: [87, 98, 243] }
}).pretty();

print("READ: $and");
db.folders.find({
  $and: [
    { shared: { $eq: false } },
    { _id: { $gt: 1003 } }
  ]
}).pretty();

print("READ: $or");
db.folders.find({
  $or: [
    { category: { $eq: "projects" } },
    { category: { $eq: "drafts" } }
  ]
}).pretty();

print("READ: папка по имени");
db.folders.find({ name: "projects_1001" }).pretty();

print("READ: папка, у которой есть файл size_bytes > 5000000");
db.folders.find({
  files: {
    $elemMatch: {
      size_bytes: { $gt: 5000000 }
    }
  }
}).pretty();

print("READ: одна папка через findOne");
db.folders.findOne({ _id: 1001 });

print("CREATE: добавить новую папку через insertOne");
db.folders.insertOne({
  _id: 1011,
  owner_user_id: 555,
  name: "archive_1011",
  category: "archive",
  shared: false,
  created_at: new Date("2026-04-21T10:00:00Z"),
  last_accessed_at: new Date("2026-04-21T10:30:00Z"),
  tags: ["archive", "backup"],
  settings: {
    color: "brown",
    pinned: false,
    access_level: "private"
  },
  files: [
    {
      id: 101,
      name: "backup_1011.zip",
      storage_key: "users/555/folders/1011/101_backup_1011.zip",
      download_url: "https://storage.example.com/users/555/folders/1011/101_backup_1011.zip",
      mime_type: "application/zip",
      size_bytes: 3456789,
      created_at: new Date("2026-04-21T10:05:00Z"),
      tags: ["backup", "zip"],
      metadata: {
        extension: "zip",
        source: "mongo-create"
      },
      is_deleted: false
    }
  ]
});

print("CREATE: добавить несколько папок через insertMany");
db.folders.insertMany([
  {
    _id: 1012,
    owner_user_id: 556,
    name: "photos_1012",
    category: "photos",
    shared: true,
    created_at: new Date("2026-04-21T11:00:00Z"),
    last_accessed_at: new Date("2026-04-21T11:20:00Z"),
    tags: ["photos", "shared"],
    settings: {
      color: "pink",
      pinned: true,
      access_level: "team"
    },
    files: []
  },
  {
    _id: 1013,
    owner_user_id: 557,
    name: "reports_1013",
    category: "reports",
    shared: false,
    created_at: new Date("2026-04-21T12:00:00Z"),
    last_accessed_at: new Date("2026-04-21T12:10:00Z"),
    tags: ["reports"],
    settings: {
      color: "navy",
      pinned: false,
      access_level: "private"
    },
    files: []
  }
]);

print("UPDATE: изменить название папки");
db.folders.updateOne(
  { _id: 1002 },
  {
    $set: {
      name: "downloads_1002_updated"
    }
  }
);

print("UPDATE: изменить access_level у нескольких папок через updateMany");
db.folders.updateMany(
  { shared: true },
  {
    $set: {
      "settings.access_level": "shared-updated"
    }
  }
);

print("UPDATE: добавить новый файл в папку через $push");
db.folders.updateOne(
  { _id: 1001 },
  {
    $push: {
      files: {
        id: 102,
        name: "summary_1001.pdf",
        storage_key: "users/87/folders/1001/102_summary_1001.pdf",
        download_url: "https://storage.example.com/users/87/folders/1001/102_summary_1001.pdf",
        mime_type: "application/pdf",
        size_bytes: 222000,
        created_at: new Date("2026-04-21T13:00:00Z"),
        tags: ["summary", "pdf"],
        metadata: {
          extension: "pdf",
          source: "mongo-push"
        },
        is_deleted: false
      }
    }
  }
);

print("UPDATE: добавить тег в массив через $addToSet");
db.folders.updateOne(
  { _id: 1001 },
  {
    $addToSet: {
      tags: "mongo"
    }
  }
);

print("UPDATE: изменить имя файла внутри массива");
db.folders.updateOne(
  {
    _id: 1001,
    "files.id": 1
  },
  {
    $set: {
      "files.$.name": "invoice_4413_updated.txt"
    }
  }
);

print("DELETE: удалить файл из папки через $pull");
db.folders.updateOne(
  { _id: 1001 },
  {
    $pull: {
      files: { id: 102 }
    }
  }
);

print("DELETE: удалить одну папку через deleteOne");
db.folders.deleteOne({ _id: 1011 });

print("DELETE: удалить несколько папок через deleteMany");
db.folders.deleteMany({
  _id: { $in: [1012, 1013] }
});

print("READ: итоговое состояние коллекции");
db.folders.find().pretty();