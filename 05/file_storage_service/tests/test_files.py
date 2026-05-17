from conftest import create_authorized_user, create_folder, create_file


async def test_create_file(service_client):
    auth_data = await create_authorized_user(service_client)

    folder_response = await create_folder(
        service_client,
        token=auth_data['token'],
        owner_user_id=auth_data['user_id'],
        name='folder-for-file',
    )

    assert folder_response.status == 201
    folder_id = folder_response.json()['id']

    response = await create_file(
        service_client,
        token=auth_data['token'],
        folder_id=folder_id,
        owner_user_id=auth_data['user_id'],
        name='test-file.txt',
        content='hello world',
    )

    assert response.status == 201

    data = response.json()
    assert 'id' in data
    assert data['name'] == 'test-file.txt'
    assert data['folder_id'] == folder_id
    assert data['owner_user_id'] == auth_data['user_id']


async def test_get_file_by_name(service_client):
    auth_data = await create_authorized_user(service_client)

    folder_response = await create_folder(
        service_client,
        token=auth_data['token'],
        owner_user_id=auth_data['user_id'],
        name='folder-for-search-file',
    )

    assert folder_response.status == 201
    folder_id = folder_response.json()['id']

    create_file_response = await create_file(
        service_client,
        token=auth_data['token'],
        folder_id=folder_id,
        owner_user_id=auth_data['user_id'],
        name='hello-search-file.txt',
        content='abc',
    )

    assert create_file_response.status == 201

    response = await service_client.get('/v1/files/by-name/hello-search-file.txt')

    assert response.status == 200

    data = response.json()
    assert data['name'] == 'hello-search-file.txt'
    assert data['folder_id'] == folder_id


async def test_delete_file(service_client):
    auth_data = await create_authorized_user(service_client)

    folder_response = await create_folder(
        service_client,
        token=auth_data['token'],
        owner_user_id=auth_data['user_id'],
        name='folder-for-delete-file',
    )

    assert folder_response.status == 201
    folder_id = folder_response.json()['id']

    file_response = await create_file(
        service_client,
        token=auth_data['token'],
        folder_id=folder_id,
        owner_user_id=auth_data['user_id'],
        name='delete-me.txt',
        content='123',
    )

    assert file_response.status == 201
    file_id = file_response.json()['id']

    response = await service_client.delete(
        f'/v1/files/{file_id}',
        headers={
            'Authorization': f"Bearer {auth_data['token']}",
        },
    )

    assert response.status == 200
    assert response.json()['status'] == 'file deleted'