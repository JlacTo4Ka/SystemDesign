from conftest import create_user


async def test_create_user(service_client):
    response = await create_user(
        service_client,
        first_name='Alexey',
        last_name='Pavlov',
    )

    assert response.status == 201

    data = response.json()
    assert 'id' in data
    assert data['first_name'] == 'Alexey'
    assert data['last_name'] == 'Pavlov'
    assert 'login' in data


async def test_get_user_by_login(service_client):
    create_response = await create_user(
        service_client,
        first_name='Kirill',
        last_name='Zhuravlev',
    )

    assert create_response.status == 201
    created_user = create_response.json()

    response = await service_client.get(
        f"/v1/users/by-login/{created_user['login']}"
    )

    assert response.status == 200

    data = response.json()
    assert data['id'] == created_user['id']
    assert data['login'] == created_user['login']
    assert data['first_name'] == 'Kirill'
    assert data['last_name'] == 'Zhuravlev'


async def test_search_users(service_client):
    await create_user(
        service_client,
        login='ivan-aaa-test',
        first_name='Ivan',
        last_name='Petrov',
    )

    await create_user(
        service_client,
        login='sergey-bbb-test',
        first_name='Sergey',
        last_name='Ivanov',
    )

    response = await service_client.get('/v1/users/search?mask=ivan')

    assert response.status == 200

    data = response.json()
    assert isinstance(data, list)
    assert len(data) >= 1

    found = False
    for item in data:
        if item['last_name'] == 'Ivanov' or item['first_name'] == 'Ivan':
            found = True
            break

    assert found