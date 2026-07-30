# net_server

Windows + Python 实现的 TCP echo 服务端，用于联调测试。

## 文件

- [server.py](file:///c:/workspace/stm32/src/net_server/server.py) — 多线程 echo 服务端，监听 `0.0.0.0:8888`
- [client.py](file:///c:/workspace/stm32/src/net_server/client.py) — 交互式测试客户端，连接 `127.0.0.1:8888`

## 运行

```powershell
# 终端 1：启动服务端
python src\net_server\server.py

# 终端 2：启动客户端
python src\net_server\client.py
```

客户端输入消息回车发送，服务端原样回传；输入 `q` 退出。
服务端按 `Ctrl+C` 关闭。

## 协议

纯 TCP，无自定义帧结构，每条 `recv` 即时回写 `sendall`。
最大单包长度 1024 字节（`BUF_SIZE`）。