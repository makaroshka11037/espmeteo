from fastapi import FastAPI, WebSocket, Request, WebSocketDisconnect
from fastapi.templating import Jinja2Templates
from endpoints import register_endpoints
import uvicorn
from zeroconf.asyncio import AsyncServiceInfo, AsyncZeroconf
import socket
import asyncio
from contextlib import asynccontextmanager


templates = Jinja2Templates(directory="templates")


def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # адрес не обязательно должен быть существующим
        s.connect(('10.255.255.255', 1))
        ip = s.getsockname()[0]
    except Exception:
        ip = '127.0.0.1'
    finally:
        s.close()
    return ip


async def register_mdns(): 
    desc = {'version': '1.0'}
    host_name = "meteohub11037"
    service_type = "_http._tcp.local."
    port = 8000
    
    ip_address = get_ip()
    info = AsyncServiceInfo(
        type_=service_type,
        name=f"{host_name}.{service_type}",
        addresses=[socket.inet_aton(ip_address)],
        port=port,
        properties=desc,
        server=f"{host_name}.local."
    )
    
    aiozc = AsyncZeroconf()
    await aiozc.async_register_service(info)
    return aiozc, info

@asynccontextmanager
async def lifespan(app: FastAPI):
    zc, info = await register_mdns()
    print(f"мднс успешно зарегистрирован: {info.name}")
    yield
    await zc.async_unregister_service(info)
    await zc.async_close()
    print("мднс отключен")

app = FastAPI(lifespan=lifespan)
register_endpoints(app)

# активные соединения
class ConnectionManager:
    def __init__(self):
        self.active_connections: list[WebSocket] = []

    async def connect(self, websocket: WebSocket):
        await websocket.accept()
        self.active_connections.append(websocket)
        print(f"Новое подключение. Всего: {len(self.active_connections)}")

    def disconnect(self, websocket: WebSocket):
        if websocket in self.active_connections:
            self.active_connections.remove(websocket)
            print(f"Соединение закрыто. Осталось: {len(self.active_connections)}")

    async def broadcast(self, message: str):
        for connection in self.active_connections[:]:
            try:
                await connection.send_text(message)
            except Exception:
                self.disconnect(connection)


manager = ConnectionManager()


@app.get("/")
async def get(request: Request):
    return templates.TemplateResponse(
        request=request, 
        name="index.html", 
        context={}
    )


@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await manager.connect(websocket)
    try:
        while True:
            # Ждем данные от любого клиента
            data = await websocket.receive_text()
            print(f"Лог сообщения: {data}")

            # Рассылаем полученное сообщение всем остальным
            await manager.broadcast(data)

    except WebSocketDisconnect:
        manager.disconnect(websocket)
    except Exception as e:
        print(f"Ошибка: {e}")
        manager.disconnect(websocket)







if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
