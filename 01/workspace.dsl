workspace "File Storage System" "Архитектура системы хранения файлов" {

    !identifiers hierarchical

    model {
        user = person "Пользователь" "Использует систему для регистрации, поиска пользователей, создания папок, загрузки, получения и удаления файлов."

        externalSystem = softwareSystem "Notification Service" "Внешний сервис отправки уведомлений пользователям (email или push уведомления)."
        
        fileStorage = softwareSystem "Система хранения файлов" "Система для управления пользователями, папками, файлами и их содержимым." {

            webUi = container "Web UI" "Веб-интерфейс, через который пользователь работает с системой хранения файлов." "Web Application"

            apiGateway = container "API Gateway" "Единая точка входа для клиентских и внешних запросов. Выполняет маршрутизацию и предоставляет единый HTTP API." "C++, userver"

            userService = container "User Service" "Управляет пользователями: создание пользователя, поиск по логину, поиск по маске имени и фамилии." "C++, userver"

            folderService = container "Folder Service" "Управляет папками: создание папки, получение списка папок, удаление папки." "C++, userver"

            fileService = container "File Service" "Управляет файлами: создание файла в папке, получение файла по имени, удаление файла." "C++, userver"

            metadataDb = container "Metadata Database" "Хранит пользователей, папки, метаданные файлов и связи между ними." "PostgreSQL"

            objectStorage = container "Object Storage" "Хранит бинарное содержимое файлов." "S3-compatible storage / MinIO"

            webUi -> apiGateway "Вызывает API системы" "HTTPS/JSON"

            apiGateway -> userService "Маршрутизирует запросы управления пользователями" "HTTP/JSON"
            apiGateway -> folderService "Маршрутизирует запросы управления папками" "HTTP/JSON"
            apiGateway -> fileService "Маршрутизирует запросы управления файлами" "HTTP/JSON"

            userService -> metadataDb "Создает и ищет пользователей" "PostgreSQL / SQL via userver driver"
            folderService -> metadataDb "Создает, читает и удаляет папки" "PostgreSQL / SQL via userver driver"
            fileService -> metadataDb "Создает, читает и удаляет метаданные файлов" "PostgreSQL / SQL via userver driver"
            fileService -> objectStorage "Сохраняет, читает и удаляет содержимое файлов" "S3 API"
        }

        user -> fileStorage.webUi "Работает с системой через браузер" "HTTPS"
        externalSystem -> fileStorage.apiGateway "Интегрируется с системой программно" "HTTPS/JSON"
    }

    views {
        themes default

        systemContext fileStorage "system-context" {
            include *
            autolayout lr
        }

        container fileStorage "containers" {
            include *
            autolayout lr
        }

        dynamic fileStorage "create-file-in-folder" "Создание файла в папке" {
            1: user -> fileStorage.webUi "Выбирает папку и загружает файл"
            2: fileStorage.webUi -> fileStorage.apiGateway "POST /folders/{folderId}/files"
            3: fileStorage.apiGateway -> fileStorage.folderService "Проверить существование папки"
            4: fileStorage.folderService -> fileStorage.metadataDb "SELECT folder by id"
            5: fileStorage.apiGateway -> fileStorage.fileService "Сохранить файл"
            6: fileStorage.fileService -> fileStorage.objectStorage "Сохранить бинарное содержимое файла"
            7: fileStorage.fileService -> fileStorage.metadataDb "INSERT file metadata"
            8: fileStorage.apiGateway -> fileStorage.webUi "Вернуть результат операции"
        }

        styles {
            element "Person" {
                shape person
                background #08427b
                color #ffffff
            }

            element "Software System" {
                background #1168bd
                color #ffffff
            }

            element "Container" {
                background #438dd5
                color #ffffff
            }

            element "Database" {
                shape cylinder
            }
        }
    }
}