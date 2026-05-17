import uuid

pytest_plugins = [
    'pytest_userver.plugins.core',
]


def _uniq(prefix='test'):
    return f'{prefix}-{uuid.uuid4().hex[:8]}'


async def create_user(service_client, login=None, password='12345',
                      first_name='Test', last_name='User'):
    if login is None:
        login = _uniq('user')

    response = await service_client.post(
        '/v1/users',
        json={
            'login': login,
            'password': password,
            'first_name': first_name,
            'last_name': last_name,
        },
    )

    return response


async def login_user(service_client, login, password='12345'):
    response = await service_client.post(
        '/v1/auth/login',
        json={
            'login': login,
            'password': password,
        },
    )
    return response


async def create_authorized_user(service_client):
    login = _uniq('auth-user')

    create_response = await create_user(
        service_client,
        login=login,
        password='12345',
        first_name='Auth',
        last_name='User',
    )

    assert create_response.status == 201

    user_data = create_response.json()

    login_response = await login_user(service_client, login=login, password='12345')
    assert login_response.status == 200

    token = login_response.json()['token']

    return {
        'user_id': user_data['id'],
        'login': login,
        'token': token,
    }


async def create_folder(service_client, token, owner_user_id, name=None):
    if name is None:
        name = _uniq('folder')

    response = await service_client.post(
        '/v1/folders',
        json={
            'name': name,
            'owner_user_id': owner_user_id,
        },
        headers={
            'Authorization': f'Bearer {token}',
        },
    )
    return response


async def create_file(service_client, token, folder_id, owner_user_id,
                      name=None, content='hello'):
    if name is None:
        name = _uniq('file') + '.txt'

    response = await service_client.post(
        f'/v1/folders/{folder_id}/files',
        json={
            'name': name,
            'content': content,
            'owner_user_id': owner_user_id,
        },
        headers={
            'Authorization': f'Bearer {token}',
        },
    )
    return response