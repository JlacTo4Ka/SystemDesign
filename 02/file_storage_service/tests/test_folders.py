from conftest import create_authorized_user, create_folder


async def test_create_folder(service_client):
    auth_data = await create_authorized_user(service_client)

    response = await create_folder(
        service_client,
        token=auth_data['token'],
        owner_user_id=auth_data['user_id'],
        name='docs-folder',
    )

    assert response.status == 201

    data = response.json()
    assert 'id' in data
    assert data['name'] == 'docs-folder'
    assert data['owner_user_id'] == auth_data['user_id']


async def test_get_all_folders(service_client):
    auth_data = await create_authorized_user(service_client)

    create_response = await create_folder(
        service_client,
        token=auth_data['token'],
        owner_user_id=auth_data['user_id'],
        name='my-folder-for-list',
    )

    assert create_response.status == 201

    response = await service_client.get('/v1/folders')

    assert response.status == 200

    data = response.json()
    assert isinstance(data, list)

    found = False
    for item in data:
        if item['name'] == 'my-folder-for-list':
            found = True
            break

    assert found


async def test_delete_folder(service_client):
    auth_data = await create_authorized_user(service_client)

    create_response = await create_folder(
        service_client,
        token=auth_data['token'],
        owner_user_id=auth_data['user_id'],
        name='folder-to-delete',
    )

    assert create_response.status == 201
    folder_id = create_response.json()['id']

    response = await service_client.delete(
        f'/v1/folders/{folder_id}',
        headers={
            'Authorization': f"Bearer {auth_data['token']}",
        },
    )

    assert response.status == 200
    assert response.json()['status'] == 'folder deleted'