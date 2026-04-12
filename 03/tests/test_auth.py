from conftest import create_user, login_user


async def test_login(service_client):
    create_response = await create_user(
        service_client,
        login='login-user-test',
        password='12345',
        first_name='Login',
        last_name='User',
    )

    assert create_response.status == 201

    response = await login_user(
        service_client,
        login='login-user-test',
        password='12345',
    )

    assert response.status == 200

    data = response.json()
    assert 'token' in data
    assert data['login'] == 'login-user-test'
    assert 'user_id' in data