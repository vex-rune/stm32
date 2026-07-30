"""Echo 客户端测试脚本。

连接服务端后发送命令行输入，输出服务端回显。
输入 q 退出。
"""
import socket

HOST = "127.0.0.1"
PORT = 8888
BUF_SIZE = 1024


def main() -> None:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((HOST, PORT))
    print(f"[已连接] {HOST}:{PORT}，输入消息回车发送，q 退出")

    try:
        while True:
            msg = input("> ")
            if msg.strip().lower() == "q":
                break
            sock.sendall(msg.encode("utf-8"))
            data = sock.recv(BUF_SIZE)
            print(f"[回显] {data.decode('utf-8', errors='replace')}")
    except (ConnectionError, KeyboardInterrupt):
        print("\n[断开] 连接异常")
    finally:
        sock.close()


if __name__ == "__main__":
    main()