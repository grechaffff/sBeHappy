document.addEventListener('DOMContentLoaded', function() {
    const baseUrl = `https://localhost:8443`;
    let authToken = null;
    
    // Элементы интерфейса
    const pingBtn = document.getElementById('pingBtn');
    const pingStatus = document.getElementById('pingStatus');
    const loginForm = document.getElementById('loginForm');
    const registerForm = document.getElementById('registerForm');
    const loginStatus = document.getElementById('loginStatus');
    const registerStatus = document.getElementById('registerStatus');
    const userSection = document.getElementById('userSection');
    const userToken = document.getElementById('userToken');
    const logoutBtn = document.getElementById('logoutBtn');
    const tabs = document.querySelectorAll('.tab');
    const tabContents = document.querySelectorAll('.tab-content');
    
    // Переключение между вкладками
    tabs.forEach(tab => {
        tab.addEventListener('click', () => {
            const tabId = tab.getAttribute('data-tab');
            
            // Деактивируем все вкладки
            tabs.forEach(t => t.classList.remove('active'));
            tabContents.forEach(tc => tc.classList.remove('active'));
            
            // Активируем выбранную вкладку
            tab.classList.add('active');
            document.getElementById(`${tabId}Tab`).classList.add('active');
        });
    });
    
    // Функция для отображения статуса
    function showStatus(element, message, isSuccess) {
        element.textContent = message;
        element.classList.remove('hidden', 'success', 'error');
        element.classList.add(isSuccess ? 'success' : 'error');
    }
    
    // Функция для выполнения запросов к серверу
    async function makeRequest(url, options = {}) {
        try {
            const response = await fetch(url, {
                ...options,
                headers: {
                    'Content-Type': 'application/json',
                    ...options.headers
                }
            });
            const data = await response.text();
            return { success: response.status, data };
        } catch (error) {
            return { success: 0, data: `Ошибка: ${error.message}` };
        }
    }
    
    // Проверка соединения (ping)
    pingBtn.addEventListener('click', async () => {
        const { success, data } = await makeRequest(`${baseUrl}/ping`);
        
        if (success) {
            showStatus(pingStatus, `Сервер отвечает: ${data}`, true);
        } else {
            showStatus(pingStatus, `Ошибка: ${data.message || 'Неизвестная ошибка'}`, false);
        }
    });
    
    // Форма входа
    loginForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const username = document.getElementById('loginUsername').value;
        const password = document.getElementById('loginPassword').value;
        
        const { success, data } = await makeRequest(`${baseUrl}/api/login`, {
            method: 'POST',
            body: JSON.stringify({ username, password })
        });
        
        if (success) {
            showStatus(loginStatus, 'Успешный вход!', true);
            authToken = data.token; // Предполагаем, что сервер возвращает токен
            userToken.textContent = authToken || 'Токен не получен';
            userSection.classList.remove('hidden');
            
            // Очищаем форму
            loginForm.reset();
        } else {
            showStatus(loginStatus, `Ошибка входа: ${data.message || 'Неизвестная ошибка'}`, false);
        }
    });
    
    // Форма регистрации
    registerForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const username = document.getElementById('registerUsername').value;
        const email = document.getElementById('registerEmail').value;
        const password = document.getElementById('registerPassword').value;
        
        const { success, data } = await makeRequest(`${baseUrl}/api/register`, {
            method: 'POST',
            body: JSON.stringify({ username, email, password })
        });
        
        if (success) {
            showStatus(registerStatus, 'Регистрация успешна! Теперь вы можете войти.', true);
            registerForm.reset();
            
            // Переключаем на вкладку входа
            tabs.forEach(t => t.classList.remove('active'));
            tabContents.forEach(tc => tc.classList.remove('active'));
            document.querySelector('[data-tab="login"]').classList.add('active');
            document.getElementById('loginTab').classList.add('active');
        } else {
            showStatus(registerStatus, `Ошибка регистрации: ${data.message || 'Неизвестная ошибка'}`, false);
        }
    });
    
    // Выход
    logoutBtn.addEventListener('click', () => {
        authToken = null;
        userSection.classList.add('hidden');
        userToken.textContent = '';
    });
    
    // Предупреждение о самоподписанном сертификате
    console.warn('Внимание: используется самоподписанный сертификат. Браузер может блокировать запросы.');
    alert('Внимание: используется самоподписанный сертификат. Для работы с сайтом вам нужно разрешить доступ к небезопасному содержимому.');
});