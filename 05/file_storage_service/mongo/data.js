db = db.getSiblingDB('file_storage_mongo');

db.folders.drop();

db.folders.insertMany([
  {
    _id: 1001,
    owner_user_id: 87,
    name: 'projects_1001',
    category: 'projects',
    shared: false,
    created_at: new Date('2026-04-20T10:00:00Z'),
    last_accessed_at: new Date('2026-04-20T10:30:00Z'),
    tags: ['project', 'work', 'important'],
    settings: {
      color: 'blue',
      pinned: true,
      access_level: 'private'
    },
    files: [
      {
        id: 1,
        name: 'invoice_4413.txt',
        storage_key: 'users/662/folders/360/1_invoice_4413.txt',
        download_url: 'https://storage.example.com/users/662/folders/360/1_invoice_4413.txt',
        mime_type: 'text/plain',
        size_bytes: 6736,
        created_at: new Date('2026-04-20T10:05:00Z'),
        tags: ['invoice', 'text'],
        metadata: {
          extension: 'txt',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 33,
        name: 'contract_4258.pdf',
        storage_key: 'users/6/folders/388/33_contract_4258.pdf',
        download_url: 'https://storage.example.com/users/6/folders/388/33_contract_4258.pdf',
        mime_type: 'application/pdf',
        size_bytes: 95087,
        created_at: new Date('2026-04-20T10:08:00Z'),
        tags: ['contract', 'pdf'],
        metadata: {
          extension: 'pdf',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  },
  {
    _id: 1002,
    owner_user_id: 98,
    name: 'downloads_1002',
    category: 'downloads',
    shared: true,
    created_at: new Date('2026-04-20T11:00:00Z'),
    last_accessed_at: new Date('2026-04-20T11:45:00Z'),
    tags: ['downloads', 'shared'],
    settings: {
      color: 'green',
      pinned: false,
      access_level: 'shared'
    },
    files: [
      {
        id: 2,
        name: 'report_4155.txt',
        storage_key: 'users/752/folders/540/2_report_4155.txt',
        download_url: 'https://storage.example.com/users/752/folders/540/2_report_4155.txt',
        mime_type: 'text/plain',
        size_bytes: 1322,
        created_at: new Date('2026-04-20T11:10:00Z'),
        tags: ['report'],
        metadata: {
          extension: 'txt',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 42,
        name: 'draft_656.txt',
        storage_key: 'users/91/folders/892/42_draft_656.txt',
        download_url: 'https://storage.example.com/users/91/folders/892/42_draft_656.txt',
        mime_type: 'text/plain',
        size_bytes: 1537,
        created_at: new Date('2026-04-20T11:13:00Z'),
        tags: ['draft'],
        metadata: {
          extension: 'txt',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  },
  {
    _id: 1003,
    owner_user_id: 137,
    name: 'drafts_1003',
    category: 'drafts',
    shared: false,
    created_at: new Date('2026-04-20T12:00:00Z'),
    last_accessed_at: new Date('2026-04-20T12:20:00Z'),
    tags: ['drafts'],
    settings: {
      color: 'yellow',
      pinned: false,
      access_level: 'private'
    },
    files: [
      {
        id: 11,
        name: 'draft_4741.zip',
        storage_key: 'users/992/folders/1442/11_draft_4741.zip',
        download_url: 'https://storage.example.com/users/992/folders/1442/11_draft_4741.zip',
        mime_type: 'application/zip',
        size_bytes: 9332678,
        created_at: new Date('2026-04-20T12:05:00Z'),
        tags: ['draft', 'archive'],
        metadata: {
          extension: 'zip',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 45,
        name: 'test_4912.zip',
        storage_key: 'users/207/folders/22/45_test_4912.zip',
        download_url: 'https://storage.example.com/users/207/folders/22/45_test_4912.zip',
        mime_type: 'application/zip',
        size_bytes: 8765571,
        created_at: new Date('2026-04-20T12:06:00Z'),
        tags: ['test', 'archive'],
        metadata: {
          extension: 'zip',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  },
  {
    _id: 1004,
    owner_user_id: 840,
    name: 'shared_1004',
    category: 'shared',
    shared: true,
    created_at: new Date('2026-04-20T13:00:00Z'),
    last_accessed_at: new Date('2026-04-20T13:15:00Z'),
    tags: ['shared', 'team'],
    settings: {
      color: 'purple',
      pinned: true,
      access_level: 'team'
    },
    files: [
      {
        id: 8,
        name: 'project_1899.pdf',
        storage_key: 'users/368/folders/1924/8_project_1899.pdf',
        download_url: 'https://storage.example.com/users/368/folders/1924/8_project_1899.pdf',
        mime_type: 'application/pdf',
        size_bytes: 211829,
        created_at: new Date('2026-04-20T13:05:00Z'),
        tags: ['project', 'pdf'],
        metadata: {
          extension: 'pdf',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 44,
        name: 'presentation_4050.pdf',
        storage_key: 'users/612/folders/1974/44_presentation_4050.pdf',
        download_url: 'https://storage.example.com/users/612/folders/1974/44_presentation_4050.pdf',
        mime_type: 'application/pdf',
        size_bytes: 1000591,
        created_at: new Date('2026-04-20T13:10:00Z'),
        tags: ['presentation', 'shared'],
        metadata: {
          extension: 'pdf',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  },
  {
    _id: 1005,
    owner_user_id: 77,
    name: 'projects_1005',
    category: 'projects',
    shared: false,
    created_at: new Date('2026-04-20T14:00:00Z'),
    last_accessed_at: new Date('2026-04-20T14:40:00Z'),
    tags: ['project', 'docs'],
    settings: {
      color: 'gray',
      pinned: false,
      access_level: 'private'
    },
    files: [
      {
        id: 14,
        name: 'meeting_4950.docx',
        storage_key: 'users/362/folders/832/14_meeting_4950.docx',
        download_url: 'https://storage.example.com/users/362/folders/832/14_meeting_4950.docx',
        mime_type: 'application/vnd.openxmlformats-officedocument.wordprocessingml.document',
        size_bytes: 33459,
        created_at: new Date('2026-04-20T14:05:00Z'),
        tags: ['meeting', 'docx'],
        metadata: {
          extension: 'docx',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 25,
        name: 'guide_564.docx',
        storage_key: 'users/703/folders/1484/25_guide_564.docx',
        download_url: 'https://storage.example.com/users/703/folders/1484/25_guide_564.docx',
        mime_type: 'application/vnd.openxmlformats-officedocument.wordprocessingml.document',
        size_bytes: 150488,
        created_at: new Date('2026-04-20T14:07:00Z'),
        tags: ['guide', 'docx'],
        metadata: {
          extension: 'docx',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  },
  {
    _id: 1006,
    owner_user_id: 582,
    name: 'documents_1006',
    category: 'documents',
    shared: false,
    created_at: new Date('2026-04-20T15:00:00Z'),
    last_accessed_at: new Date('2026-04-20T15:25:00Z'),
    tags: ['documents'],
    settings: {
      color: 'white',
      pinned: true,
      access_level: 'private'
    },
    files: [
      {
        id: 3,
        name: 'photo_301.xlsx',
        storage_key: 'users/596/folders/1665/3_photo_301.xlsx',
        download_url: 'https://storage.example.com/users/596/folders/1665/3_photo_301.xlsx',
        mime_type: 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet',
        size_bytes: 628388,
        created_at: new Date('2026-04-20T15:03:00Z'),
        tags: ['xlsx', 'table'],
        metadata: {
          extension: 'xlsx',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 12,
        name: 'plan_4815.xlsx',
        storage_key: 'users/179/folders/1168/12_plan_4815.xlsx',
        download_url: 'https://storage.example.com/users/179/folders/1168/12_plan_4815.xlsx',
        mime_type: 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet',
        size_bytes: 735073,
        created_at: new Date('2026-04-20T15:06:00Z'),
        tags: ['plan', 'xlsx'],
        metadata: {
          extension: 'xlsx',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  },
  {
    _id: 1007,
    owner_user_id: 446,
    name: 'drafts_1007',
    category: 'drafts',
    shared: false,
    created_at: new Date('2026-04-20T16:00:00Z'),
    last_accessed_at: new Date('2026-04-20T16:18:00Z'),
    tags: ['drafts', 'images'],
    settings: {
      color: 'red',
      pinned: false,
      access_level: 'private'
    },
    files: [
      {
        id: 15,
        name: 'scan_632.jpg',
        storage_key: 'users/904/folders/1028/15_scan_632.jpg',
        download_url: 'https://storage.example.com/users/904/folders/1028/15_scan_632.jpg',
        mime_type: 'image/jpeg',
        size_bytes: 2308142,
        created_at: new Date('2026-04-20T16:04:00Z'),
        tags: ['scan', 'image'],
        metadata: {
          extension: 'jpg',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 47,
        name: 'draft_3273.jpg',
        storage_key: 'users/1/folders/1342/47_draft_3273.jpg',
        download_url: 'https://storage.example.com/users/1/folders/1342/47_draft_3273.jpg',
        mime_type: 'image/jpeg',
        size_bytes: 1631124,
        created_at: new Date('2026-04-20T16:06:00Z'),
        tags: ['draft', 'image'],
        metadata: {
          extension: 'jpg',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  },
  {
    _id: 1008,
    owner_user_id: 988,
    name: 'music_1008',
    category: 'music',
    shared: false,
    created_at: new Date('2026-04-20T17:00:00Z'),
    last_accessed_at: new Date('2026-04-20T17:20:00Z'),
    tags: ['media', 'presentations'],
    settings: {
      color: 'orange',
      pinned: false,
      access_level: 'private'
    },
    files: [
      {
        id: 6,
        name: 'presentation_4212.pdf',
        storage_key: 'users/373/folders/813/6_presentation_4212.pdf',
        download_url: 'https://storage.example.com/users/373/folders/813/6_presentation_4212.pdf',
        mime_type: 'application/pdf',
        size_bytes: 374378,
        created_at: new Date('2026-04-20T17:02:00Z'),
        tags: ['presentation', 'pdf'],
        metadata: {
          extension: 'pdf',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 18,
        name: 'guide_1321.pptx',
        storage_key: 'users/692/folders/987/18_guide_1321.pptx',
        download_url: 'https://storage.example.com/users/692/folders/987/18_guide_1321.pptx',
        mime_type: 'application/vnd.openxmlformats-officedocument.presentationml.presentation',
        size_bytes: 2814801,
        created_at: new Date('2026-04-20T17:04:00Z'),
        tags: ['guide', 'pptx'],
        metadata: {
          extension: 'pptx',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  },
  {
    _id: 1009,
    owner_user_id: 243,
    name: 'work_1009',
    category: 'work',
    shared: true,
    created_at: new Date('2026-04-20T18:00:00Z'),
    last_accessed_at: new Date('2026-04-20T18:33:00Z'),
    tags: ['work', 'finance'],
    settings: {
      color: 'black',
      pinned: true,
      access_level: 'team'
    },
    files: [
      {
        id: 13,
        name: 'budget_1826.txt',
        storage_key: 'users/856/folders/1840/13_budget_1826.txt',
        download_url: 'https://storage.example.com/users/856/folders/1840/13_budget_1826.txt',
        mime_type: 'text/plain',
        size_bytes: 17220,
        created_at: new Date('2026-04-20T18:03:00Z'),
        tags: ['budget', 'text'],
        metadata: {
          extension: 'txt',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 40,
        name: 'statement_427.xlsx',
        storage_key: 'users/521/folders/440/40_statement_427.xlsx',
        download_url: 'https://storage.example.com/users/521/folders/440/40_statement_427.xlsx',
        mime_type: 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet',
        size_bytes: 218546,
        created_at: new Date('2026-04-20T18:06:00Z'),
        tags: ['statement', 'xlsx'],
        metadata: {
          extension: 'xlsx',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  },
  {
    _id: 1010,
    owner_user_id: 268,
    name: 'videos_1010',
    category: 'videos',
    shared: false,
    created_at: new Date('2026-04-20T19:00:00Z'),
    last_accessed_at: new Date('2026-04-20T19:22:00Z'),
    tags: ['videos', 'archives'],
    settings: {
      color: 'silver',
      pinned: false,
      access_level: 'private'
    },
    files: [
      {
        id: 5,
        name: 'alexey_4182.zip',
        storage_key: 'users/913/folders/458/5_alexey_4182.zip',
        download_url: 'https://storage.example.com/users/913/folders/458/5_alexey_4182.zip',
        mime_type: 'application/zip',
        size_bytes: 1719244,
        created_at: new Date('2026-04-20T19:05:00Z'),
        tags: ['archive', 'zip'],
        metadata: {
          extension: 'zip',
          source: 'postgres-seed'
        },
        is_deleted: false
      },
      {
        id: 50,
        name: 'final_3726.zip',
        storage_key: 'users/987/folders/712/50_final_3726.zip',
        download_url: 'https://storage.example.com/users/987/folders/712/50_final_3726.zip',
        mime_type: 'application/zip',
        size_bytes: 6977264,
        created_at: new Date('2026-04-20T19:08:00Z'),
        tags: ['final', 'zip'],
        metadata: {
          extension: 'zip',
          source: 'postgres-seed'
        },
        is_deleted: false
      }
    ]
  }
]);

print('Inserted folders: ' + db.folders.countDocuments());