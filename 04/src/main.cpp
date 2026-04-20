#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component_list.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/congestion_control/component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>

#include <userver/utils/daemon_run.hpp>

#include <storage/storage_component.hpp>

#include <auth/jwt_auth_factory.hpp>
#include <views/users/create_handler.hpp>
#include <views/users/get_by_login_handler.hpp>
#include <views/users/search_handler.hpp>
#include <views/folders/create_handler.hpp>
#include <views/folders/get_all_handler.hpp>
#include <views/files/create_handler.hpp>
#include <views/files/get_by_name_handler.hpp>
#include <views/files/delete_handler.hpp>
#include <views/folders/delete_handler.hpp>
#include <views/auth/login_handler.hpp>

#include <userver/storages/postgres/component.hpp>
#include <userver/storages/mongo/component.hpp>

#include <views/mongo_folders/create_handler.hpp>
#include <views/mongo_folders/get_all_handler.hpp>
#include <views/mongo_folders/delete_handler.hpp>
#include <storage/mongo_storage_component.hpp>

int main(int argc, char* argv[]) {
  userver::server::handlers::auth::RegisterAuthCheckerFactory<
      auth::jwt::JwtAuthCheckerFactory>();
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::TestsuiteSupport>()
          .AppendComponentList(userver::clients::http::ComponentList())
          .Append<userver::clients::dns::Component>()
          .Append<userver::server::handlers::TestsControl>()
          .Append<userver::congestion_control::Component>()
          .Append<auth::jwt::JwtAuthComponent>()
          .Append<userver::components::Postgres>("postgres-db")
          .Append<jwt_auth::users::CreateHandler>()
          .Append<jwt_auth::users::GetByLoginHandler>()
          .Append<jwt_auth::users::SearchHandler>()
          .Append<jwt_auth::folders::CreateHandler>()
          .Append<jwt_auth::folders::GetAllHandler>()
          .Append<jwt_auth::folders::DeleteHandler>()
          .Append<jwt_auth::files::CreateHandler>()
          .Append<jwt_auth::files::GetByNameHandler>()
          .Append<jwt_auth::files::DeleteHandler>()
          .Append<jwt_auth::auth_handlers::LoginHandler>()
          .Append<jwt_auth::storage::StorageComponent>()
          .Append<userver::components::Mongo>("mongo-folder-db")
          .Append<jwt_auth::storage::MongoStorageComponent>()
          .Append<jwt_auth::mongo_folders::CreateHandler>()
          .Append<jwt_auth::mongo_folders::GetAllHandler>()
          .Append<jwt_auth::mongo_folders::DeleteHandler>();

  return userver::utils::DaemonMain(argc, argv, component_list);
}