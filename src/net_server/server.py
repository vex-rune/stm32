"""Echo TCP 服务端。

监听 0.0.0.0:8888，接收客户端数据后原样回传。
支持 Ctrl+C 优雅退出。
"""
import socket
import threading

HOST = "0.0.0.0"
PORT = 8888
BUF_SIZE = 1024


def handle_client(conn: socket.socket, addr: tuple) -> None:
    print(f"[连接] 来自 {addr}")
    try:
        while True:
            data = conn.recv(BUF_SIZE)
            if not data:  # 客户端关闭
                break
            print(f"[收到] {addr} -> {data!r}")
            conn.sendall(data)  # echo
    except ConnectionResetError:
        print(f"[断开] {addr} 异常重置")
    finally:
        conn.close()
        print(f"[关闭] {addr}")


def main() -> None:
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((HOST, PORT))
    server.listen(5)
    print(f"[启动] echo 服务端 {HOST}:{PORT}")

    try:
        while True:
            conn, addr = server.accept()
            t = threading.Thread(target=handle_client, args=(conn, addr), daemon=True)
            t.start()
    except KeyboardInterrupt:
        print("\n[退出] 收到 Ctrl+C，正在关闭...")
    finally:
        server.close()


if __name__ == "__main__":
    main()